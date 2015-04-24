#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2_net/SDL_net.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SERVER_IP "193.10.39.101"
#define SERVER_PORT 3445
#define LOOPBACK "127.0.0.1"
#define LOCAL_IP "130.229.173.252"
#define MAX_LENGTH 100

SDL_Thread *tRecv;
IPaddress ip;
TCPsocket client;
void cleancomand(char comand[]);
SDL_ThreadFunction *recvThread(int i);

int main( int argc, char* args[] )
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDLNet_Init();
    char text[MAX_LENGTH];
    //char endOfmessage[MAX_LENGTH]={"..."};
    
    SDLNet_ResolveHost(&ip, LOOPBACK, SERVER_PORT);
    client=SDLNet_TCP_Open(&ip);
    
    if(client)
    {
        printf("connected!\n");
        tRecv = SDL_CreateThread(*recvThread,"recv Thread",NULL);
        while(1)
        {
            cleancomand(text);
            printf("> ");
            fgets(text,MAX_LENGTH,stdin);
            SDLNet_TCP_Send(client,text,MAX_LENGTH);
            
            if(strstr(text,"q")!=0)
            {
                break;
            }
        }
    }
    SDLNet_TCP_Close(client);
    SDLNet_Quit();
    SDL_Quit();
    return 0;
}
SDL_ThreadFunction *recvThread(int a)
{
    char ans[MAX_LENGTH];
    while(1)
    {
        cleancomand(ans);
        while (strstr(ans, "...")==0) {
            
            SDLNet_TCP_Recv(client,ans,MAX_LENGTH);
            printf("< %s",ans);
        
        }
    }
}
void cleancomand(char comand[]){
    for(int i=0;i<MAX_LENGTH;i++){
        comand[i]='\0';
    }
}
