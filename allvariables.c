#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include "allvariables.h"

#define MAX_CLIENTS 10


SDL_Thread *client_thread[MAX_CLIENTS];
SDLNet_SocketSet socketSet;
TCPsocket clientTCPsocket[MAX_CLIENTS];
UDPsocket clientUDPsocket[MAX_CLIENTS];
int activeClients;
