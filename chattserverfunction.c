#include <SDL2/SDL.h>
#include <SDL2_net/SDL_net.h>
#include "foo.h"  /* Include the header (not strictly necessary here) */

bool checkConnection(int cRecv);

SDL_ThreadFunction *chattserverfunction(int currentClientId)    /* Function definition */
{
    int clientId;
    char UDPtext[]; char TCPtext[];


    clientId = currentClientId;

    while(1)
    {
        if(!checkConnection(SDLNet_TCP_Recv(clients[clientId],TCPtext,MAX_LENGTH)))
        {
            SDLNet_TCP_Close(clients[clientId]);
            activeClients = SDLNet_TCP_DelSocket(socketSet,clients[clientId]);
            break;
        }
        for(int i=0;i<activeClients;i++)
        {
<<<<<<< HEAD
            if(i!=clientId) SDLNet_TCP_Send(clients[i],TCPtext,MAX_LENGTH);
=======

            SDLNet_TCP_Recv(clients[clientId],TCPtext,MAX_LENGTH);
            for(int i=0;i<activeClients;i++)
            {
                if(i!=clientId) SDLNet_TCP_Send(clients[i],TCPtext,MAX_LENGTH);
            }
>>>>>>> origin/master
        }
    }
    return;
}

<<<<<<< HEAD

bool checkConnection(int clientId)
=======
bool checkConnection(int cRecv)
>>>>>>> origin/master
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

