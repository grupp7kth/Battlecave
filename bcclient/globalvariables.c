#include "includes.h"

Mix_Music* music[5] = {NULL};
int currentSong = 1;

Client client;
Ship ship[MAX_PLAYERS];
Bullet bullet[MAX_BULLETS];
GameBackground gameBackground;
SideBar sideBar;
MiniMap miniMap;

int mode;                               // Modes as specified in "definitions.h"
int keyboardMode;
bool isConnected;                       // True when connected to a lobby or game

char textString[11][STRINGLENGTH];      // Used for both chat messages and for example IP addresses and ports
int textStringColor[11];                // Decides colors for text messages
int textStringCurrent[11];              // Which index in the string are we currently addressing?

char defaultIP[15];                     // The IP and port used to connect to the default server
char defaultPort[5];

char playerName[MAX_PLAYERS][MAX_NAME_LENGTH];
bool playerReady[8];
SDL_Rect readyIcon;
bool computerPlayerActive[8] = {false};

SDL_Thread* TCPthread;
SDL_Thread* UDPthread;
IPaddress ip;                               // Contains the information (host + port) for the server to connect to

int gameTimeStart;
int gameTimeRemaining;

bool throttle;
bool right;
bool left;
bool shooting;
short spectatingID;
Uint8 pressedButtons;
UDPpacket* outPacket;
UDPpacket* inPacket;

int currentBulletAmmount;                   // Number of bullets that were sent to us to be drawn on the screen

int gameFreezeTime;                         // Used to disallow player's keypresses in-game, -1 = Game Active, 0 = Game Just Started, 1-3 = N seconds left
int activeGameLength;                       // Choses which entry from the list below that's active in terms of game-length
int gameLengthList[6] = {5, 10, 15,
                         20, 25, 30};
int activeMaxSpeed;                         // Choses which entry from the list below that's active in terms of max-speed
int maxSpeedList[5] = {3, 5, 7, 10, 15};
bool infiniteMomentum;
int activeBulletInterval;                   // Choses which entry from the list below that's active in terms of bullet-interval
int bulletIntervalList[3] = {5, 10, 15};

SDL_Color colorsRGB[14] = {{225, 225, 255},     // 0  = White
                           {255, 255, 150},     // 1  = Bright Yellow
                           {0, 0, 0},           // 2  = Black
                           {100, 255, 100},     // 3  = Bright Green
                           {255, 75, 75},       // 4  = Bright Red
                           {45, 198, 250},      // 5  = Teal
                           {255,0,0},           // 6  = Red        * Colors 6-13 are used to represent players with ID 0-7, respectively
                           {0,0,255},           // 7  = Blue
                           {0,255,0},           // 8  = Green
                           {255,255,0},         // 9  = Yellow
                           {128,0,255},         // 10 = Purple
                           {255,128,0},         // 11 = Orange
                           {255,128,192},       // 12 = Pink
                           {128,64,0}};         // 13 = Brown

Uint8 colorsPlayer[24] = {255,0,0,         //   0-2  = Red
                          0,0,255,         //   3-5  = Blue
                          0,255,0,         //   6-8  = Green
                          255,255,0,       //  9-11  = Yellow
                          128,0,255,       // 12-14 = Purple
                          255,128,0,       // 15-17 = Orange
                          255,128,192,     // 18-20 = Pink
                          128,64,0};       // 21-23 = Brown
