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

// tar emot det som kommer fr}n servern och skriver ut det p} sk{rmen.
int taEmot(void* data) {
	puts("Ta-emot startad!");
	client *jag;
	jag = (client*)data;
	char meddelande[MAX_LENGTH];
	meddelande[0]='\0';
	while(1) {
		while (!strlen(meddelande)) SDLNet_TCP_Recv(jag->socket,meddelande,MAX_LENGTH);
		printf("%s",meddelande);
		meddelande[0]='\0';
	}
}

// Ber om ett namn, |ppnar en socket mot servern, skickar namnet och sen loopar runt, skickar det som skrivs.
int main(int argc, char* argv[]) {
	SDLNet_Init();
	SDL_Thread* taEmotTrad;
	client jag;
	
	char input[MAX_LENGTH];
	int i;
	fgets(input,MAX_LENGTH,stdin); // H{r ska namnet skrivas in.
	
	IPaddress minIP;
	SDLNet_ResolveHost(&minIP,"127.0.0.1",4444); // F|r n{rvarande pratar den bara med LocalHost.
	printf("Jag vill ringa %d och port %d\n",minIP.host,minIP.port);
	jag.socket = SDLNet_TCP_Open(&minIP); // \ppnar socketen.
	printf("Nu har jag ringt %d port %d\n",minIP.host,minIP.port);
	if(jag.socket) { // Om socketen inte {r noll, s} har det lyckats.
		puts("Det funka!");
		SDLNet_TCP_Send(jag.socket,input,MAX_LENGTH); // Skicka namnet man skrev in i b|rjan.
		taEmotTrad=SDL_CreateThread(taEmot,"Tar emot",&jag); // Skapa lyssnartr}den och skicka in ens klient-strukt.
		while(1) {
			fgets(input,MAX_LENGTH,stdin);
			SDLNet_TCP_Send(jag.socket,input,MAX_LENGTH); // Skicka det som skrivs.
		}
	}
	else puts("]t helvete!"); // Hit kommer vi om det inte gick att koppla upp mot servern.
}
