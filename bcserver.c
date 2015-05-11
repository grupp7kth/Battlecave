#include "bcheaders.h"
#define SERVER_IP "193.10.39.101"
#define PLAYER  "Player"


void acceptConnections();
void closeServer();
int getFreeClientID();
bool allActiveClientsAreReady();
bool initSDL();
bool loadServerMedia();
bool setStartingPositions();
void updateShip(serverShip* sk);
void moveShips(serverShip[MAX_CLIENTS]);
void moveBullets(bullet[MAX_BULLETS]);
void createAndSendUDPPackets(serverShip[MAX_CLIENTS], bullet[MAX_BULLETS]);
int udpListener(void* data);
int clientIDfromPort(Uint16 port);
double getRadians(int a);
bool isInside(int x, int y, SDL_Rect* r);

SDL_Thread* clientThreads[MAX_LENGTH];
SDL_Thread* UDPListener;
IPaddress serverIP;
TCPsocket serverTCPsocket;
TCPsocket incomming = NULL;


int main(int argc, char* args[])
{
    int i;
    
    for (int i=0; i<MAX_CLIENTS; i++){ players[i] = createClient(playerSocket[i],i); }
    
    if (!initSDL() || !loadServerMedia()) { puts("Could not load media or failed to initSDL."); return 1; }
    
    SDLNet_Init();
    
    udpSendSock = SDLNet_UDP_Open(0);
    udpRecvSock = SDLNet_UDP_Open(0);
    if (udpSendSock == NULL || udpRecvSock == NULL) {
        printf("UDP-sockets could not open: %s\n",SDLNet_GetError());}   else puts("UDP-sockets are open.");
    
    SDLNet_ResolveHost(&serverIP, NULL, 4444);
    serverTCPsocket = SDLNet_TCP_Open(&serverIP);
    socketSet = SDLNet_AllocSocketSet(MAX_CLIENTS); if (!socketSet) {printf("%s\n", SDLNet_GetError());}
    
    // Waitin for connections....
    
    while (true) {
        acceptConnections();
        
        puts("All clients are ready. Sending GO to all clients and starting the game.");
        for (i=0; i<MAX_CLIENTS; i++) { if (players[i].active) SDLNet_TCP_Send(players[i].socket,"!GO",4); } // !GO senare

        if (!setStartingPositions()) { puts("Fuck..."); return 1;} puts("Startingpositions are set.");
        
        puts("Starting the game.");
        speletPagar = true;
        
        UDPListener = SDL_CreateThread(udpListener,"UDP listen",NULL);
        SDL_DetachThread(UDPListener);
        
        skraddarsyttPaket = SDLNet_AllocPacket(940);
        
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
    closeServer();
}


void closeServer(){
    SDL_FreeSurface(serverBakgrund);
    for (int i=0; i<MAX_CLIENTS; i++) {
        SDL_FreeSurface(serverSkepp[i].bild);
    }
    SDLNet_TCP_Close(serverTCPsocket);
    SDLNet_UDP_Close(udpRecvSock);
    SDLNet_UDP_Close(udpSendSock);
    SDL_Quit();
    SDLNet_Quit();
    IMG_Quit();
}

int getFreeClientID(){ // Letar upp en ledig clientplats och returnerar ett clientID
    for (int i=0; i<MAX_CLIENTS; i++) {
        if (!players[i].active) return i;
    }
    return -1;
}

void acceptConnections(){
    
    int clientThr1 = 0, clientThr2 = 0, freeID;
    char starts[]={"$4GO!"};
    char startsInOne[]={"$4Game starts in 1 second"};
    char startsInTwo[]={"$4Game starts in 2 seconds"};
    char startsInThree[]={"$4Game starts in 3 seconds"};
    
    printf("Waiting for connections ...\n");
    while (true){
        incomming = SDLNet_TCP_Accept(serverTCPsocket);
        if (incomming !=0x0) {
            if ((freeID = getFreeClientID())<0) {printf("Too many users! \n");}     // Ger uppkopplad kient ett ledigt clientID
            else{
                
                players[freeID].socket = incomming;
                players[freeID].ID = freeID;
                players[freeID].active = true;
                players[freeID].ipadress = SDLNet_TCP_GetPeerAddress(incomming)->host;
                printf("ClientID: %d    Connected\n", players[freeID].ID);
                
                activeClients = SDLNet_TCP_AddSocket(socketSet, players[freeID].socket);
                
                clientThreads[clientThr2] = SDL_CreateThread(chattserverfunction, "ClientChattThread", &players[freeID]);
                SDL_DetachThread(clientThreads[clientThr2]);
                clientThr1 ++;
                clientThr2 ++;
                incomming = NULL;
            }
        }
        if (allActiveClientsAreReady()) {
            sendMessageAll(startsInThree); puts("Skicakt 3"); SDL_Delay(1000);
            if (allActiveClientsAreReady()) {
                sendMessageAll(startsInTwo); puts("Skickat 2"); SDL_Delay(1000);
                if (allActiveClientsAreReady()) {
                    sendMessageAll(startsInOne); puts("Skickat 1"); SDL_Delay(1000);
                    if (allActiveClientsAreReady()) {
                        sendMessageAll(starts); SDL_Delay(1000); break;
                    }
                }
            }
        }
        //SDL_Delay(500);
    }
}





//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------




/** avg|r om samtliga aktiva klienter {r klara.
 @var users: den client-array som ska unders|kas.
 @return true om alla {r klara, annars false.
 */
bool allActiveClientsAreReady() {
    bool ready = false;
    int i;
    for (i=0; i<MAX_CLIENTS; i++) {
        if (players[i].active) {
            ready = true;
            if (!players[i].ready) return false;
        }
    }
    return ready;
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
        printf("Could not load serverBakgrund: %s",IMG_GetError());
    }
    SDL_SetColorKey(serverBakgrund,SDL_TRUE,SDL_MapRGB(serverBakgrund->format,255,255,255));
    puts ("Background loaded.");
    
    // Ladda serverSkeppens grafik
    int i;
    for (i=0; i<MAX_CLIENTS; i++) {
        serverSkepp[i].bild=IMG_Load("skepp.png");
        if (serverSkepp[i].bild==NULL) {
            printf("Lyckades inte ladda Skepp: %s",IMG_GetError());
        }
        
        SDL_SetColorKey(serverSkepp[i].bild,SDL_TRUE,SDL_MapRGB(serverSkepp[i].bild->format,255,255,255));
    }
    printf("Ships 0-7 loaded\n");
    return true;
}

