#include "bcheaders.h"
#include "chattserverfunction.h"

int chattserverfunction(struct client* p)    /* Function definition */
{
    struct client* player=(struct client*) p;
    printf("ChattThread created by ClientID: %d\n", player->ID);

// ---------------------------------------------------------------------------------------------------------------------
    
    int connectionCheck = 1, i, udpIn, udpOut;
    char TCPtextIN[MAX_LENGTH];     clearString(TCPtextIN);
    char TCPtextOUT[MAX_LENGTH];    clearString(TCPtextOUT);
    char TCPidOUT[MAX_LENGTH];      clearString(TCPidOUT);
    char connected[MAX_LENGTH];     clearString(connected);
    char disconnected[MAX_LENGTH];  clearString(disconnected);
    char clientId[MAX_LENGTH];      clearString(clientId);      sprintf(clientId, "%d", player->ID);
    char playerReady[MAX_LENGTH];   clearString(playerReady);
    //char ready[MAX_LENGTH];         clearString(ready);         sprintf(ready, "%d", READY);
    //char nready[MAX_LENGTH];        clearString(nready);        sprintf(nready, "%d", NOT_READY);
    
// ---------------------------------------------------------------------------------------------------------------------
    
    //strcat(TCPidOUT, PREAMBLE_ID);                                                      // Skapar ett meddelande med playerns ID
    strcat(TCPidOUT, clientId + '0');
    for(i=0;i<MAX_LENGTH;i++){ if(TCPidOUT[i] == '\n') TCPidOUT[i] = '\0'; }
    
// ---------------------------------------------------------------------------------------
    
    while(!strlen(TCPtextIN)) SDLNet_TCP_Recv(player->socket, TCPtextIN, MAX_LENGTH);   // Tar emot klientens nickname
    for(i=0;i<MAX_LENGTH;i++){ if(TCPtextIN[i] == '\n') TCPtextIN[i] = '\0'; }          // Tar bort incommande texts eventuella enterslag
    stringCopy(TCPtextIN, player->name);                                                // Skriver in valt nickname i plyerns struct
    clearString(TCPtextIN);
    printf("%s has connected.\n", player->name);
    
    udpOut = SDLNet_UDP_GetPeerAddress(udpRecvSock,-1)->port;
    printf("Skickar att serverns UDP-ta-emot {r %x\n",udpOut);
    SDLNet_TCP_Send(player->socket,&(udpOut),sizeof(int));
    
    SDLNet_TCP_Recv(player->socket,&(udpIn),sizeof(int));                               // V{nta på inkommande UDP-portnummers.
    player->udpRecvPort = udpIn;
    printf("%s tar emot UDP p} port %x\n",player->name,player->udpRecvPort);
    clearString(TCPtextIN);
    
    SDLNet_TCP_Send(player->socket, TCPidOUT, MAX_LENGTH);                              // Skickar playerID till clienten

    /*
    SDLNet_TCP_Recv(player->socket,TCPtextIN,MAX_LENGTH);
    player->udpSendPort = atoi(TCPtextIN);
    printf("%s kommer att skicka ifr}n port %d\n",player->name,player->udpSendPort);
    clearString(TCPtextIN);
    */
    
    
    
    
// ---------------------------------------------------------------------------------------------------------------------
// ---- Skapar diverse meddelanden som ska skickas i olika scenarion
// ---------------------------------------------------------------------------------------------------------------------
    
    strcat(connected, PREAMBLE_CHAT);
    strcat(connected, player->name);
    strcat(connected, " has connected :D");   // Skapar meddelande om att Jag har kopplat upp till servern
    for(i=0;i<MAX_LENGTH;i++){ if(connected[i] == '\n') connected[i] = '\0'; }
    
    strcat(disconnected, PREAMBLE_CHAT);
    strcat(disconnected, player->name);
    strcat(disconnected, " has disconnected f:("); // Skapar meddelande om att Jag har kopplat ner från servern
    for(i=0;i<MAX_LENGTH;i++){ if(disconnected[i] == '\n') disconnected[i] = '\0'; }
    
// ---------------------------------------------------------------------------------------------------------------------
    
    sendMessageExc(connected, player->ID);          // Skickar meddelande om Jag har kopplat upp till servern
    activePlayers(playerReady);                                // Skickar meddelande om samtliga spelare som är uppkopplade till servern
    
// ---------------------------------------------------------------------------------------------------------------------
// ---- Här startar komunikationen mellan användarna
// ---------------------------------------------------------------------------------------------------------------------
    clearString(TCPtextIN);
    while(true){
        // Lystnar på socketen så tills att något kommer in
        while(!strlen(TCPtextIN) && connectionCheck)
            connectionCheck = SDLNet_TCP_Recv(player->socket,TCPtextIN,MAX_LENGTH);
        
        // Meddelanden som startar med # innebär att clienten är redo att köra
        if(TCPtextIN[0] == PREAMBLE_READY){
            player->ready = !player->ready;
            
            strcat(playerReady, "#");
            strcat(playerReady, clientId);
            if(player->ready){
                strcat(playerReady, READY);
            }
            else{
                strcat(playerReady, NOT_READY);
            }
            sendMessageAll(playerReady);
            clearString(playerReady);
            clearString(TCPtextIN);
        }
        
        // OM connnectioCheck är lika med noll ElLer om meddelandet som kommer in startar med '-'
        // så innebär det att spelaren inte längre är uppkopplad
        else if(!connectionCheck || TCPtextIN[0] == '-'){printf("ClientID: %d has disconnected...\n",player->ID);
            player->active = false;
            player->ready = false;
            sendMessageAll(disconnected);
            activePlayers(playerReady);
            disconnect(player->ID);
            break;}
        
        else {
            
            printf("%s: %s", player->name, TCPtextIN);
            clearString(TCPtextOUT);
            strcat(TCPtextOUT, PREAMBLE_CHAT);
            strcat(TCPtextOUT, player->name);   // Kopplar ihop meddelandet med Namnet
            strcat(TCPtextOUT, ": ");           // genom att klippa och klistra lite
            strcat(TCPtextOUT, TCPtextIN);
            for(i=0;i<MAX_LENGTH;i++){ if(TCPtextOUT[i] == '\n') TCPtextOUT[i] = '\0'; }
        
            clearString(TCPtextIN);
            sendMessageAll(TCPtextOUT);
        }
        printf("Player %d ready: %d \n",player->ID, player->ready);
    }
    
    SDLNet_TCP_Close(players[player->ID].socket);   // Stänger socketen och återställer klient-structen
    player->active = false;                         // clientID't är nu ledigt och kan tas av en ny klient
    player->ready = false;
    activeClients = SDLNet_TCP_DelSocket(socketSet, player[player->ID].socket);

    return 0;
}

