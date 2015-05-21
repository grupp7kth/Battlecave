 #include "includes.h"

SDL_Surface* gTempTextMessage;
SDL_Texture* mText = NULL;

void setTextMode0(SDL_Rect *textPlacement, SDL_Renderer* gRenderer, int *select);
void setTextMode1(SDL_Rect *textPlacement, SDL_Renderer* gRenderer, int *select);
void setTextMode3(SDL_Rect *textPlacement, SDL_Renderer* gRenderer, int *select);
void setTextMode4(SDL_Rect *textPlacement, SDL_Renderer* gRenderer);
void setTextMode5(SDL_Rect *textPlacement, SDL_Renderer* gRenderer);
void setTextMode6(SDL_Rect *textPlacement, SDL_Renderer* gRenderer);
void renderText(SDL_Rect *textPlacement, SDL_Renderer* gRenderer);
void renderTextCentered(SDL_Rect *textPlacement, SDL_Renderer* gRenderer, int width);
void showClientVersion(SDL_Rect *textPlacement, SDL_Renderer* gRenderer);

void setText(int *mode, SDL_Renderer* gRenderer, int *select){
    static SDL_Rect textPlacement;

    if(*mode == STARTUP)
        setTextMode0(&textPlacement, gRenderer, select);
    else if(*mode == FIND_SERVERS)
        setTextMode1(&textPlacement, gRenderer, select);
    else if(*mode == LOBBY)
        setTextMode3(&textPlacement, gRenderer, select);
    else if(*mode == JOIN_DEFAULT)
        setTextMode4(&textPlacement, gRenderer);
    else if(*mode == JOIN_CUSTOM)
        setTextMode5(&textPlacement, gRenderer);
    else if(*mode ==  IN_GAME)
        setTextMode6(&textPlacement, gRenderer);

    return;
}

void setTextMode0(SDL_Rect *textPlacement, SDL_Renderer* gRenderer, int *select){
    TTF_Font* font = TTF_OpenFont("resources/fonts/arial.ttf", 40);

    textPlacement->x = SCREENWIDTH/6 -  180;
    textPlacement->y = 4*(SCREENHEIGHT/6) + 50;
    if(*select == 0)
        gTempTextMessage = TTF_RenderText_Solid(font, "Find Servers", colorsRGB[TEXT_COLOR_BRIGHTYELLOW]);
    else
        gTempTextMessage = TTF_RenderText_Solid(font, "Find Servers", colorsRGB[TEXT_COLOR_WHITE]);
    renderText(textPlacement, gRenderer);

    textPlacement->y = 4*(SCREENHEIGHT/6) + 100;
    if(*select == 1)
        gTempTextMessage = TTF_RenderText_Solid(font, "Options", colorsRGB[TEXT_COLOR_BRIGHTYELLOW]);
    else
        gTempTextMessage = TTF_RenderText_Solid(font, "Options", colorsRGB[TEXT_COLOR_WHITE]);
    renderText(textPlacement, gRenderer);

    textPlacement->y = 4*(SCREENHEIGHT/6) + 150;
    if(*select == 2)
        gTempTextMessage = TTF_RenderText_Solid(font, "Exit", colorsRGB[TEXT_COLOR_BRIGHTYELLOW]);
    else
        gTempTextMessage = TTF_RenderText_Solid(font, "Exit", colorsRGB[TEXT_COLOR_WHITE]);
    renderText(textPlacement, gRenderer);
    TTF_CloseFont(font);

    showClientVersion(textPlacement, gRenderer);
    return;
}

