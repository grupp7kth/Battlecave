#include <SDL2/SDL.h>
#include <SDL2_net/SDL_net.h>
#include <stdio.h>
#include <stdbool.h>
#include "chattserverfunction.h"  /* Include the header (not strictly necessary here) */
#include "allvariables.h"

bool checkConnection(int cRecv);

SDL_ThreadFunction *chattfunction(int currentClientId)    /* Function definition */
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDLNet_Init();
    
    int clientId;
    //char UDPtext[MAX_LENGTH];
    char TCPtext[MAX_LENGTH];


    clientId = currentClientId;

    while(1)
    {
        if(!checkConnection(SDLNet_TCP_Recv(clients[clientId].socket,TCPtext,MAX_LENGTH)))
        {
            SDLNet_TCP_Close(clients[clientId].socket);
            activeClients = SDLNet_TCP_DelSocket(socketSet,clients[clientId].socket);
            break;
        }
        for(int i=0;i<activeClients;i++)
        {
            if(i!=clientId) SDLNet_TCP_Send(clients[i].socket,TCPtext,MAX_LENGTH);
        }
    }
    return 0;
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

