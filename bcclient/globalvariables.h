#ifndef GLOBALVARIABLES
#define GLOBALVARIABLES

#include <SDL.h>
#include <SDL_net.h>
#include <stdbool.h>
#include "definitions.h"

typedef struct{
    int id;
    TCPsocket TCPSock;
    UDPsocket UDPRecvSock, UDPSendSock;
    Uint16 ServerRecvUDPPort;
    int score;      //**************************************************************************
} Client;
extern Client client;

typedef struct{
	int x, y; // w, h,  *****************************************************
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

extern bool isConnected;                       // True when connected to a lobby or game

extern char textString[6][STRINGLENGTH];       // Used for both chat messages and for example IP addresses and ports
extern int textStringColor[6];                 // Decides clors for text messages
extern int textStringCurrent[6];               // Which index in the string are we currently addressing?
extern int tempColor;

extern char defaultIP[15];                     // The IP and port used to connect to the default server
extern char defaultPort[5];

extern char playerName[MAX_PLAYERS][MAX_NAME_LENGTH];
extern bool playerReady[8];
extern char playerReadyStr[2];
extern SDL_Rect readyIcon;
//char name[MAX_NAME_LENGTH]; *********************************************************************

extern SDL_Thread* chatRecv;
extern IPaddress ip;                           // Contains the information (host + port) for the server to connect to

#endif // GLBALVARIABLES
