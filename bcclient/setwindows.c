#include "includes.h"

void setWindows(SDL_Rect windowPlacement[]){
    windowPlacement[0].x = SCREENWIDTH/2 - 150;        // 0 = IP / Port Window  (For joining custom server)
    windowPlacement[0].y = SCREENHEIGHT/2 - 100;
    windowPlacement[0].h = 275;
    windowPlacement[0].w = 300;
    windowPlacement[1].x = 90;                         // 1 = Lobby Window Background
    windowPlacement[1].y = 50;
    windowPlacement[1].h = 620;
    windowPlacement[1].w = 1100;
    windowPlacement[2].x = SCREENWIDTH/2 - 150;         // 2 = Name Window (For joining default server)
    windowPlacement[2].y = SCREENHEIGHT/2 - 100;
    windowPlacement[2].h = 123;
    windowPlacement[2].w = 300;
    windowPlacement[3].x = SCREENWIDTH/2 - 150;         // 3 = Options Window
    windowPlacement[3].y = SCREENHEIGHT/2 - 220;
    windowPlacement[3].h = 275;
    windowPlacement[3].w = 300;
    windowPlacement[4].x = SCREENWIDTH/2 - 200;         // 4 = Score Screen
    windowPlacement[4].y = SCREENHEIGHT/2 - 250;
    windowPlacement[4].h = 550;
    windowPlacement[4].w = 400;
    return;
}
