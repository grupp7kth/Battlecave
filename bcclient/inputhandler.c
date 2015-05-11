<<<<<<< Updated upstream
#include "includes.h"
=======
<<<<<<< Updated upstream
=======
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2_Image/SDL_Image.h>
#include <SDL2_Net/SDL_Net.h>
#include <SDL2_ttf/SDL_ttf.h>
>>>>>>> Stashed changes
#include "inputhandler.h"
>>>>>>> Stashed changes

static void checkMouseMode0(SDL_Event *event, SDL_Point *currentMouseLocation, SDL_Rect buttonPlacement[], int *select, int *mode, int modeMaxButtons[], bool *match, bool *quit);
static void checkMouseMode1(SDL_Event *event, SDL_Point *currentMouseLocation, SDL_Rect buttonPlacement[], int *select, int *mode, int modeMaxButtons[], bool *match);
static void checkMouseMode3(SDL_Event *event, SDL_Point *currentMouseLocation, SDL_Rect buttonPlacement[], int *select, int *mode, int modeMaxButtons[], bool *match, int *keyboardMode);
static void checkMouseMode4(SDL_Event *event, SDL_Point *currentMouseLocation, SDL_Rect buttonPlacement[], int *select, int *mode, int modeMaxButtons[], bool *match, int *keyboardMode);
static void checkMouseMode5(SDL_Event *event, SDL_Point *currentMouseLocation, SDL_Rect buttonPlacement[], int *select, int *mode, int modeMaxButtons[], bool *match, int *keyboardMode);
static bool mouseOnButton(SDL_Point *currentMouseLocation, SDL_Rect buttonPlacement[], int *i);

void handleBackspace(int id);
void addCharToString(int id, int maxLen, SDL_Event *event);
void joinLobby(int *mode);
int resolveIPPortFromStrings(void);
void handleChatInput(SDL_Event* event);

//**************************************************************************
//                                                                         *
//                          Mouse Handling                                 *
//                                                                         *
//**************************************************************************

void checkMouse(SDL_Event *event, SDL_Rect buttonPlacement[], int *select, int *mode, int modeMaxButtons[], int *keyboardMode, bool *quit){
    SDL_Point currentMouseLocation;
    SDL_GetMouseState(&currentMouseLocation.x, &currentMouseLocation.y);

    bool match = false;

    if(*mode == STARTUP)
        checkMouseMode0(event, &currentMouseLocation, buttonPlacement, select, mode, modeMaxButtons, &match, quit);
    else if(*mode == FIND_SERVERS)
        checkMouseMode1(event, &currentMouseLocation, buttonPlacement, select, mode, modeMaxButtons, &match);
    else if(*mode == LOBBY)
        checkMouseMode3(event, &currentMouseLocation, buttonPlacement, select, mode, modeMaxButtons, &match, keyboardMode);
    else if(*mode == JOIN_DEFAULT)
        checkMouseMode4(event, &currentMouseLocation, buttonPlacement, select, mode, modeMaxButtons, &match, keyboardMode);
    else if(*mode == JOIN_CUSTOM)
        checkMouseMode5(event, &currentMouseLocation, buttonPlacement, select, mode, modeMaxButtons, &match, keyboardMode);


    if((*mode == STARTUP || *mode == FIND_SERVERS) && !match)
        *select = -1;

    return;
}

