#include "bcheaders.h"

int clientThreadFunction(struct client* p)    /* Function definition */
{
    /** Utbyter n|dv{ndig information med klienten via TCP-sockan, samt h}ller reda p} om klienten kopplar ner.
     * I ordning tar den in spelarens namn, den UDP-port som spelaren tar emot p}, samt den
     * UDP-port som spelaren skickar ifr}n. D{refter skickar den tillbaka spelarens ID och
     * den UDP-port som servern ska ha sin information till.
     * Sen startar en loop som v{ntar p} ett 'K' fr}n klienten, som {r signalen f|r att
     * klienten {r klar f|r att b|rja spela.
     * D{refter v{ntar den vidare, men kommer inte att g|ra n}got. Om TCP-porten st{ngs, kommer
     * funktionen att avaktivera klienten och avsluta sig sj{lv.
     * Denna funktion k[rs i en separat tr}d, en per klient.
     @var data: en pekare till klienten ifr}ga.
     */
    
        struct client* me;
        me = (struct client*)p;
        char message[MAX_LENGTH];
        clearString(message);
        printf("Trad startad, jag har socket %p\n",me->socket);
        // V{nta p} inkommande namn
        SDLNet_TCP_Recv(me->socket,message,MAX_LENGTH);
    
        
        int i;
        for (i=0; i<MAX_LENGTH; i++) if (message[i]=='\n') message[i]= '\0';
        printf("Fick in namn:%s!\n",message);
        stringCopy(message,me->name);
        clearString(message);
        
    
        // V{nta p} inkommande UDP-portnummer.
        SDLNet_TCP_Recv(me->socket,message,MAX_LENGTH);
        me->udpRecvPort = atoi(message);
        printf("%s tar emot UDP p} port %d\n",me->name,me->udpRecvPort);
        clearString(message);
        
        SDLNet_TCP_Recv(me->socket,message,MAX_LENGTH);
        me->udpSendPort = atoi(message);
        printf("%s kommer att skicka ifr}n port %d\n",me->name,me->udpSendPort);
        clearString(message);
        
        // Skicka spelarens ID.
        
        char sms[6];
        sms[0]=me->ID+'0';
        sms[1]='\0';
        printf("Skickar spelarens ID: %d\n",me->ID);
        SDLNet_TCP_Send(me->socket,sms,6);
        
        // Skicka den egna UDP-porten.
        //itoa((SDLNet_UDP_GetPeerAddress(udpRecvSock,-1))->port,sms,10);
        sprintf(sms, "%d", (SDLNet_UDP_GetPeerAddress(udpRecvSock,-1))->port);

        printf("Skickar att serverns UDP-ta-emot {r %s\n",sms);
        SDLNet_TCP_Send(me->socket,sms,6);
        
        me->active= true;
        clearString(message);
        int check=1;
        while (1) {
            check=SDLNet_TCP_Recv(me->socket,message,MAX_LENGTH);
            puts("N}got kom fr}n klienten.");
            if (!check) {
                printf("%s dog!\n",me->name);
                break;
            }
            printf("Fick in '%s' fr}n %s\n",message,me->name);
            if (message[0]!='K') continue;
            puts("Verkar som om klienten {r klar.");
            me->ready = true;
            SDL_Delay(1000);
        }
        SDLNet_TCP_Close(me->socket);
        me->active=false;
        bool test = false;
        for (int i=0; i<MAX_CLIENTS; i++) {
            if (players[i].active) test = true;
        }
        if (!test) speletPagar=false;
        return 255;
}
