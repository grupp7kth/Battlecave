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
    printf("GameThread created by ClientID: %d\n",player->ID);
  

    while(1)
    {
    
    
    }
    return 0;
}
