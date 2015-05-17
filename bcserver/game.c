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
        if(ships[i].isStunned)            // Move nothing if the player's stunned
            continue;

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
            if(timeWarpIsActive){               // If time warp is active the ships move slower
                ships[i].xPos += ships[i].xVel/3;
                ships[i].yPos += ships[i].yVel/3;
            }
            else{
                ships[i].xPos += ships[i].xVel;
                ships[i].yPos += ships[i].yVel;
            }


            if (ships[i].xPos > STAGE_WIDTH) ships[i].xPos=0;
            if (ships[i].xPos <0) ships[i].xPos=STAGE_WIDTH;
            if (ships[i].yPos > STAGE_HEIGHT) ships[i].yPos=0;
            if (ships[i].yPos <0) ships[i].yPos=STAGE_HEIGHT;
            ships[i].angle += ships[i].angleVel;
            if (ships[i].angle >= 360) ships[i].angle-=360;
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
    if (freeSpot < 0){
       return;
    }
    bullets[freeSpot].xPos = ship->xPos - cos(getRadians(ship->angle))*15;
    bullets[freeSpot].yPos = ship->yPos - sin(getRadians(ship->angle))*15;
    bullets[freeSpot].xVel = ship->xVel - cos(getRadians(ship->angle))*10;
    bullets[freeSpot].yVel = ship->yVel - sin(getRadians(ship->angle))*10;
    bullets[freeSpot].active = true;
    bullets[freeSpot].source = *id;

    if(ship->activePowerup == POWERUP_MULTI3X){     // If triple shot powerup is active
        for(int i=0; i < 2; i++){
            int freeSpot = findFreeBullet(bullets);
            if (freeSpot < 0){
                return;
            }
            bullets[freeSpot].xPos = ship->xPos - cos(getRadians(ship->angle - 20 + i*40))*15;
            bullets[freeSpot].yPos = ship->yPos - sin(getRadians(ship->angle - 20 + i*40))*15;
            bullets[freeSpot].xVel = ship->xVel - cos(getRadians(ship->angle - 20 + i*40))*10;
            bullets[freeSpot].yVel = ship->yVel - sin(getRadians(ship->angle - 20 + i*40))*10;
            bullets[freeSpot].active = true;
            bullets[freeSpot].source = *id;
        }
    }
    else if(ship->activePowerup == POWERUP_MULTI2X){ // If double shot powerup is active
        int freeSpot = findFreeBullet(bullets);
        if (freeSpot < 0){
           return;
        }
        bullets[freeSpot].xPos = ship->xPos - cos(getRadians(ship->angle - 180))*15;
        bullets[freeSpot].yPos = ship->yPos - sin(getRadians(ship->angle - 180))*15;
        bullets[freeSpot].xVel = ship->xVel - cos(getRadians(ship->angle - 180))*10;
        bullets[freeSpot].yVel = ship->yVel - sin(getRadians(ship->angle - 180))*10;
        bullets[freeSpot].active = true;
        bullets[freeSpot].source = *id;
    }

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
		ships[i].health=100;
	}
    fetchMapData();

    for(int i=0; i < MAX_BULLETS; i++)
        bullets[i].active = false;

    for(int i=0; i < MAX_CLIENTS; i++){
        clients[i].viewportID = i;
        ships[i].activePowerup = -1;
        ships[i].isStunned = false;
        ships[i].isTeleporting = false;
        ships[i].teleportDisplacementPerformed = false;
    }

    powerupSpawnTimerStart = POWERUP_SPAWNRATE;

    return true;
}
void checkShipHealth(){
    for (int i=0; i<MAX_CLIENTS; i++) {
        if(clients[i].active && ships[i].health<=0 && !ships[i].isDead && clients[i].playerType == PLAYER_TYPE_HUMAN) {
            SDLNet_TCP_Send(clients[i].socket,PREAMBLE_KILLED,sizeof(PREAMBLE_KILLED));
            ships[i].deathTimer=RESPAWN_TIME_MS;
            ships[i].deathTimerStart = SDL_GetTicks();
            ships[i].isDead = true;
            ships[i].xVel = 0;
            ships[i].yVel = 0;
            short attempts = 0;
            do{                                 // When the player dies, their viewport will randomly change to the first match of id 0-7 that's elgible for spectating
                clients[i].viewportID = rand() % 8;
                attempts++;
            } while((!clients[clients[i].viewportID].active || ships[clients[i].viewportID].isDead) && attempts < 50);
            if(attempts >= 50)
                clients[i].viewportID = i;
        }
        else if(clients[i].active && ships[i].isDead && ships[i].deathTimer > 0){
            ships[i].deathTimer = RESPAWN_TIME_MS - (SDL_GetTicks() - ships[i].deathTimerStart);
        }
        else if(clients[i].active && ships[i].isDead && ships[i].deathTimer <= 0){
            ships[i].health = 100;
            ships[i].isDead = false;
            int tempSpawnID = rand() % 8;
            ships[i].xPos = playerSpawnPoint[tempSpawnID].x;
            ships[i].yPos = playerSpawnPoint[tempSpawnID].y;
            ships[i].angle = 0;
            clients[i].viewportID = i;          // When the player respawns his viewport will once again be his own
        }
    }
}


