#include "serverheader.h"

int Lobby(void * data) {
    int clientId = *((int*)data);

    char TCPrecv[MAX_LENGTH];       clearString(TCPrecv);
    char TCPsend[MAX_LENGTH];       clearString(TCPsend);

// ---------------------------------------------------------------------------------------------------------------------
    //receive name and UDP port, send UDP port
    SDLNet_TCP_Recv(clients[clientId].socket,(clients[clientId].name),MAX_LENGTH);
    clearReturn(clients[clientId].name);
    printf("lobby thread created by: %s %d\n", clients[clientId].name,clients[clientId].id);
    SendAndgetPort(clientId);
    SDL_Delay(400);

    //send client id
    SDLNet_TCP_Send(clients[clientId].socket, &clientId, sizeof(int));
    clearString(TCPsend);
    SDL_Delay(400);

    //create connection message
    sprintf(TCPsend,PREAMBLE_CONN"%s has connected",clients[clientId].name);
    clearReturn(TCPsend);
    sendMessageExc(TCPsend, clientId);
    clearString(TCPsend);

    humanPlayerCount++;
// ---------------------------------------------------------------------------------------------------------------------

    activePlayers(); // Sending information

    clearString(TCPrecv);
    while (true) {
        if (checkConnection(clientId, TCPrecv) || TCPrecv[0] == '-') {

            if (TCPrecv[0] == PREAMBLE_READY) {
                clients[clientId].ready = !clients[clientId].ready;
                clearString(TCPsend);
                sprintf(TCPsend, "#%d", clientId);
                if (clients[clientId].ready) {
                    strcat(TCPsend, READY);
                }
                else {
                    strcat(TCPsend, NOT_READY);
                }
                broadCast(TCPsend);
                clearString(TCPsend);
                clearString(TCPrecv);
            }
            else if(TCPrecv[0] == PREAMBLE_TOGGLEBOT){
                int botID = TCPrecv[1], temporaryInt;

                temporaryInt = fetchCPUname();

                computerPlayerActive[botID] = !computerPlayerActive[botID];
                if(computerPlayerActive[botID]){
                    clearString(clients[botID].name);
                    computerPlayerCount++;
                    strcpy(clients[botID].name, "CPU ");

                    clients[botID].active = true;
                    clients[botID].id = botID;
                    clients[botID].playerType = PLAYER_TYPE_BOT;
                    clients[botID].ready = true;
                    clients[botID].name[4] = temporaryInt+48; // +48 because ASCII to int, +1 because we want 1-8 rather than 0-7
                }
                else
                    removeBOT(&botID);

                clearString(TCPsend);
                sprintf(TCPsend, "?%d", botID);
                broadCast(TCPsend);
                activePlayers();
            }
            else if(TCPrecv[0] == PREAMBLE_OPTIONS){
                if (TCPrecv[1]-48 == TOGGLE_BULLETINTERVAL) {
                    activeBulletInterval++;
                    if(activeBulletInterval>=3)
                        activeBulletInterval=0;
                    sprintf(TCPsend, "*1%d", activeBulletInterval);
                    broadCast(TCPsend);
                }
                else if (TCPrecv[1]-48 == TOGGLE_MAXSPEED) {
                    activeMaxSpeed++;
                    if(activeMaxSpeed>=5)
                        activeMaxSpeed=0;
                    sprintf(TCPsend, "*2%d", activeMaxSpeed);
                    broadCast(TCPsend);
                }
                else if (TCPrecv[1]-48 == TOGGLE_GAMELENGTH) {
                    activeGameLength++;
                    if(activeGameLength>=6)
                        activeGameLength=0;
                    sprintf(TCPsend, "*3%d", activeGameLength);
                    broadCast(TCPsend);
               }
            }
            else {
                clearString(TCPsend);
                sprintf(TCPsend, PREAMBLE_CHAT"%c%s: %s",clients[clientId].id+54,clients[clientId].name,TCPrecv); // +54 Because we have to send the ID as ASCII, and then add 6 to get to the player-index-colors in the client color table
                clearReturn(TCPsend);
                clearString(TCPrecv);
                broadCast(TCPsend);
            }
        }
        else{
            printf("ClientID: %d has disconnected...\n",clientId);
            clearString(TCPsend);
            sprintf(TCPsend,PREAMBLE_DISC"%s has disconnected",clients[clientId].name);
            clients[clientId].active = false;
            clients[clientId].ready = false;
            SDLNet_TCP_Close(clients[clientId].socket);
            broadCast(TCPsend);
            clearString(TCPsend);
            activePlayers();
            disconnect(clientId);
            humanPlayerCount--;
            if(humanPlayerCount == 0){              // Kick all bots if there are no human players
                for(int i=0; i < MAX_CLIENTS; i++){
                    if(clients[i].playerType == PLAYER_TYPE_BOT){
                        removeBOT(&i);
                    }
                }

            }

            break;
        }
    }
    return 0;
}

