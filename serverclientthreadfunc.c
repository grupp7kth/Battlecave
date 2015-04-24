#include <SDL2/SDL.h>
#include <SDL2_net/SDL_net.h>
#include <stdio.h>
#include <stdbool.h>
#include "serverclientthreadfunc.h"
#include "chattserverfunction.h" /* Include the header (not strictly necessary here) */
#include "allvariables.h"


int clientThreadFunction(int clientID)    /* Function definition */
{
    int ID = clientID;
    printf("ClientThread:GAME, clientID: %d\n",ID);
  

    while(1)
    {
    
    
    }
    return 0;
}
