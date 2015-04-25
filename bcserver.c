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

int getFreeClientID();

SDL_Thread* clientThreads[MAX_CLIENTS*2];
IPaddress serverIP;
TCPsocket serverTCPsocket;
TCPsocket incomming = NULL;

int main(int argc, char* args[])
{
    int clientThr1 = 0, clientThr2 = 0, freeID;
    
    /*for (int i=0; i<MAX_CLIENTS; i++) {
        players[i] = createClient(playerSocket[i],"Player",0);
    }*/
    
    SDL_Init(SDL_INIT_EVERYTHING);
    SDLNet_Init();
    
    SDLNet_ResolveHost(&serverIP, NULL, 3445);
    serverTCPsocket = SDLNet_TCP_Open(&serverIP);
    socketSet = SDLNet_AllocSocketSet(MAX_CLIENTS); if (!socketSet) {printf("%s\n", SDLNet_GetError());}
    
    
    printf("Waiting for connection...\n");
    while (true){
        while ((incomming = SDLNet_TCP_Accept(serverTCPsocket)) == 0x0 );       // Väntar in connections
        if ((freeID = getFreeClientID())<0) {printf("Too many users! \n");}     // Ger uppkopplad kient ett ledigt clientID
        else{
            
            players[freeID].socket = incomming;
            players[freeID].ID = freeID;
            players[freeID].active = true;
            activeClients = SDLNet_TCP_AddSocket(socketSet, players[freeID].socket);
            printf("ClientID: %d    Connected\n", players[freeID].ID);
            
            clientThreads[clientThr1] = SDL_CreateThread(clientThreadFunction, "ClientMainThread", &players[freeID]);
            clientThreads[clientThr2] = SDL_CreateThread(chattserverfunction, "ClientChattThread", &players[freeID]);
            SDL_DetachThread(clientThreads[clientThr1]);
            SDL_DetachThread(clientThreads[clientThr2]);
            clientThr1 ++;
            clientThr2 ++;
            incomming = NULL;
        }
    }
    
    SDLNet_TCP_Close(serverTCPsocket);
    SDLNet_Quit();
    SDL_Quit();
    return 0;
}

int getFreeClientID(){ // Letar upp en ledig clientplats och returnerar ett clientID
    for (int i=0; i<MAX_CLIENTS; i++) {
        if (!players[i].active) return i;
    }
    return -1;
}






























