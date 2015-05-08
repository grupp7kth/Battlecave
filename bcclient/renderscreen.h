#ifndef RENDERSCREEN
#define RENDERSCREEN

#include "definitions.h"

// renderScreen - Renders the screen, based on mode
void renderScreen(int *mode, int *select, SDL_Rect buttonPlacement[], SDL_Rect windowPlacement[]);

// loadMedia - Loads all media required
void loadMedia(void);

// initSDL - Initiates SDL; creates the window etc
void initSDL(void);

// closeRenderer - Closes all render-related items
void closeRenderer(void);

#endif // RENDERSCREEN
