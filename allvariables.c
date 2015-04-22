#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2_net/SDL_net.h>
#include "allvariables.h"


#define MAX_CLIENTS 8
#define LENGTH 100
struct client createClient(TCPsocket clientTCPsocket,char name[LENGTH],int score)
{
    struct client c;
    
    c.TCPsocket=clientTCPsocket;
    strcpy(c.name,name);
    c.score=score;
    
    return c;
}

struct client clients[MAX_CLIENTS];
SDLNet_SocketSet socketSet;
UDPsocket UDPsocketIN;
UDPsocket UDPsocketOUT;
int activeClients;
char name[LENGTH]={ "Player" };

