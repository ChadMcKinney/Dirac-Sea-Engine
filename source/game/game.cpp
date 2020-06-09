/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "diracsea.h"
#include "game.h"

#include <SDL_keycode.h>

#include "camera.h"
#include "platform.h"
#include "quaternion.h"
#include "renderer.h"

namespace game
{
/////////////////////////////////////////////////////////
// Constants
static constexpr int kDebugVerbosity = 2;

/////////////////////////////////////////////////////////
// State
SCamera g_camera;

#define DIRECTION_LIST(x)\
    x(Forward)\
    x(Back)\
    x(Left)\
    x(Right)\
    x(Up)\
    x(Down)

AUTO_BITFIELD_ENUM(Direction, DIRECTION_LIST, uint16_t);

#define ROTATION_LIST(x)\
    x(Roll)\
    x(RollBack)

AUTO_BITFIELD_ENUM(Rotation, ROTATION_LIST, uint16_t);

struct SPlayer
{
    Quaternionl orientation = { EIdentity::Constructor };
    Quaternionl targetOrientation = { EIdentity::Constructor };
    Vec3l position = { EZero::Constructor };
    ERotation::Flags rotations = ERotation::Flags::FLAGS_NONE;
    ERotation::Flags activeRotations = ERotation::Flags::FLAGS_NONE;
    EDirection::Flags directions = EDirection::Flags::FLAGS_NONE;
    EDirection::Flags activeDirections = EDirection::Flags::FLAGS_NONE;
    Vec3l controlDir = { EZero::Constructor };
    Vec3l velocity = { EZero::Constructor };
    float movAccelPerSec = 0.1f;
    float rotSpdPerSec = kFLocalPi;

