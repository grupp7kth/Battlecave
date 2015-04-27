/***********************
******** SERVER ********
************************/

/** SERVER IP 193.10.39.101 **/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#define MAX_CLIENTS 8
#define MAX_LENGTH 100
#define PLAYER "player"

typedef struct _client
{
    bool connected;
    int id;
    TCPsocket socket;
    char name[MAX_LENGTH];
    int score;
}client;

client createClient(bool connected, int id, TCPsocket socket,char name[MAX_LENGTH],int score)
{
    client c;
    c.connected = connected;
    c.id = id;
    c.socket = socket;
    strcpy(c.name,name);
    c.score = score;

    return c;
}

/**< FUNCTIONS */
bool init();
bool checkConnection(int cRes);
static int chatt(void *data);
static int UDPfunction(void *data);
void serverQuit();
/**************/

/**< GLOBAL VARS */
UDPsocket UDPsock;
UDPpacket *packet;
client clients[MAX_CLIENTS];
SDL_Thread *UDPthread,*TCPThreads[MAX_CLIENTS];
IPaddress ip;
TCPsocket serverSocket;
/*****************/

int main(int argc, char *args[])
{
    int clientID = 0;

    if(!init())
    {
        exit(1);
    }

    printf("Waiting for connection...\n");
    while(true)
    {
        clients[clientID].socket=SDLNet_TCP_Accept(serverSocket);
        if(clients[clientID].socket)
        {
            clients[clientID].connected = true;
            clients[clientID].id = clientID;
            TCPThreads[clientID]=SDL_CreateThread(chatt,"TCP thread",&clientID);
            clientID++;
        }
    }
    serverQuit();
    return 0;
}
bool init()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDLNet_Init();

    SDLNet_ResolveHost(&ip, NULL, 3445);
    serverSocket = SDLNet_TCP_Open(&ip);
    UDPsock = SDLNet_UDP_Open(5443);
    packet = SDLNet_AllocPacket(1024);

    for(int i=0; i<MAX_CLIENTS; i++)
    {
        clients[i]=createClient(false,0,NULL,PLAYER,0);
    }
    UDPthread=SDL_CreateThread(UDPfunction,"UDP thread",NULL);
    return true;
}
void serverQuit()
{
    SDLNet_TCP_Close(serverSocket);
    SDLNet_FreePacket(packet);
    SDLNet_UDP_Close(UDPsock);
    SDLNet_Quit();
    SDL_Quit();
}
static int UDPfunction(void *data)
{
    while (true)
	{
		if (SDLNet_UDP_Recv(UDPsock, packet))
		{
			printf("UDP Packet incoming\n");
			printf("\tChan:    %d\n", packet->channel);
			printf("\tData:    %s\n", (char *)packet->data);
			printf("\tLen:     %d\n", packet->len);
			printf("\tMaxlen:  %d\n", packet->maxlen);
			printf("\tStatus:  %d\n", packet->status);
			printf("\tAddress: %x %x\n", packet->address.host, packet->address.port);
		}
	}
    return 0;
}
static int chatt(void *data)
{
    int clientId;
    char TCPrecv[MAX_LENGTH];
    char TCPsend[300];

    clientId = *((int*)data);
    SDLNet_TCP_Recv(clients[clientId].socket,TCPrecv,MAX_LENGTH);
    strcpy(clients[clientId].name,TCPrecv);
    printf("%s connected!\n",clients[clientId].name);
    while(true)
    {

        if(!checkConnection(SDLNet_TCP_Recv(clients[clientId].socket,TCPrecv,MAX_LENGTH)))
        {
            SDLNet_TCP_Close(clients[clientId].socket);
            clients[clientId].connected = false;
            break;
        }
        strcpy(TCPsend,clients[clientId].name);
        strcat(TCPsend,TCPrecv);
        for(int i=0; i<MAX_CLIENTS; i++)
        {
            if(clients[i].connected)
            {
                SDLNet_TCP_Send(clients[i].socket,TCPsend,MAX_LENGTH);
            }
        }
    }
    return 0;
}
bool checkConnection(int cRes)
{
    bool connection;
    int check = cRes;
    if(check <= 0)
    {
        connection = false;
    }
    else
    {
        connection = true;
    }
    return connection;
}
