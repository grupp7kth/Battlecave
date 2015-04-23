#include <SDL2/SDL.h>
#include <SDL2_net/SDL_net.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "chattserverfunction.h"
#include "serverclientthreadfunc.h"
#include "allvariables.h"

#define SERVER_IP "193.10.39.101"
#define PLAYER  "Player"

SDL_Thread* clientThreads[MAX_CLIENTS];
IPaddress serverIP;
TCPsocket serverTCPsocket;
TCPsocket playerSocket;
struct client players[MAX_CLIENTS];

int main(int argc, char* args[]){

    int clientID = 0;
    for (int i; i<MAX_CLIENTS; i++) {
        players[i] = createClient(NULL,PLAYER,0);
    }
    
    
    SDL_Init(SDL_INIT_EVERYTHING);
    SDLNet_Init();
    
    SDLNet_ResolveHost(&serverIP, NULL, 3445);
    serverTCPsocket = SDLNet_TCP_Open(&serverIP);
    socketSet = SDLNet_AllocSocketSet(MAX_CLIENTS); if (!socketSet) {printf("%s\n", SDLNet_GetError());}
    
    //UDPsocketIN = SDLNet_UDP_Open(0);
    //UDPsocketOUT = SDLNet_UDP_Open(0);
    
    printf("Waiting for connection...\n");
    
    while (true) {
        
        if (clientID<MAX_CLIENTS) {
            
            players[clientID].socket = SDLNet_TCP_Accept(serverTCPsocket);
            if (players[clientID].socket) {
                printf("Connect...\n");
                activeClients = SDLNet_TCP_AddSocket(socketSet, players[clientID].socket);
                players[clientID].ID = clientID;
                clientThreads[clientID] = SDL_CreateThread(*clientThreadFunction(), "clientThread", &players[clientID]);
                clientID ++; 
            }
        }
    }
    return 0;
}