// ---------------------------------------------------------------------------------------------------------------------
// ---- Funtionerna
// ---------------------------------------------------------------------------------------------------------------------

void sendMessageAll(char TCPtextOUT[])
{
    for(int i=0;i<MAX_CLIENTS;i++){
        if(players[i].active)   // Skickar meddelandet ut till de andra klienterna och hoppar över sig själv
            SDLNet_TCP_Send(players[i].socket, TCPtextOUT, MAX_LENGTH);
    }
}
void sendMessageExc(char TCPtextOUT[], int ID)
{
    for(int i=0;i<MAX_CLIENTS;i++){
        if(players[i].active && i != ID)   // Skickar meddelandet ut till de andra klienterna och hoppar över sig själv
            SDLNet_TCP_Send(players[i].socket, TCPtextOUT, MAX_LENGTH);}

}
    
void activePlayers(char playerReady[])
{

    char TCPtextOUT[MAX_LENGTH];
    char charid[MAX_LENGTH];
    //char ready[MAX_LENGTH];      clearString(ready);     sprintf(ready, "%d", READY);
    //char nready[MAX_LENGTH];     clearString(nready);    sprintf(nready, "%d", NOT_READY);
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
    clearString(TCPtextOUT);
    for(int i=0; i<MAX_CLIENTS;i++)
    {
        strcat(TCPtextOUT, "#");
        sprintf(charid, "%d", players[i].ID);
        strcat(TCPtextOUT, charid);
        if (players[i].ready) {
            strcat(TCPtextOUT, READY);
        }
        else{
            strcat(TCPtextOUT, NOT_READY);
        }
        sendMessageAll(TCPtextOUT);
        clearString(TCPtextOUT);
    }
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

