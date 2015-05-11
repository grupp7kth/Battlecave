#ifndef SETTEXT
#define SETTEXT

#include <SDL2/SDL.h>
#include <SDL2_Image/SDL_Image.h>
#include <SDL2_Net/SDL_Net.h>
#include <SDL2_ttf/SDL_ttf.h>
#include "definitions.h"

// setText - Initates locations and text messages depending on mode and renders them
void setText(int *mode, SDL_Renderer* gRenderer, int *select);

#endif // SETTEXT
