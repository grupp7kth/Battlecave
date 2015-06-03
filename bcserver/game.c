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

void checkCollisions(Ship* ship, Bullet* bullet) {
	// Check every ship first
	int i, j, k, l, xcoord, ycoord, xcoord2, ycoord2;
	double angleCos, angleSin, angleCos2, angleSin2;
	for (i=0; i<MAX_CLIENTS; i++) {
		if (!clients[i].active || ship[i].isDead) continue;
		angleCos = cos(ship[i].angle*PI/180);
		angleSin = sin(ship[i].angle*PI/180);

		//If the ship has not yet landed, check every collision pixel in the ship and check collision against the background
		if (!ship[i].isLanded) {
			for (j=0; j<ship[i].pixelCount; j++) {
				xcoord = (int)ship[i].xPos+(angleCos*ship[i].pixels[j].x-angleSin*ship[i].pixels[j].y);
				ycoord = (int)ship[i].yPos+(angleSin*ship[i].pixels[j].x+angleCos*ship[i].pixels[j].y);
				if (backgroundBumpmap[(ycoord*STAGE_WIDTH+xcoord)]) {
					if ((backgroundBumpmap[(ycoord*STAGE_WIDTH+xcoord)] == 2) && (ship[i].angle<15 || ship[i].angle>345)  && ship[i].yVel <1) {
						ship[i].xPos += ship[i].xVel;
						ship[i].yPos -= ship[i].yVel;
						if (ship[i].yVel < 0.2) {
							ship[i].reloadTime=0;
							ship[i].yVel=0;
							ship[i].xVel=0;
							ship[i].angle=0;
							ship[i].angleVel=0;
							ship[i].shooting=false;
							ship[i].isLanded=true;
						}
						else {
							ship[i].yVel*=-0.5;
							ship[i].xVel*=0.5;
						}
						break;
					}
					else ship[i].health=0;
				}
			}
		}
		//Check the distance to every bullet; if less than 15, check collision
		for (k=0; k<MAX_BULLETS; k++) {
			if (!bullet[k].active) continue;
			double distance = sqrt(pow(bullet[k].xPos-ships[i].xPos,2)+pow(bullet[k].yPos-ships[i].yPos,2));
			if (distance < 15) {
				for (j=0; j<ship[i].pixelCount; j++) {
					xcoord = (int)ship[i].xPos+(angleCos*ship[i].pixels[j].x-angleSin*ship[i].pixels[j].y);
					ycoord = (int)ship[i].yPos+(angleSin*ship[i].pixels[j].x+angleCos*ship[i].pixels[j].y);
					if ((int)bullet[k].xPos == xcoord && (int)bullet[k].yPos == ycoord && !ship[i].isLanded) {
						ship[i].health-=10*(1+(ship[bullet[k].source].activePowerup==POWERUP_DOUBLEDAMAGE));
						ship[i].xVel+=bullet[k].xVel*0.1;
						ship[i].yVel+=bullet[k].yVel*0.1;
						bullet[k].active = false;
						ship[i].latestTag = bullet[k].source;
						break;
					}
				}
			}
		}
		//Check the distance to every other ship; if less than 30, check for a collision
		bool collision;
		for (k=i+1; k<MAX_CLIENTS; k++) {
			if (!clients[k].active || ship[k].isDead) continue;
			double distance = sqrt(pow(ship[k].xPos-ships[i].xPos,2)+pow(ship[k].yPos-ships[i].yPos,2));
			collision=false;
			if (distance < 30) {
				angleCos2 = cos(ship[k].angle*PI/180);
				angleSin2 = sin(ship[k].angle*PI/180);
				for (j=0; j<ship[i].pixelCount; j++) {
					xcoord = (int)ship[i].xPos+(angleCos*ship[i].pixels[j].x-angleSin*ship[i].pixels[j].y);
					ycoord = (int)ship[i].yPos+(angleSin*ship[i].pixels[j].x+angleCos*ship[i].pixels[j].y);
					for (l=0; l<ship[k].pixelCount; l++) {
						xcoord2 = (int)ship[k].xPos+(angleCos2*ship[k].pixels[l].x-angleSin2*ship[k].pixels[l].y);
						ycoord2 = (int)ship[k].yPos+(angleSin2*ship[k].pixels[l].x+angleCos2*ship[k].pixels[l].y);
						if (xcoord2 == xcoord && ycoord2 == ycoord) {
							collision = true;
							break;
						}
					}
					if (collision) break;
				}
			}
			if (collision) {
				ship[i].isLanded=false;
				ship[k].isLanded=false;
				collision=false;
				double deltaX,deltaY,totVelX,totVelY,deltaR,weightConstant,bX,bY;
				deltaX = ship[i].xPos-ship[k].xPos;
				deltaY = ship[i].yPos-ship[k].yPos;
				totVelX = ship[i].xVel-ship[k].xVel;
				totVelY = ship[i].yVel-ship[k].yVel;
				int subjectWeight, objectWeight;
				deltaR=1;
				if (deltaX!=0) deltaR = deltaY/deltaX;
				subjectWeight = 1;
				objectWeight = 1;
				weightConstant = 2/(subjectWeight+objectWeight);
				bX = ((totVelX+totVelY*deltaR)/(1+pow(deltaR,2)));
				bY = bX*deltaR;
				ship[i].xVel = totVelX-weightConstant*objectWeight*bX + ship[k].xVel;
				ship[i].yVel = totVelY-weightConstant*objectWeight*bY + ship[k].yVel;
				ship[k].xVel = weightConstant*subjectWeight*bX + ship[k].xVel;
				ship[k].yVel = weightConstant*subjectWeight*bY + ship[k].yVel;
				ship[i].latestTag=k;
				ship[k].latestTag=i;
			}
		}
	}
	//Check if a bullet collides with the background
	for (i=0; i<MAX_BULLETS; i++) {
		if (bullets[i].active) {
			if (backgroundBumpmap[(int)bullets[i].yPos*STAGE_WIDTH+(int)bullets[i].xPos]) {
				bullets[i].active=false;
			}
		}
	}
}

