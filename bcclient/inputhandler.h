#ifndef CHECKMOUSE
#define CHECKMOUSE

#include "definitions.h"

// checkMouse - Determines whether the mouse is placed and/or clicking on anything of interest
void checkMouse(SDL_Event *event, SDL_Rect buttonPlacement[], int *select, int *mode, int modeMaxButtons[], int *keyboardMode, bool *quit);

// checkKeypress - Handles a keypress depending on the situation
void checkKeypress(SDL_Event *event, int *mode, int *select);

#endif // CHECKMOUSE
