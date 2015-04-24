#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2_Image/SDL_Image.h>
#include <SDL2_Net/SDL_Net.h>

#define MAX_LENGTH 50

typedef struct client {
	TCPsocket socket;
	char namn[MAX_LENGTH];
	int poang;
	bool aktiv;
}client;

int antalKlienter;
client klienter[10];

void clearString(char m[MAX_LENGTH]) {
	int i;
	for (i=0; i<MAX_LENGTH; i++) m[i]='\0';
}
void stringCopy(char* source, char* dest) {
	while ((*dest = *source) != '\0') {
		source++;
		dest++;
	}
}
int checkClients(client kli[10]) {
	int found;
	for (found=0; found<10; found++) {
		if (!kli[found].aktiv) return found;
	}
	return -1;
}

int klientTrad(void* data) {
	client* jag;
	jag = (client*)data;
	char meddelande[MAX_LENGTH];
	char full[MAX_LENGTH];
	clearString(meddelande);
	printf("Trad startad, jag har socket %p\n",jag->socket);
	while(!strlen(meddelande)) SDLNet_TCP_Recv(jag->socket,meddelande,MAX_LENGTH);
	int i;
	for (i=0; i<MAX_LENGTH; i++) if (meddelande[i]=='\n') meddelande[i]= '\0';
	printf("Fick in namn:%s!\n",meddelande);
	stringCopy(meddelande,jag->namn);
	clearString(meddelande);
	int check=1;
	while (1) {
		while (!strlen(meddelande) && check) check=SDLNet_TCP_Recv(jag->socket,meddelande,MAX_LENGTH);
		if (!check) {
			puts("Klienten dog!");
			break;
		}
		printf("%s: %s",jag->namn, meddelande);
		full[0] = '\0';
		strcat(full,jag->namn);
		strcat(full,": ");
		strcat(full,meddelande);
		for (i=0; i<10; i++) {
			if (klienter[i].aktiv && jag->socket!=klienter[i].socket) SDLNet_TCP_Send(klienter[i].socket,full,MAX_LENGTH);
		}
		clearString(meddelande);
	}
	SDLNet_TCP_Close(jag->socket);
	jag->aktiv=false;
	return 255;
}

int main(int argc, char* argv[]) {
	SDLNet_Init();
	TCPsocket inkommande;
	SDL_Thread* klienttradar[10];
	int freeID;

	char meddelande[MAX_LENGTH];
	printf("L{ngd %lu\n",strlen(meddelande));
	IPaddress serverip;
	serverip.host = 0;
	serverip.port = 69;
	TCPsocket serversocket;

	const char* namn = SDLNet_ResolveIP(&serverip);
	printf("Mitt namn = %s\n",namn);
	SDLNet_ResolveHost(&serverip,namn,4444);
	printf("Serverns ip (nu) {r %d och port %d\n",serverip.host,serverip.port);
	serversocket = SDLNet_TCP_Open(&serverip);
	printf("Serverns ip (efter open) {r %d och port %d\nWaiting...\n",serverip.host,serverip.port);
	while(1) {
		while ((inkommande = SDLNet_TCP_Accept(serversocket)) == 0x0);
		if ((freeID=checkClients(klienter))<0) puts("Too many users!");
		else {
			printf("KlientID %d! Socketen {r %p, startar ny trad!\n",freeID,inkommande);
			klienter[freeID].socket = inkommande;
			klienter[freeID].aktiv = true;
			klienttradar[freeID] = SDL_CreateThread(klientTrad,"Generic",&klienter[freeID]);
			SDL_DetachThread(klienttradar[freeID]);
			inkommande = NULL;
		}
	}
}
