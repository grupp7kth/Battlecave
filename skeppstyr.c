#include <SDL2/SDL.h>
#include <SDL2_Image/SDL_Image.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define SCREENHEIGHT 600
#define SCREENWIDTH 800

SDL_Window* ram;
SDL_Renderer* raster;
SDL_Texture* rittextur;
SDL_Rect skepprekt;
SDL_RendererFlip flipp;


double getRadians(int a) {
	return (a+90)*3.14159265/180;
}

struct ship {
	double x,y,xvel,yvel;
	short angle, angleVel;
	bool acc;
};
struct ship skepp;

void accelerateShip(struct ship* sk) {
	sk->yvel-=sin(getRadians(sk->angle))*0.1;
	sk->xvel-=cos(getRadians(sk->angle))*0.1;
}

void updateShip(struct ship *skepp) {
	if (skepp->acc) accelerateShip(skepp);
	skepp->x += skepp->xvel;
	skepp->y += skepp->yvel;
	skepp->angle += skepp->angleVel;
	if (skepp->x > SCREENWIDTH+20) skepp->x = -20;
	if (skepp->x < -20) skepp->x = SCREENWIDTH+20;
	if (skepp->y > SCREENHEIGHT+20) skepp->y = -20;
	if (skepp->y < -20) skepp->y = SCREENHEIGHT+20;
	
	skepprekt.x = (int)(skepp->x);
	skepprekt.y = (int)(skepp->y);
}

void init() {
	ram = SDL_CreateWindow("titel",100,100,SCREENWIDTH,SCREENHEIGHT,0);
	raster = SDL_CreateRenderer(ram,-1,SDL_RENDERER_ACCELERATED);
	SDL_Surface* skeppbild = IMG_Load("skepp.png");
	rittextur = SDL_CreateTextureFromSurface(raster,skeppbild);
	skepp.x = 400;
	skepp.y = 400;
	skepp.xvel = 0;
	skepp.yvel = 0;
	skepp.angle = 0;
	skepprekt.x=0;
	skepprekt.y=0;
	skepprekt.w=23;
	skepprekt.h=31;
	updateShip(&skepp);
	flipp=SDL_FLIP_NONE;
}

int main(void) {
	skepp.x = 400;
	SDL_Init(SDL_INIT_VIDEO);
	init();
	SDL_Event e;
	bool quit = false;
	while (!quit) {
		while (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) quit = true;
			if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.scancode) {
				case SDL_SCANCODE_Z:
					skepp.angleVel=-2;
					break;
				case SDL_SCANCODE_X:
					skepp.angleVel=2;
					break;
				case 56:
					skepp.acc=true;
					break;
				default:
					break;
				}
			}
			if (e.type == SDL_KEYUP) {
				switch (e.key.keysym.scancode) {
				case SDL_SCANCODE_Z:
					skepp.angleVel=0;
					break;
				case SDL_SCANCODE_X:
					skepp.angleVel=0;
					break;
				case 56:
					skepp.acc=false;
					break;
				default:
					break;
				}
			}
		}
		updateShip(&skepp);
		SDL_SetRenderDrawColor(raster,0,0,0,255);
		SDL_RenderClear(raster);
		SDL_RenderCopyEx(raster,rittextur,NULL,&skepprekt,skepp.angle,NULL,flipp);
		SDL_RenderPresent(raster);
		SDL_Delay(20);
	}
}



	
	
	
