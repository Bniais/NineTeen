#include "pieces.h"

#define FRAMES_PER_SECOND 30
const int FRAME_TIME = 1000 / FRAMES_PER_SECOND;

//
#define NO_MOVE 0
#define MOVE_RIGHT 1
#define MOVE_LEFT -1

//Grille
#define GRILLE_W 10
#define GRILLE_H 20
SDL_Color colors[NB_PIECES] = {{150,100,100,100},{100,150,100,100},{100,100,150,100},{000,100,100,100},{100,000,100,100},{100,100,000,100},{100,255,100,100}};

//pieces
typedef struct {unsigned int rota; int id; float x; float y; int* grille; int size; int giant; int frameToGo; int dir; int frameDir; int frameStop; int firstCol; int lastCol; int firstRow; int lastRow;} Piece;
const SDL_Point UNDEFINED = {-500, -500};
#define FRAME_TO_GO_SPAWN 15

#define FRAME_LATERAL 7
const float MOVE_LATERAL=(1./FRAME_LATERAL);

#define FRAME_DOWN 9
const float MOVE_DOWN=(1./FRAME_DOWN);

const int MAX_STOP = FRAMES_PER_SECOND*0.8;
#define STOPPED -1
#define EMPTY -1

#define ROUNDABLE 0.0001

#define CASE_SIZE 40
const SDL_Point matrixSize = {500, 1000};
