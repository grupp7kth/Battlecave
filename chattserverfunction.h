#include <SDL2/SDL.h>
#include <SDL2_net/SDL_net.h>
#include <stdio.h>
#include <stdbool.h>
#ifndef chattserverfunction_H_   /* Include guard */
#define chattserverfunction_H_

int chattserverfunction();  /* An example function declaration */
void clearString(char message[]);
void sendMessage(int ID,char TCPtextOUT[]);
void stringCopy(char* source, char* dest);


#endif// chattserverfunction_H_