void activePlayers() {
    char TCPsend[MAX_LENGTH];
    clearString(TCPsend);
    sprintf(TCPsend, "*1%d", activeBulletInterval);
    broadCast(TCPsend);
    clearString(TCPsend);
    sprintf(TCPsend, "*2%d", activeMaxSpeed);
    broadCast(TCPsend);
    clearString(TCPsend);
    sprintf(TCPsend, "*3%d", activeGameLength);
    broadCast(TCPsend);
    clearString(TCPsend);
    for(int i=0; i<MAX_CLIENTS;i++)
    {
        if(clients[i].active)
        {
            sprintf(TCPsend,PREAMBLE_PLAYERS"%d%s",clients[i].id,clients[i].name);
            broadCast(TCPsend);
            puts(TCPsend);
            clearString(TCPsend);
        }
    }
    clearString(TCPsend);
    for(int i=0; i<MAX_CLIENTS;i++)
    {
        sprintf(TCPsend,"#%d",clients[i].id);
        if (clients[i].ready) {
            strcat(TCPsend, READY);
        }
        else{
            strcat(TCPsend, NOT_READY);
        }
        broadCast(TCPsend);
        clearString(TCPsend);
    }
}
void sendMessageExc(char TCPsend[], int id) {
    for (int i=0;i<MAX_CLIENTS;i++) {
        if(clients[i].active && i != id && clients[i].playerType == PLAYER_TYPE_HUMAN)
            SDLNet_TCP_Send(clients[i].socket, TCPsend, MAX_LENGTH);
    }
}

void SendAndgetPort(int id) {
    int sPort,rPort;
    sPort=SDLNet_UDP_GetPeerAddress(udpRecvSock,-1)->port;
    SDLNet_TCP_Send(clients[id].socket,&(sPort),sizeof(int));
    SDLNet_TCP_Recv(clients[id].socket,&(rPort),sizeof(int));
    clients[id].sendPort=rPort;

    SDLNet_TCP_Recv(clients[id].socket,&(rPort),sizeof(int));
    clients[id].recvPort=rPort;
}
void clearReturn(char message[]) {
    for (int i=0;i<MAX_LENGTH;i++) {
        if(message[i] == '\n') message[i] = '\0';
    }
}
void clearString(char message[]) {
    for (int i=0;i<MAX_LENGTH;i++) {
        message[i]='\0';
    }
}
bool checkConnection(int id,char TCPrecv[]) {
    if ((SDLNet_TCP_Recv(clients[id].socket,TCPrecv,MAX_LENGTH))<=0) return false;

    else if(strcmp(TCPrecv,PREAMBLE_DISCONNECT)==0){
        return false;
    }
    return true;
}
void disconnect(int id) {
    char TCPsend[MAX_LENGTH];
    clearString(TCPsend);

    sprintf(TCPsend,PREAMBLE_PLAYERS"%d",id);
    broadCast(TCPsend);
}
void broadCast(char TCPsend[]) {
    for (int i=0;i<MAX_CLIENTS;i++) {
        if(clients[i].active && clients[i].playerType == PLAYER_TYPE_HUMAN)
            SDLNet_TCP_Send(clients[i].socket, TCPsend, MAX_LENGTH);
    }
}

int fetchCPUname(void){
    bool match = false;
    char tempBotString[6];
    int i;

    for(i=0; i < 8; i++){
        match = false;
        tempBotString[0] = '\0';
        sprintf(tempBotString, "CPU %d", i+1);

        for(int j=0; j < MAX_CLIENTS; j++)
            if(strcmp(tempBotString, clients[j].name) == 0)
                match = true;

        if(match == 0)
            return i+1;

    }
    puts("Fetch CPU Name Error!");
    exit(EXIT_FAILURE);
}

void removeBOT(int *id){
    clearString(clients[*id].name);
    computerPlayerCount--;
    clients[*id].active = false;
    clients[*id].playerType = PLAYER_TYPE_HUMAN;
    clients[*id].ready = false;
    disconnect(*id);
    computerPlayerActive[*id] =  false;
    return;
}
