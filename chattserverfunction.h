#ifndef chattserverfunction_H_   /* Include guard */
#define chattserverfunction_H_

SDL_ThreadFunction *chattfunction(int currentclientid);  /* An example function declaration */
bool checkConnection(int cRecv);

#endif// chattserverfunction_H_
