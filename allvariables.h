#ifndef ALVARIABLES_H_   /* Include guard */
#define ALVARIABLES_H_
#define LENGTH 100
#define MAX_CLIENTS 8
struct client{
    TCPsocket TCPsocket;
    char name[LENGTH];
    int score;
};
struct client createClient(TCPsocket clientTCPsocket,char name[LENGTH],int score);
extern SDLNet_SocketSet socketSet;
extern UDPsocket UDPsocketIN;
extern UDPsocket UDPsocketOUT;
extern int activeClients;
struct client clients[8];
extern char name[LENGTH];
#endif // ALLVARIABLES_H_
