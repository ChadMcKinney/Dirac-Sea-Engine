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
  g_camera.transform.m43 = -8.0f + (float)std::sin(TSeconds(frameContext.gameDuration).count()) * 4.0f;
  renderer::SetViewMatrix(g_camera.transform);
  return eRR_Success;
}

ERunResult Shutdown()
{
  return eRR_Success;
}

} // namespace game
