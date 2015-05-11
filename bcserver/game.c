#include "serverheader.h"

double getRadians(int a) {
	return (a+90)*3.14159265/180;
}
bool isInside(int x, int y, SDL_Rect* r) {
	return (x >= r->x && x < (r->x+r->w) && y >= r->y && y < (r->y+r->h));
}
int IdFromPort(Uint16 port) {
	for (int i=0; i<MAX_CLIENTS; i++) {
		if (clients[i].active && clients[i].sendPort==port) return i;
	}
	return -1;
}
bool initGame() {
    for (int i=0; i<MAX_CLIENTS; i++) {
		ships[i].surface = NULL;
		ships[i].xPos =0;
		ships[i].yPos =0;
		ships[i].xVel =0;
		ships[i].yVel =0;
		ships[i].bulletIntervall = 10;
		ships[i].bulletCooldown = 0;
		ships[i].angleVel = 0;
		ships[i].angle = 0;
		ships[i].acceleration = false;
		ships[i].shooting = false;
		ships[i].alive = false;
	}
	ships[0].xPos = 400;
	ships[0].yPos = 300;
	ships[0].bulletIntervall =10;
	ships[0].bulletCooldown=0;
	ships[1].xPos = 500;
	ships[1].yPos = 400;
	ships[1].xVel = 0;
	ships[1].angleVel = -3;
	ships[2].xPos = 1000;
	ships[2].yPos = 1200;
	ships[2].angleVel = +5;
	ships[3].xPos = 2000;
	ships[3].yPos = 1200;
	ships[3].yVel = -2;
	ships[4].xPos = 10;
	ships[4].yPos = 10;
	ships[4].xVel = 2;
	ships[4].yVel = 2;
	ships[5].xPos = 1200;
	ships[5].yPos = 800;
	ships[5].xVel = 1.5;
	ships[5].yVel = -1.5;
    return true;
}
int udpListener(void* data) {

	UDPpacket *packetIn;
	packetIn = SDLNet_AllocPacket(16);
	short clientId=0, key=0;
	puts("udplistener startad.");
	while(1){
        if(SDLNet_UDP_Recv(udpRecvSock,packetIn)) {

            printf("data:%d\n",packetIn->data[0]);

        }
    }
    while (1) {
        if (SDLNet_UDP_Recv(udpRecvSock,packetIn)) {
            if ((clientId = IdFromPort(packetIn->address.port)) < 0 ) {
                printf("error packet/client conflict");
            }

            key = packetIn->data[0];
            if ((key & 3) == 1) ships[clientId].angleVel=5;
            else if ((key & 3) == 2) ships[clientId].angleVel=-5;
            else ships[clientId].angleVel = 0;
            
            if ((key & 4) == 4) ships[clientId].acceleration=true;
            else ships[clientId].acceleration=false;
            
            if ((key & 8) == 8) ships[clientId].shooting=true;
            else ships[clientId].shooting=false;
        }
		SDL_Delay(5);
	}
	return 0;
}

//int findFreeBullet(bullet skotten[MAX_BULLETS])
//void updateShip(serverShip* skeppet)
//void addBullet(serverShip* skeppet)
//void createAndSendUDPPackets(serverShip skeppen[8],bullet skotten[MAX_BULLETS])
//void moveBullets(bullet skotten[MAX_BULLETS])
//void moveShips(serverShip skeppen[MAX_CLIENTS])
