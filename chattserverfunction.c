#include <SDL2/SDL.h>
#include "foo.h"  /* Include the header (not strictly necessary here) */

bool checkConnection(int clientId);

SDL_TreadFunction *chattserverfunction(int currentClientId)    /* Function definition */
{
    int clientId;
    char UDPtext[]; char TCPtext[];
    

    clientId = currentClientId;

    while(1)
    {
        if(!checkConnection(clients[clientId]))
        {
            SDLNet_TCP_Close(clients[clientId]);
            activeClients=SDLNet_TCP_DelSocket(socketSet,clients[clientId]);
            break;
        }
        else
        {
        
            SDLNet_TCP_Recv(clients[clientId],TCPtext,MAX_LENGTH);
            for(int i=0;i<activeClients;i++)
            {
                if(i!=clientId) SDLNet_TCP_Send(clients[i],TCPtext,MAX_LENGTH);
            }
        }
    }

    return;
}

bool checkConnection(int clientId)
{
    bool connection;

    if(SDLNet_TCP_Recv(clients[clientId]) <= 0)
    {
        connection=false;
    }
    else
    {
        connection=true;
    }
    return connection;
}

    
    clientid = currentclientid;
    
    while(){
        
        ;
    }

