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
int activeClients;
TCPsocket playerSocket[MAX_CLIENTS];
int clientList[1000];
