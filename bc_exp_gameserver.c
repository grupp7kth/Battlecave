#include "bc_exp_serverheader.h"

 #include <SDL2/SDL.h>
 #include <SDL2/SDL_image.h>
 #include <SDL2/SDL_net.h>


bool initSDL();
bool loadServerMedia();
bool setStartingPositions();
void updateShip(serverShip* sk);
void moveShips(serverShip[MAX_CLIENTS]);
void moveBullets(bullet[MAX_BULLETS]);
void createAndSendUDPPackets(serverShip[MAX_CLIENTS], bullet[MAX_BULLETS]);

// Globala variabler
bullet serverSkott[MAX_BULLETS];
serverShip serverSkepp[MAX_CLIENTS];
client klienter[MAX_CLIENTS];
SDL_Surface* serverBakgrund;

UDPsocket udpSendSock,udpRecvSock;
UDPpacket *skraddarsyttPaket;
Uint8 dataToClient[1000];
int paketnummer;
bool speletPagar;

/** utility-funktion som rensar en str{ng. Ligger h{r ist{llet f|r i genericFunctions
 * f|r att den beh|vde en maxl{ngd.
 @var m: Den str{ng som ska rensas.
 */
void clearString(char m[MESSAGE_MAX_LENGTH]) {
	int i;
	for (i=0; i<MESSAGE_MAX_LENGTH; i++) m[i]='\0';
}

/** hittar client-ID baserat p} port.
 @var port: en Uint16; porten som man vill identifiera.
 @return: klient-ID:t om funnen, annars -1.
 */
int clientIDfromPort(Uint16 port) {
	int i;
	for (i=0; i<MAX_CLIENTS; i++) {
		if (klienter[i].aktiv && klienter[i].udpSendPort==port) return i;
	}
	return -1;
}

/** l{ser av UDP-porten f|r inkommande paket, identifierar klient-ID och st{ller in
 * r{tt spelares olika statusar d{refter.
 @var data: dummy-indata. Anv{nds inte.
 @return: en dummy-int som inte anv{nds. Loopen stannar aldrig.
 */
int udpListener(void* data) {

	// H{r ska den l{sa in fr{n UDP, l{gga den i 'inkommandeData' (en Uint8), identifiera VEM som skickade,
	// och lagra detta i klientID (en int). D} kommer denna klients skepp att s{ttas till r{tt v{rden.
	// Som synes, s} Šr klientID nu alltid 0, och 'inkommandeData' {r alltid 'minAktivitet'.
	// Tittar ni i gameclient.c, ser ni att 'minAktivitet' {r den Uint8 som s{tts till olika v{rden
	// beroende p} vilken knapp man trycker ner.
	UDPpacket *inkommandepaket;
	inkommandepaket = SDLNet_AllocPacket(16);
	short klientID=0, siffranSomKomIn=0;
	puts("udplistener startad.");
	//printf("data:%d\n",inkommandepaket->data[0]);
	int check;
//	while(1){
//        if(SDLNet_UDP_Recv(udpRecvSock,inkommandepaket)) {
//
//            printf("data:%d\n",inkommandepaket->data[0]);
//
//        }
//    }
	while (1) {
		check = SDLNet_UDP_Recv(udpRecvSock,inkommandepaket);
		if (!check) continue;
		printf("Fick in ett UDP-paket fr}n port %d: %d\n",inkommandepaket->address.port,inkommandepaket->data[0]);
		if ((klientID = clientIDfromPort(inkommandepaket->address.port)) < 0 ) {
			printf("Fel! paket fr}n ok{nt klient, port %d\n",inkommandepaket->address.port);
			exit(1);
		}
		siffranSomKomIn = inkommandepaket->data[0];
        //printf("klientid: %d\n siffransomkomin: %d\n", klientID,siffranSomKomIn);
		if ((siffranSomKomIn & 3) == 1) serverSkepp[klientID].vinkelHast=5;
		else if ((siffranSomKomIn & 3) == 2) serverSkepp[klientID].vinkelHast=-5;
		else serverSkepp[klientID].vinkelHast = 0;
		if ((siffranSomKomIn & 4) == 4) serverSkepp[klientID].accar=true;
		else serverSkepp[klientID].accar=false;
		if ((siffranSomKomIn & 8) == 8) serverSkepp[klientID].skjuter=true;
		else serverSkepp[klientID].skjuter=false;

		SDL_Delay(5);
	}
	return 255;
}

