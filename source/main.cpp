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
  puts("[DiracSea] Initializing...");
  if (platform::Initialize() != eRR_Success)
  {
    puts("Platform initialization failed!");
    return eRR_Error;
  }

  if (renderer::Initialize() != eRR_Success)
  {
    puts("Renderer initialization failed!");
    return eRR_Error;
  }

  return eRR_Success;
}

ERunResult Run()
{
		puts("[DiracSea] Running...");
    RunTests();

    bool bExit = false;
    ERunResult platformRunIOResult = eRR_Success;
    ERunResult renderResult = eRR_Success;
    while (bExit == false && (platformRunIOResult | renderResult) == eRR_Success)
    {
			platformRunIOResult = platform::RunIO(&bExit);
			renderResult = renderer::Render();
    }

    if (platformRunIOResult != eRR_Success)
    {
      puts("Platform RunIO failed!");
    }

    if (renderResult != eRR_Success)
    {
      puts("Renderer render failed!");
    }

    return ERunResult(platformRunIOResult | renderResult);
}

ERunResult Shutdown()
{
		puts("[DiracSea] Shutting down...");
    ERunResult rendererShutdownResult = renderer::Shutdown();
    if (rendererShutdownResult != eRR_Success)
    {
      puts("Renderer shutdown error!");
    }
    ERunResult platformShutdownResult = platform::Shutdown();
    if (platformShutdownResult != eRR_Success)
    {
      puts("Platform shutdown error!");
    }
    return ERunResult(platformShutdownResult | rendererShutdownResult);
}

int main()
{
  ERunResult initializationResult = Initialize();

  ERunResult runResult = eRR_Success;
  if (initializationResult == eRR_Success)
  {
		runResult = Run();
  }

  ERunResult shutdownResult = Shutdown();
  int diracSeaEngineResult = initializationResult | runResult | shutdownResult;
  if (diracSeaEngineResult != eRR_Success)
  {
    printf("[DiracSea] exiting with error: %d\n", diracSeaEngineResult);
  }

  return diracSeaEngineResult;
}
