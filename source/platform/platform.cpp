/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include "diracsea.h"
#include "platform.h"

#include <filesystem>
#include <iostream>
#include <SDL.h>

#include "game.h"
#include "vector2.h"

namespace platform
{
////////////////////////////////////////////////
// Constants

static constexpr int kScreenWidth = 1920;
static constexpr int kScreenHeight = 1080;
static constexpr int kScreenHalfWidth = kScreenWidth / 2;
static constexpr int kScreenHalfHeight = kScreenHeight / 2;

////////////////////////////////////////////////
// State

static SDL_Window* g_pWindow = nullptr;

typedef std::pair<TActionMapId, TActionMap> TActionMapEntry;
typedef std::vector<TActionMapEntry> TActionMapStack;
TActionMapId g_actionMapIdAllocator = 0;
TActionMapStack g_actionMapStack;

////////////////////////////////////////////////
// Functions

ERunResult Initialize()
{
	assert(g_pWindow == nullptr);
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		DiracError("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return eRR_Error;
	}

	SDL_Window* pWindow = SDL_CreateWindow(
		"Dirac Sea Engine",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		kScreenWidth,
		kScreenHeight,
		SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN /*| SDL_WINDOW_FULLSCREEN */);

	if (pWindow == nullptr)
	{
		DiracError("SDL could not create window! SDL_Error: %s\n", SDL_GetError());
		return eRR_Error;
	}

	SDL_SetRelativeMouseMode(SDL_TRUE);

	g_pWindow = pWindow;
	return eRR_Success;
}

ERunResult RunIO(const SFrameContext& /*frameContext*/, bool* pExit)
{
	assert(pExit != nullptr);

	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				*pExit = true;
			}
			if (g_actionMapStack.size() > 0)
			{
				bool bActionHandled = false;
				for (
					TActionMapStack::reverse_iterator it = g_actionMapStack.rbegin();
					it != g_actionMapStack.rend() && bActionHandled == false;
					++it)
				{
					const TActionMap& rActionMap = it->second;
					for (const SActionHandler& rHandler : rActionMap)
					{
						if (rHandler.keyCode == event.key.keysym.sym && 
							(rHandler.requiredKeyMods == 0 || (rHandler.requiredKeyMods == event.key.keysym.mod)))
						{
							assert(rHandler.callback != nullptr);
							EKeyChange eKeyChange = event.type == SDL_KEYDOWN ? EKeyChange::Pressed : EKeyChange::Released;
							rHandler.callback(event.key.keysym.sym, event.key.keysym.mod, eKeyChange);
							bActionHandled = true;
							break;
						}
					}
				}
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
		DiracError("[platform::Shutdown] window is null!");
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
					DiracError("[%s] file error: %d, failed for file name: %s", __FUNCTION__, fileError, fileNames[i]);
					bSuccess = false;
				}
			}
			else
			{
				DiracError("[%s] file size 0 or error detecting size. Failed for file name: %s", __FUNCTION__, fileNames[i]);
				bSuccess = false;
			}
			fclose(pFile);
		}
		else
		{
			pOutArray[i] = SFile();
			DiracError("[%s] Failed to load file: %s", __FUNCTION__, fileNames[i]);
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
		DiracError("[%s] failed to load image: %s\n", __FUNCTION__, SDL_GetError());
		return ImageSurfacePtr();
	}

	return ImageSurfacePtr(pSurface);
}

TActionMapId PushActionMap(TActionMap&& actionMap)
{
	TActionMapId id = g_actionMapIdAllocator++;
	g_actionMapStack.emplace_back(id, actionMap);
	return id;
}

void RemoveActionMap(TActionMapId id)
{
	assert(g_actionMapStack.empty() == false);
	for (
		TActionMapStack::iterator it = g_actionMapStack.begin();
		it != g_actionMapStack.end();
		++it)
	{
		if (it->first == id)
		{
			g_actionMapStack.erase(it);
			return;
		}
	}

	assert(false && "RemoveActionMap failed to find id!");
}

uint32_t GetRelativeMouseState(Vec2<int>* pOutRawRel, Vec2<float>* pOutScreenRatioRel)
{
	assert(pOutRawRel != nullptr);
	assert(pOutScreenRatioRel != nullptr);
	int x, y;
	int buttonState = SDL_GetRelativeMouseState(&x, &y);
	pOutRawRel->x = x;
	pOutRawRel->y = y;
	pOutScreenRatioRel->x = float(x) / float(kScreenHalfWidth);
	pOutScreenRatioRel->y = float(y) / float(kScreenHalfHeight);
	return buttonState;
}

} // platform namespace
