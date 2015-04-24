#include <SDL2/SDL.h>
#include <SDL2_net/SDL_net.h>
#include <stdio.h>
#include <stdbool.h>
#include "chattserverfunction.h"  /* Include the header (not strictly necessary here) */
#include "allvariables.h"

bool checkConnection(int cRecv);
void cleanMessage(char message[]);


int chattserverfunction(struct client* p)    /* Function definition */
{
    struct client* player=(struct client*) p;
    
    char TCPtext[MAX_LENGTH];
    int connectionCheck;

    printf("ClientID: %d    ClientThread:CHATT\n", player->ID);

    
    
    while(1){
        
            cleanMessage(TCPtext);
            connectionCheck = SDLNet_TCP_Recv(player->socket,TCPtext,MAX_LENGTH);
        
        if(!connectionCheck){
            
            SDLNet_TCP_Close(players[player->ID].socket);
            player->active = false;
            activeClients = SDLNet_TCP_DelSocket(socketSet, player[player->ID].socket);
            printf("ClientID: %d has disconnected\n", player->ID);
            break;
        
        }
            for(int i=0;i<activeClients;i++){
                
                if(players[i].active && player->socket != players[i].socket)
                    SDLNet_TCP_Send(players[i].socket, TCPtext, MAX_LENGTH);
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