/** tar in ett x och ett y-v{rde och en rektangel och avg|r om positionen {r i rektangeln.
 @var x: x-v{rdet.
 @var y: y-v{rdet.
 @var r: den SDL_Rect man vill veta om den omsluter positionen.
 @return true om punkten {r i rektangeln, annars false.
 */
bool isInside(int x, int y, SDL_Rect* r) {
	return (x >= r->x && x < (r->x+r->w) && y >= r->y && y < (r->y+r->h));
}

/** skapar en en Uint8-array (dataToClient) av alla skeppspositioner och skottpositioner.
 * Denna skr{ddarsys fšr varje klient, eftersom den bara skickar de skottpositioner
 * som klienten ska se p} sin sk{rm. Rent konkret inneh}ller varje paket:
 * Byte 0Ð3 (4 bytes) en int som {r en tidsangivelse (paketnumrering).
 * Byte 4Ð35 (32 bytes) information om det skeppen (4 bytes * 8 skepp).
 * Byte 36Ð (max Ð935) Upp till 300 positioner (3 bytes var).
 *	Alla dessa {r skottpositioner f|r klientens sk{rm, plus 4 kontrollbitar per skott.
 *	Metoden kontrollerar vilka skott som ska vara med p} spelarens sk{rm, och skickar positionen
 *	endast f|r detta skott.
 *	Efter sista relevanta skottet skickar den en byte '11111111' som markerar slutet.
 *	Arrayen {r allts} 937 bytes stor (0Ð936).
 * Fult nog {r dataToClient global f|r tillf{llet.
 @var skeppen: en serverShip-array av de skepp som finns i spelet.
 @var skotten: en serverBullet-array av de skott som finns i spelet.
 */
void createAndSendUDPPackets(serverShip skeppen[8],bullet skotten[MAX_BULLETS]) {
	SDL_Rect viewport;
	viewport.w = SCREEN_WIDTH;
	viewport.h = SCREEN_HEIGHT;
	int i, player, secondary, counter,UDPpacketLength;
	Uint32 tempint;

	paketnummer++; // Det h{r {r tidsangivelsen. Den b|rjar p} 0 och |kar med 1 f|r varje paket.

	for (i=0; i<4; i++) {
		dataToClient[i] = paketnummer >> i*8;
	}
	for (player=0; player<MAX_CLIENTS; player++) {
		tempint=0;
		tempint = (int)skeppen[player].xPos | (int)skeppen[player].yPos << 12 | (int)(skeppen[player].vinkel/6) << 24;
		tempint = tempint | skeppen[player].blammad <<30 | klienter[player].aktiv << 31;
		for (i=0; i<4; i++) {
			dataToClient[4+player*4+i] = tempint >> i*8;
		}
	}
	// Nu har metoden lagt in informationen om paketnummer och alla skepp. Denna information {r
	// densamma f|r ALLA spelare. Nu {r det dags att filtrera vilka skott som ska skickas med.
	// D{rf|r k|r vi en ny player-loop, d{r den kollar spelarens position, och skickar med
	// de relevanta skotten. Efter att den gjort det, ska den skicka UPD-paketet till den spelaren
	// och sen g|ra samma sak f|r n{sta spelare.

	for (player=0; player<MAX_CLIENTS; player++) {
		if (!klienter[player].aktiv) continue;
		// H{r ska den r{kna ut "viewport" f|r spelaren, eller kontrekt uttryckt:
		// Vilken position p} banan som |vre v{nstra h|rnet p} hans sk{rm har.
		if (skeppen[player].xPos < SCREEN_WIDTH/2)
			viewport.x=0;
		else if (skeppen[player].xPos > STAGE_WIDTH-SCREEN_WIDTH/2)
			viewport.x=STAGE_WIDTH-SCREEN_WIDTH;
		else viewport.x=skeppen[player].xPos-SCREEN_WIDTH/2;
		if (skeppen[player].yPos < SCREEN_HEIGHT/2)
			viewport.y=0;
		else if (skeppen[player].yPos > STAGE_HEIGHT-SCREEN_HEIGHT/2)
			viewport.y=STAGE_HEIGHT-SCREEN_HEIGHT;
		else viewport.y=skeppen[player].yPos-SCREEN_HEIGHT/2;

		// Viewporten {r nu utr{knad. Dags att g} igenom skott-arrayen och kolla vilka skott som ska skickas.

		for (secondary=0, counter=0; secondary<MAX_BULLETS; secondary++) {
			if (serverSkott[secondary].on && isInside((int)serverSkott[secondary].xPos, (int)serverSkott[secondary].yPos, &viewport)) {
				tempint=0;
				tempint = (int)(serverSkott[secondary].xPos-viewport.x) | (int)(serverSkott[secondary].yPos-viewport.y) << 10 | (int)(serverSkott[secondary].typ) << 20;
				for (i=0; i<3; i++) {
					dataToClient[36+counter*3+i] = tempint >> i*8;
				}
				counter++;
			}
		}
		UDPpacketLength = 36+counter*3;
		dataToClient[UDPpacketLength++]=0xFF;
		skraddarsyttPaket->data = dataToClient;
		skraddarsyttPaket->len = UDPpacketLength;
		skraddarsyttPaket->address.host=klienter[player].ipadress;
		skraddarsyttPaket->address.port=klienter[player].udpRecvPort;
		SDLNet_UDP_Send(udpSendSock,-1,skraddarsyttPaket);
//		printf("Skickade paket till %s\n",klienter[player].namn);

	}
}

