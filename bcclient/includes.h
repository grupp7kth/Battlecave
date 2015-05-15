#ifndef INCLUDES_H
#define INCLUDES_H

#ifdef _WIN32
 #include <SDL.h>
 #include <SDL_net.h>
 #include <SDL_image.h>
 #include <SDL_ttf.h>
 #include <SDL_mixer.h>
#elif __APPLE__
 #include <SDL2/SDL.h>
 #include <SDL2_Net/SDL_Net.h>
 #include <SDL2_image/SDL_image.h>
 #include <SDL2_ttf/SDL_ttf.h>
 #include <SDL2_mixer/SDL_mixer.h>
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

void handleLeave(void);

// ********************************    CLEARSTRINGS.C    ****************************************

// clearStrings - Sets all elements of the strings to NULL
void clearTextStrings(int total);

void clearTextString(int id);

void clearAllPlayerNameStrings(int total);

void clearPlayerNameString(int id);

// ********************************    GLOBALVARIABLES.C    *************************************

extern Mix_Music* music[5];
extern int currentSong;

typedef struct{
    int id;
    TCPsocket TCPSock;
    UDPsocket UDPRecvSock, UDPSendSock;
    Uint16 ServerRecvUDPPort;
    short health;
    int deathTimer;
} Client;
extern Client client;

typedef struct{
	int x, y, w, h;
	short angle;
	bool active, isDead;
	SDL_Texture* texture;
	SDL_Rect placement;
    int score;      //**************************************************************************
} Ship;
extern Ship ship[MAX_PLAYERS];

typedef struct{
	int x, y;
	short type; // ***********************************************************************
	short source;
} Bullet;
extern Bullet bullet[MAX_BULLETS];

typedef struct{
	int w, h;
    SDL_Texture* texture;
	SDL_Rect source;
	SDL_Rect dest;
} GameBackground;
extern GameBackground gameBackground;

typedef struct{
    SDL_Texture* texture;
	SDL_Rect placement;
} SideBar;
extern SideBar sideBar;

typedef struct{
	SDL_Rect mapPlacement;
    SDL_Texture* playerTexture[MAX_PLAYERS];
	SDL_Rect playerPlacement[MAX_PLAYERS];
} MiniMap;
extern MiniMap miniMap;

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
extern SDL_Rect readyIcon;
extern bool computerPlayerActive[8];

extern SDL_Thread* TCPthread;
extern SDL_Thread* UDPthread;
extern IPaddress ip;                           // Contains the information (host + port) for the server to connect to

extern int gameTimeStart;
extern int gameTimeRemaining;
extern int deathTimerStart;

extern bool throttle;
extern bool right;
extern bool left;
extern bool shooting;
extern short spectatingID;
extern Uint8 pressedButtons;
extern UDPpacket* outPacket;
extern UDPpacket* inPacket;

extern int currentBulletAmmount;                   // Number of bullets that were sent to us to be drawn on the screen

extern int gameFreezeTime;                         // Used to disallow player's keypresses in-game, -1 = Game Active, 0 = Game Just Started, 1-3 = N seconds left
extern int activeGameLength;                       // Choses which entry from the list below that's active in terms of game-length
extern int gameLengthList[6];
extern int activeMaxSpeed;                         // Choses which entry from the list below that's active in terms of max-speed
extern int maxSpeedList[5];
extern bool infiniteMomentum;
extern int activeBulletInterval;                   // Choses which entry from the list below that's active in terms of bullet-interval
extern int bulletIntervalList[3];

extern SDL_Color colorsRGB[14];                    // SDL_Color format RGB
extern Uint8 colorsPlayer[24];                     // Split colors (3 entries per color: R/G/B)

// ********************************    TCPHANDLER.C    ******************************************

int TCPhandler(Client* client);

// ********************************    UDPHANDLER.C    ******************************************

int UDPhandler(void);

// ********************************    SOUND.C    ***********************************************

void soundHandler(void);

// **************************** END OF FUNCTION PROTOTYPES **************************************
#endif // INCLUDES_H
