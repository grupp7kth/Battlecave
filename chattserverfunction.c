#include "bcheaders.h"
#include "chattserverfunction.h"
/* Include the header (not strictly necessary here) */

int chattserverfunction(struct client* p)    /* Function definition */
{
    struct client* player=(struct client*) p;
    printf("ChattThread created by ClientID: %d\n", player->ID);

// ---------------------------------------------------------------------------------------------------------------------
    
    char TCPtextIN[MAX_LENGTH];     clearString(TCPtextIN);
    char TCPtextOUT[MAX_LENGTH];    clearString(TCPtextOUT);
    char TCPidOUT[MAX_LENGTH];      clearString(TCPidOUT);
    char connected[MAX_LENGTH];     clearString(connected);
    char disconnected[MAX_LENGTH];  clearString(disconnected);
    char clientId[MAX_LENGTH];      clearString(clientId);
    
    int connectionCheck = 1, i;
// ---------------------------------------------------------------------------------------------------------------------
    
    sprintf(clientId, "%d", player->ID);
    strcat(TCPidOUT, "%");                                                              // Skapar ett meddelande med playerns ID
    strcat(TCPidOUT, clientId);
    for(i=0;i<MAX_LENGTH;i++){ if(TCPidOUT[i] == '\n') TCPidOUT[i] = '\0'; }
// ---------------------------------------------------------------------------------------
    
    while(!strlen(TCPtextIN)) SDLNet_TCP_Recv(player->socket, TCPtextIN, MAX_LENGTH);   // Tar emot klientens nickname
    
    for(i=0;i<MAX_LENGTH;i++){ if(TCPtextIN[i] == '\n') TCPtextIN[i] = '\0'; }          // Tar bort incommande texts enterslag
    
    SDLNet_TCP_Send(player->socket, TCPidOUT, MAX_LENGTH);                              // Skickar playerID till clienten
    
    stringCopy(TCPtextIN, player->name);                                                // Skriver in valt nickname i plyerns struct
    
    printf("%s has connected.\n", player->name);

// ---------------------------------------------------------------------------------------------------------------------
// ---- Skapar diverse meddelanden som ska skickas i olika scenarion
// ---------------------------------------------------------------------------------------------------------------------
    
    strcat(connected, "$");
    strcat(connected, player->name);
    strcat(connected, " has connected!!! :D \n");   // Skapar meddelande om att Jag har kopplat upp till servern
    for(i=0;i<MAX_LENGTH;i++){ if(connected[i] == '\n') connected[i] = '\0'; }
    
    strcat(disconnected, "$");
    strcat(disconnected, player->name);
    strcat(disconnected, " has disconnected...\n"); // Skapar meddelande om att Jag har kopplat ner från servern
    for(i=0;i<MAX_LENGTH;i++){ if(disconnected[i] == '\n') disconnected[i] = '\0'; }
    
// ---------------------------------------------------------------------------------------------------------------------
    
    
    sendMessageExc(connected, player->ID);          // Skickar meddelande om Jag har kopplat upp till servern
    activePlayers();                                // Skickar meddelande om samtliga spelare som är uppkopplade till servern
    
    clearString(TCPtextIN);

// ---------------------------------------------------------------------------------------------------------------------
// ---- Här startar komunikationen mellan användarna
// ---------------------------------------------------------------------------------------------------------------------
    while(true){
        // Lystnar på socketen så tills att något kommer in
        while(!strlen(TCPtextIN) && connectionCheck)
            connectionCheck = SDLNet_TCP_Recv(player->socket,TCPtextIN,MAX_LENGTH);
        
        // Meddelanden som startar med # innebär att clienten är redo att köra
        if(TCPtextIN[0] == '#'){
            player->ready = !player->ready;
            clearString(TCPtextIN);
        }
        
        // OM connnectioCheck är lika med noll ElLer om meddelandet som kommer in startar med '-'
        // så innebär det att spelaren inte längre är uppkopplad
        else if(!connectionCheck || TCPtextIN[0] == '-'){printf("ClientID: %d has disconnected...\n", player->ID);player->active = false;  //
                                sendMessageAll(disconnected);   disconnect(player->ID);break;}
        else {
            
            printf("%s: %s", player->name, TCPtextIN);
            clearString(TCPtextOUT);
            strcat(TCPtextOUT, "$");
            strcat(TCPtextOUT, player->name);   // Kopplar ihop meddelandet med Namnet
            strcat(TCPtextOUT, ": ");           // genom att klippa och klistra lite
            strcat(TCPtextOUT, TCPtextIN);
            for(i=0;i<MAX_LENGTH;i++){ if(TCPtextOUT[i] == '\n') TCPtextOUT[i] = '\0'; }
        
            clearString(TCPtextIN);
            sendMessageAll(TCPtextOUT);
        }
        printf("Player %d ready: %d \n",player->ID, player->ready);
    }
    
    // Stänger socketen och återställer klient-structen
    SDLNet_TCP_Close(players[player->ID].socket);
    player->active = false;     // clientID't är nu ledigt och kan tas av en ny klient
    player->ready = false;
    activeClients = SDLNet_TCP_DelSocket(socketSet, player[player->ID].socket);

    return 0;
}

void sendMessageAll(char TCPtextOUT[])
{
    for(int i=0;i<activeClients;i++){
        if(players[i].active)   // Skickar meddelandet ut till de andra klienterna och hoppar över sig själv
            SDLNet_TCP_Send(players[i].socket, TCPtextOUT, MAX_LENGTH);
    }
}
void sendMessageExc(char TCPtextOUT[], int ID)
{
    for(int i=0;i<activeClients;i++){
        if(players[i].active && i != ID)   // Skickar meddelandet ut till de andra klienterna och hoppar över sig själv
            SDLNet_TCP_Send(players[i].socket, TCPtextOUT, MAX_LENGTH);}

}
    
void activePlayers()
{
    char TCPtextOUT[MAX_LENGTH];
    char charid[MAX_LENGTH];
    clearString(TCPtextOUT);
    for(int i=0; i<MAX_CLIENTS;i++)
    {
        if(players[i].active)
        {
            strcat(TCPtextOUT, "@");
            sprintf(charid, "%d", players[i].ID);
            strcat(TCPtextOUT, charid);
            strcat(TCPtextOUT, players[i].name);
            sendMessageAll(TCPtextOUT);
            puts(TCPtextOUT);
            clearString(TCPtextOUT);
        }
    }
    printf("Nu har jag skickat alla namn\n");
}
void disconnect(int ID)
{
    char TCPtextOUT[MAX_LENGTH];
    clearString(TCPtextOUT);
    char charid[MAX_LENGTH];
    sprintf(charid, "%d", ID);
    
    strcat(TCPtextOUT, "@");
    strcat(TCPtextOUT, charid);
    sendMessageAll(TCPtextOUT);
    puts(TCPtextOUT);
    

}
void clearString(char message[]){
    for(int i=0;i<MAX_LENGTH;i++){
        message[i]='\0';
    }
}

// Kopierar en str{ng till en annan str{ng. Fanns inte den h{r i standardbiblioteket?
void stringCopy(char* source, char* dest)
{
    while ((*dest = *source) != '\0') {
        source++;
        dest++;
    }
}