/** Hittar en ledig plats i arrayen av skott
 @var skotten: Arrayen av skott f|r spelet.
 @return: ett ID som motsvarar en ledig position i arrayen.
 */
int findFreeBullet(bullet skotten[MAX_BULLETS]) {
	int i;
	for (i=0; i<MAX_BULLETS; i++) {
		if (!skotten[i].on) return i;
	}
	return -1;
}

/** L{gger till ett skott i spelet och r{knar ut dess hastighet.
 * Fult nog {r arrayen 'serverSkott' global.
 @var skeppet: Det serverShip som just skjutit.
 */

void addBullet(serverShip* skeppet) {
	int ledigPlats = findFreeBullet(serverSkott);
	if (ledigPlats <0) {
		puts("FEL: Hittade inte ledig plats i skottlistan!");
		exit(1);
	}
	serverSkott[ledigPlats].xPos=skeppet->xPos;
	serverSkott[ledigPlats].yPos=skeppet->yPos;
	serverSkott[ledigPlats].yVel=skeppet->yVel-sin(getRadians(skeppet->vinkel))*6;
	serverSkott[ledigPlats].xVel=skeppet->xVel-cos(getRadians(skeppet->vinkel))*6;
	serverSkott[ledigPlats].on = true;
//	printf("Bam[%d]: %f,%f\n",antalSkott,serverSkott[antalSkott].xPos,serverSkott[antalSkott].yPos);
}

/** uppdaterar ett skepps position baserat p} hastighet.
 @var skeppet: Det skepp som ska uppdateras.
 */
void updateShip(serverShip* skeppet) {
	if (skeppet->accar) {
		skeppet->yVel-=sin(getRadians(skeppet->vinkel))*0.1;
		skeppet->xVel-=cos(getRadians(skeppet->vinkel))*0.1;
	}
	if (skeppet->skjuter) {
		if (skeppet->skottKylning ==0) {
			addBullet(skeppet);
			skeppet->skottKylning = skeppet->skottIntervall;
		}
	}
	skeppet->xPos += skeppet->xVel;
	skeppet->yPos += skeppet->yVel;
	if (skeppet->xPos > STAGE_WIDTH) skeppet->xPos=0;
	if (skeppet->xPos <0) skeppet->xPos=STAGE_WIDTH;
	if (skeppet->yPos > STAGE_HEIGHT) skeppet->yPos=0;
	if (skeppet->yPos <0) skeppet->yPos=STAGE_HEIGHT;
	skeppet->vinkel += skeppet->vinkelHast;
	if (skeppet->vinkel > 360) skeppet->vinkel-=360;
	if (skeppet->vinkel < 0) skeppet->vinkel+=360;
	if (skeppet->skottKylning>0) skeppet->skottKylning--;
}

/** uppdaterar alla skotts positioner. St{nger av dem om de hamnar utanf|r spelf{ltet.
 @var skotten: den array av serverBullet som ska uppdateras.
 */
