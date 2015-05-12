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
/** uppdaterar ett skepps position baserat p} hastighet.
 @var skeppet: Det skepp som ska uppdateras.
 */ /// alkdjlaksjdlaskjd
void updateShip(Ship ships[MAX_CLIENTS]) {
    for (int i; i<MAX_CLIENTS; i++) {
        if (ships->acceleration) {
            ships->yVel-=sin(getRadians(ships->angle))*0.1;
            ships->xVel-=cos(getRadians(ships->angle))*0.1;
        }
        if (ships->shooting) {
            if (ships->bulletCooldown ==0) {
                addBullet(ships);
                ships->bulletCooldown = ships->bulletIntervall;
            }
        }
        ships->xPos += ships->xVel;
        ships->yPos += ships->yVel;
        if (ships->xPos > STAGE_WIDTH) ships->xPos=0;
        if (ships->xPos <0) ships->xPos=STAGE_WIDTH;
        if (ships->yPos > STAGE_HEIGHT) ships->yPos=0;
        if (ships->yPos <0) ships->yPos=STAGE_HEIGHT;
        ships->angle += ships->angleVel;
        if (ships->angle > 360) ships->angle-=360;
        if (ships->angle < 0) ships->angle+=360;
        if (ships->bulletCooldown>0) ships->bulletCooldown--;
    }
}

/** L{gger till ett skott i spelet och r{knar ut dess hastighet.
 * Fult nog {r arrayen 'serverSkott' global.
 @var skeppet: Det serverShip som just skjutit.
 */

void addBullet(Ship* ship) {
    int freeSpot = findFreeBullet(bullets);
    if (freeSpot <0) {
        puts("FEL: Hittade inte ledig plats i skottlistan!");
        exit(1);
    }
    bullets[freeSpot].xPos=ship->xPos;
    bullets[freeSpot].yPos=ship->yPos;
    bullets[freeSpot].yVel=ship->yVel-sin(getRadians(ship->angle))*6;
    bullets[freeSpot].xVel=ship->xVel-cos(getRadians(ship->angle))*6;
    bullets[freeSpot].active = true;
    //	printf("Bam[%d]: %f,%f\n",antalSkott,serverSkott[antalSkott].xPos,serverSkott[antalSkott].yPos);
}
/** Hittar en ledig plats i arrayen av skott
 @var skotten: Arrayen av skott f|r spelet.
 @return: ett ID som motsvarar en ledig position i arrayen.
 */