/** Updates a ships position based on its velocity
 @var ships: The ship that will update
 */
void updateShip(Ship ships[MAX_CLIENTS]) {
    for (int i=0; i<MAX_CLIENTS; i++){
        if(ships[i].isStunned)            // Move nothing if the player's stunned
            continue;

        if(clients[i].active && !ships[i].isDead){
            if (ships[i].acceleration && ships[i].fuel >0) {
            	ships[i].fuel-=0.1;
                ships[i].yVel-=sin(getRadians(ships[i].angle))*0.015;
                ships[i].xVel-=cos(getRadians(ships[i].angle))*0.015;
                if(ships[i].yVel > MaxSpeedList[activeMaxSpeed])
                    ships[i].yVel = MaxSpeedList[activeMaxSpeed];
                else if(ships[i].yVel < -MaxSpeedList[activeMaxSpeed])
                    ships[i].yVel = -MaxSpeedList[activeMaxSpeed];
                if(ships[i].xVel > MaxSpeedList[activeMaxSpeed])
                    ships[i].xVel = MaxSpeedList[activeMaxSpeed];
                else if(ships[i].xVel < -MaxSpeedList[activeMaxSpeed])
                    ships[i].xVel = -MaxSpeedList[activeMaxSpeed];
            }
            if (!ships[i].isLanded) ships[i].yVel+=0.0015;
            if (ships[i].xVel!=0 || ships[i].yVel != 0) {
            	    ships[i].isLanded=false;
            	    ships[i].yVel+=0.0015;
            }
            else {
            	    ships[i].reloadTime++;
            	    if (ships[i].reloadTime%1==0 && ships[i].fuel < 400) ships[i].fuel++;
            	    if (ships[i].reloadTime%20==0 && ships[i].ammo < 60) ships[i].ammo++;
            }
            if (ships[i].shooting) {
                if (ships[i].bulletCooldown ==0 && ships[i].ammo>0) {
                    addBullet(&ships[i] , &i);
                    ships[i].bulletCooldown = ships[i].bulletIntervall;
                    ships[i].ammo--;
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

/** Adds a bullet and calculates its velocity
 @var ship: the ship that fired the bullet
 */

void addBullet(Ship* ship, int *id){
    int freeSpot = findFreeBullet(bullets);
    if (freeSpot < 0){
       return;
    }
    bullets[freeSpot].xPos = ship->xPos - cos(getRadians(ship->angle))*15;
    bullets[freeSpot].yPos = ship->yPos - sin(getRadians(ship->angle))*15;
    bullets[freeSpot].xVel = ship->xVel - cos(getRadians(ship->angle))*2.2;
    bullets[freeSpot].yVel = ship->yVel - sin(getRadians(ship->angle))*2.2;
    bullets[freeSpot].active = true;
    bullets[freeSpot].source = *id;
    ship->xVel -= bullets[freeSpot].xVel*0.02;
    ship->yVel -= bullets[freeSpot].yVel*0.02;

    if(ship->activePowerup == POWERUP_MULTI3X){     // If triple shot powerup is active
        for(int i=0; i < 2; i++){
            int freeSpot = findFreeBullet(bullets);
            if (freeSpot < 0){
                return;
            }
            bullets[freeSpot].xPos = ship->xPos - cos(getRadians(ship->angle - 20 + i*40))*15;
            bullets[freeSpot].yPos = ship->yPos - sin(getRadians(ship->angle - 20 + i*40))*15;
            bullets[freeSpot].xVel = ship->xVel - cos(getRadians(ship->angle - 20 + i*40))*1.6;
            bullets[freeSpot].yVel = ship->yVel - sin(getRadians(ship->angle - 20 + i*40))*1.6;
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
        bullets[freeSpot].xVel = ship->xVel - cos(getRadians(ship->angle - 180))*0.8;
        bullets[freeSpot].yVel = ship->yVel - sin(getRadians(ship->angle - 180))*0.8;
        bullets[freeSpot].active = true;
        bullets[freeSpot].source = *id;
    }

}
/** Finds a free spot in the bullet array
 @var bullets: the array of bullets
 @return: an ID that corresponds with an unoccupied spot in the bullet array
 */
int findFreeBullet(Bullet bullets[MAX_BULLETS]) {
    int i;
    for (i=0; i<MAX_BULLETS; i++) {
        if (!bullets[i].active) return i;
    }
    return -1;
}

void resetShip(Ship* ship, int i) {
	ship->xVel=0;
	ship->yVel=0;
	ship->bulletIntervall = bulletIntervalList[activeBulletInterval];
	ship->bulletCooldown = 0;
	ship->angleVel = 0;
	ship->angle = 0;
	ship->acceleration = false;
	ship->shooting = false;
	ship->isDead = false;
	ship->deathTimer=0;
	ship->health=100;
	ship->latestTag = i;
	ship->isLanded=false;
	ship->ammo=60;
	ship->fuel=400;
}

bool initGame(char map[]){
    for (int i=0; i<MAX_CLIENTS; i++) {
		ships[i].surface = NULL;
		resetShip(&ships[i],i);
	}

    fetchMapData(map);

    for(int i=0; i < MAX_BULLETS; i++)
        bullets[i].active = false;

    for(int i=0; i < MAX_CLIENTS; i++){
        clients[i].viewportID = i;
        ships[i].activePowerup = -1;
        ships[i].isStunned = false;
        ships[i].isTeleporting = false;
        ships[i].teleportDisplacementPerformed = false;
        ships[i].health = 100;
    }

    powerupSpawnTimerStart = POWERUP_SPAWNRATE;
    return true;
}
void checkShipHealth(){
	for (int i=0; i<MAX_CLIENTS; i++) {
		if(clients[i].active && ships[i].health<=0 && !ships[i].isDead) {
			char sendMessage[4];
			sendMessage[0]='}';
			sendMessage[1]=i+'0';
			sendMessage[2]=ships[i].latestTag+'0';
			sendMessage[3]='\0';
			broadCast(sendMessage);
			ships[i].deathTimer=RESPAWN_TIME_MS;
			ships[i].deathTimerStart = SDL_GetTicks();
			ships[i].isDead = true;
			ships[i].xVel = 0;
			ships[i].yVel = 0;
			ships[i].acceleration =0;
			ships[i].latestTag = i;
			if (ships[i].latestTag != i) clients[ships[i].latestTag].score+=1;
			int freespot;
			double j;
			for (j=0; j<2*PI; j+=PI/12) {
				freespot = findFreeBullet(bullets);
				if (freespot < 0) continue;
				bullets[freespot].xPos = ships[i].xPos - cos(j)*15;
				bullets[freespot].yPos = ships[i].yPos - sin(j)*15;
				bullets[freespot].xVel = - cos(j)*1.6;
				bullets[freespot].yVel = - sin(j)*1.6;
				bullets[freespot].active = true;
				bullets[freespot].source = i;
			}
			if (clients[i].playerType == PLAYER_TYPE_HUMAN)
				getSpectatingViewport(&i);
		}
		else if(clients[i].active && ships[i].isDead && ships[i].deathTimer > 0){
			ships[i].deathTimer = RESPAWN_TIME_MS - (SDL_GetTicks() - ships[i].deathTimerStart);
			if(ships[clients[i].viewportID].isDead && clients[i].playerType == PLAYER_TYPE_HUMAN)
                getSpectatingViewport(&i);
		}
		else if(clients[i].active && ships[i].isDead && ships[i].deathTimer <= 0){
			bool isValidSpawnpoint;
			int tempSpawnID, deltaX, deltaY;
			do{
                isValidSpawnpoint = true;
                tempSpawnID = rand() % 8;
                for(int i = 0; i < MAX_CLIENTS; i++){
                    deltaX = getDelta(playerSpawnPoint[tempSpawnID].x, (int)ships[i].xPos);
                    deltaY = getDelta(playerSpawnPoint[tempSpawnID].y, (int)ships[i].yPos);
                    if(getObjectDistance(deltaX, deltaY) < 150)
                        isValidSpawnpoint = false;
                }
			} while(!isValidSpawnpoint);

			ships[i].xPos = playerSpawnPoint[tempSpawnID].x;
			ships[i].yPos = playerSpawnPoint[tempSpawnID].y;
			resetShip(&ships[i],i);
			clients[i].viewportID = i;          // When the player respawns his viewport will once again be his own
		}
	}
}

void getSpectatingViewport(int *id){
    short attempts = 0;
    do{ // When the player dies, their viewport will randomly change to the first match of id 0-7 that's elgible for spectating
        clients[*id].viewportID = rand() % 8;
        attempts++;
    } while((!clients[clients[*id].viewportID].active || ships[clients[*id].viewportID].isDead) && attempts < 50);
    if(attempts >= 50)
        clients[*id].viewportID = *id;

    return;
}

void fetchMapData(char map[]){
    char mapName[30] = {'\0'}, readNum[5] = {'\0'};
    strcat(mapName, "maps/");
    strcat(mapName, map);
    strcat(mapName, ".bcmf");                           // BattleCave Map File

    FILE *fp;
    fp = fopen(mapName,"r");
    if(fp != NULL){
        for(int i=0; i < MAX_CLIENTS; i++){             // Fetch all player's spawn points from the file
            for(int j=0; readNum[j-1] != '.'; j++){
                readNum[j] = fgetc(fp);
            }
            playerSpawnPoint[i].x = atoi(readNum);
            ships[i].xPos = playerSpawnPoint[i].x;
            for(int j=0; readNum[j-1] != '\n'; j++){
                readNum[j] = fgetc(fp);
            }
            playerSpawnPoint[i].y = atoi(readNum);
            ships[i].yPos = playerSpawnPoint[i].y;
        }
        for(int i=0; readNum[i-1] != '\n'; i++){
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
            for(int j=0; readNum[j-1] != '.'; j++){
                readNum[j] = fgetc(fp);
            }
            powerupSpawnPoint[i].x = atoi(readNum);
            for(int j=0; readNum[j-1] != '\n'; j++){
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
                            else if(powerupSpawnPoint[j].type == POWERUP_HEALTHPACK){
                                ships[i].health = 100;
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
	puts("the UDP listener has started.");
    while (gameIsActive) {
        if (SDLNet_UDP_Recv(udpRecvSock,packetIn)) {
            if ((clientId = IdFromPort(packetIn->address.host)) < 0 ) {
                printf("error packet/client conflict"); }
            key = packetIn->data[0];
            if (((key & 3) == 1) && !ships[clientId].isLanded) ships[clientId].angleVel=1.5;
	    else if (((key & 3) == 2) && !ships[clientId].isLanded) ships[clientId].angleVel=-1.5;
            else ships[clientId].angleVel = 0;

            if ((key & 4) == 4) {
            	    ships[clientId].acceleration=true;
            	    ships[clientId].isLanded=false;
            }
            else ships[clientId].acceleration=false;

            if (((key & 8) == 8) && !ships[clientId].isLanded) ships[clientId].shooting=true;
            else ships[clientId].shooting=false;
        }
		SDL_Delay(5);
	}
	SDLNet_FreePacket(packetIn);
	return 0;
}

/** Creates an Uint8 array (gameData) of all ships' positions and bullets'
 * It will be tailord for every client, because it only sends the bullet
 * positions the client will be able to see
 * every packet will include
 * Byte 0-3 (4 bytes) an int that will act as a timestamp (PacketID)
 * Byte 4-35 (32 bytes) information about the ships(4 bytes * 8 ships).
 * Byte 36- (max -857) Up to 250 bullet positions(3 bytes var).
 * All this is for bullet positions that the client will be able to see plus 4 control bits per bullet
 * the method checks if a bullet is on a client's screen and sends the position for this bullet
 * after the last relevant bullet position the array contains one byte '11111111' that will mark the end of the packet.
 * the array is 857 bytes long (0-856)
 @var ships: The array of the ships in the game
 @var bullets:the bullet array of all the bullets
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
        tempint = (int)ships[player].xPos | (int)ships[player].yPos << 12 | (ships[player].health / 5) << 24;       // Health is sent divided by 5 so it fits in 5 bits (0-20 represents 0-100% hp whens packaged)
        tempint = tempint | ships[player].isDead << 29 | clients[player].active << 30 | (int)ships[player].angle << 31;
        for (i=0; i<4; i++) {
            gameData[4+player*5+i] = tempint >> i*8;
        }
        tempint = 0;
        gameData[4+player*5+4] = ((int)ships[player].angle >> 1) & 0b11111111;
    }

    // Send powerup info: (4 bytes per powerup * 15 powerups) ( each uses 28 bits currently)
    for(i=0; i < MAX_ALLOWED_POWERUP_SPAWNPOINTS; i++){
        tempint = 0;
        tempint = (int)powerupSpawnPoint[i].x | (int)powerupSpawnPoint[i].y << 12 | (int)powerupSpawnPoint[i].isActive << 24 | (int)powerupSpawnPoint[i].type << 25;
        for (j=0; j<4; j++){
            gameData[44+i*4+j] = tempint >> j*8;
        }
    }

    // the function have now included the information of the packet number and all the ships
	// this information is the same for all clients
	// Now the function will determine which of the bullets all the individual clients will be able to see
    // that's why a new player-loop will execute, where it checks the client's ship's positon, and calculates
    // all the relevant bullets
    // after this is done the packet will be sent to the client and will do the same for every other active client

    for (player=0; player<MAX_CLIENTS; player++){
        if (!clients[player].active)
            continue;

        // Send the player's active viewport since it's variable (changes to spectate someone else upon death)
        gameData[104] =  clients[player].viewportID;

        // Put the ship's fuel and ammo in two bytes
        tempint = (int)ships[player].fuel | ships[player].ammo << 9;
        gameData[105] = tempint;
        gameData[106] = tempint >> 8;

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

        // The viewport is now calculated. it's now time to check which bullet to send

        for (secondary=0, counter=0; secondary<MAX_BULLETS; secondary++) {
            if (bullets[secondary].active && isInside((int)bullets[secondary].xPos, (int)bullets[secondary].yPos, &viewport)) {
                tempint=0;
                tempint = (int)(bullets[secondary].xPos-viewport.x) | (int)(bullets[secondary].yPos-viewport.y) << 11 | (int)(bullets[secondary].source) << 21;
                for (i=0; i<3; i++) {
                    gameData[107+counter*3+i] = tempint >> i*8;
                }
                counter++;
            }
        }
        UDPpacketLength = 107+counter*3;
        gameData[UDPpacketLength++]=0xFF;
        packetOut->data = gameData;
        packetOut->len = UDPpacketLength;
        packetOut->address.host=clients[player].ipadress;
        packetOut->address.port=clients[player].recvPort;
        SDLNet_UDP_Send(udpSendSock,-1,packetOut);
    }
}

/** Updates all bullet positions
 @var bullets: the bullet array which is updated
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
        if(clients[i].playerType == PLAYER_TYPE_BOT && !ships[i].isDead){
            handleBot(&i);
        }
    }
    return;
}

void handleBot(int *id){
    float netSpeed = getNetSpeed(ships[*id].xVel, ships[*id].yVel);

    // Get the closest object (enemy ship or active powerup)
    int closestDistance = 0, distance, deltaX, deltaY, closestAngle = 0, closestType = 0; // closestType: 0 = Enemy ship , 1 = Powerup Spawn

    for(int i=0; i < MAX_CLIENTS; i++){     // First determine the closest enemy ship
        if(i != *id && clients[i].active && !ships[i].isDead && !ships[i].isTeleporting && !ships[i].isLanded){
            deltaX = getDelta(ships[*id].xPos, ships[i].xPos);
            deltaY = getDelta(ships[*id].yPos, ships[i].yPos);
            distance = getObjectDistance(deltaX, deltaY);

            if(closestDistance <= 0 || distance < closestDistance){
                closestDistance = distance;
                closestType = 0;

                closestAngle = getObjectAngle(deltaX, deltaY);
                if(deltaX <= 0)              // The angle will be relative to the ship; 0-180 means it's to the right, 0-(-180) means left
                    closestAngle += 90;
                else
                    closestAngle += 270;
                closestAngle = closestAngle - ships[*id].angle;
            }
        }
    }
    for(int i=0; i < numberOfPowerups; i++){  // Secondly determine the closest powerup
        if(powerupSpawnPoint[i].isActive){
            deltaX = getDelta(ships[*id].xPos, powerupSpawnPoint[i].x);
            deltaY = getDelta(ships[*id].yPos, powerupSpawnPoint[i].y);
            distance = getObjectDistance(deltaX, deltaY);


            if((closestDistance <= 0 || (distance * 2) < closestDistance) && (powerupSpawnPoint[i].type != POWERUP_HEALTHPACK || ships[*id].health < 100)){ // '* 2' because we want to heavily prioritize enemy ships over powerups

                closestDistance = distance;
                closestType = 1;

                closestAngle = getObjectAngle(deltaX, deltaY);
                if(deltaX <= 0)              // The angle will be relative to the ship; 0-180 means it's to the right, 0-(-180) means left
                    closestAngle += 90;
                else
                    closestAngle += 270;
                closestAngle = closestAngle - ships[*id].angle;
            }
        }

    }

    //*******************************************************************
    //     Initial Analyzing Done, Now Determine What Action to Take    *
    //*******************************************************************

    // We determine which degree we're heading (not facing!)
    float scanDegrees;
    if(ships[*id].xVel > 0)
        scanDegrees = 90 + (atan(ships[*id].yVel / ships[*id].xVel) * 57.2957795);
    else if(ships[*id].xVel < 0)
        scanDegrees = 270 + (atan(ships[*id].yVel / ships[*id].xVel) * 57.2957795);
    else if(ships[*id].yVel >= 0)
        scanDegrees = 180;
    else
        scanDegrees = 0;

    if(scanDegrees >= 360)
        scanDegrees -= 360;

    if(botScanLine((int)((netSpeed * 300) + 100), scanDegrees, 5, id) > 0)
        goDefensive(id, &netSpeed, scanDegrees);
    else if(botScanLine(100, scanDegrees+25, 5, id) > 0)  // Cone scanning to counter irregularities whilst turning
        goDefensive(id, &netSpeed, scanDegrees+25);
    else if(botScanLine(100, scanDegrees-25, 5, id) > 0)
        goDefensive(id, &netSpeed, scanDegrees-25);
    else
        goAggressive(id, &closestDistance, &closestType, &closestAngle, &netSpeed, &deltaX, &deltaY);

    handleBotShooting(id, &closestDistance, &closestType, &closestAngle);
    return;
}

float getNetSpeed(float xVel, float yVel){
    return(sqrt(pow(xVel, 2) + pow(yVel, 2)));
}

int botScanLine(int length, int angle, int stepSize, int *id){
    short xcoord, ycoord, deltaX, deltaY, i;
    double angleCos, angleSin;
    angleCos = cos((90+angle) * PI/180);
    angleSin = sin((90+angle) * PI/180);

    for(i = 0; i < length; i += stepSize){
        xcoord = (int)ships[*id].xPos - (i * angleCos);
        ycoord = (int)ships[*id].yPos - (i * angleSin);
        if(backgroundBumpmap[(ycoord*STAGE_WIDTH+xcoord)]){
            deltaX = getDelta((int)ships[*id].xPos, xcoord);
            deltaY = getDelta((int)ships[*id].yPos, ycoord);
            return getObjectDistance(deltaX, deltaY);
        }
    }
    return 0;
}

void goDefensive(int *id, float *netSpeed, float scanDegrees){
    int scanDegreesInt = (int)scanDegrees;
    int oppositeDegrees = 180 + scanDegreesInt;
    if(oppositeDegrees >= 360)
        oppositeDegrees -= 360;

    if(abs((int)ships[*id].angle - oppositeDegrees) <= 15){
        ships[*id].acceleration = true;
        ships[*id].angleVel = 0;
    }
    else{                                                 // If not facing the right direction; turn
        if(ships[*id].angle >= oppositeDegrees && ships[*id].angle < (oppositeDegrees + 180))
            ships[*id].angleVel = -5;
        else
            ships[*id].angleVel = 5;

        ships[*id].acceleration = false;
    }
    return;
}

void goAggressive(int *id, int *closestDistance, int *closestType, int *closestAngle, float *netSpeed, int *deltaX, int *deltaY){
    if(*closestDistance <= 0){
        ships[*id].shooting = false;
        ships[*id].angleVel = 0;
        return;
    }

    if((*closestAngle >= 5 && *closestAngle <= 180) || *closestAngle < -180)
        ships[*id].angleVel = 5;
    else if((*closestAngle <= -5 && *closestAngle >= -180 ) || *closestAngle > 180)
        ships[*id].angleVel = -5;
    else
        ships[*id].angleVel = 0;

    if(*closestDistance > 5 && botScanLine(150, (int)ships[*id].angle, 5, id) == 0){
        if(*deltaY > 0 && ships[*id].yVel > -0.2)
            ships[*id].acceleration = true;
        else if(*deltaX <= 0 && ships[*id].xVel <= 0 && ships[*id].yVel > -0.3)
            ships[*id].acceleration = true;
        else if(*deltaX > 0 && ships[*id].xVel > 0 && ships[*id].yVel < 0.3)
            ships[*id].acceleration = true;
        else if(*netSpeed < 0.55)
            ships[*id].acceleration = true;
        else
            ships[*id].acceleration = false;
    }
    else
        ships[*id].acceleration = false;

    return;
}

void handleBotShooting(int *id, int *closestDistance, int *closestType, int *closestAngle){
   if(abs(*closestAngle) <= 5){             // If we've facing our target
        if(*closestType == 0){              // If the target is an enemy ship
            if(botScanLine(*closestDistance, *closestAngle, 10, id) == 0)
                ships[*id].shooting = true;
            else
                ships[*id].shooting = false;
        }
        else
            ships[*id].shooting = false;
    }
    return;
}

int getDelta(int p1, int p2){
    return (p1 - p2);
}

int getObjectDistance(int deltaX, int deltaY){
    float distance;
    distance = sqrt(pow(deltaX,2) + pow(deltaY,2));
    return (int)distance;
}

int getObjectAngle(int deltaX, int deltaY){
    float angle;
    angle = atan((float)deltaY/(float)deltaX) * 57.2957795;
    return (int)angle;
}
