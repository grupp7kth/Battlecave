#include <SDL2/SDL.h>
#include <SDL2_net/SDL_net.h>
#include <stdio.h>
#include <stdbool.h>
#include "serverclientthreadfunc.h"
#include "chattserverfunction.h" /* Include the header (not strictly necessary here) */
#include "allvariables.h"


SDL_ThreadFunction *clientThreadFunction(struct client* player)    /* Function definition */
{
    //struct client* player=(struct client*) p ;
    
    int clientID = player->ID;
    SDL_Thread *chattThreads[2];
    
    
    //char UDPtext[MAX_LENGTH];
    char TCPtext[MAX_LENGTH];
    
    chattThreads[1] = SDL_CreateThread(*chattserverfunction(), "clientchattT", player);
    
    
    while(1)
    {
        if(!checkConnection(SDLNet_TCP_Recv(player->socket,TCPtext,MAX_LENGTH)))
        {
            SDLNet_TCP_Close(player->socket);
            activeClients = SDLNet_TCP_DelSocket(socketSet,player->socket);
            break;
        }
    }
    return 0;
}
