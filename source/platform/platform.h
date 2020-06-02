/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include <memory>

struct SDL_Window;
struct SDL_Surface;

namespace platform
{
	ERunResult Initialize();
	ERunResult RunIO(bool* pExit);
	ERunResult Shutdown();
	SDL_Window* GetWindow();

	enum class EFileType : uint8_t
	{
		Text,
		Binary,
		NUM_FILETYPES
	};

	struct SFile
	{
		size_t numBytes = 0;
		std::unique_ptr<char[]> pData = nullptr;
	};

	// pOutArray is assumed to be the same size as numFiles
	// caller owns data allocation (hence, unique_ptr in SFile)
	bool LoadFiles(const char* fileNames[], size_t numFiles, EFileType fileType, SFile* pOutArray);

	struct SImageSurfaceDeleter
	{
		void operator()(SDL_Surface* pSurface);
	};
	typedef std::unique_ptr<SDL_Surface, SImageSurfaceDeleter> ImageSurfacePtr;

	ImageSurfacePtr LoadImage(const char* filePath);
} // platform namespace

