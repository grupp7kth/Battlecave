#ifndef ALLVARIABLES_H_   /* Include guard */
#define ALLVARIABLES_H_
#define MAX_LENGTH 100
#define MAX_CLIENTS 8

struct client{
    TCPsocket socket;
    char name[MAX_LENGTH];
    int score;
    int ID;
};
struct client createClient(TCPsocket clientTCPsocket,char name[MAX_LENGTH],int score);
extern SDLNet_SocketSet socketSet;
extern UDPsocket UDPsocketIN;
extern UDPsocket UDPsocketOUT;
extern int activeClients;
extern char name[MAX_LENGTH];



#endif // ALLVARIABLES_H_
