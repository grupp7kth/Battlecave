#ifndef INCLUDES_H
#define INCLUDES_H

#ifdef _WIN32
 #include <SDL.h>
 #include <SDL_net.h>
 #include <SDL_image.h>
 #include <SDL_ttf.h>
#elif __APPLE__
 #include <SDL2/SDL.h>
 #include <SDL2_Net/SDL_Net.h>
 #include <SDL2_image/SDL_image.h>
 #include <SDL2_ttf/SDL_ttf.h>
#endif

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "definitions.h"

//***********************************************************************************************
//  ___ _   _ _  _  ___ _____ ___ ___  _  _   ___ ___  ___ _____ ___ _______   _____ ___ ___    *
// | __| | | | \| |/ __|_   _|_ _/ _ \| \| | | _ \ _ \/ _ \_   _/ _ \_   _\ \ / / _ \ __/ __|   *
// | _|| |_| | .` | (__  | |  | | (_) | .` | |  _/   / (_) || || (_) || |  \ V /|  _/ _|\__ \   *
// |_|  \___/|_|\_|\___| |_| |___\___/|_|\_| |_| |_|_\\___/ |_| \___/ |_|   |_| |_| |___|___/   *
//                                                                                              *
//***********************************************************************************************

// ********************************    RENDERSCREEN.C    ****************************************

// renderScreen - Renders the screen, based on mode
void renderScreen(int *mode, int *select, SDL_Rect buttonPlacement[], SDL_Rect windowPlacement[]);

// loadMedia - Loads all media required
void loadMedia(void);

// initSDL - Initiates SDL; creates the window etc
void initSDL(void);

// closeRenderer - Closes all render-related items
void closeRenderer(void);

// ********************************    SETBUTTONS.C    ******************************************

// setButtons - Determines which buttons are active and enters their placement information into 'buttonPlacement[]' depending on the mode
void setButtons(SDL_Rect buttonPlacement[], int *mode);

// ********************************    SETWINDOWS.C    ******************************************

// setWindows - Initates locations for windows that are unclickable
void setWindows(SDL_Rect windowPlacement[]);

// ********************************    SETTEXT.C    *********************************************

// setText - Initates locations and text messages depending on mode and renders them
void setText(int *mode, SDL_Renderer* gRenderer, int *select);

// ********************************    INPUTHANDLER.C    ****************************************

// checkMouse - Determines whether the mouse is placed and/or clicking on anything of interest
void checkMouse(SDL_Event *event, SDL_Rect buttonPlacement[], int *select, int *mode, int modeMaxButtons[], int *keyboardMode, bool *quit);

// checkKeypress - Handles a keypress depending on the situation
void checkKeypress(SDL_Event *event, int *mode, int *select);


// ********************************    CLEARSTRINGS.C    ****************************************

// clearStrings - Sets all elements of the strings to NULL
void clearTextStrings(int total);

void clearTextString(int id);

void clearPlayerNameString(int id);

// ********************************    GLOBALVARIABLES.C    *************************************

typedef struct{
    int id;
    TCPsocket TCPSock;
    UDPsocket UDPRecvSock, UDPSendSock;
    Uint16 ServerRecvUDPPort;
    int score;      //**************************************************************************
} Client;
extern Client client;

typedef struct{
	int x, y, w, h;
	short angle;
	bool active, blown;
	SDL_Texture* texture;
	SDL_Rect placement;
} Ship;
extern Ship ship[MAX_PLAYERS];

typedef struct{
	int x, y;
	short type; // ***********************************************************************
} Bullet;
extern Bullet bullet[MAX_BULLETS];

typedef struct{
	int w, h;
    SDL_Texture* texture;                      // Width, Height
	SDL_Rect source;
} GameBackground;
extern GameBackground gameBackground;

extern int mode;                               // Modes as specified in "definitions.h"
extern int keyboardMode;

extern bool isConnected;                       // True when connected to a lobby or game

extern char textString[11][STRINGLENGTH];      // Used for both chat messages and for example IP addresses and ports
extern int textStringColor[11];                // Decides colors for text messages
extern int textStringCurrent[11];              // Which index in the string are we currently addressing?

extern char defaultIP[15];                     // The IP and port used to connect to the default server
extern char defaultPort[5];

extern char playerName[MAX_PLAYERS][MAX_NAME_LENGTH];
extern bool playerReady[8];
extern char playerReadyStr[2];
extern SDL_Rect readyIcon;
//char name[MAX_NAME_LENGTH]; *********************************************************************

extern SDL_Thread* TCPthread;
extern SDL_Thread* UDPthread;
extern IPaddress ip;                           // Contains the information (host + port) for the server to connect to

extern bool throttle;
extern bool right;
extern bool left;
extern bool shooting;
extern Uint8 pressedButtons;
extern UDPpacket* outPacket;
extern UDPpacket* inPacket;

extern int currentBulletAmmount;               // Number of bullets that were sent to us to be drawn on the screen

// ********************************    TCPHANDLER.C    ******************************************

int TCPhandler(Client* client);

// ********************************    UDPHANDLER.C    ******************************************

int UDPhandler(void);

// **************************** END OF FUNCTION PROTOTYPES **************************************
#endif // INCLUDES_H
