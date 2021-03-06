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
void setButtons(SDL_Rect buttonPlacement[], SDL_Rect windowPlacement[], int *mode);

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
    int deathTimer;
    int powerupTimerStart;
	short activePowerup, shipAlpha;
} Client;
extern Client client;

typedef struct{
	int x, y, w, h;
	short angle;
	bool active, isDead;
	SDL_Texture* texture;
	SDL_Rect placement;
    int fuel, ammo, health;
} Ship;
extern Ship ship[MAX_PLAYERS];

typedef struct{
	int x, y;
	short source;
} Bullet;
extern Bullet bullet[MAX_BULLETS];

typedef struct{
	int w, h;
    SDL_Texture* texture;
	SDL_Rect source;
	SDL_Rect dest;
	SDL_Rect mapPreviewPlacement;
} GameMapBackground;
extern GameMapBackground gameMapBackground;

typedef struct{
    SDL_Texture* texture;
    SDL_Rect source;
    SDL_Rect dest;
} GameLowLayerBackground;
extern GameLowLayerBackground gameLowLayerBackground;

typedef struct{
    SDL_Texture* texture;
	SDL_Rect placement;
} SideBar;
extern SideBar sideBar;

typedef struct{
	SDL_Rect mapPlacement;
    SDL_Texture* playerTexture[MAX_PLAYERS];
	SDL_Rect playerPlacement[MAX_PLAYERS];
	SDL_Texture* powerupTexture;
	SDL_Rect powerupPlacement;
} MiniMap;
extern MiniMap miniMap;

typedef struct{
    Uint16 x,y;
    bool isActive;
    short type;
    SDL_Rect placement;
} PowerupSpawnPoint;
extern PowerupSpawnPoint powerupSpawnPoint[MAX_ALLOWED_POWERUP_SPAWNPOINTS];

typedef struct{
    SDL_Rect framePlacement;
    SDL_Texture* frameTexture;
    SDL_Rect barPlacement;
} BelowShipHealthBar;
extern BelowShipHealthBar belowShipHealthBar;

extern SDL_Rect healthBar;
extern SDL_Rect powerupBar;

extern int mode;                               // Modes as specified in "definitions.h"
extern int keyboardMode;

extern bool isConnected;                       // True when connected to a lobby or game

extern char textString[11][STRINGLENGTH];      // Used for both chat messages and for example IP addresses and ports
extern int textStringColor[11];                // Decides colors for text messages
extern int textStringCurrent[11];              // Which index in the string are we currently addressing?
extern int chatMessageRecvTime[5];             // Used to track when a chat message is received so that they can stop being displayed after a while to make the game screen less cluttered

extern char defaultIP[16];                     // The IP and port used to connect to the default server
extern char defaultPort[6];
extern bool namesBelowShipsEnabled;            // Show names below their ships game while in enabled
extern bool healthBelowOwnShipEnabled;
extern bool healthBelowEnemyShipsEnabled;      // Shows small health-bars below ships in game while enabled
extern bool fancyBackgroundEnabled;            // Far background behind the map game while in enabled
extern bool musicEnabled;
extern SDL_Rect checkBox;

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
extern int timedTextStart;
extern short timedTextID;
extern short killerID, killedID;                   // killedID = ship that just got killed; killerID = the killer
extern int playerScore[MAX_PLAYERS];               // Contains the players' current scores
extern int scoreID[MAX_PLAYERS];                   // The rankings of the players, by ID
extern int playersInGame;

extern bool throttle;
extern bool right;
extern bool left;
extern bool shooting;
extern short viewportID;
extern Uint8 pressedButtons;
extern Uint8 lastPressedButtons;
extern UDPpacket* outPacket;
extern UDPpacket* inPacket;

extern int currentBulletAmmount;                   // Number of bullets that were sent to us to be drawn on the screen

extern int gameFreezeTime;                         // Used to disallow player's keypresses in-game, -1 = Game Active, 0 = Game Just Started, 1-3 = N seconds left
extern int activeGameLength;                       // Choses which entry from the list below that's active in terms of game-length
extern int gameLengthList[6];
extern int activeMaxSpeed;                         // Choses which entry from the list below that's active in terms of max-speed
extern int maxSpeedList[5];
extern int activeBulletInterval;                   // Choses which entry from the list below that's active in terms of bullet-interval
extern int bulletIntervalList[3];

extern bool timeWarpIsActive;                      // Whether the timewarp powerup is active for the game
extern int timeWarpStart;
extern SDL_Rect timeWarpRect;

extern SDL_Color colorsRGB[14];                    // SDL_Color format RGB
extern Uint8 colorsPlayer[24];                     // Split colors (3 entries per color: R/G/B)
extern char powerupNames[6][14];                   // Contains all the powerup's names

// ********************************    TCPHANDLER.C    ******************************************

int TCPhandler(Client* client);

// ********************************    UDPHANDLER.C    ******************************************

int UDPhandler(void);

// ********************************    SOUND.C    ***********************************************

void soundHandler(void);

// **************************** END OF FUNCTION PROTOTYPES **************************************
#endif // INCLUDES_H
