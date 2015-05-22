#include "includes.h"

void soundHandler(void){
    if(musicEnabled && Mix_PlayingMusic() == 0){
        // The following 3 lines is to counter a bug which sometimes skips songs
//        SDL_Delay(250);
//        if(Mix_PlayingMusic() > 0)
//            return;

        char songNames[5][25] = {{"Space Industry"},
                                 {"Saturn's Graveyard"},
                                 {"--TEMPORARY SONG--"},
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
