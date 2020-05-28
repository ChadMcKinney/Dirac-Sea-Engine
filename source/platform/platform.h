/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

namespace platform
{
	ERunResult Initialize();
	ERunResult RunIO(bool* pExit);
	ERunResult Shutdown();
	struct SDL_Window* GetWindow();
} // platform namespace

