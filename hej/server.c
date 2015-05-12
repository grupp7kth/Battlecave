#include "serverheader.h"

int main(int argc, char *argv[]) {
    bool gameIsActive;

    if (!init()) {
        printf("error initializing");
        return 1;
    }
    printf("initialized\n");
    printf("Waiting for connection...\n");
    while (true) {
        acceptConnection();
        initGame();
        printf("Game initialized");
        gameIsActive = true;
        packetId=0;
        SDL_DetachThread(SDL_CreateThread(udpListener, "udpThread", NULL));
        while (1) {
            updateBullets();
            updateShips();
            createAndSendUDPPackets();
            SDL_Delay(20);
//            if (!ClientsAreReady()) {
//                gameIsActive = false;
//            }
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
                clients[clientId].ip = SDLNet_TCP_GetPeerAddress(incomming)->host;
                SDL_DetachThread(SDL_CreateThread(Lobby, "lobbyThread", &clientId));
                incomming = NULL;
            }
        }
        if(ClientsAreReady()) {
            printf("starting in 3...\n");
            Broadcast("$4starting in 3...");
            SDL_Delay(1000);
            if(ClientsAreReady()) {
                printf("starting in 2...\n");
                Broadcast("$4starting in 2...");
                SDL_Delay(1000);
                if(ClientsAreReady()) {
                    printf("starting in 1...\n");
                    Broadcast("$4starting in 1...");
                    SDL_Delay(1000);
                    printf("!GO\n");
                    Broadcast("!GO");
                    break;
                }
            }
        }
        incomming = NULL;
        SDL_Delay(1000);
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
	SDL_SetColorKey(background,SDL_TRUE,SDL_MapRGB(background->format,255,255,255));
	for (int i=0; i<MAX_CLIENTS; i++) {
		ships[i].surface=IMG_Load(SHIP_TEXTURE);
		if (ships[i].surface==NULL) return false;
		SDL_SetColorKey(ships[i].surface,SDL_TRUE,SDL_MapRGB(ships[i].surface->format,255,255,255));
	}
	return true;
}
bool init() {
    if (SDL_Init(SDL_INIT_VIDEO)<0) { printf("SDL init failure.\n"); return false;}
    if (SDLNet_Init()<0) printf("SDL net init failure.\n");
    int initFlags = IMG_INIT_PNG;
    if (!(IMG_Init(initFlags) & initFlags)) { printf("IMG init failure.\n"); return false;}
    if (!loadMedia()) { printf("Couldn't load meida.\n"); return false;}
    udpSendSock = SDLNet_UDP_Open(0);
    udpRecvSock = SDLNet_UDP_Open(0);
    if (udpSendSock == NULL || udpRecvSock == NULL) { printf("Couldn't open UDP sockets.\n"); return false;}
    IPaddress serverIP;
    SDLNet_ResolveHost(&serverIP, NULL, 4444);
    TCPsock = SDLNet_TCP_Open(&serverIP);
    if (TCPsock==NULL) { printf("Couldn't open TCP socket.\n"); return false;}
    packetOut = SDLNet_AllocPacket(1024);
    //packetOut->data=gameData;
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
