/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "diracsea.h"

#include "game/game.h"
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

  if (game::Initialize() != eRR_Success)
  {
    puts("Game initialized failed!");
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
    ERunResult gameRunResult = eRR_Success;
		ERunResult renderResult = eRR_Success;

		TTime lastFrameTime = std::chrono::steady_clock::now();
		SFrameContext frameContext = { lastFrameTime, TMilliseconds() };

		while (bExit == false && (platformRunIOResult | gameRunResult | renderResult) == eRR_Success)
		{
			lastFrameTime = frameContext.frameStartTime;
			frameContext.frameStartTime = std::chrono::steady_clock::now();
			frameContext.lastFrameDuration = frameContext.frameStartTime - lastFrameTime;
      frameContext.gameDuration += frameContext.lastFrameDuration;

			platformRunIOResult = platform::RunIO(&bExit);
      gameRunResult = game::Run(frameContext);
			renderResult = renderer::Render(frameContext);
		}

    if (platformRunIOResult != eRR_Success)
    {
      puts("Platform RunIO failed!");
    }

    if (gameRunResult != eRR_Success)
    {
      puts("Game run failed!");
    }

    if (renderResult != eRR_Success)
    {
      puts("Renderer render failed!");
    }

    return ERunResult(platformRunIOResult | gameRunResult | renderResult);
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
