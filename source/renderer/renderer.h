/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

template <typename T>
struct Matrix44;

namespace renderer
{
    ERunResult Initialize();
    ERunResult Render(const SFrameContext& frameContext);
    ERunResult Shutdown();
    void SetViewMatrix(const Matrix44<float>& viewMatrix);
} // renderer namespace