int udpListener(void* data) {
    
    // H{r ska den l{sa in fr{n UDP, l{gga den i 'inkommandeData' (en Uint8), identifiera VEM som skickade,
    // och lagra detta i klientID (en int). D} kommer denna klients skepp att s{ttas till r{tt v{rden.
    // Som synes, s} är klientID nu alltid 0, och 'inkommandeData' {r alltid 'minAktivitet'.
    // Tittar ni i gameclient.c, ser ni att 'minAktivitet' {r den Uint8 som s{tts till olika v{rden
    // beroende p} vilken knapp man trycker ner.
    UDPpacket *incommingpacket;
    incommingpacket = SDLNet_AllocPacket(1);
    short clientID, incommingNumber;
    puts("udplistener startad.");
    
    int check;
    while (1) {
        check = SDLNet_UDP_Recv(udpRecvSock,incommingpacket);
        if (!check) continue;
        //		printf("Fick in ett UDP-paket fr}n port %d: %d\n",inkommandepaket->address.port,siffranSomKomIn);
        if ((clientID = clientIDfromPort(incommingpacket->address.port)) < 0 ) {
            printf("Fel! paket fr}n ok{nt klient, port %d\n",incommingpacket->address.port);
            exit(1);
        }
        incommingNumber = incommingpacket->data[0];
        
        if ((incommingNumber & 3) == 1) serverSkepp[clientID].vinkelHast=5;
        else if ((incommingNumber & 3) == 2) serverSkepp[clientID].vinkelHast=-5;
        else serverSkepp[clientID].vinkelHast = 0;
        if ((incommingNumber & 4) == 4) serverSkepp[clientID].accar=true;
        else serverSkepp[clientID].accar=false;
        if ((incommingNumber & 8) == 8) serverSkepp[clientID].skjuter=true;
        else serverSkepp[clientID].skjuter=false;
        
        SDL_Delay(5);
    }
    return 255;
}


/** hittar client-ID baserat p} port.
 @var port: en Uint16; porten som man vill identifiera.
 @return: klient-ID:t om funnen, annars -1.
 */
int clientIDfromPort(Uint16 port) {
    int i;
    for (i=0; i<MAX_CLIENTS; i++) {
        if (players[i].active && players[i].udpSendPort==port) return i;
    }
    return -1;
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

/** skapar en en Uint8-array (dataToClient) av alla skeppspositioner och skottpositioner.
 * Denna skr{ddarsys för varje klient, eftersom den bara skickar de skottpositioner
 * som klienten ska se p} sin sk{rm. Rent konkret inneh}ller varje paket:
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
        tempint = tempint | skeppen[player].blammad <<30 | players[player].active << 31;
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
        if (!players[player].active) continue;
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


/** R{knar om grader till radianer.
 @var a: vinkeln i grader.
 @return vinkeln i radianer.
 */
double getRadians(int a) {
    return (a+90)*3.14159265/180;
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