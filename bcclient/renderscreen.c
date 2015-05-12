#include "includes.h"

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Texture* mBackground = NULL;
SDL_Texture* mBlackOverlay = NULL;
SDL_Texture* mIPPortWindow = NULL;
SDL_Texture* mLobbyWindow = NULL;
SDL_Texture* mReady = NULL;
SDL_Texture* mNameWindow = NULL;

SDL_Texture* loadTexture(char* filname);

//**************************************************************************
//*                                FUNCTIONS                               *
//**************************************************************************

void renderScreen(int *mode, int *select, SDL_Rect buttonPlacement[], SDL_Rect windowPlacement[]){
    SDL_RenderClear(gRenderer);

    setButtons(buttonPlacement, mode);

//********************** MODE 0 : STARTUP SCREEN ***************************
    if(*mode == STARTUP){
        SDL_RenderCopy(gRenderer, mBackground, NULL, NULL);
        setText(mode, gRenderer, select);
    }

//********************** MODE 1 : FIND SERVERS SCREEN **********************
    else if(*mode == FIND_SERVERS){
        SDL_RenderCopy(gRenderer, mBackground, NULL, NULL);
        setText(mode, gRenderer, select);
    }
//********************** MODE 3 : LOBBY SCREEN ****************************
    else if(*mode == LOBBY){
        SDL_RenderCopy(gRenderer, mBackground, NULL, NULL);
        SDL_RenderCopy(gRenderer, mLobbyWindow, NULL, &windowPlacement[1]);

        setText(mode, gRenderer, select);

        for(int i=0; i < MAX_PLAYERS; i++){     // Ready icons
            if(playerReady[i]){
                readyIcon.x = 497;
                readyIcon.y = 116 + (i * 64);
                readyIcon.h = 40;
                readyIcon.w = 40;
                SDL_RenderCopy(gRenderer, mReady, NULL, &readyIcon);
            }
        }
    }
//********************** MODE 4 : JOIN DEFAULT SERVER *********************
    else if(*mode == JOIN_DEFAULT){
        SDL_RenderCopy(gRenderer, mBackground, NULL, NULL);
        SDL_RenderCopy(gRenderer, mBlackOverlay, NULL, NULL);
        SDL_RenderCopy(gRenderer, mNameWindow, NULL, &windowPlacement[2]);

        setText(mode, gRenderer, select);
    }
//********************** MODE 5 : JOIN CUSTOM SERVER **********************
    else if(*mode == JOIN_CUSTOM){
        SDL_RenderCopy(gRenderer, mBackground, NULL, NULL);
        SDL_RenderCopy(gRenderer, mBlackOverlay, NULL, NULL);
        SDL_RenderCopy(gRenderer, mIPPortWindow, NULL, &windowPlacement[0]);

        setText(mode, gRenderer, select);
    }
//*********************** MODE 6 : In Game ********** **********************
    else if(*mode == IN_GAME){
        SDL_RenderCopy(gRenderer, gameBackground.texture, &gameBackground.source, NULL);

        for (int i=0; i < 8; i++){
            if (i == client.id)
                continue;
            ship[i].placement.x = ship[i].x - gameBackground.source.x - ship[i].w/2;
            ship[i].placement.y = ship[i].y - gameBackground.source.y - ship[i].h/2;
            SDL_RenderCopyEx(gRenderer ,ship[i].texture ,NULL,&ship[i].placement, ship[i].angle, NULL, SDL_FLIP_NONE);
        }

        setText(mode, gRenderer, select);
    }

//***************************************************************************

    SDL_RenderPresent(gRenderer);
    return;
}


void loadMedia(void){
    mBackground = loadTexture("resources/images/startupbackground.png");

    mBlackOverlay = loadTexture("resources/images/blackoverlay.png");
    SDL_SetTextureAlphaMod(mBlackOverlay, 100);
    SDL_SetTextureBlendMode(mBlackOverlay, SDL_BLENDMODE_BLEND);

    mIPPortWindow = loadTexture("resources/images/ipportwindow.png");
    mNameWindow = loadTexture("resources/images/namewindow.png");
    mLobbyWindow = loadTexture("resources/images/lobbybackground.png");
    mReady = loadTexture("resources/images/ready.png");

    gameBackground.texture = loadTexture("resources/images/cave.png");
    gameBackground.source.w = SCREENWIDTH;
    gameBackground.source.h = SCREENHEIGHT;

    for(int i=0; i < MAX_PLAYERS; i++){
        ship[i].texture = loadTexture("resources/images/ship.png");
        SDL_QueryTexture(ship[i].texture, NULL, NULL, &ship[i].placement.w, &ship[i].placement.h);
    }
    return;
}

void initSDL(void){
    SDL_Init(SDL_INIT_VIDEO);
    gWindow = SDL_CreateWindow("Battlecave", SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED, SCREENWIDTH, SCREENHEIGHT, SDL_WINDOW_SHOWN);
    if(gWindow == NULL){
        printf("Window creation failed!\n");
        exit(EXIT_FAILURE);
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if(gRenderer == NULL){
        printf("Renderer creation failed!\n");
        exit(EXIT_FAILURE);
    }

    if(TTF_Init() < 0){
        printf("TTF Init failed!\n");
        exit(EXIT_FAILURE);
    }

    if(SDLNet_Init() < 0){
        printf("SDL Net Init failed!\n");
        exit(EXIT_FAILURE);
    }
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    return;
}

SDL_Texture* loadTexture(char* filename){
	SDL_Surface* temp = IMG_Load(filename);
	if(temp == NULL){
		printf("Failed to load image %s: %s\n",filename,IMG_GetError());
	}
	SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, 255,0, 255));
	SDL_Texture* returnTexture = SDL_CreateTextureFromSurface(gRenderer, temp);
	if(returnTexture == NULL){
		printf("Failed to convert the surface %s to a texture: %s\n",filename,IMG_GetError());
	}
	SDL_FreeSurface(temp);
	return returnTexture;
}

void closeRenderer(void){
    SDL_DestroyTexture(mBackground);
    SDL_DestroyTexture(mBlackOverlay);
    SDL_DestroyTexture(mIPPortWindow);
    SDL_DestroyTexture(mLobbyWindow);
    SDL_DestroyWindow(gWindow);
    //closeRenderResources();
    //closeNetResources(); //SDLNet_Quit();
    //SDL_StopTextInput();
    return;
}
