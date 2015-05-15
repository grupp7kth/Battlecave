#include "includes.h"

static void setButtonsMode0(SDL_Rect buttonPlacement[]);
static void setButtonsMode1(SDL_Rect buttonPlacement[]);
static void setButtonsMode3(SDL_Rect buttonPlacement[]);
static void setButtonsMode4(SDL_Rect buttonPlacement[]);
static void setButtonsMode5(SDL_Rect buttonPlacement[]);
static void setButtonsMode6(SDL_Rect buttonPlacement[]);

void setButtons(SDL_Rect buttonPlacement[], int *mode){
    if(*mode == STARTUP)                                    // Startup Screen
        setButtonsMode0(buttonPlacement);
    else if(*mode == FIND_SERVERS)                          // Show Servers Screen
        setButtonsMode1(buttonPlacement);
    else if(*mode == OPTIONS)                               // Options Screen
        printf("SET OPTIONS BUTTONS HERE\n");
    else if(*mode == LOBBY)                                 // Lobby Screen
        setButtonsMode3(buttonPlacement);
    else if(*mode == JOIN_DEFAULT)                          // Join Default Server Screen
        setButtonsMode4(buttonPlacement);
    else if(*mode == JOIN_CUSTOM)                           // Join Custom Server Screen
        setButtonsMode5(buttonPlacement);
    else if(*mode == IN_GAME)
        setButtonsMode6(buttonPlacement);
    return;
    }

static void setButtonsMode0(SDL_Rect buttonPlacement[]){ // 0 = Find Servers , 1 = Options , 2 = Exit
    buttonPlacement[0].x = SCREENWIDTH/6 - 180;
    buttonPlacement[0].y = 4*(SCREENHEIGHT/6) + 50;
    buttonPlacement[0].h = 40;
    buttonPlacement[0].w = 220;
    buttonPlacement[1].x = SCREENWIDTH/6 - 180;
    buttonPlacement[1].y = 4*(SCREENHEIGHT/6) + 100;
    buttonPlacement[1].h = 40;
    buttonPlacement[1].w = 131;
    buttonPlacement[2].x = SCREENWIDTH/6 - 180;
    buttonPlacement[2].y = 4*(SCREENHEIGHT/6) + 150;
    buttonPlacement[2].h = 40;
    buttonPlacement[2].w = 65;
    return;
}

static void setButtonsMode1(SDL_Rect buttonPlacement[]){ // 0 = Default Server , 1 = Custom Server , 2 = Back
    buttonPlacement[0].x = SCREENWIDTH/6 - 180;
    buttonPlacement[0].y = 4*(SCREENHEIGHT/6) + 50;
    buttonPlacement[0].h = 40;
    buttonPlacement[0].w = 334;
    buttonPlacement[1].x = SCREENWIDTH/6 - 180;
    buttonPlacement[1].y = 4*(SCREENHEIGHT/6) + 100;
    buttonPlacement[1].h = 40;
    buttonPlacement[1].w = 345;
    buttonPlacement[2].x = SCREENWIDTH/6 - 180;
    buttonPlacement[2].y = 4*(SCREENHEIGHT/6) + 150;
    buttonPlacement[2].h = 40;
    buttonPlacement[2].w = 352;
    return;
}

static void setButtonsMode3(SDL_Rect buttonPlacement[]){ // 0 = Write Chat Message Field , 1 = Leave , 2 = Start Game , 3-10 = Player Boxes , 11-16 = Option Boxes
    buttonPlacement[0].x = 537;
    buttonPlacement[0].y = 587;
    buttonPlacement[0].h = 33;
    buttonPlacement[0].w = 590;
    buttonPlacement[1].x = 869;
    buttonPlacement[1].y = 350;
    buttonPlacement[1].h = 45;
    buttonPlacement[1].w = 119;
    buttonPlacement[2].x = 1008;
    buttonPlacement[2].y = 350;
    buttonPlacement[2].h = 45;
    buttonPlacement[2].w = 119;
    // Player Fields 1-8 :
    for(int i = 0; i < MAX_PLAYERS; i++){
        buttonPlacement[i+3].x = 183;
        buttonPlacement[i+3].y = 112 + i*65;
        buttonPlacement[i+3].h = 45;
        buttonPlacement[i+3].w = 300;
    }
    // Option Boxes 1-5 :
    for(int i = 0; i < 5; i++){
        buttonPlacement[i+11].x = 537;
        buttonPlacement[i+11].y = 118 + i*58;
        buttonPlacement[i+11].h = 45;
        buttonPlacement[i+11].w = 310;
    }
    return;
}

static void setButtonsMode4(SDL_Rect buttonPlacement[]){ // 0 = Close , 1 = GO!
    buttonPlacement[0].x = SCREENWIDTH/2 + 119;
    buttonPlacement[0].y = SCREENHEIGHT/2 - 91;
    buttonPlacement[0].h = 25;
    buttonPlacement[0].w = 25;
    buttonPlacement[1].x = SCREENWIDTH/2 - 49;
    buttonPlacement[1].y = SCREENHEIGHT/2 - 16;
    buttonPlacement[1].h = 31;
    buttonPlacement[1].w = 100;
    return;
}

static void setButtonsMode5(SDL_Rect buttonPlacement[]){ // 0 = Enter IP Field , 1 = Enter Port Field , 2 = Enter Name Field , 3 = Close , 4 = GO!
    buttonPlacement[0].x = SCREENWIDTH/2 - 131;
    buttonPlacement[0].y = SCREENHEIGHT/2 - 57;
    buttonPlacement[0].h = 38;
    buttonPlacement[0].w = 260;
    buttonPlacement[1].x = SCREENWIDTH/2 - 131;
    buttonPlacement[1].y = SCREENHEIGHT/2 + 18;
    buttonPlacement[1].h = 38;
    buttonPlacement[1].w = 260;
    buttonPlacement[2].x = SCREENWIDTH/2 - 131;
    buttonPlacement[2].y = SCREENHEIGHT/2 + 93;
    buttonPlacement[2].h = 38;
    buttonPlacement[2].w = 260;
    buttonPlacement[3].x = SCREENWIDTH/2 + 119;
    buttonPlacement[3].y = SCREENHEIGHT/2 - 91;
    buttonPlacement[3].h = 25;
    buttonPlacement[3].w = 25;
    buttonPlacement[4].x = SCREENWIDTH/2 - 49;
    buttonPlacement[4].y = SCREENHEIGHT/2 + 136;
    buttonPlacement[4].h = 31;
    buttonPlacement[4].w = 100;
    return;
}

static void setButtonsMode6(SDL_Rect buttonPlacement[]){ // 0 = Leave
    buttonPlacement[0].x = GAME_AREA_WIDTH + 72;
    buttonPlacement[0].y = 666;
    buttonPlacement[0].h = 40;
    buttonPlacement[0].w = 110;
    return;
}
