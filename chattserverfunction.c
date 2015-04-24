#include <SDL2/SDL.h>
#include <SDL2_net/SDL_net.h>
#include <stdio.h>
#include <stdbool.h>
#include "chattserverfunction.h"  /* Include the header (not strictly necessary here) */
#include "allvariables.h"

bool checkConnection(int cRecv);
void cleanMessage(char message[]);
SDL_ThreadFunction *chattserverfunction(int clientID)    /* Function definition */
{
    int ID = clientID;
    printf("ClientThread:CHATT, clientID: %d\n", clientID);

    
    char TCPtext[MAX_LENGTH];
    char endOfmessage[]= {"..."};

    while(1){
            cleanMessage(TCPtext);
            SDLNet_TCP_Recv(players[ID].socket,TCPtext,MAX_LENGTH);
        
        for(int i=0;i<activeClients;i++){
            if(i!=ID)SDLNet_TCP_Send(players[i].socket, TCPtext, MAX_LENGTH);
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

