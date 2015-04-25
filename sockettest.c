#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2_Image/SDL_Image.h>
#include <SDL2_Net/SDL_Net.h>

#define MAX_LENGTH 50
// En strukt som inneh}ller information om varje klient.
typedef struct client {
	TCPsocket socket;
	char namn[MAX_LENGTH];
	int poang;
	bool aktiv;
}client;

// Dessa variabler m}ste vara globala eftersom SDL_ThreadFunction m}ste ha tillg}ng till dem.
int antalKlienter;
client klienter[10];

// S{tter alla bytes i den inkommande str{ngen till \0
void clearString(char m[MAX_LENGTH]) {
	int i;
	for (i=0; i<MAX_LENGTH; i++) m[i]='\0';
}
// Kopierar en str{ng till en annan str{ng. Fanns inte den h{r i standardbiblioteket?
void stringCopy(char* source, char* dest) {
	while ((*dest = *source) != '\0') {
		source++;
		dest++;
	}
}
// Kontrollerar klient-arrayen och returnerar det f|rsta lediga numret i arrayen.
// Returnerar -1 om ingen var ledig.
int checkClients(client kli[10]) {
	int found;
	for (found=0; found<10; found++) {
		if (!kli[found].aktiv) return found;
	}
	return -1;
}

// Tr}dfunktion som l{ser av klientens socket f|r ett meddelande, l{gger till klientens namn till meddelandet
// och sen skickar ut till alla andra klienter.
int klientTrad(void* data) {
	client* jag; // Skapa en klient och kasta om den void-pekare som kom in, till en client-pekare.
	jag = (client*)data; 
	char meddelande[MAX_LENGTH]; // Det meddelande som kommer in.
	char full[MAX_LENGTH]; // Det meddelande som ska ut (namn+": "+meddelande)
	clearString(meddelande); // Rensa meddelande-arayen f|r s{kerhets skull.
	printf("Trad startad, jag har socket %p\n",jag->socket);
	while(!strlen(meddelande)) SDLNet_TCP_Recv(jag->socket,meddelande,MAX_LENGTH); // L{s fr}n socketen s} l{nge inget kommer in.
	int i;
	for (i=0; i<MAX_LENGTH; i++) if (meddelande[i]=='\n') meddelande[i]= '\0'; // Ers{tt returslag fr}n inkommande text med en EOS.
	printf("Fick in namn:%s!\n",meddelande);
	stringCopy(meddelande,jag->namn);
	clearString(meddelande);
	int check=1;
	while (1) {
		while (!strlen(meddelande) && check) check=SDLNet_TCP_Recv(jag->socket,meddelande,MAX_LENGTH); // L{s fr}n socketen s} l{nge inget kommrere in.
		if (!check) { // Om TCP_Recv returnerade 0, betyder det att klienten kopplat ner.
			puts("Klienten dog!");
			break;
		}
		printf("%s: %s",jag->namn, meddelande); // Servern skriver ut det den f}tt in.
		full[0] = '\0'; // S{tt full till l{ngd 0.
		strcat(full,jag->namn); // Full ska bli namn+": "+meddelande.
		strcat(full,": ");
		strcat(full,meddelande);
		for (i=0; i<10; i++) {
			// Skicka full till alla klienter som a) {r aktiva och b) inte {r jag sj{lv.
			if (klienter[i].aktiv && jag->socket!=klienter[i].socket) SDLNet_TCP_Send(klienter[i].socket,full,MAX_LENGTH);
		}
		clearString(meddelande);
	}
	// Har vi kommit hit s} har klienten loggat ur. St{ng socketen, och inaktivera klienten.
	SDLNet_TCP_Close(jag->socket);
	jag->aktiv=false;
	return 255; // Dummy-siffra.
}

// lyssnar efter nyinkomna anrop, och startar en ny tr}d f|r varje anrop som kommer in.
int main(int argc, char* argv[]) {
	SDLNet_Init();
	TCPsocket inkommande;
	SDL_Thread* klienttradar[10]; // En array av 10 tr}dar.
	int freeID;

	char meddelande[MAX_LENGTH];
	printf("L{ngd %lu\n",strlen(meddelande));
	IPaddress serverip;
	serverip.host = 0;
	serverip.port = 69;
	TCPsocket serversocket;

	SDLNet_ResolveHost(&serverip,NULL,4444); // Vet inte riktigt vad den h{r har f|r funktion.
	printf("Serverns ip (nu) {r %d och port %d\n",serverip.host,serverip.port);
	serversocket = SDLNet_TCP_Open(&serverip); // \ppna socketen.
	printf("Serverns ip (efter open) {r %d och port %d\nWaiting...\n",serverip.host,serverip.port);
	while(1) {
		// Lagra det som TCP_Accept returnerar i inkommande, och h}ll p} {nda tills det blir n}t.
		// Observera att detta {r en while-loop som har tom kropp.
		while ((inkommande = SDLNet_TCP_Accept(serversocket)) == 0x0); 
		if ((freeID=checkClients(klienter))<0) puts("Too many users!"); // Om det inte fanns n}gon ledig klient, skrik till.
		else {
			printf("KlientID %d! Socketen {r %p, startar ny trad!\n",freeID,inkommande);
			klienter[freeID].socket = inkommande;
			klienter[freeID].aktiv = true;
			klienttradar[freeID] = SDL_CreateThread(klientTrad,"Generic",&klienter[freeID]); // Skapa en tr}d och skicka in den klient-strukt som tillh|r tr}den.
			SDL_DetachThread(klienttradar[freeID]);  // Det h{r g|r att tr}den sj{lvd[r n{r den {r slut. Annars blir den en zombietr}d som v{ntar p} en WaitThread.
			inkommande = NULL; // Viktigt att nollst{lla 'inkommande' s} att inte det blir flera tr}dskapningar p} en g}ng.
		}
	}
}
