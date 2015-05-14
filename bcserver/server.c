#include "serverheader.h"

IPaddress serverIP;
TCPsocket TCPsock;

int main(int argc, char *argv[]) {
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

        gameIsActive = true;
        SDL_DetachThread(SDL_CreateThread(udpListener, "udpThread", NULL));

        packetOut = SDLNet_AllocPacket(940);
        packetID=0;

        for(int i=0; i<8; i++){
            if(clients[i].active)
                printf("AACTIVE CLIENT (ID %d) HAS IP:%u RPORT:%u SPORT:%u\n", i, clients[i].ipadress, clients[i].recvPort, clients[i].sendPort);

        }

        while (1) {
            if (!ClientsAreReady()) { gameIsActive = false; puts("All clients gone, game resset"); }
            updateShip(ships);
            moveBullets(bullets);
            createAndSendUDPPackets(ships, bullets);
            SDL_Delay(20);
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
        if(ClientsAreReady()) {
            printf("starting in 3...\n");
            broadCast("$4starting in 3...");
            SDL_Delay(1000);
            if(ClientsAreReady()) {
                printf("starting in 2...\n");
                broadCast("$4starting in 2...");
                SDL_Delay(1000);
                if(ClientsAreReady()) {
                    printf("starting in 1...\n");
                    broadCast("$4starting in 1...");
                    SDL_Delay(1000);
                    if (ClientsAreReady()) {
                        printf("!GO\n");
                        broadCast("!GO");
                        SDL_Delay(500);
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
	SDL_SetColorKey(background,SDL_TRUE,SDL_MapRGB(background->format,255,255,255));
	for (int i=0; i<MAX_CLIENTS; i++) {
		ships[i].surface=IMG_Load(SHIP_TEXTURE);
		if (ships[i].surface==NULL) return false;
		SDL_SetColorKey(ships[i].surface,SDL_TRUE,SDL_MapRGB(ships[i].surface->format,255,255,255));
	}
	return true;
}
bool init() {

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