void moveBullets(bullet skotten[MAX_BULLETS]) {
	int i;
	for (i=0; i<MAX_BULLETS; i++) {
		if (skotten[i].on) {
			skotten[i].xPos += skotten[i].xVel;
			skotten[i].yPos += skotten[i].yVel;
			if (skotten[i].xPos > STAGE_WIDTH || skotten[i].xPos < 0 || skotten[i].yPos<0 || skotten[i].yPos>STAGE_HEIGHT) {
				skotten[i].on=false;
			}
		}
	}
}

/** anropar updateShip f|r en array av serverShip.
 @var skeppen: den serverShip-array som ska uppdateras.
 */
void moveShips(serverShip skeppen[MAX_CLIENTS]) {
	int i;
	for (i=0; i<MAX_CLIENTS; i++) {
		updateShip(&skeppen[i]);
	}
}

/** Initierar SDL.
 @return: true om det gick bra, annars false.
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
	if (SDLNet_Init() <0) {
		printf("SDL_Net fejlade: %s\n",SDL_GetError());
		return false;
	}
	return true;
	SDL_Window *ram;
	ram = SDL_CreateWindow("Server dummy",10,10,10,10,SDL_WINDOW_SHOWN);
}

/** Laddar bakgrundgrafiken och skeppens grafik. Dess beh|vs i stort sett endast f|r
 * kollisionskontroll.
 */
bool loadServerMedia() {
	// Ladda serverBakgrundens grafik
	serverBakgrund=IMG_Load("cave.png");
	if (serverBakgrund==NULL) {
		printf("Lyckades inte ladda serverBakgrund: %s",IMG_GetError());
	}
	SDL_SetColorKey(serverBakgrund,SDL_TRUE,SDL_MapRGB(serverBakgrund->format,255,255,255));
	puts ("Laddat bakrunden.");

	// Ladda serverSkeppens grafik
	int i;
	for (i=0; i<MAX_CLIENTS; i++) {
		serverSkepp[i].bild=IMG_Load("skepp.png");
		if (serverSkepp[i].bild==NULL) {
			printf("Lyckades inte ladda Skepp: %s",IMG_GetError());
		}
		printf("Laddat skepp %d\n",i);
		SDL_SetColorKey(serverSkepp[i].bild,SDL_TRUE,SDL_MapRGB(serverSkepp[i].bild->format,255,255,255));
	}

	return true;
}

/** Frig|r alla laddade ytor.
 */
void serverClose() {
	SDL_FreeSurface(serverBakgrund);
	int i;
	for (i=0; i<MAX_CLIENTS; i++) {
		SDL_FreeSurface(serverSkepp[i].bild);
	}
	SDL_Quit();
	IMG_Quit();
}

/** St{ller in n}gra dummy-v{rden f|r skeppen f|r testets skull.
 */
bool setStartingPositions() {
	// Nollst{ll alla serverSkepp i arrayen.
	int i;
	for (i=0; i<MAX_CLIENTS; i++) {
		serverSkepp[i].bild = NULL;
		serverSkepp[i].xPos =0;
		serverSkepp[i].yPos =0;
		serverSkepp[i].xVel =0;
		serverSkepp[i].yVel =0;
		serverSkepp[i].skottIntervall = 10;
		serverSkepp[i].skottKylning = 0;
		serverSkepp[i].vinkelHast = 0;
		serverSkepp[i].vinkel = 0;
		serverSkepp[i].accar = false;
		serverSkepp[i].skjuter = false;
		serverSkepp[i].blammad = false;
	}
	// S{tt in lite startv{rden.
	serverSkepp[0].xPos = 400;
	serverSkepp[0].yPos = 300;
	serverSkepp[0].skottIntervall =10;
	serverSkepp[0].skottKylning=0;
	serverSkepp[1].xPos = 500;
	serverSkepp[1].yPos = 400;
	serverSkepp[1].xVel = 0;
	serverSkepp[1].vinkelHast = -3;
	serverSkepp[2].xPos = 1000;
	serverSkepp[2].yPos = 1200;
	serverSkepp[2].vinkelHast = +5;
	serverSkepp[3].xPos = 2000;
	serverSkepp[3].yPos = 1200;
	serverSkepp[3].yVel = -2;
	serverSkepp[4].xPos = 10;
	serverSkepp[4].yPos = 10;
	serverSkepp[4].xVel = 2;
	serverSkepp[4].yVel = 2;
	serverSkepp[5].xPos = 1200;
	serverSkepp[5].yPos = 800;
	serverSkepp[5].xVel = 1.5;
	serverSkepp[5].yVel = -1.5;
	return true;
}

