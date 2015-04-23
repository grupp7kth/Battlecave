#include <SDL2/SDL.h>
#include <SDL2_net/SDL_net.h>
#include <stdio.h>
#include <stdbool.h>
#include "chattserverfunction.h"  /* Include the header (not strictly necessary here) */
#include "allvariables.h"

bool checkConnection(int cRecv);
void cleanMessage(char message[]);
SDL_ThreadFunction *chattserverfunction(struct client* player)    /* Function definition */
{
    //struct client* player=(struct client*) p ;
    
    int clientID = player->ID;
    TCPsocket socket = player->socket;
    
    //char UDPtext[MAX_LENGTH];
    char TCPtext[MAX_LENGTH];
    printf("Chatt\n");
    printf("%s\n", TCPtext);
    while(1)
    {
        cleanMessage(TCPtext);
        SDLNet_TCP_Recv(player->socket,TCPtext,MAX_LENGTH);
        printf("%s\n", TCPtext);
        SDLNet_TCP_Send(player->socket,TCPtext,MAX_LENGTH);
        
        for(int i=0;i<activeClients;i++)
        {
            //if(i!=clientID)
            
        }
    }
    return 0;
}

void cleanMessage(char message[]){
    for(int i=0;i<MAX_LENGTH;i++){
        message[i]='\0';
    }

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

