#include "includes.h"

void unpackPacket(void);

int UDPhandler(void){
    // Set the static information for the packet used for outgoing UDP (Used to transmit the player's keypresses)
    outPacket = SDLNet_AllocPacket(16);
    outPacket->address.port = client.ServerRecvUDPPort;
    outPacket->address.host = SDLNet_TCP_GetPeerAddress(client.TCPSock)->host;
    inPacket = SDLNet_AllocPacket(940);

    for(;;){
        if(mode == IN_GAME){
            // Send key-press data to server
            if(!ship[client.id].isDead && gameFreezeTime <= 0 && pressedButtons != lastPressedButtons){ // We only want to send if the pressed buttons have changed
                outPacket->data[0] = pressedButtons;
                outPacket->len = sizeof(pressedButtons);
                SDLNet_UDP_Send(client.UDPSendSock, -1, outPacket);
                lastPressedButtons = pressedButtons;
            }

            // Recieve placement data from server
            if(SDLNet_UDP_Recv(client.UDPRecvSock, inPacket) > 0){
                unpackPacket();
            }
        }
        SDL_Delay(20);

        if(!isConnected)
            break;

    }
    SDLNet_FreePacket(outPacket);
    SDLNet_FreePacket(inPacket);

    return 0;
}

void unpackPacket(void){
    Uint32 read, i, j, tempint, bulletID;

    // Read packet number
	for (i = 0, read = 0; i < 4; i++){
		tempint = inPacket->data[i];
		read = read | tempint << i*8;
	}

    // Read ship data
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

    // Read powerup data
    for(i = 0; i < MAX_ALLOWED_POWERUP_SPAWNPOINTS; i++){
        for(j = 0, read = 0; j < 4; j++){
            tempint = inPacket->data[36+(4*i)+j];
            read = read | tempint << j*8;
        }
        powerupSpawnPoint[i].x = read & 0b111111111111;
        powerupSpawnPoint[i].y = (read >> 12) & 0b111111111111;
        powerupSpawnPoint[i].isActive = (read >> 24) & 1;
        powerupSpawnPoint[i].type = (read >> 25) & 0b111;
    }

    client.health = (inPacket->data[96] & 0b11111) * 5;   // The first 5 bits of this byte is the player's health divided by 5 (to use fewer bits)
    viewportID = (inPacket->data[96] >> 5) & 0b111;       // The last 3 bits is the viewport-id to use for the player

    for(bulletID = 0; (inPacket->data[97+(bulletID)*3] != 0xFF) && (bulletID < 250); bulletID++){
		for (i = 0, read = 0; i < 3; i++){
			tempint = inPacket->data[97+(bulletID)*3+i];
			read = read | tempint << i*8;
		}
		bullet[bulletID].x = read & 0b11111111111;
		bullet[bulletID].y = (read >> 11) & 0b1111111111;
		bullet[bulletID].source = (read >> 21) & 0b1111;
	}
    currentBulletAmmount = bulletID;

    return;
}
