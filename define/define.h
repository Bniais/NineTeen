#include <SDL.h>
#define TRUE  1
#define FALSE  0
#define PI 3.1415

// launcher params
#define LAUNCHER_WIDTH_SIZE 640
#define LAUNCHER_HEIGHT_SIZE 480

// error code //
#define SDL_CREATE_WINDOW_ECHEC -100
#define SDL_LOAD_BMP_ECHEC -101
#define SDL_CREATE_TEXTURE_FROM_SURFACE_ECHEC -102
#define SDL_CREATE_TEXTURE_ECHEC -103

#define FILE_NOT_FOUND -201
#define FILE_CORRUPTED -202

#define SHAPE_SIZE 500

//Window
#define WINDOW_FLAG 0
#define PLAYGROUND_SIZE_W 1728
#define PLAYGROUND_SIZE_H 972
#define BASE_WINDOW_W 1920
#define BASE_WINDOW_H 1080

#define HUD_W 96
#define HUD_H 54
const SDL_Point ESPACE_DISPLAY_WINDOW = {67, 57};


//font
#define OPEN_FONT_SIZE 100

//Textures
char *DIR_HUD = "../../assets/texture/hud.png";

//pause
#define NB_BOUTON_PAUSE 3
#define BOUTON_PAUSE_SIZE_W 400
#define BOUTON_PAUSE_SIZE_H 150
const SDL_Rect BOUTON_PAUSE_BASE = {(PLAYGROUND_SIZE_W + 2 * HUD_W) / 2 - BOUTON_PAUSE_SIZE_W / 2, ((PLAYGROUND_SIZE_H + 2 * HUD_H) / NB_BOUTON_PAUSE) - (BOUTON_PAUSE_SIZE_H / 2), BOUTON_PAUSE_SIZE_W, BOUTON_PAUSE_SIZE_H};
#define ESPACEMENT_BOUTON_PAUSE 100

//Colors
const SDL_Color Black = {0, 0, 0, 255};
const SDL_Color White = {255, 255, 255, 255};

//fonctions
SDL_Point maximizeWindow(SDL_Rect , float* );

void afficherPauseMenu(SDL_Renderer *, SDL_Point , SDL_Texture *, SDL_Texture *);
