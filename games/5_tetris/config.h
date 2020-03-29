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


//Grille
#define CASE_SIZE 38
#define BONUS_SIZE 20
#define GRILLE_W 10
#define GRILLE_H 20
#define MATRIX_X (PLAYGROUND_SIZE_W/2-(CASE_SIZE*GRILLE_W)/2)
#define MATRIX_Y (PLAYGROUND_SIZE_H/2-(CASE_SIZE*GRILLE_H)/2)
SDL_Rect MATRIX = {MATRIX_X,MATRIX_Y,GRILLE_W*CASE_SIZE,GRILLE_H*CASE_SIZE };
#define TOO_CLOSE_FROM_TOP 420
#define FRAME_REMIND_ROTATE (FRAMES_PER_SECOND/3)
#define SHIFT_RIGHT_NEXT 26


//Score
#define NB_CHAR_AFFICHAGE_SCORE 21
typedef struct {int score; int frame; int frameCombo; int scoreDest; int combo; int flat;int flatDest; int multi;}Score;
#define SIZE_SCORE (3*CASE_SIZE/4)


#define SCORE_TTL 40
#define RESET_ANIM 34
#define REACH_TARGET_SCORE 17
#define WAIT_SCORE_ANIM 2
const int ALPHA_SCORE[SCORE_TTL] = { 20, 40, 65, 90, 120, 135, 160, 175, 195, 195, 195, 195, 195, 195, 195, 195, 195, 195, 195, 195, 195,195, 195, 195, 195, 195, 195, 195, 195, 195, 195, 195, 195, 175, 145, 110, 90, 70, 40, 20 };
SDL_Rect SCORE_SRC = {0,0, 12,18};
const int SCORE_DEST = (MATRIX_X + (GRILLE_W-0.5)/2 * CASE_SIZE);
#define FONT_HEIGHT_RATIO 1.5

#define MAX_COMBO_LENGHT 100
#define MAX_APPEND_LENGHT 50

#define SCORE_BASE 10
#define RATIO_COMBO_LINE 2
#define NB_FLAT_POINT 100
#define RATIO_MULTI_POINT 3

#define COMBO_DRAW_X (MATRIX_X + GRILLE_W * CASE_SIZE + 80)

#define NB_COMBO 8

SDL_Color COMBO_COLOR[NB_COMBO] = { // rouges
	{0xfe, 0xdf, 0x15},
	{0xfe, 0x88, 0x28},
	{0xf9, 0x47, 0x3b},
	{0xe5, 0x21, 0x37},
	{0xab, 0x01, 0x32},
	{0x89, 0x04, 0x2f},
	{0x7d, 0x06, 0x2c},
	{0x6c, 0x08, 0x2c}
};

SDL_Color FLAT_COLOR[GRILLE_W] = {//vert bleu
	{0x00, 0xad, 0xf1},
	{0x00, 0xb1, 0x94},
	{0x00, 0xac, 0x70},
	{0x00, 0xa6, 0x50},
	{0x03, 0x8c, 0x45},
	{0x03, 0x8c, 0x45},
	{0x03, 0x8c, 0x45},
	{0x03, 0x8c, 0x45},
	{0x03, 0x8c, 0x45},
	{0x03, 0x8c, 0x45}
};

SDL_Color MULTI_COLOR[GRILLE_W] = {
	{0x30, 0x29, 0x76},
	{0x4e, 0x00, 0x91},
	{0x57, 0x11, 0x8d},
	{0x53, 0x0d, 0x6f},
	{0x5b, 0x18, 0x5b},
	{0x5b, 0x18, 0x5b},
	{0x5b, 0x18, 0x5b},
	{0x5b, 0x18, 0x5b},
	{0x5b, 0x18, 0x5b},
	{0x5b, 0x18, 0x5b}
};

