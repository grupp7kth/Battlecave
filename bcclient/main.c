#include "includes.h"

int handleEvent(SDL_Event *event, SDL_Rect buttonPlacement[], int *select, int *mode, int modeMaxButtons[], int *keyboardMode, bool *quit);
void initModeMaxButtons(int modeMaxButtons[]);

int main(int argc, char* args[]){
    bool quit = false;
    int select = -1;
    int modeMaxButtons[6];
    keyboardMode = -1;
    isConnected = false;

    for(int i=0; i < MAX_PLAYERS; i++)
        playerReady[i] = 0;

    SDL_Event event;
    SDL_Rect buttonPlacement[MAXBUTTONS];   // Interactable buttons
    SDL_Rect windowPlacement[3];            // Window backgrounds

    initSDL();
    loadMedia();
    initModeMaxButtons(modeMaxButtons);
    setWindows(windowPlacement);
    clearTextStrings(11);
    //SDL_StartTextInput();

    strcpy(defaultIP, "83.250.47.232"); //83.250.47.232
    strcpy(defaultPort, "4444");

    while(!quit){
        while (SDL_PollEvent(&event))
            handleEvent(&event, buttonPlacement, &select, &mode, modeMaxButtons, &keyboardMode, &quit);

        renderScreen(&mode, &select, buttonPlacement, windowPlacement);
        SDL_Delay(10);
    }

    closeRenderer();
    return 0;
}

int handleEvent(SDL_Event *event, SDL_Rect buttonPlacement[], int *select, int *mode, int modeMaxButtons[], int *keyboardMode, bool *quit){
    if(event->type == SDL_QUIT)
        *quit = true;
    else if(event->type == SDL_MOUSEBUTTONDOWN || event->type == SDL_MOUSEMOTION)
        checkMouse(event, buttonPlacement, select, mode, modeMaxButtons, keyboardMode, quit);
    else if(event->type == SDL_KEYDOWN || event->type == SDL_KEYUP) //|| event->type == SDL_TEXTINPUT)
        checkKeypress(event, mode, select);
    return 0;
}

void initModeMaxButtons(int modeMaxButtons[]){  // How many buttons does mode N have?
    modeMaxButtons[STARTUP]       = 3;
    modeMaxButtons[FIND_SERVERS]  = 3;
    modeMaxButtons[OPTIONS]       = 0;          // UNUSED ATM
    modeMaxButtons[LOBBY]         = 3;
    modeMaxButtons[JOIN_DEFAULT]  = 2;
    modeMaxButtons[JOIN_CUSTOM]   = 5;
    return;
}

