#include <SDL2/SDL.h>
#include <SDL2_net/SDL_net.h>
#include <stdio.h>
#include <stdbool.h>
#include "chattserverfunction.h"  /* Include the header (not strictly necessary here) */
#include "allvariables.h"

#define MAX_LENGTH 100
#define MAX_CLIENTS 8

bool checkConnection(int cRecv);

SDL_ThreadFunction *chattserverfunction(int currentClientId)    /* Function definition */
{
    int clientId;
    char UDPtext[MAX_LENGTH]; char TCPtext[MAX_LENGTH];


    clientId = currentClientId;

    while(1)
    {
        if(!checkConnection(SDLNet_TCP_Recv(clients[clientId],TCPtext,MAX_LENGTH)))
        {
            SDLNet_TCP_Close(clientTCPsockets[clientId]);
            activeClients = SDLNet_TCP_DelSocket(socketSet,clients[clientId]);
            break;
        }
        for(int i=0;i<activeClients;i++)
        {
            if(i!=clientId) SDLNet_TCP_Send(clients[i],TCPtext,MAX_LENGTH);
        }
    }
    return;
}
bool checkConnection(int cRecv)
{
    bool connection;
    int check = cRecv;
    if(check<=0)
    {
        connection=false;
    }
    else
    {
        connection=true;
    }
    return connection;
}