void fetchMapData(void){
    char mapName[30] = {'\0'}, readNum[5] = {'\0'};
    strcat(mapName, "cave2"); //******************************************************************************* MAKE VARIABLE??
    strcat(mapName, ".bcmf");                           // BattleCave Map File

    FILE *fp;
    fp = fopen(mapName,"r");
    if(fp != NULL){
        for(int i=0; i < MAX_CLIENTS; i++){             // Fetch all player's spawn points from the file
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
        for(int i=0; readNum[i-1] != '\n' && i < 5; i++){
            readNum[i] = fgetc(fp);
        }
        numberOfPowerups = atoi(readNum);               // Get how many total powerups the map has from the file
        if(numberOfPowerups > MAX_ALLOWED_POWERUP_SPAWNPOINTS){
            printf("Bad .bcmf file!\n");
            exit(EXIT_FAILURE);
        }

        for(int i=0; i < MAX_ALLOWED_POWERUP_SPAWNPOINTS; i++){
            powerupSpawnPoint[i].x = 0;                 // Init the powerups
            powerupSpawnPoint[i].y = 0;
            powerupSpawnPoint[i].type = 0;
            powerupSpawnPoint[i].isActive = false;
        }

        for(int i=0; i < numberOfPowerups; i++){        // Set the powerup's spawn points from the file
            for(int j=0; readNum[j-1] != '.' && j < 5; j++){
                readNum[j] = fgetc(fp);
            }
            powerupSpawnPoint[i].x = atoi(readNum);
            for(int j=0; readNum[j-1] != '\n' && j < 5; j++){
                readNum[j] = fgetc(fp);
            }
            powerupSpawnPoint[i].y = atoi(readNum);
        }
    }
    fclose(fp);
    return;
}

void handlePowerupSpawns(void){
    if(activePowerupSpawns < numberOfPowerups && (SDL_GetTicks() - powerupSpawnTimerStart) >= POWERUP_SPAWNRATE){ // If theres a free spot for powerup placement and it's time to place a new one
        short powerupID, attempts = 0;
        int closestDistance = 1000, tempDist, deltaX, deltaY;

        do{
            powerupID = rand() % numberOfPowerups;

            for(int i=0; i < MAX_CLIENTS; i++){             // We don't want to spawn a powerup if a player is too close to the spawn-point
                if(clients[i].active && !ships[i].isDead){
                    deltaX = getDelta(powerupSpawnPoint[powerupID].x, (int)ships[i].xPos);
                    deltaY = getDelta(powerupSpawnPoint[powerupID].y, (int)ships[i].yPos);
                    tempDist = getObjectDistance(deltaX, deltaY);
                    if(closestDistance > tempDist)
                        closestDistance = tempDist;
                }
            }
            attempts++;
        } while(attempts < 50 && (powerupSpawnPoint[powerupID].isActive || closestDistance < 150));
        if(attempts >= 50)
            return;

        powerupSpawnTimerStart = SDL_GetTicks();            // Reset the timer for the next powerup spawn

        powerupSpawnPoint[powerupID].isActive = true;
        powerupSpawnPoint[powerupID].type = rand() % 6;     // 6 different powerups exist in the game
        activePowerupSpawns++;
        printf("PUT PWRUP ID=%d AT X:%d Y:%d (TYPE=%d)\n", powerupID, powerupSpawnPoint[powerupID].x, powerupSpawnPoint[powerupID].y, powerupSpawnPoint[powerupID].type);
    }
    return;
}

void handlePowerupGains(void){
    for(int i=0; i < MAX_CLIENTS; i++){
        if(clients[i].active && !ships[i].isDead){
            for(int j=0; j < MAX_ALLOWED_POWERUP_SPAWNPOINTS; j++){
                if(powerupSpawnPoint[j].isActive){
                    Uint16 deltaX = getDelta((int)ships[i].xPos, powerupSpawnPoint[j].x);
                    Uint16 deltaY = getDelta((int)ships[i].yPos, powerupSpawnPoint[j].y);
                    if(getObjectDistance(deltaX, deltaY) < 55){                                 // If the ship is close enough to an active powerup
                        char TCPsend[MAX_LENGTH];
                        clearString(TCPsend);
                        sprintf(TCPsend, PREAMBLE_POWERUP"%d", powerupSpawnPoint[j].type);

                        powerupSpawnPoint[j].isActive = false;
                        activePowerupSpawns--;

                        if(powerupSpawnPoint[j].type != POWERUP_TIMEWARP && powerupSpawnPoint[j].type != POWERUP_TELEPORT) // Timewarp is not bound to a player/ship, Teleport is instant and functions server-side only
                            ships[i].activePowerup = powerupSpawnPoint[j].type;
                        else
                            ships[i].activePowerup = -1;

                        if(powerupSpawnPoint[j].type == POWERUP_TIMEWARP){
                            timeWarpIsActive = true;
                            timeWarpStartTime = SDL_GetTicks();
                            broadCast(TCPsend);                                                 // TimeWarp affects everyone, and so it will be sent to everyone
                        }
                        else{
                            if(powerupSpawnPoint[j].type == POWERUP_TELEPORT){
                                ships[i].isStunned = true;                                      // Stun the ship for a while before and after moving it
                                ships[i].isTeleporting = true;
                                ships[i].teleportDisplacementPerformed = false;
                                ships[i].stunDurationStart = SDL_GetTicks();
                                ships[i].xVel = 0;                                              // We dont want the ship to instantly crash into a wall upon arrival
                                ships[i].yVel = 0;
                            }
                            else if(powerupSpawnPoint[j].type == POWERUP_MULTI3X || powerupSpawnPoint[j].type == POWERUP_MULTI2X || powerupSpawnPoint[j].type == POWERUP_DOUBLEDAMAGE){
                                    ships[i].powerupTimerStart = SDL_GetTicks();                // If the power up is of a type which is timed, start the timer on 10 seconds
                            }
                            if(clients[i].playerType == PLAYER_TYPE_HUMAN)
                                SDLNet_TCP_Send(clients[i].socket, TCPsend, strlen(TCPsend));   // Let human players know they've gained a powerup
                        }
                    }
                }
            }
        }
    }
    return;
}

void handleActivePowerups(void){
    for(int i=0; i < MAX_CLIENTS; i++){
        if(ships[i].activePowerup >= 0){            // If the ships has an active powerup running (such as multishot/doubledamage) , -1 = No powerup
            if((SDL_GetTicks() - ships[i].powerupTimerStart) >= SHIP_POWERUP_DURATION){
                ships[i].activePowerup = -1;
            }
        }
        else if(ships[i].isTeleporting){
            int tempTeleportTimerInt;
            tempTeleportTimerInt = SDL_GetTicks() - ships[i].stunDurationStart;                                 // Free the ship once the full duration has passed
            if(tempTeleportTimerInt >= TELEPORT_DURATION){
                ships[i].isTeleporting = false;
                ships[i].isStunned = false;
            }
            else if(tempTeleportTimerInt >= TELEPORT_DURATION/2 && !ships[i].teleportDisplacementPerformed){    // Move the ship once half the duration has passed
                Uint8 teleportID = rand() % MAX_CLIENTS;
                ships[i].xPos = playerSpawnPoint[teleportID].x;
                ships[i].yPos = playerSpawnPoint[teleportID].y;
                ships[i].teleportDisplacementPerformed = true;
            }
        }
    }
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
            if ((key & 3) == 1) ships[clientId].angleVel=6;
            else if ((key & 3) == 2) ships[clientId].angleVel=-6;
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
    int i, j, player, secondary, counter,UDPpacketLength;
    Uint32 tempint;

    packetID++;

    for (i=0; i<4; i++) {
        gameData[i] = packetID >> i*8;
    }

    // Send ship info
    for (player=0; player<MAX_CLIENTS; player++) {
        tempint = 0;
        tempint = (int)ships[player].xPos | (int)ships[player].yPos << 12 | (int)(ships[player].angle/6) << 24;
        tempint = tempint | ships[player].isDead <<30 | clients[player].active << 31;
        for (i=0; i<4; i++) {
            gameData[4+player*4+i] = tempint >> i*8;
        }
    }

    // Send powerup info: (4 bytes per powerup * 15 powerups) ( each uses 28 bits currently)
    for(i=0; i < MAX_ALLOWED_POWERUP_SPAWNPOINTS; i++){
        tempint = 0;
        tempint = (int)powerupSpawnPoint[i].x | (int)powerupSpawnPoint[i].y << 12 | (int)powerupSpawnPoint[i].isActive << 24 | (int)powerupSpawnPoint[i].type << 25;
        for (j=0; j<4; j++){
            gameData[36+i*4+j] = tempint >> j*8;
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

        tempint = ships[player].health/5 | clients[player].viewportID << 5;
        gameData[96] = tempint;                             // Make Health scale to 5 (Hp 0-100% is represented by 0-20) to use 5 bits of this byte, Remaining 3 bits of the byte will be the viewport ID for the client

        // Calculate viewport:
        if (ships[clients[player].viewportID].xPos < GAME_AREA_WIDTH/2)
            viewport.x=0;
        else if (ships[clients[player].viewportID].xPos > STAGE_WIDTH-GAME_AREA_WIDTH/2)
            viewport.x=STAGE_WIDTH-GAME_AREA_WIDTH;
        else viewport.x=ships[clients[player].viewportID].xPos-GAME_AREA_WIDTH/2;
        if (ships[clients[player].viewportID].yPos < GAME_AREA_HEIGHT/2)
            viewport.y=0;
        else if (ships[clients[player].viewportID].yPos > STAGE_HEIGHT-GAME_AREA_HEIGHT/2)
            viewport.y=STAGE_HEIGHT-GAME_AREA_HEIGHT;
        else viewport.y=ships[clients[player].viewportID].yPos-GAME_AREA_HEIGHT/2;

        // Viewporten {r nu utr{knad. Dags att g} igenom skott-arrayen och kolla vilka skott som ska skickas.

        for (secondary=0, counter=0; secondary<MAX_BULLETS; secondary++) {
            if (bullets[secondary].active && isInside((int)bullets[secondary].xPos, (int)bullets[secondary].yPos, &viewport)) {
                tempint=0;
                tempint = (int)(bullets[secondary].xPos-viewport.x) | (int)(bullets[secondary].yPos-viewport.y) << 11 | (int)(bullets[secondary].source) << 21;
                for (i=0; i<3; i++) {
                    gameData[97+counter*3+i] = tempint >> i*8;
                }
                counter++;
            }
        }
        UDPpacketLength = 97+counter*3;
        gameData[UDPpacketLength++]=0xFF;
        packetOut->data = gameData;
        packetOut->len = UDPpacketLength;
        packetOut->address.host=clients[player].ipadress;
        packetOut->address.port=clients[player].recvPort;
        SDLNet_UDP_Send(udpSendSock,-1,packetOut);
    }
}

/** uppdaterar alla skotts positioner. St{nger av dem om de hamnar utanf|r spelf{ltet.
 @var skotten: den array av serverBullet som ska uppdateras.
 */
void moveBullets(Bullet bullets[MAX_BULLETS]){
    int i;
    for (i=0; i<MAX_BULLETS; i++) {
        if (bullets[i].active) {
            if(timeWarpIsActive){       // If time warp is active the bullets move slower
                bullets[i].xPos += bullets[i].xVel/3;
                bullets[i].yPos += bullets[i].yVel/3;
            }
            else{
                bullets[i].xPos += bullets[i].xVel;
                bullets[i].yPos += bullets[i].yVel;
            }
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
        if(i != id && clients[i].active && !ships[i].isDead && !ships[i].isTeleporting){
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


    //printf("MY CLOSEST ENEMY WAS AT DISTANCE=%d\n", closestDistance);
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
