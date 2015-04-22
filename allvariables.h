#ifndef ALVARIABLES_H_   /* Include guard */
#define ALVARIABLES_H_

extern SDL_Thread *client_thread[MAX_CLIENTS];
extern SDLNet_SocketSet socketSet;
extern TCPsocket clientTCPsocket[MAX_CLIENTS];
extern UDPsocket clientUDPsocket[MAX_CLIENTS];
extern int activeClients;

#endif // FOO_H_
