#include "globalvariables.h"

Client client;
GameBackground gameBackground;
Ship ship[MAX_PLAYERS];
Bullet bullet[MAX_BULLETS];

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
char playerReadyStr[2];
SDL_Rect readyIcon;
//char name[MAX_NAME_LENGTH]; *********************************************************************

SDL_Thread* TCPthread;
SDL_Thread* UDPthread;
IPaddress ip;                           // Contains the information (host + port) for the server to connect to

bool throttle;
bool right;
bool left;
bool shooting;
Uint8 pressedButtons;
UDPpacket* outPacket;
