#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include "settext.h"
#include "definitions.h"
#include "globalvariables.h"

SDL_Surface* gTempTextMessage;
SDL_Texture* mText = NULL;

SDL_Color colors[6] = {{225, 225, 255},   // 0 = White
                       {255, 255, 150},   // 1 = Yellow
                       {0, 0, 0},         // 2 = Black
                       {100, 255, 100},   // 3 = Green
                       {255, 100, 100},   // 4 = Red
                       {45, 198, 250}};   // 5 = Teal

void setTextMode0(SDL_Rect *textPlacement, SDL_Renderer* gRenderer, int *select);
void setTextMode1(SDL_Rect *textPlacement, SDL_Renderer* gRenderer, int *select);
void setTextMode3(SDL_Rect *textPlacement, SDL_Renderer* gRenderer);
void setTextMode4(SDL_Rect *textPlacement, SDL_Renderer* gRenderer);
void setTextMode5(SDL_Rect *textPlacement, SDL_Renderer* gRenderer);
void setTextMode6(SDL_Rect *textPlacement, SDL_Renderer* gRenderer);
void renderText(SDL_Rect *textPlacement, SDL_Renderer* gRenderer);
void showClientVersion(SDL_Rect *textPlacement, SDL_Renderer* gRenderer);

void setText(int *mode, SDL_Renderer* gRenderer, int *select){
    static SDL_Rect textPlacement;

    if(*mode == STARTUP)
        setTextMode0(&textPlacement, gRenderer, select);
    else if(*mode == FIND_SERVERS)
        setTextMode1(&textPlacement, gRenderer, select);
    else if(*mode == LOBBY)
        setTextMode3(&textPlacement, gRenderer);
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

    textPlacement->x = SCREENWIDTH/6;
    textPlacement->y = 4*(SCREENHEIGHT/6);
    if(*select == 0)
        gTempTextMessage = TTF_RenderText_Solid(font, "Find Servers", colors[TEXT_COLOR_YELLOW]);
    else
        gTempTextMessage = TTF_RenderText_Solid(font, "Find Servers", colors[TEXT_COLOR_WHITE]);
    renderText(textPlacement, gRenderer);

    textPlacement->y = 4*(SCREENHEIGHT/6) + 50;
    if(*select == 1)
        gTempTextMessage = TTF_RenderText_Solid(font, "Options", colors[TEXT_COLOR_YELLOW]);
    else
        gTempTextMessage = TTF_RenderText_Solid(font, "Options", colors[TEXT_COLOR_WHITE]);
    renderText(textPlacement, gRenderer);

    textPlacement->y = 4*(SCREENHEIGHT/6) + 100;
    if(*select == 2)
        gTempTextMessage = TTF_RenderText_Solid(font, "Exit", colors[TEXT_COLOR_YELLOW]);
    else
        gTempTextMessage = TTF_RenderText_Solid(font, "Exit", colors[TEXT_COLOR_WHITE]);
    renderText(textPlacement, gRenderer);
    TTF_CloseFont(font);

    showClientVersion(textPlacement, gRenderer);
    return;
}

void setTextMode1(SDL_Rect *textPlacement, SDL_Renderer* gRenderer, int *select){
    TTF_Font* font = TTF_OpenFont("resources/fonts/arial.ttf", 40);

    textPlacement->x = SCREENWIDTH/6;
    textPlacement->y = 4*(SCREENHEIGHT/6);
    if(*select == 0)
        gTempTextMessage = TTF_RenderText_Solid(font, "Join Default Server", colors[TEXT_COLOR_YELLOW]);
    else
        gTempTextMessage = TTF_RenderText_Solid(font, "Join Default Server", colors[TEXT_COLOR_WHITE]);
    renderText(textPlacement, gRenderer);

    textPlacement->y = 4*(SCREENHEIGHT/6) + 50;
    if(*select == 1)
        gTempTextMessage = TTF_RenderText_Solid(font, "Join Custom Server", colors[TEXT_COLOR_YELLOW]);
    else
        gTempTextMessage = TTF_RenderText_Solid(font, "Join Custom Server", colors[TEXT_COLOR_WHITE]);
    renderText(textPlacement, gRenderer);

    textPlacement->y = 4*(SCREENHEIGHT/6) + 100;
    if(*select == 2)
        gTempTextMessage = TTF_RenderText_Solid(font, "Back", colors[TEXT_COLOR_YELLOW]);
    else
        gTempTextMessage = TTF_RenderText_Solid(font, "Back", colors[TEXT_COLOR_WHITE]);
    renderText(textPlacement, gRenderer);

    TTF_CloseFont(font);

    showClientVersion(textPlacement, gRenderer);
}

