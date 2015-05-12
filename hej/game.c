#include "serverheader.h"

double getRadians(int a) {
	return (a+90)*3.14159265/180;
}
bool isInside(int x, int y, SDL_Rect* r) {
	return (x >= r->x && x < (r->x+r->w) && y >= r->y && y < (r->y+r->h));
}
int IdFromPort(Uint16 port,Uint32 host) {
	for (int i=0; i<MAX_CLIENTS; i++) {
		if (clients[i].active && clients[i].sendPort==port && clients[i].ip==host) return i;
	}
	return -1;
}
bool initGame() {
    for (int i=0; i<MAX_CLIENTS; i++) {
		ships[i].surface = NULL;
		ships[i].xPos =400+(i*10);
		ships[i].yPos =400+(i*10);
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
	return true;
}
int udpListener(void* data) {

	UDPpacket *packetIn;
	packetIn = SDLNet_AllocPacket(16);
	short clientId=0, keysPressed=0;
	puts("udplistener startad.");
	while (1) {
        if (SDLNet_UDP_Recv(udpRecvSock,packetIn)) {
            if ((clientId = IdFromPort(packetIn->address.port,packetIn->address.host)) < 0 ) {
                printf("error packet/client conflict");
            }
            //printf("data:%d\n",packetIn->data[0]);
            keysPressed = packetIn->data[0];
            if ((keysPressed & 3) == 1) ships[clientId].angleVel=5;
            else if ((keysPressed & 3) == 2) ships[clientId].angleVel=-5;
            else ships[clientId].angleVel = 0;
            if ((keysPressed & 4) == 4) ships[clientId].acceleration=true;
            else ships[clientId].acceleration=false;
            if ((keysPressed & 8) == 8) ships[clientId].shooting=true;
            else ships[clientId].shooting=false;

        }
		SDL_Delay(5);
	}
	return 0;
}
int newBullet() {
	for (int i=0; i<MAX_BULLETS; i++) {
		if (!bullets[i].active) return i;
	}
	return -1;
}
void updateBullets() {
	for (int i=0; i<MAX_BULLETS; i++) {
		if (bullets[i].active) {
			bullets[i].xPos += bullets[i].xVel;
			bullets[i].yPos += bullets[i].yVel;
			if (bullets[i].xPos > STAGE_WIDTH || bullets[i].xPos < 0 || bullets[i].yPos<0 || bullets[i].yPos>STAGE_HEIGHT) {
				bullets[i].active=false;
			}
		}
	}
}
void updateShips() {
	for (int i=0; i<MAX_CLIENTS; i++) {
		if (ships[i].acceleration) {
			ships[i].yVel-=sin(getRadians(ships[i].angle))*0.1;
			ships[i].xVel-=cos(getRadians(ships[i].angle))*0.1;
		}
		if (ships[i].shooting) {
			if (ships[i].bulletCooldown ==0) {
				addBullet(&ships[i]);
				ships[i].bulletCooldown = ships[i].bulletIntervall;
			}
		}
		ships[i].xPos += ships[i].xVel;
		ships[i].yPos += ships[i].yVel;
		if (ships[i].xPos > STAGE_WIDTH) ships[i].xPos=0;
		if (ships[i].xPos <0) ships[i].xPos=STAGE_WIDTH;
		if (ships[i].yPos > STAGE_HEIGHT) ships[i].yPos=0;
		if (ships[i].yPos <0) ships[i].yPos=STAGE_HEIGHT;
		ships[i].angle += ships[i].angleVel;
		if (ships[i].angle > 360) ships[i].angle-=360;
		if (ships[i].angle < 0) ships[i].angle+=360;
		if (ships[i].bulletCooldown>0) ships[i].bulletCooldown--;
	}
}
void addBullet(Ship* ship) {
	int i = newBullet();
	if (i <0) {
		puts("No bullets left!");
		return;
	}
	bullets[i].xPos=ship->xPos;
	bullets[i].yPos=ship->yPos;
	bullets[i].yVel=ship->yVel-sin(getRadians(ship->angle))*6;
	bullets[i].xVel=ship->xVel-cos(getRadians(ship->angle))*6;
	bullets[i].active = true;

}
void createAndSendUDPPackets() {
    SDL_Rect viewport;
	viewport.w = SCREEN_WIDTH;
	viewport.h = SCREEN_HEIGHT;

	int i, player, secondary, counter,UDPpacketLength;
	Uint32 tmp;

	packetId++;
	for (i=0; i<4; i++) {
		gameData[i] = packetId >> i*8;
	}
	for (player=0; player<MAX_CLIENTS; player++) {
		tmp=0;
		tmp = (int)ships[player].xPos | (int)ships[player].yPos << 12 | (int)(ships[player].angle/6) << 24;
		tmp = tmp | ships[player].alive <<30 | clients[player].active << 31;
		printf("tmp:%u\n",tmp);
		for (i=0; i<4; i++) {
			gameData[4+player*4+i] = tmp >> i*8;
		}
	}
	printf("__________________________\n");
	for (player=0; player<MAX_CLIENTS; player++) {
		if (!clients[player].active) continue;
		if (ships[player].xPos < SCREEN_WIDTH/2)
			viewport.x=0;
		else if (ships[player].xPos > STAGE_WIDTH-SCREEN_WIDTH/2)
			viewport.x=STAGE_WIDTH-SCREEN_WIDTH;
		else viewport.x=ships[player].xPos-SCREEN_WIDTH/2;
		if (ships[player].yPos < SCREEN_HEIGHT/2)
			viewport.y=0;
		else if (ships[player].yPos > STAGE_HEIGHT-SCREEN_HEIGHT/2)
			viewport.y=STAGE_HEIGHT-SCREEN_HEIGHT;
		else viewport.y=ships[player].yPos-SCREEN_HEIGHT/2;

		for (secondary=0, counter=0; secondary<MAX_BULLETS; secondary++) {
			if (bullets[secondary].active && isInside((int)bullets[secondary].xPos, (int)bullets[secondary].yPos, &viewport)) {
				tmp=0;
				tmp = (int)(bullets[secondary].xPos-viewport.x) | (int)(bullets[secondary].yPos-viewport.y) << 11 | (int)(bullets[secondary].type) << 21;
				for (i=0; i<3; i++) {
					gameData[36+counter*3+i] = tmp >> i*8;
				}
				counter++;
			}
		}
		UDPpacketLength = 36+counter*3;
		gameData[UDPpacketLength++]=0xFF;
		packetOut->data = gameData;
		packetOut->len = UDPpacketLength;
		packetOut->address.host=clients[player].ip;
		packetOut->address.port=clients[player].recvPort;
		SDLNet_UDP_Send(udpSendSock,-1,packetOut);
	}
	printf("xPos:%d    yPos:%d     active:%d \n",(int)ships[0].xPos, (int)ships[0].yPos,(int)clients[0].active);
}
