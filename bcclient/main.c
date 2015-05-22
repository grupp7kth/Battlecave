#include "includes.h"

int handleEvent(SDL_Event *event, SDL_Rect buttonPlacement[], int *select, int *mode, int modeMaxButtons[], int *keyboardMode, bool *quit);
void initModeMaxButtons(int modeMaxButtons[]);
void init(void);
void closeMixer(void);

int main(int argc, char* args[]){
    bool quit = false;
    int select = -1;
    int modeMaxButtons[7];

    SDL_Event event;
    SDL_Rect buttonPlacement[MAXBUTTONS];   // Interactable buttons
    SDL_Rect windowPlacement[4];            // Window backgrounds

    init();
    initSDL();
    loadMedia();
    initModeMaxButtons(modeMaxButtons);
    setWindows(windowPlacement);
    clearTextStrings(11);
    //SDL_StartTextInput();

    while(!quit){
        while (SDL_PollEvent(&event))
            handleEvent(&event, buttonPlacement, &select, &mode, modeMaxButtons, &keyboardMode, &quit);
        renderScreen(&mode, &select, buttonPlacement, windowPlacement);

        soundHandler();
        SDL_Delay(10);
    }

    closeRenderer();
    closeMixer();
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
    modeMaxButtons[STARTUP]       =  3;
    modeMaxButtons[FIND_SERVERS]  =  3;
    modeMaxButtons[OPTIONS]       =  6;
    modeMaxButtons[LOBBY]         = 16;
    modeMaxButtons[JOIN_DEFAULT]  =  2;
    modeMaxButtons[JOIN_CUSTOM]   =  5;
    modeMaxButtons[IN_GAME]       =  1;
    return;
}

void init(void){
    keyboardMode = -1;
    isConnected = false;

    for(int i=0; i < MAX_PLAYERS; i++)
        playerReady[i] = 0;

    SDL_Init(SDL_INIT_AUDIO);
    Mix_Init(MIX_INIT_MP3);

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1){
        printf("SDL Mixer Open Audio Failed! (%s)\n", Mix_GetError());
        exit(0);
    }

    music[0] = Mix_LoadMUS("resources/music/Battlecave1.mp3");
    music[1] = Mix_LoadMUS("resources/music/Battlecave2.mp3");
    music[2] = Mix_LoadMUS("resources/music/Battlecave3.mp3");
    music[3] = Mix_LoadMUS("resources/music/Battlecave4.mp3");
    music[4] = Mix_LoadMUS("resources/music/Battlecave5.mp3");

    if(music[0] == NULL || music[1] == NULL || music[2] == NULL || music[3] == NULL || music[4] == NULL){
        printf("SDL Mixer Load Music Failed! (%s)\n", Mix_GetError());
        exit(0);
    }

    Mix_VolumeMusic(MIX_MAX_VOLUME/2);

    for(int i=0; i < MAX_ALLOWED_POWERUP_SPAWNPOINTS; i++){
        powerupSpawnPoint[i].placement.w = 30;
        powerupSpawnPoint[i].placement.h = 30;
    }

    healthBar.x = GAME_AREA_WIDTH + 24;
    healthBar.y = 391;
    healthBar.h = 25;
    powerupBar.x = GAME_AREA_WIDTH + 24;
    powerupBar.y = 445;
    powerupBar.h = 25;

    timedTextID = -1;           // Not active

    // Load client settings from 'settings.txt' file
    for(int i=0; i < 16; i++)
        defaultIP[i] = '\0';
    for(int i=0; i < 6; i++)
        defaultPort[i] = '\0';

    FILE *fp;
    fp = fopen("settings.txt","r");
    if(fp != NULL){
        char tempStr[16];
        int i;
        for(i = 0; i <= 15 && tempStr[i-1] != '\n'; i++){ // Load default IP address
            tempStr[i] = fgetc(fp);
        }
        tempStr[i-1] = '\0';
        strcpy(defaultIP, tempStr);

        for(i = 0; i <= 5 && tempStr[i-1] != '\n'; i++){ // Load default port
            tempStr[i] = fgetc(fp);
        }
        tempStr[i-1] = '\0';
        strcpy(defaultPort, tempStr);

        namesBelowShipsEnabled = fgetc(fp) - 48;
        fgetc(fp);                                      // Remove newline
        fancyBackgroundEnabled = fgetc(fp) - 48;
        fgetc(fp);                                      // Remove newline
        musicEnabled = fgetc(fp) - 48;
    }
    fclose(fp);

    checkBox.w = 15;
    checkBox.h = 15;

    return;
}

void closeMixer(void){
    for(int i=0; i < 5; i++)
        Mix_FreeMusic(music[i]);
    Mix_CloseAudio();
    return;
}