/** Utbyter n|dv{ndig information med klienten via TCP-sockan, samt h}ller reda p} om klienten kopplar ner.
 * I ordning tar den in spelarens namn, den UDP-port som spelaren tar emot p}, samt den
 * UDP-port som spelaren skickar ifr}n. D{refter skickar den tillbaka spelarens ID och
 * den UDP-port som servern ska ha sin information till.
 * Sen startar en loop som v{ntar p} ett 'K' fr}n klienten, som {r signalen f|r att
 * klienten {r klar f|r att b|rja spela.
 * D{refter v{ntar den vidare, men kommer inte att g|ra n}got. Om TCP-porten st{ngs, kommer
 * funktionen att avaktivera klienten och avsluta sig sj{lv.
 * Denna funktion k[rs i en separat tr}d, en per klient.
 @var data: en pekare till klienten ifr}ga.
 */
int handshakeClient(void* data) {
	client* dennaKlient;
	dennaKlient = (client*)data;
	char meddelande[MESSAGE_MAX_LENGTH];
	clearString(meddelande);
	printf("Trad startad, jag har socket %p\n",dennaKlient->tcpSocket);
	// V{nta p} inkommande namn
	SDLNet_TCP_Recv(dennaKlient->tcpSocket,meddelande,MESSAGE_MAX_LENGTH);

	int i;
	for (i=0; i<MESSAGE_MAX_LENGTH; i++) if (meddelande[i]=='\n') meddelande[i]= '\0';
	printf("Fick in namn:%s!\n",meddelande);
	stringCopy(meddelande,dennaKlient->namn);
	clearString(meddelande);

	// V{nta p} inkommande UDP-portnummer.
	SDLNet_TCP_Recv(dennaKlient->tcpSocket,meddelande,MESSAGE_MAX_LENGTH);
	dennaKlient->udpRecvPort = atoi(meddelande);
	printf("%s tar emot UDP p} port %d\n",dennaKlient->namn,dennaKlient->udpRecvPort);
	clearString(meddelande);

	SDLNet_TCP_Recv(dennaKlient->tcpSocket,meddelande,MESSAGE_MAX_LENGTH);
	dennaKlient->udpSendPort = atoi(meddelande);
	printf("%s kommer att skicka ifr}n port %d\n",dennaKlient->namn,dennaKlient->udpSendPort);
	clearString(meddelande);

	// Skicka spelarens ID.
	char sms[6];
	sms[0]=dennaKlient->ID+'0';
	sms[1]='\0';
	printf("Skickar spelarens ID: %d\n",dennaKlient->ID);
	SDLNet_TCP_Send(dennaKlient->tcpSocket,sms,6);

	// Skicka den egna UDP-porten.
	itoa((SDLNet_UDP_GetPeerAddress(udpRecvSock,-1))->port,sms,10);
	printf("Skickar att serverns UDP-ta-emot {r %s\n",sms);
	SDLNet_TCP_Send(dennaKlient->tcpSocket,sms,6);

	dennaKlient->aktiv = true;
	clearString(meddelande);
	int check=1;
	while (1) {
		check=SDLNet_TCP_Recv(dennaKlient->tcpSocket,meddelande,MESSAGE_MAX_LENGTH);
		puts("N}got kom fr}n klienten.");
		if (!check) {
			printf("%s dog!\n",dennaKlient->namn);
			break;
		}
		printf("Fick in '%s' fr}n %s\n",meddelande,dennaKlient->namn);
		if (meddelande[0]!='K') continue;
		puts("Verkar som om klienten {r klar.");
		dennaKlient->klar = true;
		SDL_Delay(1000);
	}
	SDLNet_TCP_Close(dennaKlient->tcpSocket);
	dennaKlient->aktiv=false;
	bool test = false;
	for (int i=0; i<MAX_CLIENTS; i++) {
		if (klienter[i].aktiv) test = true;
	}
	if (!test) speletPagar=false;
	return 255;
}

/** Hitta en ledig klientplats i arrayen
 @var users: den client-array som ska unders|kas.
 @return ett ledigt ID.
 */
int findFreeClient(client users[MAX_CLIENTS]) {
	int found;
	for (found=0; found<MAX_CLIENTS; found++) {
		if (!users[found].aktiv) return found;
	}
	return -1;
}

