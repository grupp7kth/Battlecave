#include "includes.h"

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Texture* mBackground = NULL;
SDL_Texture* mBlackOverlay = NULL;
SDL_Texture* mOptionsWindow = NULL;
SDL_Texture* mIPPortWindow = NULL;
SDL_Texture* mLobbyWindow = NULL;
SDL_Texture* mReady = NULL;
SDL_Texture* mNameWindow = NULL;
SDL_Texture* mHealthBar = NULL;
SDL_Texture* mPowerupBar = NULL;
SDL_Texture* mTimeWarpBar = NULL;
SDL_Texture* mPowerupIcon[6] = {NULL};

SDL_Texture* loadTexture(char* filname);

//**************************************************************************
//*                                FUNCTIONS                               *
//**************************************************************************

void renderScreen(int *mode, int *select, SDL_Rect buttonPlacement[], SDL_Rect windowPlacement[]){
    SDL_SetRenderDrawColor(gRenderer,48,0,0,255);
    SDL_RenderClear(gRenderer);

    setButtons(buttonPlacement, windowPlacement, mode);

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
//********************** MODE 2 : OPTIONS SCREEN **************************
    else if(*mode == OPTIONS){
        SDL_RenderCopy(gRenderer, mBackground, NULL, NULL);
        SDL_RenderCopy(gRenderer, mBlackOverlay, NULL, NULL);

        SDL_RenderCopy(gRenderer, mOptionsWindow, NULL, &windowPlacement[3]);  // Shows a preview of the map

        // Show the boxes checked if they are enabled
        checkBox.x = windowPlacement[3].x + 175;
        if(namesBelowShipsEnabled){
            checkBox.y = windowPlacement[3].y + 152;
            SDL_RenderCopy(gRenderer, mReady, NULL, &checkBox);
        }
        if(fancyBackgroundEnabled){
            checkBox.y = windowPlacement[3].y + 182;
            SDL_RenderCopy(gRenderer, mReady, NULL, &checkBox);
        }
        if(musicEnabled){
            checkBox.y = windowPlacement[3].y + 209;
            SDL_RenderCopy(gRenderer, mReady, NULL, &checkBox);
        }
        setText(mode, gRenderer, select);
    }
//********************** MODE 3 : LOBBY SCREEN ****************************
    else if(*mode == LOBBY){
        SDL_RenderCopy(gRenderer, mBackground, NULL, NULL);
        SDL_RenderCopy(gRenderer, mLobbyWindow, NULL, &windowPlacement[1]);

        SDL_RenderCopy(gRenderer, gameMapBackground.texture, NULL, &gameMapBackground.mapPreviewPlacement);  // Shows a preview of the map

        for(int i=0; i < MAX_PLAYERS; i++){     // Ready icons
            if(playerReady[i]){
                readyIcon.x = 485;
                readyIcon.y = 116 + (i * 65);
                readyIcon.h = 40;
                readyIcon.w = 40;
                SDL_RenderCopy(gRenderer, mReady, NULL, &readyIcon);
            }
        }
        setText(mode, gRenderer, select);
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
        // Decide Map Background Placement
        gameMapBackground.source.x = ship[viewportID].x - (GAME_AREA_WIDTH/2);
        gameMapBackground.source.y = ship[viewportID].y - (GAME_AREA_HEIGHT/2);

        if(ship[viewportID].x < GAME_AREA_WIDTH/2)
            gameMapBackground.source.x = 0;
        else if(ship[viewportID].x > gameMapBackground.w - GAME_AREA_WIDTH/2)
            gameMapBackground.source.x = gameMapBackground.w - GAME_AREA_WIDTH;

        if(ship[viewportID].y < GAME_AREA_HEIGHT/2)
            gameMapBackground.source.y = 0;
        else if(ship[viewportID].y > gameMapBackground.h - GAME_AREA_HEIGHT/2)
            gameMapBackground.source.y = gameMapBackground.h - GAME_AREA_HEIGHT;
    
        // Render Low (Far) Layer Background if enabled
        if(fancyBackgroundEnabled){
            gameLowLayerBackground.source.x = gameMapBackground.source.x/3;
            gameLowLayerBackground.source.y = gameMapBackground.source.y/3;
            SDL_RenderCopy(gRenderer, gameLowLayerBackground.texture, &gameLowLayerBackground.source, &gameLowLayerBackground.dest);
        }
        // Render Map Background
        SDL_RenderCopy(gRenderer, gameMapBackground.texture, &gameMapBackground.source, &gameMapBackground.dest);

        // If the client is currently teleporting; change the alpha of his ship to tone it in/out
        if(client.activePowerup == POWERUP_TELEPORT){
            int tempTeleInt;
            tempTeleInt = SDL_GetTicks() - client.powerupTimerStart;

            if(tempTeleInt < TELEPORT_DURATION/2)               // For the first half of the teleport duration; fade out
                client.shipAlpha = 255 - ((tempTeleInt)/(TELEPORT_DURATION/(2*255)));
            else                                                // For the second half of the teleport duration; fade in again as we've now been teleported
                client.shipAlpha = (tempTeleInt-1500)/(TELEPORT_DURATION/(2*255));

            if(client.shipAlpha > 255)
                client.shipAlpha = 255;
            else if(client.shipAlpha < 0)
                client.shipAlpha = 0;
            SDL_SetTextureAlphaMod(ship[client.id].texture, client.shipAlpha);

            if(tempTeleInt >= TELEPORT_DURATION)                // Disable it once the duration is over
                client.activePowerup = -1;
        }
        else
            SDL_SetTextureAlphaMod(ship[client.id].texture, 0xFF);

        // Render ships
        for(int i = 0; i < MAX_PLAYERS; i++){
            if(!ship[i].active || ship[i].isDead)
                continue;
            int temp;
            ship[i].placement.x = ship[i].x - gameMapBackground.source.x - ship[i].w/2;
            ship[i].placement.y = ship[i].y - gameMapBackground.source.y - ship[i].h/2;

            if(ship[client.id].placement.x >= GAME_AREA_WIDTH/2)                            // Decide whether the ship is within the game area of the window
                temp = ship[i].placement.x - ship[client.id].placement.x;
            else
                temp = ship[i].placement.x - GAME_AREA_WIDTH/2;
            if(temp < GAME_AREA_WIDTH/2){
                SDL_RenderCopyEx(gRenderer, ship[i].texture, NULL, &ship[i].placement, ship[i].angle, NULL, SDL_FLIP_NONE); // Render the actual ship

                if((i != client.id && healthBelowEnemyShipsEnabled) || (i == client.id && healthBelowOwnShipEnabled)){ // If the options to show health bars below ships have been enabled through the options menu
                    belowShipHealthBar.framePlacement.x = ship[i].placement.x - 5;
                    if(namesBelowShipsEnabled)                                              // Make sure the players' name and HP-bar aren't stacked
                        belowShipHealthBar.framePlacement.y = ship[i].placement.y + 55;
                    else
                        belowShipHealthBar.framePlacement.y = ship[i].placement.y + 35;
                    belowShipHealthBar.barPlacement.x = belowShipHealthBar.framePlacement.x + 1;
                    belowShipHealthBar.barPlacement.y = belowShipHealthBar.framePlacement.y + 1;
                    belowShipHealthBar.barPlacement.w = ship[i].health * 0.3;
                    SDL_RenderCopy(gRenderer, belowShipHealthBar.frameTexture, NULL, &belowShipHealthBar.framePlacement);
                    SDL_RenderCopy(gRenderer, mHealthBar, NULL, &belowShipHealthBar.barPlacement);
                }
            }
        }

        // Render powerups
        for(int i = 0; i < MAX_ALLOWED_POWERUP_SPAWNPOINTS; i++){
            if(!powerupSpawnPoint[i].isActive)
                continue;
            powerupSpawnPoint[i].placement.x = powerupSpawnPoint[i].x - gameMapBackground.source.x;
            powerupSpawnPoint[i].placement.y = powerupSpawnPoint[i].y - gameMapBackground.source.y;
            if(powerupSpawnPoint[i].placement.x < GAME_AREA_WIDTH)                  // Don't bother rendering it if it's under the side-bar
                SDL_RenderCopy(gRenderer, mPowerupIcon[powerupSpawnPoint[i].type], NULL, &powerupSpawnPoint[i].placement);

        }

        // Render bullets
        SDL_Rect bulletPlacement;
        bulletPlacement.w = 4;
        bulletPlacement.h = 4;

        for(int i = 0; i < currentBulletAmmount; i++){
            bulletPlacement.x = bullet[i].x-1;
            bulletPlacement.y = bullet[i].y-1;
            SDL_RenderCopy(gRenderer, miniMap.playerTexture[bullet[i].source] , NULL, &bulletPlacement);
        }

        // Render a time-warp duration bar if it's active
        if(timeWarpIsActive && !ship[viewportID].isDead){                                         // If the global powerup 'timewarp' is active, we graphically show its duration
            SDL_SetRenderDrawColor(gRenderer, 0x88, 0x00, 0x15, 0xFF);
            timeWarpRect.x = 265;
            timeWarpRect.y = 100;
            timeWarpRect.h = 20;

            timeWarpRect.w = 500 - (SDL_GetTicks() - timeWarpStart)/20;
            SDL_RenderCopy(gRenderer, mTimeWarpBar, NULL, &timeWarpRect);

            timeWarpRect.w = 500;

            SDL_RenderDrawRect(gRenderer, &timeWarpRect);

            if((SDL_GetTicks() - timeWarpStart) >= TIMEWARP_DURATION)
                timeWarpIsActive = false;

            SDL_SetRenderDrawColor(gRenderer, 0x50, 0x50, 0x50, 0xFF);
        }

        // RENDER EVERYTHING SIDEBAR RELATED
        // Health-bar
        healthBar.w = 2*ship[client.id].health;
        SDL_RenderCopy(gRenderer, mHealthBar, NULL, &healthBar);
        // Powerup-bar
        if(client.activePowerup == POWERUP_MULTI3X || client.activePowerup == POWERUP_MULTI2X || client.activePowerup == POWERUP_DOUBLEDAMAGE){ // Only these powerups have a duration on the powerup-bar
            int tempTimeInt;
            tempTimeInt = SDL_GetTicks() - client.powerupTimerStart;
            if(tempTimeInt <= SHIP_POWERUP_DURATION){
                powerupBar.w = (SHIP_POWERUP_DURATION - tempTimeInt)/50;    // Scale the width to match the remaining time within the 200pix width window
                SDL_RenderCopy(gRenderer, mPowerupBar, NULL, &powerupBar);
            }
            else
                client.activePowerup = -1;                                  // If the time has run out, no powerup is active
        }

        // Render the sidebar texture
        SDL_RenderCopy(gRenderer, sideBar.texture, NULL, &sideBar.placement);

        // Render MiniMap
        SDL_RenderCopy(gRenderer, gameMapBackground.texture, NULL, &miniMap.mapPlacement);
        for(int i=0; i < MAX_PLAYERS; i++){
            if(ship[i].active && !ship[i].isDead){
                miniMap.playerPlacement[i].x = ship[i].x/10 + miniMap.mapPlacement.x - 5;
                miniMap.playerPlacement[i].y = ship[i].y/10 + miniMap.mapPlacement.y - 5;
                SDL_RenderCopy(gRenderer, miniMap.playerTexture[i], NULL, &miniMap.playerPlacement[i]);
            }
        }
        for(int i=0; i < MAX_ALLOWED_POWERUP_SPAWNPOINTS; i++){
            if(powerupSpawnPoint[i].isActive){
                miniMap.powerupPlacement.x = powerupSpawnPoint[i].x/10 +  + miniMap.mapPlacement.x - 5;
                miniMap.powerupPlacement.y = powerupSpawnPoint[i].y/10 +  + miniMap.mapPlacement.y - 5;
                SDL_RenderCopy(gRenderer, miniMap.powerupTexture, NULL, &miniMap.powerupPlacement);
            }
        }




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

    mOptionsWindow = loadTexture("resources/images/optionswindow.png");
    mIPPortWindow = loadTexture("resources/images/ipportwindow.png");
    mNameWindow = loadTexture("resources/images/namewindow.png");
    mLobbyWindow = loadTexture("resources/images/lobbybackground.png");
    mReady = loadTexture("resources/images/ready.png");
    mHealthBar = loadTexture("resources/images/healthbar.png");
    mPowerupBar = loadTexture("resources/images/powerupbar.png");
    mTimeWarpBar = loadTexture("resources/images/timewarpbar.png");

    belowShipHealthBar.frameTexture = loadTexture("resources/images/healthframe.png");

    mPowerupIcon[0] = loadTexture("resources/images/tripleshoticon.png");  // There are 6 different powerups available currently
    mPowerupIcon[1] = loadTexture("resources/images/doubleshoticon.png");
    mPowerupIcon[2] = loadTexture("resources/images/healthpackicon.png");
    mPowerupIcon[3] = loadTexture("resources/images/timewarpicon.png");
    mPowerupIcon[4] = loadTexture("resources/images/doubledamageicon.png");
    mPowerupIcon[5] = loadTexture("resources/images/teleporticon.png");

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
    miniMap.powerupTexture = loadTexture("resources/images/powerupicon.png");
    miniMap.powerupPlacement.w = 10;
    miniMap.powerupPlacement.h = 10;

    miniMap.mapPlacement.x = GAME_AREA_WIDTH + 7;
    miniMap.mapPlacement.y = 490;
    miniMap.mapPlacement.w = 240;
    miniMap.mapPlacement.h = 160;

    gameMapBackground.texture = loadTexture("resources/images/bestmapworld.png");
    gameMapBackground.source.w = SCREENWIDTH-250;
    gameMapBackground.source.h = SCREENHEIGHT;
    gameMapBackground.dest.x = 0;
    gameMapBackground.dest.y = 0;
    gameMapBackground.dest.w = SCREENWIDTH-250;
    gameMapBackground.dest.h = SCREENHEIGHT;
    SDL_QueryTexture(gameMapBackground.texture, NULL, NULL, &gameMapBackground.w, &gameMapBackground.h);
    gameMapBackground.mapPreviewPlacement.x = 879;
    gameMapBackground.mapPreviewPlacement.y = 124;
    gameMapBackground.mapPreviewPlacement.w = 240;
    gameMapBackground.mapPreviewPlacement.h = 160;

    gameLowLayerBackground.texture = loadTexture("resources/images/ingamebackground.png");
    if (gameLowLayerBackground.texture == NULL) {
    	    printf("Background not loaded\n");
    	    exit(1);
    }
    gameLowLayerBackground.dest.x = 0;
    gameLowLayerBackground.dest.y = 0;
    gameLowLayerBackground.dest.w = GAME_AREA_WIDTH;
    gameLowLayerBackground.dest.h = GAME_AREA_HEIGHT;
    gameLowLayerBackground.source.x = 0;
    gameLowLayerBackground.source.y = 0;
    gameLowLayerBackground.source.w = GAME_AREA_WIDTH;
    gameLowLayerBackground.source.h = GAME_AREA_HEIGHT;

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
    SDL_SetRenderDrawColor(gRenderer, 0x50, 0x50, 0x50, 0xFF);
    return;
}

SDL_Texture* loadTexture(char* filename){
	SDL_Surface* temp = IMG_Load(filename);
	printf("Filename: %s\n",filename);
	printf("W: %d, H: %d\n",temp->w,temp->h);
	printf("Pitch: %d\n",temp->pitch);
	int pixelSize = temp->pitch/temp->w;
	printf("Det {r allts} %d bytes per pixel.\n",pixelSize);
	if(temp == NULL){
		printf("Failed to load image %s: %s\n",filename,IMG_GetError());
	}
	if (temp->w < 4000) SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format,0,0,0));
	else puts("Image too big, no colour key");
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
