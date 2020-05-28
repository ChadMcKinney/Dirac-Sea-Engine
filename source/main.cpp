/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "diracsea.h"

#include "platform/platform.h"
#include "renderer/renderer.h"
#include "tests/tests.h"

ERunResult Initialize()
{
  ERunResult platformInitializationResult = platform::Initialize();
  if (platformInitializationResult != 0)
    return platformInitializationResult;

	ERunResult rendererInitializationResult = renderer::Initialize();
  if (rendererInitializationResult != 0)
    return rendererInitializationResult;

  return rendererInitializationResult;
}

ERunResult Run()
{
    RunTests();
    ERunResult platformRunIOResult = platform::RunIO();
    ERunResult renderResult = renderer::Render();
    ERunResult platformRunResult = platform::RunPlatform();
    return ERunResult(platformRunIOResult | renderResult | platformRunResult);
}

ERunResult Shutdown()
{
    ERunResult rendererShutdownResult = renderer::Shutdown();
    ERunResult platformShutdownResult = platform::Shutdown();
    return ERunResult(platformShutdownResult | rendererShutdownResult);
}

int main()
{
  ERunResult initializationResult = Initialize();

  ERunResult runResult = eRR_Success;
  if (initializationResult == 0)
  {
		runResult = Run();
  }

  ERunResult shutdownResult = Shutdown();
  return initializationResult | runResult | shutdownResult;
}