//**************************************************************************
//*                   MODE 0 (Startup Screen)                              *
//**************************************************************************
static void checkMouseMode0(SDL_Event *event, SDL_Point *currentMouseLocation, SDL_Rect buttonPlacement[], int *select, int *mode, int modeMaxButtons[], bool *match, bool *quit){
    for(int i=0; i < modeMaxButtons[0]; i++){
        if(mouseOnButton(currentMouseLocation, buttonPlacement, &i)){ // Is the mouse on button 'i' ?
            *select = i;
            *match = true;
            if(event->type == SDL_MOUSEBUTTONDOWN){
                if(i == 2){     // EXIT
                    *quit = true;
                }
                else if(i == 1) // OPTIONS
                    printf("OPTIONS\n");
                else{           // FIND SERVERS
                    *mode = FIND_SERVERS;
                }
            }
        }
    }
    return;
}
//**************************************************************************
//*                   MODE 1 (Find Servers Screen)                         *
//**************************************************************************
static void checkMouseMode1(SDL_Event *event, SDL_Point *currentMouseLocation, SDL_Rect buttonPlacement[], int *select, int *mode, int modeMaxButtons[], bool *match){
    for(int i=0; i < modeMaxButtons[1]; i++){
        if(mouseOnButton(currentMouseLocation, buttonPlacement, &i)){ // Is the mouse on button 'i' ?
            *select = i;
            *match = true;
            if(event->type == SDL_MOUSEBUTTONDOWN){
                if(i == 2){         // BACK
                    *mode = STARTUP;
                }
                else if(i == 1){     // JOIN CUSTOM SERVER
                    *mode = JOIN_CUSTOM;
                    *select = ENTERING_IP;
                }
                else{               // JOIN DEFAULT SERVER
                    *mode = JOIN_DEFAULT;
                    *select = ENTERING_NAME;
                }
            }
        }
    }
    return;
}

//**************************************************************************
//*                   MODE 3 Lobby Screen                                  *
//**************************************************************************
static void checkMouseMode3(SDL_Event *event, SDL_Point *currentMouseLocation, SDL_Rect buttonPlacement[], int *select, int *mode, int modeMaxButtons[], bool *match, int *keyboardMode){
    for(int i=0; i < modeMaxButtons[3]; i++){
        if(mouseOnButton(currentMouseLocation, buttonPlacement, &i)){ // Is the mouse on button 'i' ?
            if(event->type == SDL_MOUSEBUTTONDOWN){
                *keyboardMode = ENTERING_TEXT;

                if(i == 2)                  // Ready
                    SDLNet_TCP_Send(client.TCPSock, "#", strlen("#"));
                else if(i == 1){            // Leave
                    isConnected = false;
                    SDLNet_TCP_Send(client.TCPSock, "-", strlen("-"));
                    SDLNet_TCP_Close(client.TCPSock);
                    SDLNet_UDP_Close(client.UDPRecvSock);
                    SDLNet_UDP_Close(client.UDPSendSock);

                    for(int i=0; i < MAX_PLAYERS; i++)
                        playerReady[i] = 0;

                    clearTextStrings(11);
                    printf("LEFT; '-' sent to server, socket closed, ready statuses cleared, textstrings cleared, mode changed\n"); //*****************************************
                    *mode = FIND_SERVERS;
                }                           // (ELSE: Enter Chat Message Window Pressed)
            }
        }
    }
}
//**************************************************************************
//*                   MODE 4 (Join Default Server Screen)                  *
//**************************************************************************
static void checkMouseMode4(SDL_Event *event, SDL_Point *currentMouseLocation, SDL_Rect buttonPlacement[], int *select, int *mode, int modeMaxButtons[], bool *match, int *keyboardMode){
    for(int i=0; i < modeMaxButtons[4]; i++){
        if(mouseOnButton(currentMouseLocation, buttonPlacement, &i)){ // Is the mouse on button 'i' ?
            if(event->type == SDL_MOUSEBUTTONDOWN){
                if(i == 1)          // GO!
                    joinLobby(mode);
                else                // Close
                    *mode = FIND_SERVERS;
            }
        }
    }
}
//**************************************************************************
//*                   MODE 5 (Join Custom Server Screen)                   *
//**************************************************************************
static void checkMouseMode5(SDL_Event *event, SDL_Point *currentMouseLocation, SDL_Rect buttonPlacement[], int *select, int *mode, int modeMaxButtons[], bool *match, int *keyboardMode){
    for(int i=0; i < modeMaxButtons[5]; i++){
        if(mouseOnButton(currentMouseLocation, buttonPlacement, &i)){ // Is the mouse on button 'i' ?
            if(event->type == SDL_MOUSEBUTTONDOWN){
                *keyboardMode = NONE;

                if(i == 4){         // GO!
                    joinLobby(mode);
                }
                else if(i == 3){    // Close
                    *mode = FIND_SERVERS;
                }
                else{               // Enter IP _OR_ Port _OR_ Name Field
                    *select = i;
                    *keyboardMode = ENTERING_TEXT;
                    *match = true;
                }
            }
        }
    }
    return;
}
//**************************************************************************



