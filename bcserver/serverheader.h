#ifndef SERVERHEADER_H_INCLUDED
#define SERVERHEADER_H_INCLUDED

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#ifdef _WIN32
    #include <SDL.h>
    #include <SDL_image.h>
    #include <SDL_net.h>
#elif __APPLE__
    #include <SDL2/SDL.h>
    #include <SDL2_Image/SDL_Image.h>
    #include <SDL2_Net/SDL_Net.h>
#else
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_image.h>
    #include <SDL2/SDL_net.h>
#endif

#define STAGE_HEIGHT 1600
#define STAGE_WIDTH 2400
#define SHIP_HEIGHT 31
#define SHIP_WIDTH 23
#define MAX_BULLETS 1000
#define MAX_BULLETS_ON_SCREEN 250
#define SCREEN_HEIGHT 720
#define SCREEN_WIDTH 1280
#define GAME_AREA_HEIGHT 720
#define GAME_AREA_WIDTH 1030
#define MAX_LENGTH 100
#define MAX_CLIENTS 8
#define PREAMBLE_CHAT "$"
#define PREAMBLE_CONN "$3"
#define PREAMBLE_DISC "$1"
#define PREAMBLE_PLAYERS "@"
#define PREAMBLE_READY '#'
#define PREAMBLE_TOGGLEBOT '?'
#define PREAMBLE_OPTIONS '*'
#define PREAMBLE_DISCONNECT "-"
#define PREAMBLE_GAMEFREEZE "¤"
#define PREAMBLE_GAMEEND "="
#define PREAMBLE_KILLED "}"
#define PREAMBLE_POWERUP "+"
#define POWERUP_MULTI3X 0
#define POWERUP_MULTI2X 1
#define POWERUP_BLACKHOLE 2
#define POWERUP_TIMEWARP 3
#define POWERUP_DOUBLEDAMAGE 4
#define POWERUP_TELEPORT 5
#define TIMEWARP_DURATION 10000
#define POWERUP_SPAWNRATE 10000
#define SHIP_POWERUP_DURATION 10000         // Duration for non instant powerups gained by ships, such as double/triple shot or double damage
#define TELEPORT_DURATION 3000
#define MAX_ALLOWED_POWERUP_SPAWNPOINTS 15
#define READY "1"
#define NOT_READY "0"
#define SHIP_TEXTURE "ship.png"
#define BACKGROUND_TEXTURE "cave.png"
#define PLAYER_TYPE_HUMAN 0
#define PLAYER_TYPE_BOT 1
#define TOGGLE_BULLETINTERVAL 1
#define TOGGLE_INFINITEMOMENTUM 2
#define TOGGLE_MAXSPEED 3
#define TOGGLE_GAMELENGTH 4
#define MAX_HEALTH 100
#define RESPAWN_TIME_MS 10000

typedef struct{
    TCPsocket socket;
    Uint16 sendPort,recvPort;
    char name[MAX_LENGTH];
    int score;
    int id;
    bool active;
    bool ready;
    Uint32 ipadress;
    int playerType;
    int viewportID;
}Client;

typedef struct{
    SDL_Surface* surface;
    int bulletIntervall,bulletCooldown;
    double xPos,yPos,xVel,yVel;
    short angle,angleVel;
    Uint8 health;
    bool acceleration,shooting, isDead, isStunned, isTeleporting, teleportDisplacementPerformed;    // isTeleporting is for the whole teleport process, teleportDisplacementPerformed is the actual displacement
    int deathTimer,deathTimerStart;
    short activePowerup;
    int powerupTimerStart, stunDurationStart;
}Ship;

typedef struct{
    double xPos,yPos,xVel,yVel;
    short type;
    bool active;
    short source;
}Bullet;

typedef struct{
    Uint16 x, y;
}PlayerSpawnPoint;

typedef struct{
    Uint16 x,y;
    bool isActive;
    short type;
}PowerupSpawnPoint;

extern bool init();
extern bool initGame();
extern void closeServer();
extern void acceptConnection();
extern int getClientId();
extern bool ClientsAreReady();
extern void SendAndgetPort(int id);
extern int Lobby (void * data);
extern bool checkConnection(int id, char TCPrecv[]);
extern void clearString(char message[]);
extern void broadCast(char TCPsend[]);
extern void clearReturn(char message[]);
extern void sendMessageExc(char TCPsend[], int id);
extern void activePlayers();
extern void disconnect(int id);
extern double getRadians(int a);
extern int IdFromPort(Uint32 ip);
extern bool isInside(int x, int y, SDL_Rect* r);
extern int udpListener(void* data);
extern int fetchCPUname(void);
extern void updateBots(void);
extern void handleBot(int id);
extern int getDelta(int p1, int p2);
extern int getObjectDistance(int deltaX, int deltaY);
extern float getObjectAngle(int deltaX, int deltaY);
extern void removeBOT(int *id);
extern void fetchMapData(void);
extern void checkShipHealth(void);
extern void handlePowerupSpawns(void);
extern void handlePowerupGains(void);
extern void handleActivePowerups(void);
//------------ game.c --------------------------------------------------------------
extern void createAndSendUDPPackets(Ship ships[8],Bullet bullets[MAX_BULLETS]);
extern void moveBullets(Bullet bullets[MAX_BULLETS]);
extern void moveShips(Ship ships[MAX_CLIENTS]);
extern void updateShip(Ship* ship);
extern void addBullet(Ship* ship, int *id);
extern int findFreeBullet(Bullet bullets[MAX_BULLETS]);

extern UDPsocket udpSendSock, udpRecvSock;
extern Client clients[MAX_CLIENTS];
extern Bullet bullets[MAX_BULLETS];
extern Ship ships[MAX_CLIENTS];
extern PlayerSpawnPoint playerSpawnPoint[MAX_CLIENTS];
extern PowerupSpawnPoint powerupSpawnPoint[];
extern SDL_Surface* background;

extern UDPpacket *packetOut;
extern Uint8 gameData[1000];
extern int packetID;
extern bool gameIsActive;
extern bool computerPlayerActive[MAX_CLIENTS];
extern int computerPlayerCount;
extern int humanPlayerCount;
extern int GameFreezeTime;

extern int activeGameLength; // Choses which entry from the list below that's active in terms of game-length
extern int gameLenghtList[6];
extern int activeMaxSpeed; // Choses which entry from the list below that's active in terms of max-speed
extern int MaxSpeedList[5];
extern int activeBulletInterval;// Choses which entry from the list below that's active in terms of bullet-interval
extern int bulletIntervalList[3];
extern bool infiniteMomentum;

extern short numberOfPowerups;
extern short activePowerupSpawns;
extern int powerupSpawnTimerStart;
extern bool timeWarpIsActive;
extern int timeWarpStartTime;
extern int timeWarpFreq;
#endif // SERVERHEADER_H_INCLUDED
