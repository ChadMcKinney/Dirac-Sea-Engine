/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include <memory>
#include <vector>

struct SDL_Window;
struct SDL_Surface;

template <typename T>
struct Vec2;

namespace platform
{
	ERunResult Initialize();
	ERunResult RunIO(const SFrameContext& frameContext, bool* pExit);
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

	enum class EKeyChange
	{
		Pressed,
		Released
	};

	typedef int32_t TKeyCode;
	typedef uint16_t TKeyMod;

	struct SActionHandler
	{
		TKeyCode keyCode = { 0 };
		TKeyMod requiredKeyMods = { 0 };
		void (*callback)(TKeyCode keyCode, TKeyMod keyMod, EKeyChange keyChange);
	};

	typedef std::vector<SActionHandler> TActionMap;
	typedef uint8_t TActionMapId;
	
	TActionMapId PushActionMap(TActionMap&& actionMap);
	void RemoveActionMap(TActionMapId id);

	uint32_t GetRelativeMouseState(Vec2<int>* pOutRawRel, Vec2<float>* pOutScreenRatioRel);
} // platform namespace