//**************************************************************************
//                                                                         *
//                          KeyPress Handling                              *
//                                                                         *
//**************************************************************************

void checkKeypress(SDL_Event *event, int *mode, int *select){
//    if(*keyboardMode < 0)
//        return;

//******************* MODE : JOIN DEFAULT SERVER *****************************
    if(*mode == JOIN_DEFAULT && event->type == SDL_KEYDOWN){
        if(event->key.keysym.sym == SDLK_RETURN){
            joinLobby(mode);
        }
        else{
            if(event->key.keysym.sym == SDLK_BACKSPACE)
                handleBackspace(ENTERING_NAME);
            else
                addCharToString(ENTERING_NAME, MAX_IP_LENGTH, event);
        }
    }

//******************* MODE : JOIN CUSTOM SERVER *****************************
    else if(*mode == JOIN_CUSTOM && event->type == SDL_KEYDOWN){
        if(event->key.keysym.sym == SDLK_TAB){                      // Tab will change the window that the user is currently entering text to
            (*select)++;
            if(*select > 2)
                *select = 0;
        }
        else if(event->key.keysym.sym == SDLK_RETURN){
            joinLobby(mode);
        }


        else{
            if(*select == ENTERING_IP){
                if(event->key.keysym.sym == SDLK_BACKSPACE)
                    handleBackspace(ENTERING_IP);
                else
                    addCharToString(ENTERING_IP, MAX_IP_LENGTH, event);
            }

            else if(*select == ENTERING_PORT){
                if(event->key.keysym.sym == SDLK_BACKSPACE)
                    handleBackspace(ENTERING_PORT);
                else
                    addCharToString(ENTERING_PORT, MAX_PORT_LENGTH, event);
            }

            else if(*select == ENTERING_NAME){
                if(event->key.keysym.sym == SDLK_BACKSPACE)
                    handleBackspace(ENTERING_NAME);
                else
                    addCharToString(ENTERING_NAME, MAX_NAME_LENGTH, event);

            }
        }
    }
//******************* MODE : IN LOBBY ************************************
    else if(*mode == LOBBY && event->type == SDL_KEYDOWN){ // If we're in a lobby
        handleChatInput(event);
    }
//******************* MODE : IN GAME *************************************
    else if(*mode == IN_GAME){
        if(keyboardMode == PLAYING){
            if(event->type == SDL_KEYDOWN){
                if(event->key.keysym.sym == SDLK_UP)
                    throttle = true;
                else if(event->key.keysym.sym == SDLK_LEFT)
                    left = true;
                else if(event->key.keysym.sym == SDLK_RIGHT)
                    right = true;
                else if(event->key.keysym.sym == SDLK_SPACE)
                    shooting = true;
            }
            else if(event->type == SDL_KEYUP){
                if(event->key.keysym.sym == SDLK_UP)
                    throttle = false;
                else if(event->key.keysym.sym == SDLK_LEFT)
                    left = false;
                else if(event->key.keysym.sym == SDLK_RIGHT)
                    right = false;
                else if(event->key.keysym.sym == SDLK_SPACE)
                    shooting = false;
                else if(event->key.keysym.sym == SDLK_RETURN)
                    keyboardMode = ENTERING_TEXT;
            }
            pressedButtons = ((int)right | (int)left << 1 | (int)throttle << 2 | (int)shooting << 3);
        }
        else if(keyboardMode == ENTERING_TEXT && event->type == SDL_KEYUP)
            handleChatInput(event);
    }
//**********************************************************************************
    return;
}

void handleBackspace(int id){
    if(textStringCurrent[id] > 0){
        textStringCurrent[id]--;
        textString[id][textStringCurrent[id]] = '\0';
    }
    return;
}

void addCharToString(int id, int maxLen, SDL_Event *event){
    if(textStringCurrent[id] < maxLen){
        textString[id][textStringCurrent[id]] = event->key.keysym.sym;
        textStringCurrent[id]++;
    }
    return;
}


