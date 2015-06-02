#ifndef MODEDEFINITIONS
#define MODEDEFINITIONS

#define CLIENT_VERSION "v 1.00"

#define SCREENWIDTH 1280
#define SCREENHEIGHT 720
#define GAME_AREA_WIDTH 1030
#define GAME_AREA_HEIGHT 720
#define SHIP_HEIGHT 31
#define SHIP_WIDTH 23

#define MAXBUTTONS 14

// The different modes that the client can be in. Determines what to render and how to interpret mouse clicks / keystrokes etc...
#define STARTUP 0
#define FIND_SERVERS 1
#define OPTIONS 2
#define LOBBY 3
#define JOIN_DEFAULT 4
#define JOIN_CUSTOM 5
#define IN_GAME 6
#define SCORE_SCREEN 7

// Which mode the keyboard is currently in (0 could be used to suspend all game-play keys, and 1 could be used to parse ONLY buttons used for playing)
#define NONE -1
#define ENTERING_TEXT 0
#define PLAYING 1

// Selections while in mode 3
#define LOBBY_ENTER_CHAT_MESSAGE 0

// Selections while in mode 5
#define ENTERING_IP 0
#define ENTERING_PORT 1
#define ENTERING_NAME 2

#define PLAYER_MESSAGE_WRITE 5

#define STRINGLENGTH 100

#define MAX_NAME_LENGTH 15
#define MAX_IP_LENGTH 15
#define MAX_PORT_LENGTH 5
#define MAX_PLAYERS 8

#define QUIT "false"
#define GO  "true"

// Codes that are sent at the beginning of tcp messages which indicate what the content is
#define PREAMBLE_CHAT '$'
#define PREAMBLE_PLAYERS '@'
#define PREAMBLE_READY '#'
#define PREAMBLE_TOGGLEBOT '?'
#define PREAMBLE_DISCONNECT '-'
#define PREAMBLE_GAMESTART '!'
#define PREAMBLE_GAMEFREEZE '¤'
#define PREAMBLE_GAMEEND '='
#define PREAMBLE_OPTIONS '*'
#define PREAMBLE_KILLED '}'
#define PREAMBLE_POWERUP '+'

#define POWERUP_MULTI3X 0
#define POWERUP_MULTI2X 1
#define POWERUP_HEALTHPACK 2
#define POWERUP_TIMEWARP 3
#define POWERUP_DOUBLEDAMAGE 4
#define POWERUP_TELEPORT 5

#define TIMEWARP_DURATION 10000
#define MAX_ALLOWED_POWERUP_SPAWNPOINTS 15
#define SHIP_POWERUP_DURATION 10000         // Duration for non instant powerups gained by ships, such as double/triple shot or double damage
#define POWERUP_NOTIFICATION_DURATION 1500
#define DEATH_NOTIFICATION_DURATION 3000
#define TELEPORT_DURATION 3000

#define TOGGLE_BULLETINTERVAL 1
#define TOGGLE_MAXSPEED 2
#define TOGGLE_GAMELENGTH 3

#define TEXT_COLOR_WHITE 0
#define TEXT_COLOR_BRIGHTYELLOW 1
#define TEXT_COLOR_BLACK 2
#define TEXT_COLOR_BRIGHTGREEN 3
#define TEXT_COLOR_BRIGHTRED 4
#define TEXT_COLOR_TEAL 5
#define TEXT_COLOR_RED 6
#define TEXT_COLOR_BLUE 7
#define TEXT_COLOR_GREEN 8
#define TEXT_COLOR_YELLOW 9
#define TEXT_COLOR_PURPLE 10
#define TEXT_COLOR_ORANGE 11
#define TEXT_COLOR_PINK 12
#define TEXT_COLOR_BROWN 13

#define MAX_BULLETS 250

#endif // MODEDEFINITIONS
