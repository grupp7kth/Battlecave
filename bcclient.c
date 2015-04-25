#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2_net/SDL_net.h>
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define SERVER_IP "193.10.39.101"
#define SERVER_PORT 3445
#define LOOPBACK "127.0.0.1"
#define LOCAL_IP "130.229.173.252"
#define MAX_LENGTH 100
#define MAX_NAME_LENGTH 30
#define QUIT "false"
#define GO "true"

struct client{
    TCPsocket socket;
    char name[MAX_NAME_LENGTH];
    int score;
}
typedef client;

SDL_Thread *threadChattRecv;
IPaddress ip;
client me;
void clearString(char comand[]);
int chattRecvThread(client* m);
bool flag;

int main( int argc, char* args[] )
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDLNet_Init();
    
    char TCPtextOUT[MAX_LENGTH];
    char inputName[MAX_NAME_LENGTH];
    char inputIP[MAX_NAME_LENGTH];
    clearString(TCPtextOUT);clearString(inputName);clearString(inputIP);
    printf("Enter nickname: ");
    fgets(inputName, MAX_NAME_LENGTH, stdin);
    printf("Enter server ip: ");
    fgets(inputIP,MAX_NAME_LENGTH, stdin);
    
    SDLNet_ResolveHost(&ip, inputIP, SERVER_PORT);
    me.socket = SDLNet_TCP_Open(&ip);
    
    if(me.socket)
    {
        printf("connected!\n");
        flag = GO;
        SDLNet_TCP_Send(me.socket, inputName, MAX_NAME_LENGTH);
        threadChattRecv = SDL_CreateThread(chattRecvThread, "Chatt reciev thread", &me);
        
        while(1)
        {
            clearString(TCPtextOUT);
            printf("> ");
            fgets(TCPtextOUT,MAX_LENGTH,stdin);
            if(strstr(TCPtextOUT,"&&&")!=0)
            {
                SDLNet_TCP_Send(me.socket,TCPtextOUT,MAX_LENGTH);
                flag = QUIT;
                break;
            }
            SDLNet_TCP_Send(me.socket,TCPtextOUT,MAX_LENGTH);
        }
    }
    SDLNet_TCP_Close(me.socket);
    SDLNet_Quit();
    SDL_Quit();
    return 0;
}
int chattRecvThread(client* m)
{
    printf("ChattRecvThread \n");
    client* me; me = (client*) m;
    
    char TCPtextIN[MAX_LENGTH]; clearString(TCPtextIN);
    
    while(flag){
        while (!strlen(TCPtextIN) && flag) SDLNet_TCP_Recv(me->socket,TCPtextIN,MAX_LENGTH);
        printf("< %s",TCPtextIN);
        clearString(TCPtextIN);
    }
    return 0;
}
void clearString(char comand[]){
    for(int i=0;i<MAX_LENGTH;i++){
        comand[i]='\0';
    }
}
