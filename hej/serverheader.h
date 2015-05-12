#ifndef SERVERHEADER_H_INCLUDED
#define SERVERHEADER_H_INCLUDED

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_net.h>

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
    Uint32 ip;
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

bool init();
bool initGame();
bool ClientsAreReady();
bool checkConnection(int id, char TCPrecv[]);
bool isInside(int x, int y, SDL_Rect* r);
void closeServer();
void acceptConnection();
void SendAndgetPort(int id);
void clearString(char message[]);
void Broadcast(char TCPsend[]);
void clearReturn(char message[]);
void sendMessageExc(char TCPsend[], int id);
void activePlayers();
void disconnect(int id);
void updateBullets();
void updateShips();
void addBullet(Ship* ship);
void createAndSendUDPPackets();
double getRadians(int a);
int Lobby (void * data);
int getClientId();
int IdFromPort(Uint16 port,Uint32 host);
int udpListener(void* data);
int newBullet();

TCPsocket TCPsock;
UDPsocket udpSendSock, udpRecvSock;
Client clients[MAX_CLIENTS];
Bullet bullets[MAX_BULLETS];
Ship ships[MAX_CLIENTS];
SDL_Surface* background;
bool gameIsActive;
int packetId;
Uint8 *gameData[940];
UDPpacket *packetOut;

#endif // SERVERHEADER_H_INCLUDED
