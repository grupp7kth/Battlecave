#include "includes.h"

void unpackPacket(void);

int UDPhandler(void){
    // Set the static information for the packet used for outgoing UDP (Used to transmit the player's keypresses)
    outPacket = SDLNet_AllocPacket(16);
    outPacket->address.port = client.ServerRecvUDPPort;
    outPacket->address.host = SDLNet_TCP_GetPeerAddress(client.TCPSock)->host;
    inPacket = SDLNet_AllocPacket(1024);

    for(;;){
        if(mode == IN_GAME){
            // Send key-press data to server
                if(!ship[client.id].isDead && gameFreezeTime < 0){
                outPacket->data[0] = pressedButtons;
                outPacket->len = sizeof(pressedButtons);
                SDLNet_UDP_Send(client.UDPSendSock, -1, outPacket);
            }

            // Recieve placement data from server
            if(SDLNet_UDP_Recv(client.UDPRecvSock, inPacket) > 0){
                unpackPacket();
            }

        }
        SDL_Delay(20);
    }
    return 0;
}

void unpackPacket(void){
    Uint32 read, i, tempint, bulletID;

	for (i = 0, read = 0; i < 4; i++){
		tempint = inPacket->data[i];
		read = read | tempint << i*8;
	}

    for(int player = 0; player < MAX_PLAYERS; player++){
		for(i = 0, read = 0; i < 4; i++){
			tempint = inPacket->data[4+(4*player)+i];
			read = read | tempint << i*8;
		}
		ship[player].x = read & 0b111111111111;
		ship[player].y = (read >> 12) & 0b111111111111;
		ship[player].angle = (read >> 24) & 0b111111;
		ship[player].angle *= 6;
		ship[player].isDead = (read >> 30) & 1;
		ship[player].active = (read >> 31) & 1;
	}

    client.health = inPacket->data[36];

    for(bulletID = 0; (inPacket->data[37+(bulletID)*3] != 0xFF) && (bulletID < 300); bulletID++){
		for (i = 0, read = 0; i < 3; i++){
			tempint = inPacket->data[37+(bulletID)*3+i];
			read = read | tempint << i*8;
		}
		bullet[bulletID].x = read & 0b11111111111;
		bullet[bulletID].y = (read >> 11) & 0b1111111111;
		bullet[bulletID].source = (read >> 21) & 0b1111;
	}
    currentBulletAmmount = bulletID;

    return;
}
