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
int IdFromPort(Uint16 port);
bool isInside(int x, int y, SDL_Rect* r);
int udpListener(void* data);

void createAndSendUDPPackets(Ship ships[8],Bullet bullets[MAX_BULLETS]);
void moveBullets(Bullet bullets[MAX_BULLETS]);
void moveShips(Ship ships[MAX_CLIENTS]);
void updateShip(Ship* ship);
void addBullet(Ship* ship);
int findFreeBullet(Bullet bullets[MAX_BULLETS]);

UDPsocket udpSendSock, udpRecvSock;
Client clients[MAX_CLIENTS];
Bullet bullets[MAX_BULLETS];
Ship ships[MAX_CLIENTS];
SDL_Surface* background;

//byt från globala!!
UDPpacket *packetOut;
Uint8 gameData[1000];
int packetID;
bool gameIsActive;
