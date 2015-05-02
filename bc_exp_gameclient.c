#include "bc_exp_clientheader.h"

bool initSDL();
bool initializeClientMedia();
SDL_Texture* laddaTextur();
void drawScreen(int clientID, clientShip* , background , clientBullet* , int skottAnt);
void clientClose();
void sendButtonsPressed(Uint8 act);
int unpackDataPacket(Uint8* data, clientShip* , clientBullet* , int* skottmangd);
int receiveGameData(void *data);

// Globala variabler
SDL_Rect skottrekt;
SDL_Window* ram;
SDL_Renderer* raster;
clientBullet skott[300];
background klientBakgrund;
clientShip klientSkepp[8];
Uint8 minAktivitet;
Uint8 *gameData;

TCPsocket tcpSocket;
UDPsocket udpSendSock,udpRecvSock;
UDPpacket *sandpaket, *speldatapaket;
Uint16 servernsUDPPort;
short mittID, poang;
char namn[16];

/** Main {r f|r tillf{llet ett fulhack; alldeles f|r l}ng. Den ska delas upp.
 * Den ber om namn, |ppnar tv} UDP-sockor, sen en TCP-socka (till LoopBack).
 * Om TCP-sockan kopplar, skickas, i ordning: namnet, UDPrecv och UDPsend.
 * D{refter v{ntar den p} ID och vilken UDP-port den ska skicka till.
 * D{refter startar den en separat tr}d som tar emot UDP-paket till UDPRecv-porten.
 * Sen startar spelloopen, som kollar knapptryckningar och sen, i ordning:
 * 1) skickar knapptryckningarna som UDP till servern.
 * 2) kollar igenom gameData (som UDPRecv-tr}den fyller) och konverterar till
 	skepps- och skottpositioner.
 * 3) Ritar upp sk}rmen.
 * 4) VŠntar i 20 ms.
	*/	
int main(int argc, char* arg[]) {
	SDL_Thread* taEmotTCPTrad, *taEmotUDPTrad;
	
	char input[MESSAGE_MAX_LENGTH];
	puts("Skriv in ditt namn:");
	fgets(input,16,stdin);
	
	udpRecvSock = SDLNet_UDP_Open(0);
//	Uint16 minUDPport = (SDLNet_UDP_GetPeerAddress(jag.udpRecvSock,-1))->port;
	char udpRecvPort[6];
	itoa((SDLNet_UDP_GetPeerAddress(udpRecvSock,-1))->port,udpRecvPort,10);
	printf("Min udp mottagarport: (%s)\n",udpRecvPort);

	udpSendSock = SDLNet_UDP_Open(0);
	char udpSendPort[6];
	itoa((SDLNet_UDP_GetPeerAddress(udpSendSock,-1))->port,udpSendPort,10);
	printf("Min udp s{ndarport: (%s)\n",udpSendPort);
	
	IPaddress serverIP;
	SDLNet_ResolveHost(&serverIP,"127.0.0.1",4444);
	tcpSocket = SDLNet_TCP_Open(&serverIP);
	if(tcpSocket) {
		puts("Servern {r p}.");
		printf("Skickar %s\n",input);
		SDLNet_TCP_Send(tcpSocket,input,MESSAGE_MAX_LENGTH);
		SDL_Delay(500);
		printf("Skickar %s\n",udpRecvPort);
		SDLNet_TCP_Send(tcpSocket,udpRecvPort,6);
		SDL_Delay(500);
		printf("Skickar %s\n",udpSendPort);
		SDLNet_TCP_Send(tcpSocket,udpSendPort,6);
		puts("Har skickat allt via TCP. V{ntar p} ID och serverns UDP-port.");
		
		udpRecvPort[0] = '\0';
		SDLNet_TCP_Recv(tcpSocket,udpRecvPort,6);
		printf("Fick ID '%s' fr}n servern.\n",udpRecvPort);
		mittID = atoi(udpRecvPort);
		printf("Check: mitt ID=%d\n",mittID);

		udpRecvPort[0] = '\0';
		SDLNet_TCP_Recv(tcpSocket,udpRecvPort,6);
		printf("Fick fr}n servern att den tar emot UDP p} %s\n",udpRecvPort);
		servernsUDPPort = atoi(udpRecvPort);
		printf("Check: %d\n",servernsUDPPort);
		
		while (input[0]!='K') {
			puts("Skriv in 'K' f|r att markera att du {r klar.");
			fgets(input,16,stdin);
			SDLNet_TCP_Send(tcpSocket,input,6);
		}
		puts("V{ntar p} att de andra ska bli klara.");
		while (strcmp(input,"GO") != 0) SDLNet_TCP_Recv(tcpSocket,&input,6);
		
		puts("Startar spelet.");

		if (!initSDL() || !initializeClientMedia()) {
			puts("Det gick }t skogen.");
			return 1;
		}
		
		sandpaket = SDLNet_AllocPacket(1);
		sandpaket->address.host = (SDLNet_TCP_GetPeerAddress(tcpSocket))->host;
		sandpaket->address.port = servernsUDPPort;
		speldatapaket = SDLNet_AllocPacket(1000);
		gameData = speldatapaket->data;

		SDL_Event e;
		bool quit = false;
		int antalSkott;
		
		taEmotUDPTrad = SDL_CreateThread(receiveGameData,"Tar emot speldata",NULL);
		SDL_DetachThread(taEmotUDPTrad);
		puts("Startar spelloopen");
		bool vanster, hoger,gasar,skjuter;
		while (!quit) {
			while (SDL_PollEvent(&e) > 0) {
				if (e.type == SDL_QUIT) quit=true;
				if (e.type == SDL_KEYDOWN) {
					switch (e.key.keysym.sym) {
					case SDLK_x: hoger=true;
						break;
					case SDLK_z: vanster=true;
						break;
					case SDLK_MINUS: gasar=true;
						break;
					case SDLK_RSHIFT: skjuter=true;
						break;
					}
				}
				if (e.type == SDL_KEYUP) {
					switch (e.key.keysym.sym) {
					case SDLK_x: hoger=false;
						break;
					case SDLK_z: vanster=false;
						break;
					case SDLK_MINUS: gasar=false;
						break;
					case SDLK_RSHIFT: skjuter=false;
						break;
					}
				}
			}
			minAktivitet = ((int)hoger | (int)vanster << 1 | (int)gasar << 2 | (int)skjuter << 3);
			sendButtonsPressed(minAktivitet);
			unpackDataPacket(gameData,klientSkepp,skott,&antalSkott);
			drawScreen(mittID,klientSkepp,klientBakgrund,skott,antalSkott);
			SDL_Delay(20);
		}
		puts("Bye!");
	}
	else puts("]t helvete!");
	clientClose();
}

