kk/***********************
******** CLIENT ********
************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SERVER_IP "193.10.39.101"
#define TCP_PORT 3445
#define UDP_PORT 5443
#define LOCAL_IP "192.168.56.1"
#define MAX_LENGTH 100

SDL_Thread* tRecv;
IPaddress ip,udpip;
TCPsocket client;
UDPsocket UDPsock;
UDPpacket *packet;

static int recvThread(void* arg);

int main( int argc, char* args[] )
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDLNet_Init();
    char text[MAX_LENGTH];

    SDLNet_ResolveHost(&ip, LOCAL_IP, TCP_PORT);
    SDLNet_ResolveHost(&udpip, LOCAL_IP, UDP_PORT);
    client=SDLNet_TCP_Open(&ip);
    UDPsock = SDLNet_UDP_Open(0);
    packet = SDLNet_AllocPacket(1024);
    //ip = SDLNet_UDP_GetPeerAddress(UDPsock, -1);
    //SDLNet_UDP_Bind(UDPsock, -1, &ip);

    //UDP
    while(1)
    {
        printf("> ");
        scanf("%s", (char *)packet->data);

        packet->address.host = udpip.host;	/* Set the destination host */
        packet->address.port = udpip.port;	/* And destination port */
        packet->len = strlen((char *)packet->data) + 1;
        SDLNet_UDP_Send(UDPsock, -1, packet); /* This sets the p->channel */
    }
    //TCP
    if(client)
    {
        printf("Enter name:");
        fgets(text,MAX_LENGTH,stdin);
        SDLNet_TCP_Send(client,text,MAX_LENGTH);
        printf("connected!\n");
        tRecv = SDL_CreateThread(recvThread,"recv Thread",NULL);
        while(1)
        {
            fgets(text,MAX_LENGTH,stdin);
            SDLNet_TCP_Send(client,text,MAX_LENGTH);
        }
    }

    SDLNet_TCP_Close(client);
    SDLNet_FreePacket(packet);
    SDLNet_UDP_Close(UDPsock);
    SDLNet_Quit();
    SDL_Quit();
    return 0;
}
static int recvThread(void* arg)
{
    char ans[MAX_LENGTH];
    while(1)
    {
        if(SDLNet_TCP_Recv(client,ans,MAX_LENGTH)>0)
        {
            printf("%s",ans);
        }
    }
    return 0;
}
