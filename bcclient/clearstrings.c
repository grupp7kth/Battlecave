#include "clearstrings.h"
#include "globalvariables.h"

void clearTextStrings(int total){
    for(int i = 0; i < total; i++){
        for(int j = 0; j < STRINGLENGTH; j++){
            textString[i][j] = '\0';
        }
        textStringCurrent[i] = 0;
        textStringColor[i] = TEXT_COLOR_WHITE;
    }
    return;
}

void clearTextString(int id){
    for(int i = 0; i < STRINGLENGTH; i++)
        textString[id][i] = '\0';

    textStringCurrent[id] = 0;
    textStringColor[id] = TEXT_COLOR_WHITE;
    return;
}

void clearPlayerNameString(int id){
    for(int i = 0; i < MAX_NAME_LENGTH; i++){
        playerName[id][i] = '\0';
    }
    return;
}
