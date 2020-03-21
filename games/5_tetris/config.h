#include "pieces.h"

#define FRAMES_PER_SECOND 30
const int FRAME_TIME = 1000 / 30;

//
#define NO_MOVE 0
#define MOVE_RIGHT 1
#define MOVE_LEFT -1

enum{COULDNT_PUT, COULD_PUT};

//Rely
enum {ALREADY_ROUNDED, RELY_ON_BOTH, RELY_ON_LEFT, RELY_ON_RIGHT};

//Textures
SDL_Rect LASER_SRC = {0,0,1500,120};
SDL_Rect LASER_DEST = {0,0,40*13,40};
SDL_Rect BRICK_SRC = {0,0,32,32};
SDL_Rect BRICK_DEST = {0,0, 40,40};

//frames and distances
#define NB_FRAMES 4
#define NB_DISTANCES 2
const float GROW_RATE = 0.99993;
const float GROW_RATE_LATERAL = 0.99995;
const int NB_GROW = FRAMES_PER_SECOND / 30.;

enum{LATERAL, DOWN, TO_GO, STOP};
#define FRAME_TO_GO (0.5 * FRAMES_PER_SECOND)

#define FRAME_LATERAL (0.33 * FRAMES_PER_SECOND)

#define FRAME_DOWN (0.5 * FRAMES_PER_SECOND)

#define FRAME_STOP (0.8 * FRAMES_PER_SECOND)

//accelerate
#define NO_ACCELERATE 1
#define ACCELERATE 30

//Grille
#define CASE_SIZE 40
#define BONUS_SIZE 20
#define GRILLE_W 10
#define GRILLE_H 20
#define MATRIX_X 600
#define MATRIX_Y 100
SDL_Rect MATRIX = {MATRIX_X,MATRIX_Y,GRILLE_W*CASE_SIZE,GRILLE_H*CASE_SIZE };
#define TOO_CLOSE_FROM_TOP 420
#define FRAME_REMIND_ROTATE (FRAMES_PER_SECOND/2)

SDL_Color colors[NB_PIECES] = {{150,100,100,100},{100,150,100,100},{100,100,150,100},{000,100,100,100},{100,000,100,100},{100,100,000,100},{100,255,100,100}};

#define HUD_X (MATRIX_X - 200)


typedef struct{ int lineDown; float shift;} ShiftDown;

//Bonus
#define NB_BONUSES 6
enum{NO_BONUS, FILL, POINTS, LASER, SLOW, SPEED, GIANT};
#define BONUS_TRI 10
SDL_Color colors_b[NB_BONUSES] = {{120,000,150,100},{100,070,180,100},{170,170,000,170},{000,255,120,100}};
#define NB_LINES_LASER 3
#define SLOW_AMMOUNT (FRAMES_PER_SECOND * 45)
#define SPEED_AMMOUNT (FRAMES_PER_SECOND * 30)

#define LASER_FRAME 35
#define FRAME_COMPLETE_LINE 10
#define LASER_START_COMPLETE 22
#define LASER_START_SHOOT 15
#define DIST_PER_FRAME_LASER ((MATRIX_X - HUD_X - CASE_SIZE) / LASER_START_SHOOT)
#define DIST_PER_FRAME_LASER_DISAPEAR ((MATRIX_X - HUD_X - CASE_SIZE) / (LASER_FRAME-LASER_START_SHOOT-15))
#define LASER_RECOIL 5
#define LASER_RECOIL_DURATION 3
#define LASER_END_POS (MATRIX_X - CASE_SIZE)
#define NB_FILL 6
#define FRAME_FILL 11
#define ESPACEMENT_FRAME_FILL 2
#define FRAME_DEPLACEMENT_FILL 8

SDL_Point SPAWN_FILL={(MATRIX_X - 5* CASE_SIZE),(MATRIX_Y+(GRILLE_H/2)*CASE_SIZE)};

int LASER_RECOIL_DIST[LASER_RECOIL_DURATION] = {8,4,2};

//pieces
typedef struct {unsigned int rota; int id; float x; float y; int* grille; int size; int giant; int frameToGo; int dir; int frameDir; int frameStop; int firstCol; int lastCol; int firstRow; int lastRow; int bonus;} Piece;
const SDL_Point UNDEFINED = {-500, -500};



#define STOPPED -1
#define EMPTY -1

#define ROUNDABLE 0.0001

const SDL_Point matrixSize = {500, 1000};
