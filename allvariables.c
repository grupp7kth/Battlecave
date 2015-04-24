#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2_net/SDL_net.h>
#include "allvariables.h"




struct client createClient(TCPsocket clientTCPsocket,char name[MAX_LENGTH],int score)
{
    struct client c;
    
    c.socket=clientTCPsocket;
    strcpy(c.name,name);
    c.score=score;
    c.ID = 0;
    c.active = false;
   
    return c;
}
struct client players[MAX_CLIENTS];
SDLNet_SocketSet socketSet;
UDPsocket UDPsocketIN;
UDPsocket UDPsocketOUT;
int activeClients;
TCPsocket playerSocket[MAX_CLIENTS];
int clientList[1000];
