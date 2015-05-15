#include "includes.h"

void handleChat(char TCPTextIn[]);
void handleNames(char TCPTextIn[]);
void handleReady(char TCPTextIn[]);
void handleGameStart(void);
void handleBots(char TCPTextIn[]);
void handleFreezeTime(char TCPTextIn[]);
void handleGameOptions(char TCPTextIn[]);
void handleDeath(void);
void shiftString(char string[], int steps);

int TCPhandler(Client* client){
    Client* me;
    me = (Client*)client;

    char TCPTextIn[STRINGLENGTH];
    int connectionCheck = 1;

    for(;;){
        connectionCheck = SDLNet_TCP_Recv(me->TCPSock, TCPTextIn, STRINGLENGTH);
        if(!connectionCheck){
            printf("Client ID: %d has disconnected!\n", client->id); // *******************************************************************************'
            SDLNet_TCP_Close(me->TCPSock);
        }

        puts(TCPTextIn); //*********************************************************************************************************

        if(TCPTextIn[0] == PREAMBLE_CHAT)
            handleChat(TCPTextIn);
        else if(TCPTextIn[0] == PREAMBLE_PLAYERS)
            handleNames(TCPTextIn);
        else if(TCPTextIn[0] == PREAMBLE_READY)
            handleReady(TCPTextIn);
        else if(TCPTextIn[0] == PREAMBLE_GAMESTART)
            handleGameStart();
        else if(TCPTextIn[0] == PREAMBLE_TOGGLEBOT)
            handleBots(TCPTextIn);
        else if(TCPTextIn[0] == PREAMBLE_GAMEFREEZE)
            handleFreezeTime(TCPTextIn);
        else if(TCPTextIn[0] == PREAMBLE_GAMEEND)
            handleLeave();
        else if(TCPTextIn[0] == PREAMBLE_OPTIONS)
            handleGameOptions(TCPTextIn);
        else if(TCPTextIn[1] == PREAMBLE_KILLED)
            handleDeath();
        if(!isConnected)
            break;
    }
    return 0;
}

void handleChat(char TCPTextIn[]){
    /*  Each line of chat consists of two strings in order to use different colors for the player's names and the messages themselves
        The string parities are as follows:
        (String ID [From Top to Bottom]: Name String ID - Message String ID)
        0: 0 -  6
        1: 1 -  7
        2: 2 -  8
        3: 3 -  9
        4: 4 - 10

        (Hence the constant for name-message relationship is +6)
    */
    int tempColor;

    tempColor = TCPTextIn[1] - 48;
    shiftString(TCPTextIn, 2);

    clearTextString(0);
    clearTextString(6);
    strcpy(textString[0], textString[1]);
    strcpy(textString[6], textString[7]);
    textStringColor[0] = textStringColor[1];

    clearTextString(1);
    clearTextString(7);
    strcpy(textString[1], textString[2]);
    strcpy(textString[7], textString[8]);
    textStringColor[1] = textStringColor[2];

    clearTextString(2);
    clearTextString(8);
    strcpy(textString[2], textString[3]);
    strcpy(textString[8], textString[9]);
    textStringColor[2] = textStringColor[3];

    clearTextString(3);
    clearTextString(9);
    strcpy(textString[3], textString[4]);
    strcpy(textString[9], textString[10]);
    textStringColor[3] = textStringColor[4];

    clearTextString(4);
    clearTextString(10);
    for(int i=0; TCPTextIn[0] != ':' && i <= STRINGLENGTH; i++){
        textString[4][i] = TCPTextIn[0];
        shiftString(TCPTextIn, 1);
    }
    strcpy(textString[10], TCPTextIn);
    textStringColor[4] = tempColor;
    return;
}

void handleNames(char TCPTextIn[]){
    int id;
    id = TCPTextIn[1] - 48;
    shiftString(TCPTextIn, 2);
    clearPlayerNameString(id);
    strcat(playerName[id], TCPTextIn);
    return;
}

void handleReady(char TCPTextIn[]){
    playerReady[TCPTextIn[1]-48] = (TCPTextIn[2] - 48);
    return;
}

void handleGameStart(void){
    printf("ATTEMPTING TO START GAME...\n"); //********************************************************************************
    mode = IN_GAME;
    keyboardMode = PLAYING;
    clearTextStrings(11);
    gameFreezeTime = 4;
    client.health = 100;
    client.deathTimer = 0;
    return;
}

void handleBots(char TCPTextIn[]){
    computerPlayerActive[TCPTextIn[1]-48] = !computerPlayerActive[TCPTextIn[1]-48];

//    for(int i=0; i<8;i++) //*************************************************************************************
//        printf("STATUS %d: %d\n", i, computerPlayerActive[i]);
    return;
}

void handleFreezeTime(char TCPTextIn[]){
    gameFreezeTime--;
    if(gameFreezeTime == -1)
        gameTimeStart = SDL_GetTicks();
    return;
}

void handleGameOptions(char TCPTextIn[]){
    if(TCPTextIn[1]-48 == TOGGLE_BULLETINTERVAL)
        activeBulletInterval = TCPTextIn[2]-48;
    else if(TCPTextIn[1]-48 == TOGGLE_INFINITEMOMENTUM)
        infiniteMomentum = TCPTextIn[2]-48;
    else if(TCPTextIn[1]-48 == TOGGLE_MAXSPEED)
        activeMaxSpeed = TCPTextIn[2]-48;
    else if(TCPTextIn[1]-48 == TOGGLE_GAMELENGTH)
        activeGameLength = TCPTextIn[2]-48;

    //printf("BI=%d, IM=%d, MS=%d, GL=%d\n", bulletIntervalList[activeBulletInterval], infiniteMomentum, maxSpeedList[activeMaxSpeed], gameLengthList[activeGameLength]); //***********************************************************
    return;
}

void handleDeath(void){
    puts("YOU DIED LOL");puts("YOU DIED LOL");puts("YOU DIED LOL");puts("YOU DIED LOL");puts("YOU DIED LOL");puts("YOU DIED LOL");puts("YOU DIED LOL");puts("YOU DIED LOL");puts("YOU DIED LOL");
    client.deathTimer = 10000;
    deathTimerStart = SDL_GetTicks();
    return;
}

void shiftString(char string[], int steps){
    int temp;
    temp = strlen(string);

    for(int i=0; i < STRINGLENGTH - steps; i++){
        string[i] = string[i+steps];
    }
    string[temp] = '\0';
    return;
}
