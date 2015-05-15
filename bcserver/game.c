#include "serverheader.h"

double getRadians(int a) {
	return (a+90)*3.14159265/180;
}
bool isInside(int x, int y, SDL_Rect* r) {
	return (x >= r->x && x < (r->x+r->w) && y >= r->y && y < (r->y+r->h));
}
int IdFromPort(Uint32 ip) {
	for (int i=0; i<MAX_CLIENTS; i++) {
		if(clients[i].active && clients[i].ipadress==ip) return i;
	}
	return -1;
}
/** uppdaterar ett skepps position baserat p} hastighet.
 @var skeppet: Det skepp som ska uppdateras.
 */ /// alkdjlaksjdlaskjd
void updateShip(Ship ships[MAX_CLIENTS]) {
    for (int i=0; i<MAX_CLIENTS; i++){
        if(clients[i].active){
            if (ships[i].acceleration) {
                ships[i].yVel-=sin(getRadians(ships[i].angle))*0.1;
                ships[i].xVel-=cos(getRadians(ships[i].angle))*0.1;
                if(ships[i].yVel > MaxSpeedList[activeMaxSpeed])
                    ships[i].yVel = MaxSpeedList[activeMaxSpeed];
                else if(ships[i].yVel < -MaxSpeedList[activeMaxSpeed])
                    ships[i].yVel = -MaxSpeedList[activeMaxSpeed];
                if(ships[i].xVel > MaxSpeedList[activeMaxSpeed])
                    ships[i].xVel = MaxSpeedList[activeMaxSpeed];
                else if(ships[i].xVel < -MaxSpeedList[activeMaxSpeed])
                    ships[i].xVel = -MaxSpeedList[activeMaxSpeed];
            }
            else if (!infiniteMomentum) {
                ships[i].yVel /= 1.005;
                ships[i].xVel /= 1.005;

                if(ships[i].yVel <= 0.2 && ships[i].yVel >= -0.2)
                    ships[i].yVel = 0;
                if(ships[i].xVel <= 0.2 && ships[i].xVel >= -0.2)
                    ships[i].xVel = 0;
            }
            if (ships[i].shooting) {
                if (ships[i].bulletCooldown ==0) {
                    addBullet(&ships[i] , &i);
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
}

/** L{gger till ett skott i spelet och r{knar ut dess hastighet.
 * Fult nog {r arrayen 'serverSkott' global.
 @var skeppet: Det serverShip som just skjutit.
 */

void addBullet(Ship* ship, int *id){
    int freeSpot = findFreeBullet(bullets);
    if (freeSpot <0) {
        exit(1);
    }
    bullets[freeSpot].xPos = ship->xPos;
    bullets[freeSpot].yPos = ship->yPos;
    bullets[freeSpot].xVel = ship->xVel - cos(getRadians(ship->angle))*6;
    bullets[freeSpot].yVel = ship->yVel - sin(getRadians(ship->angle))*6;

    bullets[freeSpot].active = true;
    bullets[freeSpot].source = *id;
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

bool initGame(){
    for (int i=0; i<MAX_CLIENTS; i++) {
		ships[i].surface = NULL;
		ships[i].xVel = 0;
		ships[i].yVel = 0;
		ships[i].bulletIntervall = bulletIntervalList[activeBulletInterval];
		ships[i].bulletCooldown = 0;
		ships[i].angleVel = 0;
		ships[i].angle = 0;
		ships[i].acceleration = false;
		ships[i].shooting = false;
		ships[i].isDead = false;
		ships[i].deathTimer=0;
	}
    fetchMapData();

    for(int i=0; i < MAX_BULLETS; i++)
        bullets[i].active = false;

    ships[0].isDead = true;
    return true;
}
void checkShipHealth() {
    for (int i=0; i<MAX_CLIENTS; i++) {
        if(clients[i].active && ships[i].health<=0 && ships[i].isDead==false) {
            SDLNet_TCP_Send(clients[i].socket,PREAMBLE_KILLED,sizeof(PREAMBLE_KILLED));
            ships[i].deathTimer=10000;
        }
    }
}


void fetchMapData(void){
    char mapName[30] = {'\0'}, readNum[5] = {'\0'};
    strcat(mapName, "cave"); //******************************************************************************* MAKE VARIABLE??
    strcat(mapName, ".bcmf");       // BattleCave Map File

    FILE *fp;
    fp = fopen(mapName,"r");
    if(fp != NULL){
        for(int i=0; i < MAX_CLIENTS; i++){
            for(int j=0; readNum[j-1] != '.' && j < 5; j++){
                readNum[j] = fgetc(fp);
            }
            playerSpawnPoint[i].x = atoi(readNum);
            ships[i].xPos = playerSpawnPoint[i].x;
            for(int j=0; readNum[j-1] != '\n' && j < 5; j++){
                readNum[j] = fgetc(fp);
            }
            playerSpawnPoint[i].y = atoi(readNum);
            ships[i].yPos = playerSpawnPoint[i].y;
        }
    }

    fclose(fp);
    return;
}

int udpListener(void* data) {

	UDPpacket *packetIn;
	packetIn = SDLNet_AllocPacket(16);
	short clientId=0, key=0;
	puts("udplistener startad.");
    while (gameIsActive) {
        if (SDLNet_UDP_Recv(udpRecvSock,packetIn)) {
            if ((clientId = IdFromPort(packetIn->address.host)) < 0 ) {
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
    viewport.w = GAME_AREA_WIDTH;
    viewport.h = GAME_AREA_HEIGHT;
    int i, player, secondary, counter,UDPpacketLength;
    Uint32 tempint;

    packetID++; // Det h{r {r tidsangivelsen. Den b|rjar p} 0 och |kar med 1 f|r varje paket.

    for (i=0; i<4; i++) {
        gameData[i] = packetID >> i*8;
    }
    for (player=0; player<MAX_CLIENTS; player++) {
        tempint=0;
        tempint = (int)ships[player].xPos | (int)ships[player].yPos << 12 | (int)(ships[player].angle/6) << 24;
        tempint = tempint | ships[player].isDead <<30 | clients[player].active << 31;
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
        if (!clients[player].active)
            continue;

        short tempID;
        if(!ships[player].isDead)   // If the player is alive the viewport will be his own, else it will be the first alive players going from ID 0-7
            tempID = player;
        else{
            for(int i=0; i < MAX_CLIENTS; i++){
                if(clients[i].active && !ships[i].isDead){
                    tempID = i;
                    break;
                }
            }
        }
        if (ships[tempID].xPos < GAME_AREA_WIDTH/2)
            viewport.x=0;
        else if (ships[tempID].xPos > STAGE_WIDTH-GAME_AREA_WIDTH/2)
            viewport.x=STAGE_WIDTH-GAME_AREA_WIDTH;
        else viewport.x=ships[tempID].xPos-GAME_AREA_WIDTH/2;
        if (ships[tempID].yPos < GAME_AREA_HEIGHT/2)
            viewport.y=0;
        else if (ships[tempID].yPos > STAGE_HEIGHT-GAME_AREA_HEIGHT/2)
            viewport.y=STAGE_HEIGHT-GAME_AREA_HEIGHT;
        else viewport.y=ships[tempID].yPos-GAME_AREA_HEIGHT/2;

        // Viewporten {r nu utr{knad. Dags att g} igenom skott-arrayen och kolla vilka skott som ska skickas.

        for (secondary=0, counter=0; secondary<MAX_BULLETS; secondary++) {
            if (bullets[secondary].active && isInside((int)bullets[secondary].xPos, (int)bullets[secondary].yPos, &viewport)) {
                tempint=0;
                tempint = (int)(bullets[secondary].xPos-viewport.x) | (int)(bullets[secondary].yPos-viewport.y) << 11 | (int)(bullets[secondary].source) << 21;
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
   		//printf("Skickade paket till %s (IP=%x)\n",clients[player].name, clients[player].ipadress);
    }
}

/** uppdaterar alla skotts positioner. St{nger av dem om de hamnar utanf|r spelf{ltet.
 @var skotten: den array av serverBullet som ska uppdateras.
 */
void moveBullets(Bullet bullets[MAX_BULLETS]){
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

//***********************************************************************
//       ___  ___ _____   _  _   _   _  _ ___  _    ___ _  _  ___       *
//      | _ )/ _ \_   _| | || | /_\ | \| |   \| |  |_ _| \| |/ __|      *
//      | _ \ (_) || |   | __ |/ _ \| .` | |) | |__ | || .` | (_ |      *
//      |___/\___/ |_|   |_||_/_/ \_\_|\_|___/|____|___|_|\_|\___|      *
//                                                                      *
//***********************************************************************

void updateBots(void){
    for(int i=0; i < MAX_CLIENTS; i++){
        if(clients[i].playerType == PLAYER_TYPE_BOT){
            handleBot(i);
        }
    }
    return;
}

void handleBot(int id){
    int closestDistance = 0, distance, deltaX, deltaY;
    float closestAngle;

    for(int i=0; i < MAX_CLIENTS; i++){
        if(i != id && clients[i].active && !ships[i].isDead){
            deltaX = getDelta(ships[id].xPos, ships[i].xPos);
            deltaY = getDelta(ships[id].yPos, ships[i].yPos);
            distance = getObjectDistance(deltaX, deltaY);

            if(closestDistance <= 0 || distance < closestDistance){
                closestDistance = distance;

                closestAngle = getObjectAngle(deltaX, deltaY);
                if(deltaX <= 0)              // The angle will be relative to the ship; 0-180 means it's to the right, 0-(-180) means left
                    closestAngle += 90;
                else
                    closestAngle += 270;
                closestAngle = closestAngle - ships[id].angle;
            }
        }
    }
    if(closestDistance <= 0)
        return;

    if((closestAngle > 2 && closestAngle <= 180) || closestAngle < -180)
        ships[id].angleVel = 5;
    else if((closestAngle < -2 && closestAngle >= -180 ) || closestAngle > 180)
        ships[id].angleVel = -5;
    else
        ships[id].angleVel = 0;

    if(abs(closestAngle) < 5){
        if(closestDistance <= 400)
            ships[id].shooting = true;
        else
            ships[id].shooting = false;

        if(closestDistance > 300)
            ships[id].acceleration = true;
        else
            ships[id].acceleration = false;
    }


    printf("MY CLOSEST ENEMY WAS AT DISTANCE=%d\n", closestDistance);
    return;
}

int getDelta(int p1, int p2){
    return (p1 - p2);
}

int getObjectDistance(int deltaX, int deltaY){
    int distance;
    distance = sqrt(pow(deltaX,2) + pow(deltaY,2));
    return distance;
}

float getObjectAngle(int deltaX, int deltaY){
    float angle;
    angle = atan((float)deltaY/(float)deltaX) * 57.2957795;
    return angle;
}
