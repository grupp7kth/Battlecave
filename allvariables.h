#include "bcheaders.h"
#ifndef ALLVARIABLES_H_   /* Include guard */
#define ALLVARIABLES_H_
#define MAX_LENGTH 100
#define MAX_CLIENTS 8
#define MAX_BULLETS 1000
#define STAGE_HEIGHT 1600
#define STAGE_WIDTH 2400
#define SHIP_HEIGHT 31
#define SHIP_WIDTH 23
#define SCREEN_HEIGHT 600
#define SCREEN_WIDTH 800


typedef struct client{
    TCPsocket socket;
    Uint16 udpSendPort,udpRecvPort;
    char name[MAX_LENGTH];
    int score;
    int ID;
    bool active;
    bool ready;
    Uint32 ipadress;
}client;
typedef struct serverShip {
    SDL_Surface* bild;
    int skottIntervall,skottKylning;
    double xPos,yPos,xVel,yVel;
    short vinkel,vinkelHast;
    bool accar,skjuter, blammad;
}serverShip;

typedef struct bullet {
    double xPos,yPos,xVel,yVel;
    short typ;
    bool on;
}bullet;

extern struct client players[MAX_CLIENTS];
struct client createClient(TCPsocket clientTCPsocket,int clientID);
extern SDLNet_SocketSet socketSet;
extern UDPsocket UDPsocketIN;
extern UDPsocket UDPsocketOUT;
extern int activeClients;
extern char name[MAX_LENGTH];
extern TCPsocket playerSocket[MAX_CLIENTS];
extern int clientList[1000];
extern char clientListId[MAX_CLIENTS];


extern bullet serverSkott[MAX_BULLETS];
extern serverShip serverSkepp[MAX_CLIENTS];
extern client klienter[MAX_CLIENTS];
extern SDL_Surface* serverBakgrund;

extern UDPsocket udpSendSock,udpRecvSock;
extern UDPpacket *skraddarsyttPaket;
extern Uint8 dataToClient[1000];
extern int paketnummer;
extern bool speletPagar;





#endif // ALLVARIABLES_H_
