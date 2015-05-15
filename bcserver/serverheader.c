#include "serverheader.h"

bool init();
bool initGame();
void closeServer();
void acceptConnection();
int getClientId();
bool ClientsAreReady();
void SendAndgetPort(int id);
int Lobby (void * data);
bool checkConnection(int id, char TCPrecv[]);
void clearString(char message[]);
void broadCast(char TCPsend[]);
void clearReturn(char message[]);
void sendMessageExc(char TCPsend[], int id);
void activePlayers();
void disconnect(int id);
double getRadians(int a);
int IdFromPort(Uint32 ip);
bool isInside(int x, int y, SDL_Rect* r);
int udpListener(void* data);
int fetchCPUname(void);
void updateBots(void);
void handleBot(int id);
int getDelta(int p1, int p2);
int getObjectDistance(int deltaX, int deltaY);
float getObjectAngle(int deltaX, int deltaY);
void removeBOT(int *id);
void fetchMapData(void);
void checkShipHealth();
void createAndSendUDPPackets(Ship ships[8],Bullet bullets[MAX_BULLETS]);
void moveBullets(Bullet bullets[MAX_BULLETS]);
void moveShips(Ship ships[MAX_CLIENTS]);
void updateShip(Ship* ship);
void addBullet(Ship* ship, int *id);
int findFreeBullet(Bullet bullets[MAX_BULLETS]);

UDPsocket udpSendSock, udpRecvSock;
Client clients[MAX_CLIENTS];
Bullet bullets[MAX_BULLETS];
Ship ships[MAX_CLIENTS];
PlayerSpawnPoint playerSpawnPoint[MAX_CLIENTS];
SDL_Surface* background;

//byt från globala!!
UDPpacket *packetOut;
Uint8 gameData[1000];
int packetID;
bool gameIsActive;
bool computerPlayerActive[MAX_CLIENTS];
int computerPlayerCount;
int humanPlayerCount;
int GameFreezeTime;

int activeGameLength = 1;               // Choses which entry from the list below that's active in terms of game-length
int gameLenghtList[6] = {5, 10, 15,
                         20, 25, 30};
int activeMaxSpeed = 1;                 // Choses which entry from the list below that's active in terms of max-speed
int MaxSpeedList[5] = {3, 5, 7, 10, 15};
int activeBulletInterval = 1;           // Choses which entry from the list below that's active in terms of bullet-interval
int bulletIntervalList[3] = {5, 10, 15};
bool infiniteMomentum = false;
