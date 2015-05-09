#include "udphandler.h"

int UDPhandler(void){
    // Set the static information for the packet used for outgoing UDP (Used to transmit the player's keypresses)
    outPacket = SDLNet_AllocPacket(16);
    outPacket->address.port = client.ServerRecvUDPPort;
    outPacket->address.host = SDLNet_TCP_GetPeerAddress(client.TCPSock)->host;

    for(;;){
        if(mode == IN_GAME){
            outPacket->data[0] = pressedButtons;
            outPacket->len = sizeof(pressedButtons);
            SDLNet_UDP_Send(client.UDPSendSock, -1, outPacket);
            //printf("Sent %d\n", pressedButtons);
        }
        SDL_Delay(20);
    }
    return 0;
}
