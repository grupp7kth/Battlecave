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

    //send client id
    SDLNet_TCP_Send(clients[clientId].socket, &clientId, sizeof(int));
    clearString(TCPsend);

    //create connection message
    sprintf(TCPsend,PREAMBLE_CONN"%s has connected",clients[clientId].name);
    clearReturn(TCPsend);
    sendMessageExc(TCPsend, clientId);
    clearString(TCPsend);
// ---------------------------------------------------------------------------------------------------------------------

    activePlayers(); // Sending information 

    clearString(TCPrecv);
    while (true) {
        if (checkConnection(clientId, TCPrecv) || TCPrecv[0] == '-') {

            if (strcmp(TCPrecv,PREAMBLE_READY)==0) {
                clients[clientId].ready = !clients[clientId].ready;
                clearString(TCPsend);
                sprintf(TCPsend, PREAMBLE_READY"%d", clientId);
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
            else {

                //printf("%s: %s", clients[clientId].name, TCPrecv);
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
            clearReturn(TCPsend);
            clients[clientId].active = false;
            clients[clientId].ready = false;
            SDLNet_TCP_Close(clients[clientId].socket);
            broadCast(TCPsend);
            clearString(TCPsend);
            activePlayers();
            disconnect(clientId);
            break;
        }
    }
    return 0;
}

void activePlayers() {

    char TCPsend[MAX_LENGTH];
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
        sprintf(TCPsend,PREAMBLE_READY"%d",clients[i].id);
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
        if(clients[i].active && i != id)
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
    printf("sent %x   recvsendport: %x   recvrecvport:%x \n", sPort, clients[id].sendPort,clients[id].recvPort);
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
        if(clients[i].active) SDLNet_TCP_Send(clients[i].socket, TCPsend, MAX_LENGTH);
    }
}
