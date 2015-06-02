#include "includes.h"

void handleChat(char TCPTextIn[]);
void handleNames(char TCPTextIn[]);
void handleReady(char TCPTextIn[]);
void handleGameStart(void);
void handleBots(char TCPTextIn[]);
void handleFreezeTime(char TCPTextIn[]);
void handleGameOptions(char TCPTextIn[]);
void handleDeath(char TCPTextIn[]);
void handlePowerup(char TCPTextIn[]);
void handleGameEnd(void);
void shiftString(char string[], int steps);

int TCPhandler(Client* client){
    Client* me;
    me = (Client*)client;

    char TCPTextIn[STRINGLENGTH];
    int connectionCheck = 1;

    for(;;){
        connectionCheck = SDLNet_TCP_Recv(me->TCPSock, TCPTextIn, STRINGLENGTH);
        if(!connectionCheck)
            SDLNet_TCP_Close(me->TCPSock);

        // Determine the message's information by its preamble
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
            handleGameEnd();
        else if(TCPTextIn[0] == PREAMBLE_OPTIONS)
            handleGameOptions(TCPTextIn);
        else if(TCPTextIn[0] == PREAMBLE_KILLED)
            handleDeath(TCPTextIn);
        else if(TCPTextIn[0] == PREAMBLE_POWERUP)
            handlePowerup(TCPTextIn);

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

    // Move previous chat messages up to make room for the new one at the bottom
    for(int i=0; i < 4; i++){
        clearTextString(i);
        clearTextString(i+6);
        strcpy(textString[i], textString[i+1]);
        strcpy(textString[i+6], textString[i+7]);
        chatMessageRecvTime[i] = chatMessageRecvTime[i+1];
        textStringColor[i] = textStringColor[i+1];
    }

    // Fetch and place the new message
    clearTextString(4);
    clearTextString(10);
    for(int i=0; TCPTextIn[0] != ':' && i <= STRINGLENGTH; i++){
        textString[4][i] = TCPTextIn[0];
        shiftString(TCPTextIn, 1);
    }
    strcpy(textString[10], TCPTextIn);
    textStringColor[4] = tempColor;
    chatMessageRecvTime[4] = SDL_GetTicks();
    return;
}

void handleNames(char TCPTextIn[]){
    // Receiving the names of the player's in the lobby upon joining and when someone else joins/leaves
    int id;
    id = TCPTextIn[1] - 48;
    shiftString(TCPTextIn, 2);
    clearPlayerNameString(id);
    strcat(playerName[id], TCPTextIn);
    return;
}

void handleReady(char TCPTextIn[]){
    // Changing of people's ready statuses in a lobby
    playerReady[TCPTextIn[1]-48] = (TCPTextIn[2] - 48);
    return;
}

void handleGameStart(void){
    // When all players in a lobby are ready and the game starts
    mode = IN_GAME;
    keyboardMode = PLAYING;
    clearTextStrings(11);
    gameFreezeTime = 4;
    client.deathTimer = 0;
    client.activePowerup = -1;
    playersInGame = 0;                       // Count how many players there are
    for(int i=0; i < MAX_PLAYERS; i++){
        playerScore[i] = 0;                  // Reset all players' scores
        ship[client.id].health = 100;        // To prevent potential flickering just as the game starts
        if(strlen(playerName[i]) > 0)
            playersInGame++;
    }
    return;
}

void handleBots(char TCPTextIn[]){
    // Change the active bot status on IDs
    computerPlayerActive[TCPTextIn[1]-48] = !computerPlayerActive[TCPTextIn[1]-48];

//    for(int i=0; i<8;i++)
//        printf("STATUS %d: %d\n", i, computerPlayerActive[i]);
    return;
}

void handleFreezeTime(char TCPTextIn[]){
    // Freeze time which occurs before the game starts while in game
    gameFreezeTime--;
    if(gameFreezeTime == -1)
        gameTimeStart = SDL_GetTicks();
    return;
}

void handleGameEnd(){
    // If the player stays until the game is over (countdown has run out)
    isConnected = false;
    mode = SCORE_SCREEN;
    return;
}

void handleGameOptions(char TCPTextIn[]){
    // The options that are changeable from the lobby screen by the players
    if(TCPTextIn[1]-48 == TOGGLE_BULLETINTERVAL)
        activeBulletInterval = TCPTextIn[2]-48;
    else if(TCPTextIn[1]-48 == TOGGLE_MAXSPEED)
        activeMaxSpeed = TCPTextIn[2]-48;
    else if(TCPTextIn[1]-48 == TOGGLE_GAMELENGTH)
        activeGameLength = TCPTextIn[2]-48;
    return;
}

void handleDeath(char TCPTextIn[]){
    // When the player dies in game
	killedID = TCPTextIn[1] - 48;
	killerID = TCPTextIn[2] - 48;
    if (killedID == client.id){
	    client.deathTimer = 10000;
	    deathTimerStart = SDL_GetTicks();
    }
    if(killedID != killerID)            // Increase the killer's score only if they didn't kill themselves
        playerScore[killerID]++;
    timedTextStart = SDL_GetTicks();
    timedTextID = 6;
    return;
}

void handlePowerup(char TCPTextIn[]){
    // When the player gains a powerup in game
    if(TCPTextIn[1]-48 == POWERUP_TIMEWARP){
        timeWarpStart = SDL_GetTicks();
        timeWarpIsActive = true;
    }
    else if(TCPTextIn[1]-48 == POWERUP_MULTI3X || TCPTextIn[1]-48 == POWERUP_MULTI2X ||
            TCPTextIn[1]-48 == POWERUP_DOUBLEDAMAGE || TCPTextIn[1]-48 == POWERUP_TELEPORT){
        client.activePowerup = TCPTextIn[1] - 48;
        client.powerupTimerStart = SDL_GetTicks();
    }
    timedTextStart = SDL_GetTicks();
    timedTextID = TCPTextIn[1] - 48;
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
