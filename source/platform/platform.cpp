/* Copyright (C) Chad McKinney - All Rights Reserved
 * Unauthorized copying of this file, via any medium is strictly prohibited
 * Proprietary and confidential
 */

#include <cassert>
#include <cstdio>
#include <SDL.h>

enum ERunResult : int
{
  eRR_Success = 0,
  eRR_Error = 1
};

namespace platform
{

int RunPlatform()
{
  /////////////////////////////////////////////////////////
  // Initialization
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return eRR_Error;
  }

  static const int kScreenWidth = 1024;
  static const int kScreenHeight = 768;

  SDL_Window* pWindow = SDL_CreateWindow(
    "Dirac Sea Engine",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    kScreenWidth,
    kScreenHeight,
    SDL_WINDOW_SHOWN);

  if (pWindow == nullptr)
  {
    printf("SDL could not create window! SDL_Error: %s\n", SDL_GetError());
    return eRR_Error;
  }


  SDL_Surface* pScreenSurface = SDL_GetWindowSurface(pWindow);
  if (pScreenSurface == nullptr)
  {
    printf("SDL could not create window surface! SDL_Error: %s\n", SDL_GetError());
    return eRR_Error;
  }

  /////////////////////////////////////////////////////////
  // Runtime

  // Just make some noise to see something on the screen
  for (size_t i = 0; i < 127; ++i)
  {
    Uint8 r = i % 3 == 0 ? 0xFF : 0;
    Uint8 g = i % 4 == 0 ? 0xFF : 0;
    Uint8 b = i % 5 == 0 ? 0xFF : 0;
		SDL_FillRect(pScreenSurface, nullptr, SDL_MapRGB(pScreenSurface->format, r, g, b));
		SDL_UpdateWindowSurface(pWindow);
		SDL_Delay(20);
  }

  /////////////////////////////////////////////////////////
  // Deconstruction
  SDL_DestroyWindow(pWindow);
  SDL_Quit();
  return eRR_Success;
}

} // platform namespace
