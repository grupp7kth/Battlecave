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
    printf("ChattThread created by ClientID: %d\n", player->ID);
    
    char TCPtextIN[MAX_LENGTH];
    char TCPtextOUT[MAX_LENGTH];
    char disconnected[MAX_LENGTH];
    clearString(TCPtextIN);
    clearString(TCPtextOUT);
    clearString(disconnected);
    int connectionCheck = 1, i;
    
    
    while(!strlen(TCPtextIN))                       // Lystnar på socketen till att något kommer in, i detta fall klientens nickname
        SDLNet_TCP_Recv(player->socket, TCPtextIN, MAX_LENGTH);
    
    for(i=0;i<MAX_LENGTH;i++)                      // Tar bort meddelandets enterslag
        { if(TCPtextIN[i] == '\n') TCPtextIN[i] = '\0'; }
    
    stringCopy(TCPtextIN, player->name);
    
    printf("%s has connected.\n", player->name);

    strcat(disconnected, player->name);             // Kommer att skickas till de andra klienterna vid en disconnect
    strcat(disconnected, " has disconnected...");
    
    clearString(TCPtextIN);
    while(true){                                    // Lystnar på socketen så tills att något kommer in
        while(!strlen(TCPtextIN) && connectionCheck)  connectionCheck = SDLNet_TCP_Recv(player->socket,TCPtextIN,MAX_LENGTH);
        
                            // Om TCP_Recv returnerar o, betyder det ätt klienten har kopplat ner och den infon skickas till de andra klienterna
        if(!connectionCheck){ printf("ClientID: %d has disconnected\n", player->ID); sendMessage(player->ID, disconnected); break; }
        
        if(strstr(TCPtextIN, "&&&")){ printf("ClientID: %d has disconnected\n", player->ID); sendMessage(player->ID, disconnected); break; }
        
        printf("%s: %s", player->name, TCPtextIN);
        TCPtextOUT[0] = '\0';
        strcat(TCPtextOUT, player->name);   // Kopplar ihop meddelandet med Namnet
        strcat(TCPtextOUT, ": ");           // genom att klippa och klistra lite
        strcat(TCPtextOUT, TCPtextIN);
        
        sendMessage(player->ID, TCPtextOUT);
        TCPtextIN[0] = '\0';
        
        /*for(int i=0;i<activeClients;i++){
            if(players[i].active && player->socket != players[i].socket)   // Skickar meddelandet ut till de andra klienterna och hoppar över sig själv
                SDLNet_TCP_Send(players[i].socket, TCPtextOUT, MAX_LENGTH);
        }*/
    }
    
    // Stänger socketen och återställer klient-structen
    SDLNet_TCP_Close(players[player->ID].socket);
    player->active = false;     // clientID't är nu ledigt och kan tas av en ny klient
    activeClients = SDLNet_TCP_DelSocket(socketSet, player[player->ID].socket);

    return 0;
}

void sendMessage(int ID,char TCPtextOUT[]){
    for(int i=0;i<activeClients;i++){
        if(players[i].active && players[ID].socket != players[i].socket)   // Skickar meddelandet ut till de andra klienterna och hoppar över sig själv
            SDLNet_TCP_Send(players[i].socket, TCPtextOUT, MAX_LENGTH);
    }
}

void clearString(char message[]){
    for(int i=0;i<MAX_LENGTH;i++){
        message[i]='\0';
    }
}
// Kopierar en str{ng till en annan str{ng. Fanns inte den h{r i standardbiblioteket?
void stringCopy(char* source, char* dest) {
    while ((*dest = *source) != '\0') {
        source++;
        dest++;
    }
}