int findFreeBullet(Bullet bullets[MAX_BULLETS]) {
    int i;
    for (i=0; i<MAX_BULLETS; i++) {
        if (!bullets[i].active) return i;
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
    while (1) {
        if (SDLNet_UDP_Recv(udpRecvSock,packetIn)) {
            if ((clientId = IdFromPort(packetIn->address.port)) < 0 ) {
                printf("error packet/client conflict"); }

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

/** skapar en en Uint8-array (dataToClient) av alla skeppspositioner och skottpositioner.
 * Denna skr{ddarsys för varje klient, eftersom den bara skickar de skottpositioner
 * som klienten ska se p} sin skärm. Rent konkret inneh}ller varje paket:
 * Byte 0–3 (4 bytes) en int som {r en tidsangivelse (paketnumrering).
 * Byte 4–35 (32 bytes) information om det skeppen (4 bytes * 8 skepp).
 * Byte 36– (max –935) Upp till 300 positioner (3 bytes var).
 *	Alla dessa {r skottpositioner f|r klientens sk{rm, plus 4 kontrollbitar per skott.
 *	Metoden kontrollerar vilka skott som ska vara med p} spelarens sk{rm, och skickar positionen
 *	endast f|r detta skott.
 *	Efter sista relevanta skottet skickar den en byte '11111111' som markerar slutet.
 *	Arrayen {r allts} 937 bytes stor (0–936).
 * Fult nog {r dataToClient global f|r tillf{llet.
 @var skeppen: en serverShip-array av de skepp som finns i spelet.
 @var skotten: en serverBullet-array av de skott som finns i spelet.
 */
void createAndSendUDPPackets(Ship ships[8],Bullet bullets[MAX_BULLETS]) {
    SDL_Rect viewport;
    viewport.w = SCREEN_WIDTH;
    viewport.h = SCREEN_HEIGHT;
    int i, player, secondary, counter,UDPpacketLength;
    Uint32 tempint;
    
    packetID++; // Det h{r {r tidsangivelsen. Den b|rjar p} 0 och |kar med 1 f|r varje paket.
    
    for (i=0; i<4; i++) {
        gameData[i] = packetID >> i*8;
    }
    for (player=0; player<MAX_CLIENTS; player++) {
        tempint=0;
        tempint = (int)ships[player].xPos | (int)ships[player].yPos << 12 | (int)(ships[player].angle/6) << 24;
        tempint = tempint | ships[player].alive <<30 | clients[player].active << 31;
        for (i=0; i<4; i++) {
            gameData[4+player*4+i] = tempint >> i*8;
        }
    }
    // Nu har metoden lagt in informationen om paketnummer och alla skepp. Denna information {r
    // densamma f|r ALLA spelare. Nu {r det dags att filtrera vilka skott som ska skickas med.
    // D{rf|r k|r vi en ny player-loop, d{r den kollar spelarens position, och skickar med
    // de relevanta skotten. Efter att den gjort det, ska den skicka UPD-paketet till den spelaren
    // och sen g|ra samma sak f|r n{sta spelare.
    
    for (player=0; player<MAX_CLIENTS; player++) {
        if (!clients[player].active) continue;
        // H{r ska den r{kna ut "viewport" f|r spelaren, eller kontrekt uttryckt:
        // Vilken position p} banan som |vre v{nstra h|rnet p} hans sk{rm har.
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
        
        // Viewporten {r nu utr{knad. Dags att g} igenom skott-arrayen och kolla vilka skott som ska skickas.
        
        for (secondary=0, counter=0; secondary<MAX_BULLETS; secondary++) {
            if (bullets[secondary].active && isInside((int)bullets[secondary].xPos, (int)bullets[secondary].yPos, &viewport)) {
                tempint=0;
                tempint = (int)(bullets[secondary].xPos-viewport.x) | (int)(bullets[secondary].yPos-viewport.y) << 11 | (int)(bullets[secondary].type) << 21;
                for (i=0; i<3; i++) {
                    gameData[36+counter*3+i] = tempint >> i*8;
                }
                counter++;
            }
        }
        UDPpacketLength = 36+counter*3;
        gameData[UDPpacketLength++]=0xFF;
        packetOut->data = gameData;
        packetOut->len = UDPpacketLength;
        packetOut->address.host=clients[player].ipadress;
        packetOut->address.port=clients[player].recvPort;
        SDLNet_UDP_Send(udpSendSock,-1,packetOut);
        //		printf("Skickade paket till %s\n",klienter[player].namn);
        
    }
}

/** uppdaterar alla skotts positioner. St{nger av dem om de hamnar utanf|r spelf{ltet.
 @var skotten: den array av serverBullet som ska uppdateras.
 */
void moveBullets(Bullet bullets[MAX_BULLETS]) {
    int i;
    for (i=0; i<MAX_BULLETS; i++) {
        if (bullets[i].active) {
            bullets[i].xPos += bullets[i].xVel;
            bullets[i].yPos += bullets[i].yVel;
            if (bullets[i].xPos > STAGE_WIDTH || bullets[i].xPos < 0 || bullets[i].yPos<0 || bullets[i].yPos>STAGE_HEIGHT) {
                bullets[i].active=false;
            }
        }
    }
}
/** anropar updateShip f|r en array av serverShip.
 @var skeppen: den serverShip-array som ska uppdateras.
 */
void moveShips(Ship ships[MAX_CLIENTS]) {
    int i;
    for (i=0; i<MAX_CLIENTS; i++) {
        updateShip(&ships[i]);
    }
}
