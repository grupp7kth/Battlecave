#include "bcheaders.h"

int clientThreadFunction(struct client* p)    /* Function definition */
{
    struct client* player=(struct client*) p;
    printf("GameThread created by ClientID: %d\n",player->ID);
  

    while(1)
    {
    
    
    }
    return 0;
}