void setTextMode3(SDL_Rect *textPlacement, SDL_Renderer* gRenderer){
    TTF_Font* font = TTF_OpenFont("resources/fonts/arial.ttf", 20);

    // Chat messages (0 upper, 4 lower)
    textPlacement->x = 545;
    textPlacement->y = 411;
    for(int i=0; i < 5; i ++){
        gTempTextMessage = TTF_RenderText_Solid(font, textString[i], colors[textStringColor[i]]);
        renderText(textPlacement, gRenderer);
        textPlacement->y += 33;
    }

    // Chat message that the user is typing
    textPlacement->x += 7;
    textPlacement->y = 590;
    gTempTextMessage = TTF_RenderText_Solid(font, textString[PLAYER_MESSAGE_WRITE], colors[TEXT_COLOR_TEAL]);
    renderText(textPlacement, gRenderer);

    TTF_CloseFont(font);
    font = TTF_OpenFont("resources/fonts/arial.ttf", 25);
    // Names of connected players
    textPlacement->x = 190;
    textPlacement->y = 120;

    for(int i=0; i < MAX_PLAYERS; i++){
        gTempTextMessage = TTF_RenderText_Solid(font, playerName[i], colors[TEXT_COLOR_TEAL]);
        renderText(textPlacement, gRenderer);
        textPlacement->y += 64;
    }

    TTF_CloseFont(font);
    return;
}

void setTextMode4(SDL_Rect *textPlacement, SDL_Renderer* gRenderer){
    TTF_Font* font = TTF_OpenFont("resources/fonts/arial.ttf", 30);

    textPlacement->x = SCREENWIDTH/2 - 127;
    textPlacement->y = SCREENHEIGHT/2 - 57;
    gTempTextMessage = TTF_RenderText_Solid(font, textString[ENTERING_NAME], colors[TEXT_COLOR_TEAL]);
    renderText(textPlacement, gRenderer);

    TTF_CloseFont(font);

    showClientVersion(textPlacement, gRenderer);
    return;
}

void setTextMode5(SDL_Rect *textPlacement, SDL_Renderer* gRenderer){
    TTF_Font* font = TTF_OpenFont("resources/fonts/arial.ttf", 30);

    textPlacement->x = SCREENWIDTH/2 - 127;
    textPlacement->y = SCREENHEIGHT/2 - 57;
    gTempTextMessage = TTF_RenderText_Solid(font, textString[ENTERING_IP], colors[TEXT_COLOR_TEAL]);
    renderText(textPlacement, gRenderer);

    textPlacement->y = SCREENHEIGHT/2 + 18;
    gTempTextMessage = TTF_RenderText_Solid(font, textString[ENTERING_PORT], colors[TEXT_COLOR_TEAL]);
    renderText(textPlacement, gRenderer);

    textPlacement->y = SCREENHEIGHT/2 + 93;
    gTempTextMessage = TTF_RenderText_Solid(font, textString[ENTERING_NAME], colors[TEXT_COLOR_TEAL]);
    renderText(textPlacement, gRenderer);

    TTF_CloseFont(font);

    showClientVersion(textPlacement, gRenderer);
    return;
}

void setTextMode6(SDL_Rect *textPlacement, SDL_Renderer* gRenderer){
    TTF_Font* font = TTF_OpenFont("resources/fonts/arial.ttf", 20);

    // Chat messages (0 upper, 4 lower)
    textPlacement->x = 20;
    textPlacement->y = 500;
    for(int i=0; i < 5; i ++){
        gTempTextMessage = TTF_RenderText_Solid(font, textString[i], colors[textStringColor[i]]);
        renderText(textPlacement, gRenderer);
        textPlacement->y += 33;
    }

    // Chat message that the user is typing
    textPlacement->y = 680;
    gTempTextMessage = TTF_RenderText_Solid(font, textString[PLAYER_MESSAGE_WRITE], colors[TEXT_COLOR_TEAL]);
    renderText(textPlacement, gRenderer);

    TTF_CloseFont(font);
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

void showClientVersion(SDL_Rect *textPlacement, SDL_Renderer* gRenderer){
    textPlacement->x = 10;
    textPlacement->y = 690;
    TTF_Font* font = TTF_OpenFont("resources/fonts/arial.ttf", 20);
    gTempTextMessage = TTF_RenderText_Solid(font, CLIENT_VERSION, colors[TEXT_COLOR_RED]);
    renderText(textPlacement, gRenderer);
    TTF_CloseFont(font);
    return;
}
