#include <stdio.h>
#include <stdbool.h>

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
#define MESSAGE_MAX_LENGTH 100
#define MAX_CLIENTS 8

// Saker som ligger i genericfunctions.c
double getRadians(int a);
void itoa(int n, char s[],int bas);
void stringCopy(char* source, char* dest);
void IPtoAscii(int ipaddress, char* URL);


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

typedef struct client {
	Uint32 ipadress;
	TCPsocket tcpSocket;
	Uint16 udpSendPort,udpRecvPort;
	char namn[16];
	int poang;
	short ID;
	bool aktiv, klar;
}client;
