/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "diracsea.h"
#include "platform.h"

#include <filesystem>
#include <iostream>
#include <SDL.h>

// NOTES TO SELF:

// BREAK OUT VULKAN CODE INTO DIFFERNT FILE
// HAVE Main work like this:

// Platform::Initialize
// Renderer::Initialize
//	-> Use a #define list for shaders (should be precompiled to spir-v from via cmake build step, gathers all shaders in folder and compiles them!)
//	-> During initialize: Automatically load them into array (using TFileBatchHandle Platform::LoadFileBatch(const char* fileNames[], size_t numFiles, const char** outData), enum value is index
//  -> can then reference them during graphics pipeline building
// Renderer::Render while (true)
// Renderer::Shutdown
//	-> Calls Platform::UnloadFiles(TFileBatchHandle fileBatchHandle)
//  free dynamic memory allocation for compiled shaders
// Platform::Shutdown

namespace platform
{
static SDL_Window* g_pWindow = nullptr;

ERunResult Initialize()
{
	assert(g_pWindow == nullptr);
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return eRR_Error;
	}

	static const int kScreenWidth = 1024;
	static const int kScreenHeight = 768;

	SDL_Window* pWindow = SDL_CreateWindow(
		"Dirac Sea Engine",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		kScreenWidth,
		kScreenHeight,
		SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);

	if (pWindow == nullptr)
	{
		printf("SDL could not create window! SDL_Error: %s\n", SDL_GetError());
		return eRR_Error;
	}

	g_pWindow = pWindow;
	return eRR_Success;
}

ERunResult RunIO(bool* pExit)
{
	assert(pExit != nullptr);

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_ESCAPE ||
					(event.key.keysym.sym == SDLK_q && event.key.keysym.mod & KMOD_LCTRL))
			{
				*pExit = true;
			}
			break;
		case SDL_QUIT:
			*pExit = true;
			break;
		default:
			break;
		}
	}

	return eRR_Success;
}

ERunResult Shutdown()
{
	ERunResult shutdownResult = eRR_Success;
	if (g_pWindow != nullptr)
	{
		SDL_DestroyWindow(g_pWindow);
	}
	else
	{
		puts("[platform::Shutdown] window is null!");
		shutdownResult = eRR_Error;
	}

	SDL_Quit();
	return shutdownResult;
}

SDL_Window* GetWindow()
{
	return g_pWindow;
}

static constexpr size_t kNumFileTypes = size_t(EFileType::NUM_FILETYPES);
static const char* fileTypeDescriptors[kNumFileTypes] = { "r", "rb" };

#pragma warning(push)
#pragma warning(disable: 6385) // disable warning about invalid data access to fileTypeDescripters, we assert before access

// TODO: consider pak filesystem instead of just standard file IO
bool LoadFiles(const char* fileNames[], size_t numFiles, EFileType fileType, SFile* pOutArray)
{
	assert(numFiles > 0);
	assert((size_t) fileType < kNumFileTypes);
	assert(pOutArray != nullptr);
	bool bSuccess = true;
	for (size_t i = 0; i < numFiles; ++i)
	{
		assert(fileNames[i] && fileNames[i][0] != 0);
		FILE* pFile = fopen(fileNames[i], fileTypeDescriptors[uint8_t(fileType)]);
		if (pFile)
		{
			pOutArray[i] = SFile();
			SFile& rFile = pOutArray[i];
			fseek(pFile, 0, SEEK_END);
			size_t fileSize = ftell(pFile);
			rewind(pFile);
			if (fileSize > 0)
			{
				rFile.pData.reset(new char[fileSize]);
				rFile.numBytes = fread(rFile.pData.get(), 1, fileSize, pFile);
				int fileError = ferror(pFile);
				if (fileError != 0)
				{
					printf("[%s] file error: %d, failed for file name: %s\n", __FUNCTION__, fileError, fileNames[i]);
					bSuccess = false;
				}
			}
			else
			{
				printf("[%s] file size 0 or error detecting size. Failed for file name: %s\n", __FUNCTION__, fileNames[i]);
				bSuccess = false;
			}
			fclose(pFile);
		}
		else
		{
			pOutArray[i] = SFile();
			printf("[%s] Failed to load file: %s\n", __FUNCTION__, fileNames[i]);
			bSuccess = false;
		}
	}
	return bSuccess;
}
#pragma warning(pop)
	
void SImageSurfaceDeleter::operator()(SDL_Surface* pSurface)
{
	SDL_FreeSurface(pSurface);
}

ImageSurfacePtr LoadImage(const char* filePath)
{
	assert(filePath && filePath[0]);
	SDL_Surface* pSurface = SDL_LoadBMP(filePath);
	if (pSurface == nullptr)
	{
		printf("[%s] failed to load image: %s\n", __FUNCTION__, SDL_GetError());
		return ImageSurfacePtr();
	}

	return ImageSurfacePtr(pSurface);
}

} // platform namespace
