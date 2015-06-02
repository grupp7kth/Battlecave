#include "serverheader.h"

IPaddress serverIP;
TCPsocket TCPsock;

void itoa(int n, char s[],int bas) { // from Kerninghan & Ritchie
	int c,i,j;
	i=0;
	do s[i++]=n%bas+'0'; while ((n/=bas)>0);
	s[i]='\0';
	for (i=0, j=strlen(s)-1;i<j;i++,j--) {
		c=s[i];
		s[i]=s[j];
		s[j]=c;
	}
}

int spamUDPpackets(void* data) {
	while (gameIsActive) {
		createAndSendUDPPackets(ships, bullets);
		SDL_Delay(33);
	}
	return 1;
}

int main(int argc, char *argv[]) {
    int gameStartTime;
    int gameRunningTime;
    char map[]={"bestmapworld"};

    if (!init()) {
        printf("error initializing");
        return 1;
    }
    else {
        printf("initialized\n");
    }

    for (int i=0; i<MAX_CLIENTS; i++){ clients[i].id = i; }
    printf("Waiting for connection...\n");
    while (true) {
        acceptConnection();
        if(!loadMedia(map)) printf("Init failed at step: 4\n");
        if(!initGame(map))puts("Failed to init game"); else puts("Game initialized");
        packetOut = SDLNet_AllocPacket(860);
        packetID=0;
        createAndSendUDPPackets(ships, bullets);
        char gameCounter[MAX_LENGTH];
        GameFreezeTime = 3;                 // -1 = Game Active, 0 = Game Just Started, 1-3 = N seconds left
        while (GameFreezeTime>=-1) {
            sprintf(gameCounter,PREAMBLE_GAMEFREEZE"%d",GameFreezeTime);
            broadCast(gameCounter);
            if (GameFreezeTime== -1) break;
            SDL_Delay(1000);
            GameFreezeTime--;
        }
        gameIsActive = true;
        gameStartTime = SDL_GetTicks();
        SDL_DetachThread(SDL_CreateThread(udpListener, "udpThread", NULL));
        SDL_DetachThread(SDL_CreateThread(spamUDPpackets, "udpSpamThread", NULL));
        while (gameIsActive) {
            if(computerPlayerCount > 0)
                updateBots();

            if(timeWarpIsActive && (SDL_GetTicks() - timeWarpStartTime) >= TIMEWARP_DURATION) // If timewarp is active and has run for its duration; turn it off
                timeWarpIsActive = false;
            updateShip(ships);
            moveBullets(bullets);
            checkCollisions(ships,bullets);
            checkShipHealth();
            handlePowerupSpawns();          // Places the powerups on the map
            handlePowerupGains();           // Checks whether players aquire the placed powerups
            handleActivePowerups();         // Tests whether player's powerups run out
            gameRunningTime = SDL_GetTicks() - gameStartTime;
            if(gameRunningTime >= gameLenghtList[activeGameLength]*60000 || !ClientsAreReady()){ // *1000 for MS to S, *60 for Minutes
                SDL_Delay(5000);
                broadCast(PREAMBLE_GAMEEND);
                gameIsActive = false;
                for(int i=0; i < MAX_CLIENTS; i++){
                    if(clients[i].active){
                        clients[i].active = false;
                        clients[i].ready = false;
                    }
                }
                activeGameLength = 1;
                activeMaxSpeed = 1;
                activeBulletInterval = 1;
                activePowerupSpawns = 0;
                puts("All clients gone, game reset");
            }
            SDL_Delay(5);
        }
    }
    closeServer();
    return 0;
}

void acceptConnection() {
    int clientId;
    TCPsocket incomming;

    while (true) {
        incomming = SDLNet_TCP_Accept(TCPsock);

        if (incomming !=0x0) {
            if ((clientId = getClientId())<0) {printf("Too many users! \n");}
            else {
                clients[clientId].socket = incomming;
                clients[clientId].id = clientId;
                clients[clientId].active = true;
                clients[clientId].ipadress = SDLNet_TCP_GetPeerAddress(incomming)->host;
                SDL_Delay(100);
                SDL_DetachThread(SDL_CreateThread(Lobby, "lobbyThread", &clientId));
                incomming = NULL;
            }
        }
        if(ClientsAreReady() && humanPlayerCount > 0) {
            broadCast("$4starting in 3...");
            SDL_Delay(1000);
            if(ClientsAreReady() && humanPlayerCount > 0) {
                broadCast("$4starting in 2...");
                SDL_Delay(1000);
                if(ClientsAreReady() && humanPlayerCount > 0) {
                    broadCast("$4starting in 1...");
                    SDL_Delay(1000);
                    if (ClientsAreReady() && humanPlayerCount > 0) {
                        broadCast("!GO");
                        break;
                    }
                }
            }
        }
        incomming = NULL;
        SDL_Delay(500);
    }
}

int getClientId() {
    for (int i=0; i<MAX_CLIENTS; i++) {
        if (!clients[i].active) return i;
    }
    return -1;
}