static bool mouseOnButton(SDL_Point *currentMouseLocation, SDL_Rect buttonPlacement[], int *i){
    if(currentMouseLocation->x >= buttonPlacement[*i].x && currentMouseLocation->x <= (buttonPlacement[*i].x + buttonPlacement[*i].w) &&
       currentMouseLocation->y >= buttonPlacement[*i].y && currentMouseLocation->y <= (buttonPlacement[*i].y + buttonPlacement[*i].h))
        return true;
    else
        return false;
}

void joinLobby(int *mode){
    // Translate the strings the user entered to the address we'll use
    bool success;
    success = resolveIPPortFromStrings();
    if(!success)
        return;

    // Open the TCP Socket
    client.TCPSock = SDLNet_TCP_Open(&ip);
    if(client.TCPSock == NULL){
        printf("TCP Open Failure\n");
        //exit(EXIT_FAILURE);
        return;
    }
    // Send the name the user entered to the server
    SDLNet_TCP_Send(client.TCPSock, textString[ENTERING_NAME], MAX_NAME_LENGTH);

    // Open the UDP Sockets; Recieve & Send
    client.UDPRecvSock = SDLNet_UDP_Open(0);
    client.UDPSendSock = SDLNet_UDP_Open(0);
    if(client.UDPRecvSock == NULL || client.UDPSendSock == NULL){
        printf("Opening of UDP sockets failed!\n");
        exit(EXIT_FAILURE);
    }

    // Send both of the clients UDP ports and Recieve the servers recieve port
    int sendPort,recvPort;
    SDLNet_TCP_Recv(client.TCPSock,&(recvPort),sizeof(int));
    client.ServerRecvUDPPort = recvPort;
    sendPort = SDLNet_UDP_GetPeerAddress(client.UDPSendSock,-1)->port;
    SDLNet_TCP_Send(client.TCPSock,&(sendPort),sizeof(int));
    recvPort = SDLNet_UDP_GetPeerAddress(client.UDPRecvSock,-1)->port;
    SDLNet_TCP_Send(client.TCPSock,&(recvPort),sizeof(int));

    // Recieve the ID within the game that the client was assigned by the server
    SDLNet_TCP_Recv(client.TCPSock, &client.id, sizeof(int));

    // Start the TCP thread
    TCPthread = SDL_CreateThread(TCPhandler, "BC TCP Thread", &client);
    SDL_DetachThread(TCPthread);
    isConnected = true;

    // Start the UDP thread
    UDPthread = SDL_CreateThread(UDPhandler, "BC UDP Thread", &client);
    SDL_DetachThread(UDPthread);

    *mode = LOBBY;
    clearTextStrings(6);
    return;
}

int resolveIPPortFromStrings(void){
    int test;
    Uint16 temp;

    if(mode == JOIN_DEFAULT){
        temp = atoi(defaultPort);
        test = SDLNet_ResolveHost(&ip, defaultIP, temp);
    }
    else{
        temp = atoi(textString[ENTERING_PORT]);
        test = SDLNet_ResolveHost(&ip, textString[ENTERING_IP], temp);
    }

    if(test < 0){
        printf("IP ADDRESS RESOLVE FAILED\n");  // **************************************************************************************
        return 0;
    }
    return 1;
}

void handleChatInput(SDL_Event* event){
    if(event->key.keysym.sym == SDLK_BACKSPACE)
        handleBackspace(PLAYER_MESSAGE_WRITE);
    else if(event->key.keysym.sym == SDLK_RETURN){
        if(strlen(textString[PLAYER_MESSAGE_WRITE]) > 0){
            SDLNet_TCP_Send(client.TCPSock, textString[PLAYER_MESSAGE_WRITE], STRINGLENGTH);
            clearTextString(PLAYER_MESSAGE_WRITE);
            if(mode == IN_GAME)
                keyboardMode = PLAYING;
        }
    }
    else
        addCharToString(PLAYER_MESSAGE_WRITE, 40, event);
    return;
}
