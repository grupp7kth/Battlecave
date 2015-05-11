<<<<<<< Updated upstream
#include "includes.h"
=======
#include <SDL2/SDL.h>
#include <SDL2_Image/SDL_Image.h>
#include <SDL2_Net/SDL_Net.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tcphandler.h"
#include "definitions.h"
#include "clearstrings.h"
#include "globalvariables.h"
>>>>>>> Stashed changes

void handleChat(char TCPTextIn[]);
void handleNames(char TCPTextIn[]);
void handleReady(char TCPTextIn[]);
void handleGameStart(void);
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
    printf("ATTEMPTINT TO START GAME...\n"); //********************************************************************************
    mode = IN_GAME;
    keyboardMode = PLAYING;
    clearTextStrings(11);
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
