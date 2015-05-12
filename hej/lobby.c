#include "serverheader.h"

int Lobby(void * data) {
    int clientId = *((int*)data);

    char TCPrecv[MAX_LENGTH];       clearString(TCPrecv);
    char TCPsend[MAX_LENGTH];       clearString(TCPsend);
    //receive name and UDP port, send UDP port
    SDLNet_TCP_Recv(clients[clientId].socket,(clients[clientId].name),MAX_LENGTH);
    printf("lobby thread created by: %s %d\n", clients[clientId].name,clients[clientId].id);
    SendAndgetPort(clientId);

    //send client id
    SDLNet_TCP_Send(clients[clientId].socket, &clientId, sizeof(int));
    clearString(TCPsend);

    //send connection message
    sprintf(TCPsend,PREAMBLE_CONN"%s has connected",clients[clientId].name);
    sendMessageExc(TCPsend, clientId);

    activePlayers();

    while (true) {
        clearString(TCPrecv);
        clearString(TCPsend);
        if (checkConnection(clientId, TCPrecv)) {

            if (strcmp(TCPrecv,PREAMBLE_READY)==0) {
                clients[clientId].ready = !clients[clientId].ready;
                sprintf(TCPsend, PREAMBLE_READY"%d", clientId);
                if (clients[clientId].ready) {
                    strcat(TCPsend, READY);
                }
                else {
                    strcat(TCPsend, NOT_READY);
                }
                Broadcast(TCPsend);
            }
            else {

                printf("%s: %s", clients[clientId].name, TCPrecv);
                sprintf(TCPsend, PREAMBLE_CHAT"%d%s: %s",(clients[clientId].id+6),clients[clientId].name,TCPrecv);
                Broadcast(TCPsend);
            }

        }
        else {
            printf("ClientID: %d has disconnected...\n",clientId);
            sprintf(TCPsend,PREAMBLE_DISC"%s has disconnected",clients[clientId].name);
            clients[clientId].active = false;
            clients[clientId].ready = false;
            Broadcast(TCPsend);
            activePlayers();
            disconnect(clientId);
            SDLNet_TCP_Close(clients[clientId].socket);
            break;
        }
    }

    return 0;
}
void activePlayers() {

    char TCPsend[MAX_LENGTH];       clearString(TCPsend);

    for(int i=0; i<MAX_CLIENTS;i++)
    {
        if(clients[i].active)
        {
            sprintf(TCPsend,PREAMBLE_PLAYERS"%d%s",clients[i].id,clients[i].name);
            Broadcast(TCPsend);
            puts(TCPsend);
        }
    }
    for(int i=0; i<MAX_CLIENTS;i++)
    {
        sprintf(TCPsend,PREAMBLE_READY"%d",clients[i].id);
        if (clients[i].ready) {
            strcat(TCPsend, READY);
        }
        else{
            strcat(TCPsend, NOT_READY);
        }
        Broadcast(TCPsend);
    }
}
void sendMessageExc(char TCPsend[], int id) {
    for (int i=0;i<MAX_CLIENTS;i++) {
        if(clients[i].active && i != id)
            SDLNet_TCP_Send(clients[i].socket, TCPsend, MAX_LENGTH);
    }
}
void Broadcast(char TCPsend[]) {
    for (int i=0;i<MAX_CLIENTS;i++) {
        if(clients[i].active) SDLNet_TCP_Send(clients[i].socket, TCPsend, MAX_LENGTH);
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
    printf("sent %x   sendport: %x   recvport:%x \n", sPort, clients[id].sendPort,clients[id].recvPort);
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
    else if(strcmp(TCPrecv,PREAMBLE_DISCONNECT)==0) return false;
    return true;
}
void disconnect(int id) {
    char TCPsend[MAX_LENGTH];   clearString(TCPsend);
    sprintf(TCPsend,PREAMBLE_PLAYERS"%d",id);
    Broadcast(TCPsend);
}
