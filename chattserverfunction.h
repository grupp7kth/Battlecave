#include "bcheaders.h"

#ifndef chattserverfunction_H_   /* Include guard */
#define chattserverfunction_H_

int chattserverfunction();  /* An example function declaration */
void clearString(char message[]);
void sendMessageAll(char TCPtextOUT[]);
void sendMessageExc(char TCPtexOUT[], int ID);
void stringCopy(char* source, char* dest);
void activePlayers();
void disconnect(int ID);

#endif// chattserverfunction_H_
