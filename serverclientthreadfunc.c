#include <SDL2/SDL.h>
#include <SDL2_net/SDL_net.h>
#include <stdio.h>
#include <stdbool.h>
#include "serverclientthreadfunc.h"
#include "chattserverfunction.h" /* Include the header (not strictly necessary here) */
#include "allvariables.h"


int clientThreadFunction(struct client* p)    /* Function definition */
{
    struct client* player=(struct client*) p;
    int ID = player->ID;
    printf("ClientID: %d    ClientThread:GAME\n",ID);
  

    while(1)
    {
    
    
    }
    return 0;
}