void setTextMode1(SDL_Rect *textPlacement, SDL_Renderer* gRenderer, int *select){
    TTF_Font* font = TTF_OpenFont("resources/fonts/arial.ttf", 40);

    textPlacement->x = SCREENWIDTH/6 - 180;
    textPlacement->y = 4*(SCREENHEIGHT/6) + 50;
    if(*select == 0)
        gTempTextMessage = TTF_RenderText_Solid(font, "Join Default Server", colorsRGB[TEXT_COLOR_BRIGHTYELLOW]);
    else
        gTempTextMessage = TTF_RenderText_Solid(font, "Join Default Server", colorsRGB[TEXT_COLOR_WHITE]);
    renderText(textPlacement, gRenderer);

    textPlacement->y = 4*(SCREENHEIGHT/6) + 100;
    if(*select == 1)
        gTempTextMessage = TTF_RenderText_Solid(font, "Join Custom Server", colorsRGB[TEXT_COLOR_BRIGHTYELLOW]);
    else
        gTempTextMessage = TTF_RenderText_Solid(font, "Join Custom Server", colorsRGB[TEXT_COLOR_WHITE]);
    renderText(textPlacement, gRenderer);

    textPlacement->y = 4*(SCREENHEIGHT/6) + 150;
    if(*select == 2)
        gTempTextMessage = TTF_RenderText_Solid(font, "Back", colorsRGB[TEXT_COLOR_BRIGHTYELLOW]);
    else
        gTempTextMessage = TTF_RenderText_Solid(font, "Back", colorsRGB[TEXT_COLOR_WHITE]);
    renderText(textPlacement, gRenderer);

    TTF_CloseFont(font);

    showClientVersion(textPlacement, gRenderer);
}

void setTextMode3(SDL_Rect *textPlacement, SDL_Renderer* gRenderer, int *select){
    TTF_Font* font = TTF_OpenFont("resources/fonts/arial.ttf", 20);

    // Chat messages (0 = upper, 4 = lower)
    textPlacement->y = 411;
    for(int i=0; i < 5; i ++){
        textPlacement->x = 545;
        gTempTextMessage = TTF_RenderText_Solid(font, textString[i], colorsRGB[textStringColor[i]]);
        renderText(textPlacement, gRenderer);
        textPlacement->x += textPlacement->w;                  // Fetch the offset for the message made by the player's name
        gTempTextMessage = TTF_RenderText_Solid(font, textString[i+6], colorsRGB[TEXT_COLOR_WHITE]);
        renderText(textPlacement, gRenderer);
        textPlacement->y += 33;
    }

    // Chat message that the user is typing
    textPlacement->x = 553;
    textPlacement->y = 590;
    gTempTextMessage = TTF_RenderText_Solid(font, textString[PLAYER_MESSAGE_WRITE], colorsRGB[TEXT_COLOR_WHITE]);
    renderText(textPlacement, gRenderer);

    TTF_CloseFont(font);
    font = TTF_OpenFont("resources/fonts/arial.ttf", 25);

    // Names of connected players
    textPlacement->x = 190;
    textPlacement->y = 120;
    for(int i=0; i < MAX_PLAYERS; i++){
        gTempTextMessage = TTF_RenderText_Solid(font, playerName[i], colorsRGB[i+6]);
        renderText(textPlacement, gRenderer);
        textPlacement->y += 65;
    }
    for(int i=0; i < MAX_PLAYERS; i++){
        if(*select == (i+3) && (strcmp(playerName[i], "\0") == 0 || computerPlayerActive[i] == true)){

            textPlacement->y = 120 + i*65;

            if(computerPlayerActive[i] == false){
                textPlacement->x = 240;
                gTempTextMessage = TTF_RenderText_Solid(font, "Add CPU Player", colorsRGB[TEXT_COLOR_BRIGHTGREEN]);
            }
            else{
                textPlacement->x = 217;
                gTempTextMessage = TTF_RenderText_Solid(font, "Remove CPU Player", colorsRGB[TEXT_COLOR_BRIGHTRED]);
            }
            renderText(textPlacement, gRenderer);
        }
    }
    // Options
    char tempOptStr[3];

    textPlacement->y = 127;
    textPlacement->x = 770;
    // -- RESERVED FOR GAME OPTION ID=0 --

    // Option 1 : Bullet Interval
    textPlacement->y += 58;
    sprintf(tempOptStr, "%d", bulletIntervalList[activeBulletInterval]);
    gTempTextMessage = TTF_RenderText_Solid(font, tempOptStr, colorsRGB[TEXT_COLOR_TEAL]);
    renderText(textPlacement, gRenderer);

    // Option 2 : Infinite Momentum
    textPlacement->y += 58;
    if(infiniteMomentum)
        gTempTextMessage = TTF_RenderText_Solid(font, "ON", colorsRGB[TEXT_COLOR_BRIGHTGREEN]);
    else
        gTempTextMessage = TTF_RenderText_Solid(font, "OFF", colorsRGB[TEXT_COLOR_BRIGHTRED]);
    renderText(textPlacement, gRenderer);

    // Option 3 : Max Speed
    textPlacement->y += 58;
    sprintf(tempOptStr, "%d", maxSpeedList[activeMaxSpeed]);
    gTempTextMessage = TTF_RenderText_Solid(font, tempOptStr, colorsRGB[TEXT_COLOR_TEAL]);
    renderText(textPlacement, gRenderer);
    // Option 4 : Game Length
    textPlacement->y += 58;
    sprintf(tempOptStr, "%d", gameLengthList[activeGameLength]);
    gTempTextMessage = TTF_RenderText_Solid(font, tempOptStr, colorsRGB[TEXT_COLOR_TEAL]);
    renderText(textPlacement, gRenderer);

    TTF_CloseFont(font);
    return;
}

