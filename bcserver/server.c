#include "serverheader.h"

IPaddress serverIP;
TCPsocket TCPsock;

int spamUDPpackets(void* data) {
	while (gameIsActive) {
//		puts("Sending UDP.");
		createAndSendUDPPackets(ships, bullets);
		SDL_Delay(33);
	}
	return 1;
}

int main(int argc, char *argv[]) {
    int gameStartTime;
    int gameRunningTime;

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
        if(!initGame())puts("Failed to init game"); else puts("Game initialized");
        packetOut = SDLNet_AllocPacket(940);
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
//            createAndSendUDPPackets(ships, bullets);
            gameRunningTime = SDL_GetTicks() - gameStartTime;
            if(gameRunningTime >= gameLenghtList[activeGameLength]*60000 || !ClientsAreReady()){ // *1000 for MS to S, *60 for Minutes
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
                infiniteMomentum = false;
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
            printf("starting in 3...\n");
            broadCast("$4starting in 3...");
            SDL_Delay(1000);
            if(ClientsAreReady() && humanPlayerCount > 0) {
                printf("starting in 2...\n");
                broadCast("$4starting in 2...");
                SDL_Delay(1000);
                if(ClientsAreReady() && humanPlayerCount > 0) {
                    printf("starting in 1...\n");
                    broadCast("$4starting in 1...");
                    SDL_Delay(1000);
                    if (ClientsAreReady() && humanPlayerCount > 0) {
                        printf("!GO\n");
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

bool loadMedia() {
	background=IMG_Load(BACKGROUND_TEXTURE);
	if (background==NULL) return false;

	printf("Bakgrund W: %d, H: %d\n",background->w,background->h);
	printf("Jag mallokar %lu bytes.\n",sizeof(Uint8)*(background->w * background->h));
	backgroundBumpmap = malloc(sizeof(Uint8)*(background->w * background->h));
	printf("Pitch: %d\n",background->pitch);
	int pixelSize = background->pitch/background->w;
	printf("Det {r allts} %d bytes per pixel.\n",pixelSize);
	int i,j,k;
	Uint32 pixelResult;
	Uint8* pixlar = (Uint8*)background->pixels;
	for (i=0; i<background->h; i++) {
		for (j=0; j<background->w;j++) {
			pixelResult = 0;
			for (k=0; k<3; k++) {
				// LOL FOR THIS!
				pixelResult = pixlar[(i*(background->w)+j)*pixelSize+k] << k*8;
			}
			backgroundBumpmap[i*(background->w)+j]=pixelResult!=BACKGROUND_NONBUMPCOLOUR;
		}
	}
	
// Gammal bumpmapkod	
/*	int i,j;
	void* pixlar = background->pixels;
	int* siffra = (int*)pixlar;
	for (i=0; i<background->h; i++) {
		for (j=0; j<background->w;j++) {
			backgroundBumpmap[i* (background->w)+j]=(siffra[i* (background->w)+j]!=BACKGROUND_NONBUMPCOLOUR);
		}
	}*/
	
	for (int i=0; i<MAX_CLIENTS; i++) {
		ships[i].surface=IMG_Load(SHIP_TEXTURE);
		if (ships[i].surface==NULL) return false;
		
		printf("Sprajten: W: %d, H: %d\n",ships[i].surface->w,ships[i].surface->h);
		if (ships[i].surface->w%2!=1 || ships[i].surface->h%2!=1) {
			puts("Sprajten hade ett j{mnt antal pixlar i bredd eller h|jd!");
			exit(1);
		}
		SDL_Point mittpunkt;
		mittpunkt.x = ships[i].surface->w/2;
		mittpunkt.y = ships[i].surface->h/2;
//		printf("Mittpunkten {r (%d:%d)\n",mittpunkt.x,mittpunkt.y);
//		printf("Pitch: %d\n",laddadyta->pitch);
		//Kolla hur m}nga krockbara pixlar som finns i spriten.
		int j,k,l,counter;
		Uint8* pixlar = (Uint8*)ships[i].surface->pixels;
		// Leta upp krockbara pixlar i ytan, och malloca pixlar efter det.
		for (j=0,counter=0; j<ships[i].surface->h; j++) {
			for (k=0; k<ships[i].surface->w;k++) {
				pixelResult = 0;
				for (l=0; l<3; l++) {
					// LOL FOR THIS!
					pixelResult = pixlar[(j*(ships[i].surface->w)+k)*pixelSize+l] << l*8;
				}
				if (pixelResult!=SHIP_NONBUMPCOLOUR) counter++;
			}
		}
		// Gammal bumpmapkod
/*		int* siffra = (int*)ships[i].surface->pixels;
		for (j=0,counter=0; j<ships[i].surface->h; j++) {
			for (k=0; k<ships[i].surface->w;k++) {
				if (siffra[j* (ships[i].surface->w)+k]!=SHIP_NONBUMPCOLOUR) counter++;;
			}
		}*/
		printf("Hittade %d krockbara pixlar i spriten, mallocar.\n",counter);
		
		ships[i].pixlar = malloc(sizeof(SDL_Point)*counter);
//		Ge alla krockbara pixlar ett x och y-v{rde i f|rh}llande till sprajtens origo.
		ships[i].antalPixlar = counter;
		for (j=0,counter=0; j<ships[i].surface->h; j++) {
			for (k=0; k<ships[i].surface->w;k++) {
				pixelResult = 0;
				for (l=0; l<3; l++) {
					// LOL FOR THIS!
					pixelResult = pixlar[(j*(ships[i].surface->w)+k)*pixelSize+l] << l*8;
				}
				if (pixelResult!=SHIP_NONBUMPCOLOUR) {
					ships[i].pixlar[counter].x = k-mittpunkt.x;
					ships[i].pixlar[counter].y = j-mittpunkt.y;
					counter++;
				}
			}
		}
//		Gammal bumpmapkod
/*		for (j=0, counter=0; j<ships[i].surface->h; j++) {
			for (k=0; k<ships[i].surface->w;k++) {
				if ((Uint32)siffra[j* (ships[i].surface->w)+k]!=SHIP_NONBUMPCOLOUR) {
					ships[i].pixlar[counter].x = k-mittpunkt.x;
					ships[i].pixlar[counter].y = j-mittpunkt.y;
					printf("Krockbar pixel hittad p} (%d:%d)\n",ships[i].pixlar[counter].x,ships[i].pixlar[counter].y);
					counter++;
				}
			}
		}*/
	}
	return true;
}
bool init() {
    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO)<0) printf("Init failed at step: 1\n, %s\n", SDL_GetError());

    if(SDLNet_Init()<0) printf("Init failed at step: 2\n, %s\n", SDLNet_GetError());

    int initFlags = IMG_INIT_PNG;
    if (!(IMG_Init(initFlags) & initFlags)) printf("Init failed at step: 3\n");

    if(!loadMedia()) printf("Init failed at step: 4\n");

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
    SDL_Quit();
    IMG_Quit();
}
