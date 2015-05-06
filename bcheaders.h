#ifndef bcheaders_H_   /* Include guard */
#define bcheaders_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2_net/SDL_net.h>
#include <SDL2_image/SDL_image.h>
#include "allvariables.h"
#include "chattserverfunction.h"
#include "serverclientthreadfunc.h"
#define MAX_LENGTH 100
#define MAX_CLIENTS 8
#define MAX_BULLETS 1000
#define PREAMBLE_CHAT "$"
#define PREAMBLE_PLAYERS '@'
#define PREAMBLE_READY '#'
#define PREAMBLE_READYS "#"
#define PREAMBLE_ID "%"
#define PREAMBLE_DISCONNECT '-'
#define READY "1"
#define NOT_READY "0"

#endif // ALLVARIABLES_H_