void setTextMode4(SDL_Rect *textPlacement, SDL_Renderer* gRenderer){
    TTF_Font* font = TTF_OpenFont("resources/fonts/arial.ttf", 30);

    textPlacement->x = SCREENWIDTH/2 - 127;
    textPlacement->y = SCREENHEIGHT/2 - 57;
    gTempTextMessage = TTF_RenderText_Solid(font, textString[ENTERING_NAME], colorsRGB[TEXT_COLOR_TEAL]);
    renderText(textPlacement, gRenderer);

    TTF_CloseFont(font);

    showClientVersion(textPlacement, gRenderer);
    return;
}

void setTextMode5(SDL_Rect *textPlacement, SDL_Renderer* gRenderer){
    TTF_Font* font = TTF_OpenFont("resources/fonts/arial.ttf", 30);

    textPlacement->x = SCREENWIDTH/2 - 127;
    textPlacement->y = SCREENHEIGHT/2 - 57;
    gTempTextMessage = TTF_RenderText_Solid(font, textString[ENTERING_IP], colorsRGB[TEXT_COLOR_TEAL]);
    renderText(textPlacement, gRenderer);

    textPlacement->y = SCREENHEIGHT/2 + 18;
    gTempTextMessage = TTF_RenderText_Solid(font, textString[ENTERING_PORT], colorsRGB[TEXT_COLOR_TEAL]);
    renderText(textPlacement, gRenderer);

    textPlacement->y = SCREENHEIGHT/2 + 93;
    gTempTextMessage = TTF_RenderText_Solid(font, textString[ENTERING_NAME], colorsRGB[TEXT_COLOR_TEAL]);
    renderText(textPlacement, gRenderer);

    TTF_CloseFont(font);

    showClientVersion(textPlacement, gRenderer);
    return;
}

