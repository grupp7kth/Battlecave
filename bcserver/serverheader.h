#ifndef SERVERHEADER_H_INCLUDED
#define SERVERHEADER_H_INCLUDED

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#ifdef _WIN32
 #include <SDL.h>
 #include <SDL_image.h>
 #include <SDL_net.h>
#elif __APPLE__
 #include <SDL2/SDL.h>
 #include <SDL2_Image/SDL_Image.h>
 #include <SDL2_Net/SDL_Net.h>
#endif

#define STAGE_HEIGHT 1600
#define STAGE_WIDTH 2400
#define SHIP_HEIGHT 31
#define SHIP_WIDTH 23
#define MAX_BULLETS 1000
#define MAX_BULLETS_ON_SCREEN 300
#define SCREEN_HEIGHT 600
#define SCREEN_WIDTH 800
#define MAX_LENGTH 100
#define MAX_CLIENTS 8
#define PREAMBLE_CHAT "$"
#define PREAMBLE_CONN "$3"
#define PREAMBLE_DISC "$1"
#define PREAMBLE_PLAYERS "@"
#define PREAMBLE_READY "#"
#define PREAMBLE_DISCONNECT "-"
#define READY "1"
#define NOT_READY "0"
#define SHIP_TEXTURE "skepp.png"
#define BACKGROUND_TEXTURE "cave.png"


typedef struct _Client {
    TCPsocket socket;
    Uint16 sendPort,recvPort;
    char name[MAX_LENGTH];
    int score;
    int id;
    bool active;
    bool ready;
    Uint32 ipadress;
}Client;
typedef struct _Ship {
    SDL_Surface* surface;
    int bulletIntervall,bulletCooldown;
    double xPos,yPos,xVel,yVel;
    short angle,angleVel;
    bool acceleration,shooting, alive;
}Ship;

typedef struct _Bullet {
    double xPos,yPos,xVel,yVel;
    short type;
    bool active;
}Bullet;

extern bool init();
extern bool initGame();
extern void closeServer();
extern void acceptConnection();
extern int getClientId();
extern bool ClientsAreReady();
extern void SendAndgetPort(int id);
extern int Lobby (void * data);
extern bool checkConnection(int id, char TCPrecv[]);
extern void clearString(char message[]);
extern void Broadcast(char TCPsend[]);
extern void clearReturn(char message[]);
extern void sendMessageExc(char TCPsend[], int id);
extern void activePlayers();
extern void disconnect(int id);
extern double getRadians(int a);
extern int IdFromPort(Uint16 port);
extern bool isInside(int x, int y, SDL_Rect* r);
extern int udpListener(void* data);

extern UDPsocket udpSendSock, udpRecvSock;
extern Client clients[MAX_CLIENTS];
extern Bullet bullets[MAX_BULLETS];
extern Ship ships[MAX_CLIENTS];
extern SDL_Surface* background;

extern UDPsocket udpSendSock, udpRecvSock;
extern Client clients[MAX_CLIENTS];
extern Bullet bullets[MAX_BULLETS];
extern Ship ships[MAX_CLIENTS];
extern SDL_Surface* background;
//byt från globala!!
extern UDPpacket *packetOut;
extern Uint8 gmaeData[1000];
extern int packetID;
extern bool gameIsActive;

#endif // SERVERHEADER_H_INCLUDED