bool loadMedia(char map[]) {
    char mapName[50];
    mapName[0] = '\0';
    strcat(mapName, "maps/");
    strcat(mapName, map);
    strcat(mapName, ".png");
    printf("Trying to load media: %s",mapName);
    background=IMG_Load(mapName);
    puts ("Loading media.");
    if (background==NULL) { puts("could not load background");return false;}
	backgroundBumpmap = malloc(sizeof(Uint8)*(background->w * background->h));
	int pixelSize = background->pitch/background->w;
	int i,j,k;
	Uint32 pixelResult;
	Uint8* pixels = (Uint8*)background->pixels;
	char* binary;
	binary=malloc(33);
	for (i=0; i<background->h; i++) {
		for (j=0; j<background->w;j++) {
			pixelResult = 0;
			for (k=0; k<3; k++) {
				pixelResult = pixelResult | pixels[(i*(background->w)+j)*pixelSize+k] << k*8;
			}
			itoa(pixelResult,binary,2);
			backgroundBumpmap[i*(background->w)+j]=pixelResult!=BACKGROUND_NONBUMPCOLOUR;
			if (pixelResult==(256*256*256-1)) {
				backgroundBumpmap[i*(background->w)+j]=BACKGROUND_BUMPMAP_LANDING_SPOT;
			}
		}
	}
	for (int i=0; i<MAX_CLIENTS; i++) {
		ships[i].surface=IMG_Load(SHIP_TEXTURE);
		if (ships[i].surface==NULL) return false;
		pixelSize = ships[i].surface->pitch/ships[i].surface->w;

		if (ships[i].surface->w%2!=1 || ships[i].surface->h%2!=1) {
			puts("Sprite's width and heigth was equal!");
			exit(1);
		}
		SDL_Point centerPoint;
		centerPoint.x = ships[i].surface->w/2;
		centerPoint.y = ships[i].surface->h/2;
		//checks how many collision pixels there is in the sprite
		int j,k,l,counter;
		Uint8* pixels = (Uint8*)ships[i].surface->pixels;
		// Finds collision pixels and mallocs
		for (j=0,counter=0; j<ships[i].surface->h; j++) {
			for (k=0; k<ships[i].surface->w;k++) {
				pixelResult = 0;
				for (l=0; l<3; l++) {
					pixelResult = pixels[(j*(ships[i].surface->w)+k)*pixelSize+l] << l*8;
				}
				if (pixelResult!=SHIP_NONBUMPCOLOUR) counter++;
			}
		}
		ships[i].pixels = malloc(sizeof(SDL_Point)*counter);
//		Gives all collision pixels a valuse relative to the sprite's origin
		ships[i].pixelCount = counter;
		for (j=0,counter=0; j<ships[i].surface->h; j++) {
			for (k=0; k<ships[i].surface->w;k++) {
				pixelResult = 0;
				for (l=0; l<3; l++) {
					pixelResult = pixels[(j*(ships[i].surface->w)+k)*pixelSize+l] << l*8;
				}
				if (pixelResult!=SHIP_NONBUMPCOLOUR) {
					ships[i].pixels[counter].x = k-centerPoint.x;
					ships[i].pixels[counter].y = j-centerPoint.y;
					counter++;
				}
			}
		}
	}
	return true;
}
bool init() {
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO)<0) printf("Init failed at step: 1\n, %s\n", SDL_GetError());

    if(SDLNet_Init()<0) printf("Init failed at step: 2\n, %s\n", SDLNet_GetError());

    int initFlags = IMG_INIT_PNG;
    if (!(IMG_Init(initFlags) & initFlags)) printf("Init failed at step: 3\n");

    udpSendSock = SDLNet_UDP_Open(4446);
    udpRecvSock = SDLNet_UDP_Open(4445);
    packetOut=SDLNet_AllocPacket(16);

    if (udpSendSock == NULL || udpRecvSock == NULL) printf("Init failed at step: 5\n");
    SDLNet_ResolveHost(&serverIP, NULL, 4444);
    TCPsock = SDLNet_TCP_Open(&serverIP);
    if(TCPsock==NULL)printf("Init failed at step: 6\n");

    computerPlayerCount = 0;
    humanPlayerCount = 0;
    for(int i=0; i < MAX_CLIENTS; i++)
        computerPlayerActive[i] = false;
    return true;
}
bool ClientsAreReady() {
    bool ready = false;
    for (int i=0; i<MAX_CLIENTS; i++) {
        if (clients[i].active) {
            ready = true;
            if (!clients[i].ready) return false;
        }
    }
    return ready;
}
void closeServer() {
    SDLNet_Quit();
    SDLNet_TCP_Close(TCPsock);
    SDLNet_UDP_Close(udpSendSock);
    SDLNet_UDP_Close(udpRecvSock);
    SDLNet_FreePacket(packetOut);
    SDL_Quit();
    IMG_Quit();
}
