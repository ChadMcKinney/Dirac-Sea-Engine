/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "diracsea.h"
#include "game.h"
#include "camera.h"
#include "renderer.h"

namespace game
{
/////////////////////////////////////////////////////////
// Constants

/////////////////////////////////////////////////////////
// State
SCamera g_camera;

/////////////////////////////////////////////////////////
// Functions

ERunResult Initialize()
{
  g_camera.transform = Matrix44l::CreateRotationAndTranslation(
    EIdentity::Constructor,
    Vec3l(0, 0, -8)
  );
  return eRR_Success;
}

ERunResult Run(const SFrameContext& frameContext)
{
  fworld distance = -8.0 + std::sin(TSeconds(frameContext.gameDuration).count()) * 2.0;
  Vec3w pos(0.0, 0.0, distance);
  g_camera.transform = localspace_cast(Matrix44w::CreateRotationAndTranslation(EIdentity::Constructor, pos));
  g_camera.transform *= Matrix33l::CreateRotationY((float)std::fmod(TSeconds(frameContext.gameDuration).count() * 0.5, kFWorldTwoPi));
  // printf("pos: (x: %f, y: %f, z: %f\n", g_camera.transform.m41, g_camera.transform.m42, g_camera.transform.m43);
  renderer::SetViewMatrix(g_camera.transform);
  return eRR_Success;
}

ERunResult Shutdown()
{
  return eRR_Success;
}

} // namespace game