/** avg|r om samtliga aktiva klienter {r klara.
 @var users: den client-array som ska unders|kas.
 @return true om alla {r klara, annars false.
 */
bool allActiveClientsAreReady(client users[MAX_CLIENTS]) {
	bool ready = false;
	int i;
	for (i=0; i<MAX_CLIENTS; i++) {
		if (users[i].aktiv) {
			ready = true;
			if (!users[i].klar) return false;
		}
	}
	return ready;
}

/** En alldeles f|r l}ng huvudtr}d. Den |ppnar tv} UDP-sockor, och en server-TCP-socka.
 * Den lyssnar efter nyinkomna klienter, och startar en ny tr}d f|r varje ny klient.
 * Den kontrollerar om alla p}loggade klienter {r klara, och om de {r det, initieras
 * SDL och spelloopen startas. Spelloopen g|r, i ordning:
 * flyttar alla skepp och skott
 * skapar UDP-paketen och skickar
 * v{ntar i 20ms.
 */
int main(int argc, char* arg[]) {
	TCPsocket serversocket,inkommande;
	SDL_Thread* klienttradar[MAX_CLIENTS];
	SDL_Thread *UDPlyssnartrad;
	int freeID;
	SDLNet_Init();
	char meddelande[MESSAGE_MAX_LENGTH];
	printf("Meddelandel{ngd %lu\n",strlen(meddelande));
	IPaddress serverip;

	udpSendSock = SDLNet_UDP_Open(0);
	udpRecvSock = SDLNet_UDP_Open(0);
	if (udpSendSock == NULL || udpRecvSock == NULL) {
		printf("UDP-socketarna kunde inte |ppnas: %s\n",SDLNet_GetError());
	}
	else puts("UDP-socketarna |ppna.");

	SDLNet_ResolveHost(&serverip,NULL,4444);
	serversocket = SDLNet_TCP_Open(&serverip);
	printf("Serverns ip {r %d och port %d\nWaiting...\n",serverip.host,serverip.port);

	while(!allActiveClientsAreReady(klienter)) {
		inkommande = SDLNet_TCP_Accept(serversocket);
		if (inkommande != 0x0) {
			if ((freeID=findFreeClient(klienter))<0) puts("Too many users!");
			else {
				klienter[freeID].ID=freeID;
				printf("KlientID %d! Socketen {r %p, startar handskak!\n",freeID,inkommande);
				klienter[freeID].tcpSocket = inkommande;
				klienter[freeID].ipadress = SDLNet_TCP_GetPeerAddress(inkommande)->host;
				printf("Klienten har IP-adress (swappad): %d\n",klienter[freeID].ipadress);
				char URL[16];
				IPtoAscii(klienter[freeID].ipadress,URL);
				printf("Eller uttryckt i URL-form: %s\n",URL);
				klienttradar[freeID] = SDL_CreateThread(handshakeClient,"Generic",&klienter[freeID]);
				SDL_DetachThread(klienttradar[freeID]);
				inkommande = NULL;
			}
		}
		SDL_Delay(1000);
	}
	// H{r har (f|rhoppningsvis) alla klienter sagt att de {r redo.
	puts("Alla klienter }r klara. Skickar ett OK till alla och startar spelet.");

	int i;
	for (i=0; i<MAX_CLIENTS; i++) {
		if (klienter[i].aktiv) SDLNet_TCP_Send(klienter[i].tcpSocket,"GO",3);
	}

	if (!initSDL() || !loadServerMedia()) {
		puts("Det gick }t skogen.");
		return 1;
	}
	puts("initaliserad.");
	if (!setStartingPositions()) {
		puts("Det gick }t skogen.");
		return 1;
	}
	puts("Startpositioner satta.");

	puts("Startar spelet.");
	speletPagar = true;
	UDPlyssnartrad = SDL_CreateThread(udpListener,"UDP listen",NULL);
	SDL_DetachThread(UDPlyssnartrad);


	skraddarsyttPaket=SDLNet_AllocPacket(940);

	paketnummer=0;
	SDL_Event e;
	while (speletPagar) {
		moveShips(serverSkepp);
		moveBullets(serverSkott);
		createAndSendUDPPackets(serverSkepp, serverSkott);
		SDL_Delay(20);
		while (SDL_PollEvent(&e) > 0) {
			if (e.type == SDL_QUIT) speletPagar=false;
		}
	}
}