void setTextMode6(SDL_Rect *textPlacement, SDL_Renderer* gRenderer){
    TTF_Font* font = TTF_OpenFont("resources/fonts/arial.ttf", 20);

    // Chat messages (0 upper, 4 lower)
    textPlacement->y = 500;
    for(int i=0; i < 5; i ++){
        textPlacement->x = 20;
        gTempTextMessage = TTF_RenderText_Solid(font, textString[i], colorsRGB[textStringColor[i]]);
        renderText(textPlacement, gRenderer);
        textPlacement->x += textPlacement->w;
        gTempTextMessage = TTF_RenderText_Solid(font, textString[i+6], colorsRGB[TEXT_COLOR_WHITE]);
        renderText(textPlacement, gRenderer);
        textPlacement->y += 33;
    }

    // Chat message that the user is typing
    textPlacement->x = 20;
    textPlacement->y = 680;
    gTempTextMessage = TTF_RenderText_Solid(font, textString[PLAYER_MESSAGE_WRITE], colorsRGB[TEXT_COLOR_WHITE]);
    renderText(textPlacement, gRenderer);

    TTF_CloseFont(font);

    // FreezeTime countdown
    if(gameFreezeTime >= 0 && gameFreezeTime <= 3){
        font = TTF_OpenFont("resources/fonts/arial.ttf", 80);
        textPlacement->y = 100;

        if(gameFreezeTime == 3)
            gTempTextMessage = TTF_RenderText_Solid(font, "3...", colorsRGB[TEXT_COLOR_RED]);
        else if(gameFreezeTime == 2)
            gTempTextMessage = TTF_RenderText_Solid(font, "2...", colorsRGB[TEXT_COLOR_ORANGE]);
        else if(gameFreezeTime == 1)
            gTempTextMessage = TTF_RenderText_Solid(font, "1...", colorsRGB[TEXT_COLOR_YELLOW]);
        else if(gameFreezeTime == 0)
            gTempTextMessage = TTF_RenderText_Solid(font, "GO!", colorsRGB[TEXT_COLOR_GREEN]);

        renderTextCentered(textPlacement, gRenderer, GAME_AREA_WIDTH);
        TTF_CloseFont(font);
    }
    // Render the players' scores
    // Sort all scores
    int tempScoreID[MAX_PLAYERS] = {0, 1, 2, 3, 4, 5, 6, 7}, temp;

    for(int i=0; i < MAX_PLAYERS-1; i++){
        if(!ship[i].active)
            break;

        for(int j=0; j < MAX_PLAYERS-i-1; j++){
            if(playerScore[tempScoreID[j]] < playerScore[tempScoreID[j+1]]){
                temp = tempScoreID[j];
                tempScoreID[j] = tempScoreID[j+1];
                tempScoreID[j+1] = temp;
            }
        }
    }

    // Present the scores
    char tempScoreString[50];
    // First show the player's ranking. These are all the same color.
    font = TTF_OpenFont("resources/fonts/arial.ttf", 20);
    textPlacement->x = GAME_AREA_WIDTH + 15;
    textPlacement->y = 55;

    for(int i=0; i < MAX_PLAYERS; i++){
        if(ship[i].active){
            sprintf(tempScoreString, "%d. ", i+1);
            gTempTextMessage = TTF_RenderText_Solid(font, tempScoreString, colorsRGB[TEXT_COLOR_TEAL]); // Add 6 to get to the player color span
            renderText(textPlacement, gRenderer);
            textPlacement->y += 32;
        }
    }
    // And then the names, colored by player ID
    textPlacement->x = GAME_AREA_WIDTH + 35;
    textPlacement->y = 55;
    for(int i=0; i < MAX_PLAYERS; i++){
        if(ship[tempScoreID[i]].active){
            sprintf(tempScoreString, "%s", playerName[tempScoreID[i]]);
            gTempTextMessage = TTF_RenderText_Solid(font, tempScoreString, colorsRGB[tempScoreID[i]+6]); // Add 6 to get to the player color span
            renderText(textPlacement, gRenderer);
            textPlacement->y += 32;
        }
    }
    // And then the score, colored by player ID
    textPlacement->x = GAME_AREA_WIDTH + 185;
    textPlacement->y = 55;
    for(int i=0; i < MAX_PLAYERS; i++){
        if(ship[tempScoreID[i]].active){
            sprintf(tempScoreString, "%d", playerScore[tempScoreID[i]]);
            gTempTextMessage = TTF_RenderText_Solid(font, tempScoreString, colorsRGB[TEXT_COLOR_YELLOW]);
            renderText(textPlacement, gRenderer);
            textPlacement->y += 32;
        }
    }
    TTF_CloseFont(font);

    // Remaining time of the current game countdown
    char tempTimeStr[8];
    int minutes = 0, seconds;
    font = TTF_OpenFont("resources/fonts/arial.ttf", 30);
    textPlacement->y = 330;
    textPlacement->x = GAME_AREA_WIDTH + 90;

    if(gameFreezeTime < 0)
        gameTimeRemaining = gameLengthList[activeGameLength]*60000 - (SDL_GetTicks() - gameTimeStart);
    else
        gameTimeRemaining = gameLengthList[activeGameLength]*60000;

    if(gameTimeRemaining < 0)
        gameTimeRemaining = 0;

    seconds = gameTimeRemaining/1000;                     // Temp now contains the remaining time in seconds

    while(seconds >= 60){
        seconds -= 60;
        minutes++;
    }

    if(minutes >= 10 && seconds >= 10)
        sprintf(tempTimeStr, "%d:%d", minutes, seconds);
    else if(minutes < 10 && seconds >= 10)
        sprintf(tempTimeStr, "0%d:%d", minutes, seconds);
    else if(minutes >= 10 && seconds < 10)
        sprintf(tempTimeStr, "%d:0%d", minutes, seconds);
    else
        sprintf(tempTimeStr, "0%d:0%d", minutes, seconds);

    if(gameTimeRemaining < 30000)                         // If <30 seconds remaining ; Red
        gTempTextMessage = TTF_RenderText_Solid(font, tempTimeStr, colorsRGB[TEXT_COLOR_BRIGHTRED]);
    else if(gameTimeRemaining < 120000)                   // If <2 minutes remaining  ; Yellow
        gTempTextMessage = TTF_RenderText_Solid(font, tempTimeStr, colorsRGB[TEXT_COLOR_ORANGE]);
    else                                                  // If >=2 minutes remaining ; Green
        gTempTextMessage = TTF_RenderText_Solid(font, tempTimeStr, colorsRGB[TEXT_COLOR_BRIGHTGREEN]);

    renderText(textPlacement, gRenderer);
    TTF_CloseFont(font);

    // If the player is dead show the following
    if(ship[client.id].isDead){
        char tempSpecStr[50];

        font = TTF_OpenFont("resources/fonts/arial.ttf", 40);
        textPlacement->y = 130;
        client.deathTimer = 10000 - (SDL_GetTicks() - deathTimerStart);
        sprintf(tempSpecStr, "Respawning in %d", client.deathTimer/1000);
        gTempTextMessage = TTF_RenderText_Solid(font, tempSpecStr, colorsRGB[TEXT_COLOR_WHITE]);
        renderTextCentered(textPlacement, gRenderer, GAME_AREA_WIDTH);

        if(viewportID != client.id){
            textPlacement->y = 650;
            sprintf(tempSpecStr, "Currently Spectating %s", playerName[viewportID]);
            gTempTextMessage = TTF_RenderText_Solid(font, tempSpecStr, colorsRGB[viewportID+6]);
            renderTextCentered(textPlacement, gRenderer, GAME_AREA_WIDTH);
        }
        TTF_CloseFont(font);
    }

    // If the global timewarp-powerup is active do the following
    if(timeWarpIsActive && !ship[viewportID].isDead){
        font = TTF_OpenFont("resources/fonts/arial.ttf", 18);
        textPlacement->y = 100;
        gTempTextMessage = TTF_RenderText_Solid(font, "Time Warp Active!", colorsRGB[TEXT_COLOR_WHITE]);
        renderTextCentered(textPlacement, gRenderer, GAME_AREA_WIDTH);
        TTF_CloseFont(font);
    }
    // If the player just got a powerup; show some text
    if(timedTextID >= 0 && timedTextID <= 5){               // -1 = Inactive, 0-5 = Powerup Gained, 6 = Player Killed
        int tempDurationInt;
        tempDurationInt = SDL_GetTicks() - timedTextStart;
        if(tempDurationInt <= POWERUP_NOTIFICATION_DURATION){
            font = TTF_OpenFont("resources/fonts/arial.ttf", 50);
            textPlacement->y = 50;
            gTempTextMessage = TTF_RenderText_Solid(font, powerupNames[timedTextID], colorsRGB[TEXT_COLOR_TEAL]);
            renderTextCentered(textPlacement, gRenderer, GAME_AREA_WIDTH);
            TTF_CloseFont(font);
        }
        else
            timedTextID = -1;                               // If we've shown the text for its duration we turn if off

    }
    // If a player just died; show some text
    else if(timedTextID == 6){                              // -1 = Inactive, 0-5 = Powerup Gained, 6 = Player Killed
        char tempDeathStr[50];
        int tempDurationInt;
        tempDurationInt = SDL_GetTicks() - timedTextStart;
        if(tempDurationInt <= DEATH_NOTIFICATION_DURATION){
            font = TTF_OpenFont("resources/fonts/arial.ttf", 40);
            textPlacement->y = 50;

            if(killedID == client.id && killerID == client.id)
                sprintf(tempDeathStr, "You commited suicide!");
            else if(killedID == client.id)
                sprintf(tempDeathStr, "You were killed by %s!", playerName[killerID]);
            else if(killerID == client.id)
                sprintf(tempDeathStr, "You killed %s!", playerName[killedID]);
            else if(killerID == killedID)
                sprintf(tempDeathStr, "%s commited suicide!", playerName[killerID]);
            else
                sprintf(tempDeathStr, "%s killed %s!", playerName[killerID], playerName[killedID]);

            gTempTextMessage = TTF_RenderText_Solid(font, tempDeathStr, colorsRGB[TEXT_COLOR_ORANGE]);
            renderTextCentered(textPlacement, gRenderer, GAME_AREA_WIDTH);
            TTF_CloseFont(font);
        }
        else
            timedTextID = -1;                               // If we've shown the text for its duration we turn if off
    }

    return;
}


