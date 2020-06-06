/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

namespace game
{
	ERunResult Initialize();
	ERunResult Run(const SFrameContext& frameContext);
	ERunResult Shutdown();
} // namespace game
