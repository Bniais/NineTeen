#include <SDL2/SDL.h>
/**
*\struct Vector2f
*\brief Une structure regroupant deux float
*/
#define PI 3.1415
typedef struct {float x; float y;} Vector2f;
#define SHAPE_SIZE 500
#define HACKED -123
//Window
#define WINDOW_FLAG 0
#define PLAYGROUND_SIZE_W 1728
#define PLAYGROUND_SIZE_H 972
#define BASE_WINDOW_W 1920
#define BASE_WINDOW_H 1080
#define HUD_W 96
#define HUD_H 54

//font
#define OPEN_FONT_SIZE 100


//textures
typedef enum{TEXTURE_ASTEROID, TEXTURE_TETRIS, TEXTURE_SNAKE, TEXTURE_FLAPPY}TEXTURES;
#define DIR_LOADING "../assets/texture/loading.png"

#define NB_ASTEROID_TEXTURES 21
typedef enum{A_ARROW, A_VAISS, A_GEM, A_THRUST, A_BACKGROUND, A_HUD, A_ASTEROID, A_FISSURE, A_GLACE, A_BONUS, A_BULLET, A_LASER, A_ROUE, A_JAUGE, A_BOMB, A_BOMB_ICON,A_LOADING, A_EXPLO_MISSILE, A_EXPLO_ASTEROID, A_EXPLO_GLACE, A_PICKUP}A_TEXTURES;

#define NB_TETRIS_TEXTURES 9
typedef enum{T_LASER_ANIM, T_BRICKS, T_BONUS, T_HUD_GRILLE, T_CHIFFRE, T_SPEED_JAUGE, T_TURN, T_FLECHE, T_LOADING}T_TEXTURES;

#define NB_SNAKE_TEXTURES 9
typedef enum{S_ARROW, S_BASKET, S_BACKGROUND, S_SNAKE, S_FRUITS, S_ANIM, S_HUD, S_CHIFFRE, S_LOADING}S_TEXTURES;

#define NB_FLAPPY_TEXTURES 9
typedef enum{F_BACKGROUND, F_PIPES, F_BIRDS, F_MEDALS, F_SCOREBOARD, F_SOL, F_CHIFFRE, F_HIGHSCORE, F_LOADING}F_TEXTURES;

#define NB_SHOOTER_TEXTURES 13
typedef enum{SH_SHIP, SH_FLAME, SH_WEAPON, SH_MISSILE,SH_BACKGROUND, SH_EXPLO_MISSILE, SH_EXPLO_SHIP,SH_LASER_BEAM, SH_ENEMY_ROND, SH_ENEMY_LASER, SH_ENEMY_BOSS, SH_ARM, SH_ARM_BOSS}SH_TEXTURES;

#define NB_GAMES 5
#define NB_MAX_TEXTURES NB_ASTEROID_TEXTURES
