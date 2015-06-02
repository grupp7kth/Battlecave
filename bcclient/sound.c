#include "includes.h"

void soundHandler(void){
    if(musicEnabled && Mix_PlayingMusic() == 0){ // If the last playing song has ended

        char songNames[5][25] = {{"Space Industry"},
                                 {"Saturn's Graveyard"},
                                 {"Falling Star"},
                                 {"Tention"},
                                 {"And Then The Robots Sang"}};

        currentSong++;
        if(currentSong >= 5)
            currentSong = 0;

        if(Mix_FadeInMusic(music[currentSong], 1, 5000) == -1)
            printf("SOUND.C: Couldn't Play Music!\n");
        else
            printf("SOUND.C: Now playing : %s\n", songNames[currentSong]);

    }
    return;
}
