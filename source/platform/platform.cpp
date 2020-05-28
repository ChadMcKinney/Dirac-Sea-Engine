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

// TODO: consider pak filesystem instead of just standard file IO
bool LoadFiles(const char* fileNames[], size_t numFiles, SFile* pOutArray)
{
	assert(numFiles > 0);
	assert(pOutArray != nullptr);
	bool bSuccess = true;
	for (size_t i = 0; i < numFiles; ++i)
	{
		assert(fileNames[i] && fileNames[i][0] != 0);
		FILE* pFile = fopen(fileNames[i], "r");
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
					printf("[%s] file error: %d\n", __FUNCTION__, fileError);
					bSuccess = false;
				}
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

} // platform namespace