void renderText(SDL_Rect *textPlacement, SDL_Renderer* gRenderer){
    mText = SDL_CreateTextureFromSurface(gRenderer, gTempTextMessage);
    SDL_QueryTexture(mText, NULL, NULL, &textPlacement->w, &textPlacement->h);
    SDL_RenderCopy(gRenderer, mText, NULL, textPlacement);

    SDL_FreeSurface(gTempTextMessage);
    SDL_DestroyTexture(mText);
    return;
}

void renderTextCentered(SDL_Rect *textPlacement, SDL_Renderer* gRenderer, int width){       // Renders the text in the middle of the width specified
    mText = SDL_CreateTextureFromSurface(gRenderer, gTempTextMessage);
    SDL_QueryTexture(mText, NULL, NULL, &textPlacement->w, &textPlacement->h);
    textPlacement->x = (width - textPlacement->w) / 2;
    SDL_RenderCopy(gRenderer, mText, NULL, textPlacement);

    SDL_FreeSurface(gTempTextMessage);
    SDL_DestroyTexture(mText);
    return;
}

void showClientVersion(SDL_Rect *textPlacement, SDL_Renderer* gRenderer){
    textPlacement->x = 10;
    textPlacement->y = 690;
    TTF_Font* font = TTF_OpenFont("resources/fonts/arial.ttf", 20);
    gTempTextMessage = TTF_RenderText_Solid(font, CLIENT_VERSION, colorsRGB[TEXT_COLOR_ORANGE]);
    renderText(textPlacement, gRenderer);
    TTF_CloseFont(font);
    return;
}
