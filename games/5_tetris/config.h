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

//frames and distances
#define NB_FRAMES 4
#define NB_DISTANCES 2
const float GROW_RATE = 0.99985;
const float GROW_RATE_LATERAL = 0.9999;
const int NB_GROW = FRAMES_PER_SECOND / 30.;

enum{LATERAL, DOWN, TO_GO, STOP};
#define FRAME_TO_GO (0.5 * FRAMES_PER_SECOND)

#define FRAME_LATERAL (0.33 * FRAMES_PER_SECOND)

#define FRAME_DOWN (0.5 * FRAMES_PER_SECOND)

#define FRAME_STOP (0.8 * FRAMES_PER_SECOND)

//accelerate
#define NO_ACCELERATE 1
#define ACCELERATE 2

//Grille
#define GRILLE_W 10
#define GRILLE_H 20
SDL_Color colors[NB_PIECES] = {{150,100,100,100},{100,150,100,100},{100,100,150,100},{000,100,100,100},{100,000,100,100},{100,100,000,100},{100,255,100,100}};

//pieces
typedef struct {unsigned int rota; int id; float x; float y; int* grille; int size; int giant; int frameToGo; int dir; int frameDir; int frameStop; int firstCol; int lastCol; int firstRow; int lastRow;} Piece;
const SDL_Point UNDEFINED = {-500, -500};



#define STOPPED -1
#define EMPTY -1

#define ROUNDABLE 0.0001

#define CASE_SIZE 40
const SDL_Point matrixSize = {500, 1000};
