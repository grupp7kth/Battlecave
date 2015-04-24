#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2_Net/SDL_Net.h>

#define CJ "130.229.154.76"
#define MAX_LENGTH 50

typedef struct client {
	TCPsocket socket;
	char* namn;
	int poang;
}client;

void clearString(char m[MAX_LENGTH]) {
	int i;
	for (i=0; i<MAX_LENGTH; i++) m[i]='\0';
}

int taEmot(void* data) {
	puts("Ta-emot startad!");
	client *jag;
	jag = (client*)data;
	char meddelande[MAX_LENGTH];
	clearString(meddelande);
	while(1) {
		while (!strlen(meddelande)) SDLNet_TCP_Recv(jag->socket,meddelande,30);
		printf("%s",meddelande);
		clearString(meddelande);
	}
}

int main(int argc, char* argv[]) {
	SDLNet_Init();
	SDL_Thread* taEmotTrad;
	client jag;
	
	char input[MAX_LENGTH];
	int i;
	fgets(input,MAX_LENGTH,stdin);
	
	IPaddress minIP;
	SDLNet_ResolveHost(&minIP,"127.0.0.1",4444);
	printf("Jag vill ringa %d och port %d\n",minIP.host,minIP.port);
	jag.socket = SDLNet_TCP_Open(&minIP);
	printf("Nu har jag ringt %d port %d\n",minIP.host,minIP.port);
	if(jag.socket) {
		puts("Det funka!");
		SDLNet_TCP_Send(jag.socket,input,MAX_LENGTH);
		taEmotTrad=SDL_CreateThread(taEmot,"Tar emot",&jag);
		while(1) {
			fgets(input,MAX_LENGTH,stdin);
			SDLNet_TCP_Send(jag.socket,input,MAX_LENGTH);
		}
	}
	else puts("]t helvete!");
}