/** Tar emot datapaket fr}n UDPRecv-porten. K|rs i en tr}d separat fr}n huvudtr}den.
 * Pekaren gameData (som unpackDataPacket refererar till) {r samma pekare som speldatapaket.data.
 *
 @var data: dummyv{rde.
 @return: ingenting, v{rdet {r ett dummy-v{rde. Loopen avslutas aldrig.
 */
int receiveGameData(void *data) {
	int check;
	while (1) {
		check =SDLNet_UDP_Recv(udpRecvSock,speldatapaket);
//		if (check>0) puts("Fick n}got");
		if (check <0) {
			puts("Fel i UDP-mottagning.");
			exit(1);
		}
		SDL_Delay(10);
	}
	return 0;
}

/** Skickar iv}g den lilla byte som motsvarar knapptryckningarna, via UDPSend-porten.
 @var act: En Uint8 (char) som motsvarar knapptryckningarna.
 */
void sendButtonsPressed(Uint8 act) {
	sandpaket->data[0] = act;
//	printf("Skickar knappaket till %d: %d\n",sandpaket->address.host,sandpaket->address.port);
	SDLNet_UDP_Send(udpSendSock,-1,sandpaket);
	return;
}

/** Initierar SDL
 @return: true om det lyckades, annars false.
 */
bool initSDL() {

	if (SDL_Init(SDL_INIT_VIDEO) <0) {
		printf("SDL fejlade: %s/n",SDL_GetError());
		return false;
	}
	int initFlags = IMG_INIT_PNG;
	if (!(IMG_Init(initFlags) & initFlags)) {
		printf("SDL Image fejlade: %s\n",IMG_GetError());
		return false;
	}
	return true;
}

/** \ppnar f|nster, startar rastren, laddar bilderna samt s{tter vissa startvariabler.
 @return: true om det gick bra, annars false.
 */
