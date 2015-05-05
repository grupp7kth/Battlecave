#include "bcheaders.h"


struct client createClient(TCPsocket clientTCPsocket,int clientID)
{
    struct client c;
    
    c.socket=clientTCPsocket;
    c.score=0;
    c.ID = clientID;
    c.active = false;
    c.ready = false;
   
    return c;
}
struct client players[MAX_CLIENTS];
SDLNet_SocketSet socketSet;
UDPsocket UDPsocketIN;
UDPsocket UDPsocketOUT;
UDPsocket udpSendSock,udpRecvSock;
UDPpacket *skraddarsyttPaket;
int activeClients;
TCPsocket playerSocket[MAX_CLIENTS];
int clientList[1000];
bool speletPagar;
bullet serverSkott[MAX_BULLETS];
serverShip serverSkepp[MAX_CLIENTS];
client klienter[MAX_CLIENTS];
SDL_Surface* serverBakgrund;
Uint8 dataToClient[1000];
int paketnummer;