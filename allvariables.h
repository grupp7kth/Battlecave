#ifndef ALVARIABLES_H_   /* Include guard */
#define ALVARIABLES_H_


typedef struct Client client;
client createClient(TCPsocket clientTCPsocket,char name[LENGTH],int score);
extern SDLNet_SocketSet socketSet;
extern UDPsocket UDPsocketIN;
extern UDPsocket UDPsocketOUT;
extern int activeClients;
extern client clients[MAX_CLIENTS];

#endif // ALLVARIABLES_H_
