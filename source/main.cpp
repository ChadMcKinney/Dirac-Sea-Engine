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
    DiracLog(1, "[DiracSea] Initializing...");
    if (platform::Initialize() != eRR_Success)
    {
        DiracError("Platform initialization failed!");
        return eRR_Error;
    }

    if (game::Initialize() != eRR_Success)
    {
        DiracError("Game initialization failed!");
        return eRR_Error;
    }

    if (renderer::Initialize() != eRR_Success)
    {
        DiracError("Renderer initialization failed!");
        return eRR_Error;
    }

    return eRR_Success;
}

ERunResult Run()
{
    DiracLog(1, "[DiracSea] Running...");
    RunTests();

    bool bExit = false;
    ERunResult platformRunIOResult = eRR_Success;
    ERunResult gameRunResult = eRR_Success;
    ERunResult renderResult = eRR_Success;

    TTime lastFrameTime = TSteadyClock::now();
    SFrameContext frameContext = { lastFrameTime, TMilliseconds(), TMinutes(), 0 };

    while (bExit == false && (platformRunIOResult | gameRunResult | renderResult) == eRR_Success)
    {
        lastFrameTime = frameContext.frameStartTime;
        frameContext.frameStartTime = TSteadyClock::now();
        frameContext.lastFrameDuration = frameContext.frameStartTime - lastFrameTime;
        frameContext.gameDuration += frameContext.lastFrameDuration;
        frameContext.frameId++;

        platformRunIOResult = platform::RunIO(frameContext, &bExit);
        gameRunResult = game::Run(frameContext);
        renderResult = renderer::Render(frameContext);
        platform::RegulateFrameLimit(frameContext);
    }

    if (platformRunIOResult != eRR_Success)
    {
        DiracError("Platform RunIO failed!");
    }

    if (gameRunResult != eRR_Success)
    {
        DiracError("Game run failed!");
    }

    if (renderResult != eRR_Success)
    {
        DiracError("Renderer render failed!");
    }

    return ERunResult(platformRunIOResult | gameRunResult | renderResult);
}

ERunResult Shutdown()
{
    DiracLog(1, "[DiracSea] Shutting down...");
    ERunResult rendererShutdownResult = renderer::Shutdown();
    if (rendererShutdownResult != eRR_Success)
    {
        DiracError("Renderer shutdown error!");
    }
    ERunResult platformShutdownResult = platform::Shutdown();
    if (platformShutdownResult != eRR_Success)
    {
        DiracError("Platform shutdown error!");
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
        DiracError("[DiracSea] exiting with error: %d\n", diracSeaEngineResult);
    }

    return diracSeaEngineResult;
}
