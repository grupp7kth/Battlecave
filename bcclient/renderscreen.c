#include "includes.h"

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Texture* mBackground = NULL;
SDL_Texture* mBlackOverlay = NULL;
SDL_Texture* mIPPortWindow = NULL;
SDL_Texture* mLobbyWindow = NULL;
SDL_Texture* mReady = NULL;
SDL_Texture* mNameWindow = NULL;
SDL_Texture* mHealthBar = NULL;

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
                readyIcon.x = 485;
                readyIcon.y = 116 + (i * 65);
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
//*********************** MODE 6 : In Game *********************************
    else if(*mode == IN_GAME){
        // RENDER EVERYTHING GAME RELATED
        // Render Background

        if(!ship[client.id].isDead)         // If the player is alive the viewport will be his own, else it will be the first alive players going from ID 0-7
            spectatingID = client.id;
        else{
            for(int i=0; i < MAX_PLAYERS; i++){
                if(ship[i].active && !ship[i].isDead){
                    spectatingID = i;
                    break;
                }
            }
        }
        gameBackground.source.x = ship[spectatingID].x - (GAME_AREA_WIDTH/2);
        gameBackground.source.y = ship[spectatingID].y - (GAME_AREA_HEIGHT/2);

        if(ship[spectatingID].x < GAME_AREA_WIDTH/2)
            gameBackground.source.x = 0;
        else if(ship[spectatingID].x > gameBackground.w - GAME_AREA_WIDTH/2)
            gameBackground.source.x = gameBackground.w - GAME_AREA_WIDTH;

        if(ship[spectatingID].y < GAME_AREA_HEIGHT/2)
            gameBackground.source.y = 0;
        else if(ship[spectatingID].y > gameBackground.h - GAME_AREA_HEIGHT/2)
            gameBackground.source.y = gameBackground.h - GAME_AREA_HEIGHT;

        SDL_RenderCopy(gRenderer, gameBackground.texture, &gameBackground.source, &gameBackground.dest);

        // Render Ships
        for(int i = 0; i < MAX_PLAYERS; i++){
            if(!ship[i].active)
                continue;
            int temp;
            ship[i].placement.x = ship[i].x - gameBackground.source.x - ship[i].w/2;
            ship[i].placement.y = ship[i].y - gameBackground.source.y - ship[i].h/2;

            if(ship[client.id].placement.x >= GAME_AREA_WIDTH/2)                    // Decide whether the ship is within the game area of the window
                temp = ship[i].placement.x - ship[client.id].placement.x;
            else
                temp = ship[i].placement.x - GAME_AREA_WIDTH/2;
            if(temp < GAME_AREA_WIDTH/2)
                SDL_RenderCopyEx(gRenderer, ship[i].texture, NULL, &ship[i].placement, ship[i].angle, NULL, SDL_FLIP_NONE);
        }

        SDL_Rect bulletPlacement;
        bulletPlacement.w = 4;
        bulletPlacement.h = 4;

        for(int i = 0; i < currentBulletAmmount; i++){
            bulletPlacement.x = bullet[i].x;
            bulletPlacement.y = bullet[i].y;
            SDL_RenderCopy(gRenderer, miniMap.playerTexture[bullet[i].source] , NULL, &bulletPlacement);
        }

        // RENDER EVERYTHING SIDEBAR RELATED
        SDL_RenderCopy(gRenderer, sideBar.texture, NULL, &sideBar.placement);
        // Render MiniMap
        SDL_RenderCopy(gRenderer, gameBackground.texture, NULL, &miniMap.mapPlacement);
        for(int i=0; i < MAX_PLAYERS; i++){
            if(ship[i].active && !ship[i].isDead){
                miniMap.playerPlacement[i].x = ship[i].x/10 + miniMap.mapPlacement.x - 5;
                miniMap.playerPlacement[i].y = ship[i].y/10 + miniMap.mapPlacement.y - 5;
                SDL_RenderCopy(gRenderer, miniMap.playerTexture[i], NULL, &miniMap.playerPlacement[i]);
            }
        }

        // Health-bar
        SDL_Rect health;
        health.x = GAME_AREA_WIDTH + 27;
        health.y = 391;
        health.w = 2*client.health;
        health.h = 25;
        SDL_RenderCopy(gRenderer, mHealthBar, NULL, &health);



        // RENDER ALL TEXT
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
    mHealthBar = loadTexture("resources/images/healthbar.png");

    sideBar.texture = loadTexture("resources/images/sidebar.png");
    sideBar.placement.x = GAME_AREA_WIDTH;
    sideBar.placement.y = 0;
    sideBar.placement.w = SCREENWIDTH - GAME_AREA_WIDTH;
    sideBar.placement.h = SCREENHEIGHT;

    for(int i=0; i < MAX_PLAYERS; i++){
        miniMap.playerPlacement[i].w = 10;
        miniMap.playerPlacement[i].h = 10;
        miniMap.playerTexture[i] = loadTexture("resources/images/playericon.png");
        SDL_SetTextureColorMod(miniMap.playerTexture[i], colorsPlayer[i*3], colorsPlayer[i*3+1], colorsPlayer[i*3+2]);       // Assign each player-icon colors to match their ID-color
    }
    miniMap.mapPlacement.x = GAME_AREA_WIDTH + 7;
    miniMap.mapPlacement.y = 490;
    miniMap.mapPlacement.w = 240;
    miniMap.mapPlacement.h = 160;

    gameBackground.texture = loadTexture("resources/images/cave.png");
    gameBackground.source.w = SCREENWIDTH-250;
    gameBackground.source.h = SCREENHEIGHT;
    gameBackground.dest.x = 0;
    gameBackground.dest.y = 0;
    gameBackground.dest.w = SCREENWIDTH-250;
    gameBackground.dest.h = SCREENHEIGHT;
    SDL_QueryTexture(gameBackground.texture, NULL, NULL, &gameBackground.w, &gameBackground.h);

    for(int i=0; i < MAX_PLAYERS; i++){
        ship[i].texture = loadTexture("resources/images/ship.png");
        SDL_SetTextureColorMod(ship[i].texture, colorsPlayer[i*3], colorsPlayer[i*3+1], colorsPlayer[i*3+2]);       // Assign each ship colors to match their ID-color
        SDL_QueryTexture(ship[i].texture, NULL, NULL, &ship[i].placement.w, &ship[i].placement.h);
        ship[i].x = 0;
        ship[i].y = 0;
        ship[i].w = ship[i].placement.w;
        ship[i].h = ship[i].placement.h;
        ship[i].active = false;
        ship[i].angle = 0;
        ship[i].isDead = false;
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
    SDL_SetRenderDrawColor(gRenderer, 0xED, 0x1C, 0x24, 0xFF);
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