#define NB_RAINBOW 8
SDL_Color RAINBOW_COLOR[NB_RAINBOW] = {
	{0x30, 0x00, 0x9b},
	{0xdd, 0x00, 0x00},
	{0x00, 0xbb, 0x00},
	{0x00, 0x00, 0x83},
	{0xfe, 0x62, 0x30},
	{0x00, 0x9b, 0xfe},
	{0xfe, 0xf6, 0x00},
	{0xbc, 0xbc, 0xcd}
};

//Textures
SDL_Rect LASER_DIM = {0,0,500,40};
#define LASER_WIDTH 30


SDL_Rect BRICK_SRC = {0,0,32,32};
SDL_Rect BRICK_DEST = {0,0, CASE_SIZE,CASE_SIZE};

#define DIST_HUD_GRILLE_X 77
#define DIST_HUD_GRILLE_Y 91
SDL_Rect HUD_GRILLE_DIM = {MATRIX_X-DIST_HUD_GRILLE_X, MATRIX_Y-DIST_HUD_GRILLE_Y, 680, 950};
SDL_Rect BACKGROUND_SRC = {0,0,960,540};
#define BACKGROUND_COL 8
#define BACKGROUND_ROW 10


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
#define ACCELERATE 10

//colors
const SDL_Color CONTOUR_GRILLE = {0, 51, 94, 255};
const SDL_Color CORPS_GRILLE = {0,87,142,255};
const SDL_Color COL_PIECE = {17,108,166,255};
//{0,166,251,255};
SDL_Color colors[NB_PIECES] = {{150,100,100,100},{100,150,100,100},{100,100,150,100},{000,100,100,100},{100,000,100,100},{100,100,000,100},{100,255,100,100}};

#define HUD_X (MATRIX_X - 200)


typedef struct{ int lineDown; float shift;} ShiftDown;

//Bonus
#define NB_BONUSES 7
enum{NO_BONUS, FILL, MULTI_POINT, LASER, SLOW, SPEED, GIANT, FLAT_POINT};
#define BONUS_TRI 10
SDL_Color colors_b[NB_BONUSES] = {{120,000,150,100},{100,070,180,100},{170,170,000,170},{000,255,120,100}};
#define NB_LINES_LASER 3
#define MAX_HEIGHT_LASER 9
#define SLOW_AMMOUNT (FRAMES_PER_SECOND * 45)
#define SPEED_AMMOUNT (FRAMES_PER_SECOND * 30)


#define FRAME_COMPLETE_LINE 11


#define LASER_FRAME 40
#define LASER_START_COMPLETE 22
#define LASER_START_SHOOT 28
#define LASER_ARRIVED 33
#define LASER_RECOIL 24
#define LASER_RECOIL_DURATION 3
#define LASER_RETREAT 10
#define DURATION_SHOOT 14
int LASER_RECOIL_DIST[LASER_RECOIL_DURATION] = {8,4,2};

#define SPAWN_LASER (MATRIX_X - 3 * CASE_SIZE)
#define UNSPAWN_LASER (MATRIX_X - 4 * CASE_SIZE)
#define LASER_END_POS (MATRIX_X - 1.2*CASE_SIZE)

#define NB_FILL 8
#define FRAME_FILL 13
#define ESPACEMENT_FRAME_FILL 1.5
#define FRAME_DEPLACEMENT_FILL 13

SDL_Point SPAWN_FILL={(MATRIX_X + (GRILLE_W+3)* CASE_SIZE),(MATRIX_Y+(3*GRILLE_H/4)*CASE_SIZE)};

#define INNER_RIGHT (HUD_GRILLE_DIM.x+520)
#define INNER_LEFT (HUD_GRILLE_DIM.x+4)



//pieces
typedef struct {unsigned int rota; int id; float x; float y; int* grille; int size; int giant; int frameToGo; int dir; int frameDir; int frameStop; int firstCol; int lastCol; int firstRow; int lastRow; int bonus;} Piece;
const SDL_Point UNDEFINED = {-500, -500};



#define STOPPED -1
#define EMPTY -1

#define ROUNDABLE 0.0001

const SDL_Point matrixSize = {500, 1000};