bool initializeClientMedia() {
	ram = SDL_CreateWindow("BattleCave",200,0,SCREEN_WIDTH,SCREEN_HEIGHT,SDL_WINDOW_SHOWN);
	if (ram == NULL) {
		printf("SDL kunde inte skapa f|nstret: %s\n",SDL_GetError());
		return false;
	}
	raster = SDL_CreateRenderer(ram,-1,SDL_RENDERER_ACCELERATED);
	if (raster == NULL) {
		printf("SDL kunde inte skapa rastret: %s\n",SDL_GetError());
		return false;
	}
	if (SDLNet_Init()<0) {
		printf("SDL Net fejlade %s/n",SDL_GetError());
		return false;
	}
	
	skottrekt.w=2;
	skottrekt.h=2;

	klientBakgrund.grafik=laddaTextur("cave.png");
	if (klientBakgrund.grafik== NULL) {
		printf("Bakgrund ej laddad: %s\n",IMG_GetError());
		return false;
	}
	SDL_QueryTexture(klientBakgrund.grafik,NULL,NULL,&klientBakgrund.width,&klientBakgrund.height);
	klientBakgrund.ritrekt.x = 1200;
	klientBakgrund.ritrekt.y = 800;
	klientBakgrund.ritrekt.w = SCREEN_WIDTH;
	klientBakgrund.ritrekt.h = SCREEN_HEIGHT;

	// Ladda grafiken f|r klientSkeppen.
	int i;
	for (i=0; i<8; i++) {
		klientSkepp[i].grafik=laddaTextur("skepp.png");
		if (klientSkepp[i].grafik==NULL) {
			printf("Skeppet ej laddat: %s\n",IMG_GetError());
			return false;
		}
		SDL_QueryTexture(klientSkepp[i].grafik,NULL,NULL,&klientSkepp[i].width,&klientSkepp[i].height);
		klientSkepp[i].drawPosition.w = klientSkepp[i].width;
		klientSkepp[i].drawPosition.h = klientSkepp[i].height;
		klientSkepp[i].drawPosition.x = klientSkepp[i].xPos-klientSkepp[i].width/2;
		klientSkepp[i].drawPosition.y = klientSkepp[i].yPos-klientSkepp[i].height/2;
	}
	return true;
}

/** laddar en bild och returnerar en SDL_Texture-pekare till bilden om det gick bra.
 @var filnamn: En char-pekare (str{ng) som inneh}ller filnamnet.
 @return: en SDL_Texture-pekare om det gick bra, annars NULL.
 */
SDL_Texture* laddaTextur(char* filnamn) {
	SDL_Surface* laddadyta = IMG_Load(filnamn);
	if (laddadyta==NULL) {
		printf("Lyckades inte ladda %s: %s",filnamn,IMG_GetError());
	}
	SDL_SetColorKey(laddadyta,SDL_TRUE,SDL_MapRGB(laddadyta->format,255,255,255));
	SDL_Texture* returtextur = SDL_CreateTextureFromSurface(raster, laddadyta);
	if (returtextur == NULL) {
		printf("Lyckades inte g|ra om ytan %s till en textur: %s",filnamn,IMG_GetError());
	}
	SDL_FreeSurface(laddadyta);
	return returtextur;
}

/** ritar upp grafiken p} sk{rmen, centrerat kring det egna skeppet.
 @var clientID: klientens ID, s} att uppritningen vet vilket skepp det ska utg} ifr}n.
 @var skeppen: en array av clientShip som ska ritas ut. Deras positioner }r absoluta f|r spelytan.
 @var bakgrunden: den bakgrundbild som ska ritas ut.
 @var skotten: en array av clientBullets som ska ritas ut. Deras positioner }r relativa f|r sk}rmen.
 @var skottAnt: en int som anger hur m}nga skott som finns i arrayen 'skotten'.
 */
void drawScreen(int clientID, clientShip skeppen[8], background bakgrunden, clientBullet skotten[300], int skottAnt) {
	if (skeppen[clientID].xPos < SCREEN_WIDTH/2) {
		bakgrunden.ritrekt.x=0;
		skeppen[clientID].drawPosition.x=skeppen[clientID].xPos-skeppen[clientID].width/2;
	}
	else if (skeppen[clientID].xPos > bakgrunden.width-SCREEN_WIDTH/2) {
		bakgrunden.ritrekt.x=bakgrunden.width-SCREEN_WIDTH;
		skeppen[clientID].drawPosition.x=SCREEN_WIDTH-bakgrunden.width+skeppen[clientID].xPos-skeppen[clientID].width/2;
	}
	else {
		bakgrunden.ritrekt.x=skeppen[clientID].xPos-SCREEN_WIDTH/2;
		skeppen[clientID].drawPosition.x=SCREEN_WIDTH/2-skeppen[clientID].width/2;
	}

	if (skeppen[clientID].yPos < SCREEN_HEIGHT/2) {
		bakgrunden.ritrekt.y=0;
		skeppen[clientID].drawPosition.y=skeppen[clientID].yPos-skeppen[clientID].height/2;
	}
	else if (skeppen[clientID].yPos > bakgrunden.height-SCREEN_HEIGHT/2) {
		bakgrunden.ritrekt.y=bakgrunden.height-SCREEN_HEIGHT;
		skeppen[clientID].drawPosition.y=SCREEN_HEIGHT-bakgrunden.height+skeppen[clientID].yPos-skeppen[clientID].height/2;
	}
	else {
		bakgrunden.ritrekt.y=skeppen[clientID].yPos-SCREEN_HEIGHT/2;
		skeppen[clientID].drawPosition.y=SCREEN_HEIGHT/2-skeppen[clientID].height/2;
	}

	SDL_RenderCopy(raster,bakgrunden.grafik,&bakgrunden.ritrekt,NULL);
	SDL_RenderCopyEx(raster,skeppen[clientID].grafik,NULL,&skeppen[clientID].drawPosition,skeppen[clientID].angle,NULL,SDL_FLIP_NONE);
	int i;
	for (i=0; i<8; i++) {
		if (i==clientID) continue;
		skeppen[i].drawPosition.x = skeppen[i].xPos-bakgrunden.ritrekt.x-skeppen[i].width/2;
		skeppen[i].drawPosition.y = skeppen[i].yPos-bakgrunden.ritrekt.y-skeppen[i].height/2;
		SDL_RenderCopyEx(raster,skeppen[i].grafik,NULL,&skeppen[i].drawPosition,skeppen[i].angle,NULL,SDL_FLIP_NONE);
	}
//	printf("Skepp 0: %d,%d (%d,%d), %d grad\n",skeppen[0].xPos,skeppen[0].yPos,skeppen[0].drawPosition.x,skeppen[0].drawPosition.y,skeppen[0].angle);
//	printf("Skepp 1: %d,%d (%d,%d), %d grad\n",skeppen[1].xPos,skeppen[1].yPos,skeppen[1].drawPosition.x,skeppen[1].drawPosition.y,skeppen[1].angle);
	SDL_SetRenderDrawColor(raster,255,255,255,255);

	// H{r ska det vara en metod f|r att rita ut skotten.
	for (i=0; i<skottAnt;i++) {
		skottrekt.x=skotten[i].xPos;
		skottrekt.y=skotten[i].yPos;
		SDL_RenderDrawRect(raster,&skottrekt);
	}
	SDL_RenderPresent(raster);
}