    static constexpr float kMaxMoveSpeed = 0.5f;
};

SPlayer g_player;

/////////////////////////////////////////////////////////
// Internal Functions

#define GameDebug(...) DiracLog(kDebugVerbosity, __VA_ARGS__)

void UpdatePlayerDirection()
{
    g_player.controlDir = EZero::Constructor;

    static const Vec3l directions[EDirection::count] =
    {
      Vec3l::Forward,
      Vec3l::Back,
      Vec3l::Left,
      Vec3l::Right,
      Vec3l::Up,
      Vec3l::Down
    };

    const EDirection::TUnderlyingType flags = EDirection::TUnderlyingType(g_player.activeDirections);
    for (size_t i = 0; i < EDirection::count; ++i)
    {
        if (flags & BIT(i))
        {
            g_player.controlDir = g_player.controlDir + directions[i];
        }
    }

    g_player.controlDir.SafeNormalize();
}

void HandleDirectionKeyDown(EDirection::Flags dir, EDirection::Flags inverseDir, platform::EKeyChange keyChange, const char* name)
{
    if (keyChange == platform::EKeyChange::Pressed)
    {
        g_player.directions = g_player.directions | dir;
        g_player.activeDirections = (g_player.activeDirections & ~inverseDir) | dir;
        GameDebug("%s pressed\n", name);
    }
    else
    {
        g_player.directions = g_player.directions & ~dir;
        g_player.activeDirections = (g_player.activeDirections & ~dir) | (g_player.directions & inverseDir);
        GameDebug("%s released\n", name);
    }
    UpdatePlayerDirection();
}

void HandleW(int32_t, uint16_t, platform::EKeyChange keyChange)
{
    HandleDirectionKeyDown(EDirection::Flags::Forward, EDirection::Flags::Back, keyChange, "W");
}

void HandleA(int32_t, uint16_t, platform::EKeyChange keyChange)
{
    HandleDirectionKeyDown(EDirection::Flags::Left, EDirection::Flags::Right, keyChange, "A");
}

void HandleS(int32_t, uint16_t, platform::EKeyChange keyChange)
{
    HandleDirectionKeyDown(EDirection::Flags::Back, EDirection::Flags::Forward, keyChange, "S");
}

void HandleD(int32_t, uint16_t, platform::EKeyChange keyChange)
{
    HandleDirectionKeyDown(EDirection::Flags::Right, EDirection::Flags::Left, keyChange, "D");
}

void HandleCtrl(int32_t, uint16_t, platform::EKeyChange keyChange)
{
    HandleDirectionKeyDown(EDirection::Flags::Down, EDirection::Flags::Up, keyChange, "Ctrl");
}

void HandleSpace(int32_t, uint16_t, platform::EKeyChange keyChange)
{
    HandleDirectionKeyDown(EDirection::Flags::Up, EDirection::Flags::Down, keyChange, "Space");
}

void HandleRotationKeyDown(ERotation::Flags rot, ERotation::Flags inverseRot, platform::EKeyChange keyChange, const char* name)
{
    if (keyChange == platform::EKeyChange::Pressed)
    {
        g_player.rotations = g_player.rotations | rot;
        g_player.activeRotations = (g_player.activeRotations & ~inverseRot) | rot;
        GameDebug("%s pressed\n", name);
    }
    else
    {
        g_player.rotations = g_player.rotations & ~rot;
        g_player.activeRotations = (g_player.activeRotations & ~rot) | (g_player.rotations & inverseRot);
        GameDebug("%s released\n", name);
    }

    assert(g_player.activeRotations != ERotation::Flags::FLAGS_ALL);
}

void HandleE(int32_t, uint16_t, platform::EKeyChange keyChange)
{
    HandleRotationKeyDown(ERotation::Flags::Roll, ERotation::Flags::RollBack, keyChange, "E");
}

void HandleQ(int32_t, uint16_t, platform::EKeyChange keyChange)
{
    HandleRotationKeyDown(ERotation::Flags::RollBack, ERotation::Flags::Roll, keyChange, "Q");
}

/////////////////////////////////////////////////////////
// Exposed Functions

ERunResult Initialize()
{
    g_player.position = Vec3l(0, 0, -8);
    g_camera.transform = Matrix44l::CreateRotationAndTranslation(
        EIdentity::Constructor,
        g_player.position
    );

    platform::TActionMap actionMap = {
      { SDLK_w, 0, HandleW },
      { SDLK_a, 0, HandleA },
      { SDLK_s, 0, HandleS },
      { SDLK_d, 0, HandleD },
      { SDLK_LCTRL, 0, HandleCtrl },
      { SDLK_SPACE, 0, HandleSpace },
      { SDLK_e, 0, HandleE },
      { SDLK_q, 0, HandleQ }
    };

    platform::PushActionMap(std::move(actionMap));
    return eRR_Success;
}

ERunResult Run(const SFrameContext& frameContext)
{
    const float fTimeSecs = float(TSeconds(frameContext.lastFrameDuration).count());
    if (!g_player.controlDir.IsZero(kFLocalEpsilon))
    {
        const Matrix33l rotation = g_camera.transform.GetRotation();
        const Vec3l direction = (g_player.controlDir * rotation).Normalized();
        const Vec3l movement = direction.Scaled(g_player.movAccelPerSec * fTimeSecs);
        g_player.velocity = g_player.velocity + movement;
        float fSpeed = g_player.velocity.Magnitude();
        if (fSpeed > g_player.kMaxMoveSpeed)
        {
            g_player.velocity.Scale(g_player.kMaxMoveSpeed / fSpeed);
        }
        g_player.position = g_player.position + g_player.velocity;
    }
    else if (g_player.velocity.Magnitude() > kFLocalEpsilon)
    {
        g_player.velocity = g_player.velocity - (g_player.velocity.Scaled(g_player.movAccelPerSec * fTimeSecs * 4.0f));
        g_player.position = g_player.position + g_player.velocity;
    }
    else
    {
        g_player.velocity = EZero::Constructor;
    }

    float rollRadians = 0;

    if ((g_player.activeRotations & ERotation::Flags::Roll) != ERotation::Flags::FLAGS_NONE)
    {
        rollRadians = float(-TSeconds(frameContext.lastFrameDuration).count() * kFWorldQuarterPi * 0.5);
    }
    else if ((g_player.activeRotations & ERotation::Flags::RollBack) != ERotation::Flags::FLAGS_NONE)
    {
        rollRadians = float(TSeconds(frameContext.lastFrameDuration).count() * kFWorldQuarterPi * 0.5);
    }

    Vec2i rawRelMouse(EUninitialized::Constructor);
    Vec2l relMouse(EUninitialized::Constructor);
    platform::GetRelativeMouseState(&rawRelMouse, &relMouse);
    const Vec2l normalizedMouse = relMouse.SafeNormalized();

#if 0
    DiracLog(1, "MouseMoved frameID: %llu, rawRelMouse@(%i, %i), relMouse@(%f, %f), normalizedMouse@(%f, %f)\n",
        frameContext.frameId,
        rawRelMouse.x,
        rawRelMouse.y,
        relMouse.x,
        relMouse.y,
        normalizedMouse.x,
        normalizedMouse.y);
#endif

    float yawRadians = 0;
    float pitchRadians = 0;

    if (rawRelMouse.x != 0 || rawRelMouse.y != 0)
    {
        static const double fSensitivity = 50.f;
        const float fSpdPerSec = g_player.rotSpdPerSec * float(TSeconds(frameContext.lastFrameDuration).count() * fSensitivity);
        yawRadians = -relMouse.x * fSpdPerSec;
        pitchRadians = relMouse.y * fSpdPerSec;
    }

    if (abs(rollRadians + yawRadians + pitchRadians) > FLT_EPSILON)
    {
        g_player.targetOrientation = g_player.targetOrientation * Quaternionl::CreateRotationXYZ(pitchRadians, yawRadians, rollRadians);
        g_player.targetOrientation.Normalize();
    }

    g_player.orientation = Quaternionl::CreateSlerp(g_player.orientation, g_player.targetOrientation, float(TSeconds(frameContext.lastFrameDuration).count() * 2));
    g_player.orientation.Normalize();

    g_camera.transform = Matrix44l::CreateRotationAndTranslation(
        g_player.orientation,
        g_player.position
    );

    renderer::SetViewMatrix(g_camera.transform);
    return eRR_Success;
}

ERunResult Shutdown()
{
    return eRR_Success;
}

} // namespace game
