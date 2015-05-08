#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#ifdef _WIN32
 #include <SDL2/SDL.h>
 #include <SDL2/SDL_image.h>
 #include <SDL2/SDL_net.h>
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

typedef struct clientShip {
	SDL_Texture* grafik;
	SDL_Rect drawPosition;
	int width,height,xPos,yPos;
	short angle;
	bool active, blown;
}clientShip;

typedef struct clientBullet {
	int xPos, yPos;
	short type;
}clientBullet;

typedef struct background {
	SDL_Texture* grafik;
	int width,height;
	SDL_Rect ritrekt;
}background;