/** packar upp det som finns i 'data' och omvandlar det till skepps- och skottpositioner.
 @var data: En Uint8-array som ska packas upp.
 @var skeppen: En array av clientShip som ska fyllas med information.
 @var skotten: En array av clientBullet som ska fyllas med information.
 @var skottmangd: En int-pekare som fylls med information om hur m}nga skott som fanns.
 */
int unpackDataPacket(Uint8* data, clientShip* skeppen, clientBullet* skotten, int* skottmangd) {
	Uint32 tempint, inlast;
	int i, player;
	for (i=0, inlast=0; i<4; i++) {
		tempint = data[i];
//		printf("L{ser %d\n",data[i]);
		inlast = inlast | tempint << i*8;
	}
//	printf("Paketnummer: %d\n",inlast);
	for (player=0; player<8; player++) {
		for (i=0, inlast=0; i<4; i++) {
			tempint = data[4+player*4+i];
			inlast = inlast | tempint << i*8;
		}
		skeppen[player].xPos = inlast & 0b111111111111;
		skeppen[player].yPos = (inlast >> 12) & 0b111111111111;
		skeppen[player].angle = (inlast >> 24) & 0b111111;
		skeppen[player].angle *=6;
		skeppen[player].blown = (inlast >> 30) & 1;
		skeppen[player].active = (inlast >> 31) & 1;
	}
/*	for (player=0; player<8; player++) {
		printf("Skepp %d: %d,%d (%d) Blown:%d, active: %d\n",player,skeppen[player].xPos,skeppen[player].yPos,skeppen[player].angle,skeppen[player].blown,skeppen[player].active);
	}*/
	for (*skottmangd=0 ; data[36+(*skottmangd)*3]!=0xFF ; (*skottmangd)++) {
		for (i=0, inlast=0; i<3; i++) {
			tempint = data[36+(*skottmangd)*3+i];
			inlast = inlast | tempint << i*8;
		}
		skotten[*skottmangd].xPos = inlast & 0b1111111111;
		skotten[*skottmangd].yPos = (inlast >> 10) & 0b1111111111;
		skotten[*skottmangd].type = (inlast >> 20) & 0b1111;
	}
/*	printf("Fick in %d skott:\n",*skottmangd);
	for (i=0; i<*skottmangd; i++) {
		printf("Skott %d: %d,%d (%d)\n",i,skotten[i].xPos,skotten[i].yPos,skotten[i].type);
	}*/
	return 0;
}

/** st{nger ner allt och frig|r minnet.
 */
void clientClose() {
	SDL_DestroyWindow(ram);
	ram = NULL;
	SDL_DestroyRenderer(raster);
	raster = NULL;
	int i;
	for (i=0; i<8; i++) {
		SDL_DestroyTexture(klientSkepp[i].grafik);
		klientSkepp[i].grafik=NULL;
	}
	SDL_DestroyTexture(klientBakgrund.grafik);
	SDL_Quit();
	IMG_Quit();
	SDLNet_TCP_Close(tcpSocket);
	SDLNet_UDP_Close(udpSendSock);
	SDLNet_UDP_Close(udpRecvSock);
	SDLNet_Quit();
}
	
	
