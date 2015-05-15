#include "includes.h"

void soundHandler(void){
    if(Mix_PlayingMusic() == 0){
        if(currentSong >= 5)
            currentSong = 0;

        if(Mix_PlayMusic(music[currentSong], 1) == -1){
            printf("Couldn't Play Music!\n");
        }
        currentSong++;
    }
    return;
}
