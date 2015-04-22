#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include "allvariables.h"

#define MAX_CLIENTS 10
#define LENGTH 100
struct Client
{
    TCPsocket TCPsocket;
    char name[LENGTH];
    int score;

};
client createClient(TCPsocket clientTCPsocket,char name[LENGTH],int score)
{
    client c;
    c.clientTCPsocket=clientTCPsocket;
    strcpy(c.name,name);
    c.score=score;
    return c;
}

client clients[MAX_CLIENTS];
SDLNet_SocketSet socketSet;
UDPsocket UDPsocketIN;
UDPsocket UDPsocketOUT;
int activeClients;
