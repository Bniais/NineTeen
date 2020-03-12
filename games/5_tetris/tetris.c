#include "../../define/define.h"
#include "config.h"
#include <time.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_image.h>

#include <stdlib.h>
#include <stdio.h>

#include <math.h>

void myInit(){

	// SDL Init
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	//audio
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	if( Mix_OpenAudio( MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 1024 ) == -1 )
		printf("Erreur init SDL_Mixer\n" );

	//rand
	srand(time(NULL));
}

SDL_Point maximizeWindow(SDL_Rect displayBounds, float* ratioWindowSize){
	SDL_Point maxW = {(PLAYGROUND_SIZE_W + 2 * HUD_W), (PLAYGROUND_SIZE_H + 2 * HUD_H)};
	if( maxW.x > displayBounds.w - ESPACE_DISPLAY_WINDOW.x  ||  maxW.y > (displayBounds.h - ESPACE_DISPLAY_WINDOW.y) - ESPACE_DISPLAY_WINDOW.y){
		if( (float)maxW.x/maxW.y > (float)(displayBounds.w - ESPACE_DISPLAY_WINDOW.x)/(displayBounds.h - ESPACE_DISPLAY_WINDOW.y) ){
			*ratioWindowSize = (float)maxW.x / (displayBounds.w - ESPACE_DISPLAY_WINDOW.x);
			maxW.y /= (float)maxW.x / (displayBounds.w - ESPACE_DISPLAY_WINDOW.x);
			maxW.x = displayBounds.w - ESPACE_DISPLAY_WINDOW.x;
		}
		else{
			*ratioWindowSize = (float)maxW.y / (displayBounds.h - ESPACE_DISPLAY_WINDOW.y);
			maxW.x /= (float)maxW.y / (displayBounds.h - ESPACE_DISPLAY_WINDOW.y);
			maxW.y = (displayBounds.h - ESPACE_DISPLAY_WINDOW.y);
		}
	}
	return maxW;
}

void afficherPauseMenu(SDL_Renderer *renderer, SDL_Point mouseCoor, SDL_Texture *voileTexture, SDL_Texture *pauseMenuTexture){
	//voile noir
	SDL_RenderCopy(renderer, voileTexture, NULL, NULL);
	SDL_Rect dest = BOUTON_PAUSE_BASE;
	SDL_Rect src = {0, 0, BOUTON_PAUSE_SIZE_W, BOUTON_PAUSE_SIZE_H};

	//boutons liste
	for(int i = 0; i < NB_BOUTON_PAUSE; i++){
		if( mouseCoor.x >= dest.x && mouseCoor.x <= dest.x + dest.w && mouseCoor.y >= dest.y && mouseCoor.y <= dest.y + dest.h)
			src.y = BOUTON_PAUSE_SIZE_H;
		else
			src.y = 0;

		SDL_RenderCopy(renderer, pauseMenuTexture, &src, &dest);
		src.x += BOUTON_PAUSE_SIZE_W;
		dest.y += BOUTON_PAUSE_SIZE_H + ESPACEMENT_BOUTON_PAUSE;
	}
}

void initMatrix(int matrix[GRILLE_W][GRILLE_H]){
	for(int i=0; i<GRILLE_W; i++)
		for(int j=0; j<GRILLE_H; j++)
			matrix[i][j] = EMPTY;
}

void initPiece(Piece *piece){
	piece->rota = 0;
	piece->id = rand() % NB_PIECES;
	piece->x = UNDEFINED.x;
	piece->y = UNDEFINED.y;
	piece->giant = SDL_FALSE;
	piece->size = PIECE_SIZE;
	piece->grille = malloc(piece->size * piece->size * sizeof(int));
	piece->frameToGo = 0;
	piece->dir = NO_MOVE;
	piece->frameDir = 0;
	piece->frameStop = 0;
	piece->firstCol = 0;
	piece->lastCol = 0;
	piece->firstRow = 0;
	piece->lastRow = 0;
	piece->bonus = NO_BONUS;
}

void updateDistances(float frame[NB_FRAMES], float distances[NB_DISTANCES], int *framePassed){
	printf("fp : %d\n",*framePassed );
	for(int i=0; i<NB_FRAMES; i++){
		frame[i] *= pow(i==LATERAL ? GROW_RATE_LATERAL : GROW_RATE, *framePassed);
		if(NB_GROW == 1) //30fps
			frame[i] *= pow(i==LATERAL ? GROW_RATE_LATERAL : GROW_RATE, *framePassed);
	}

	distances[DOWN] = 1. / (int)frame[DOWN];
	distances[LATERAL] = 1. / (int)frame[LATERAL];

	*framePassed = 0;
}

int tooCloseFromMatrix(Piece piece, int matrix[GRILLE_W][GRILLE_H]){
	//Pas besoin de verifier si i + (int)piece.x < GRILLE_W car on le vérifie dans tooCloseFromWall
	int i;
	int j;

	for(i = piece.firstCol; i <= piece.lastCol; i++)
		for(j = piece.firstRow; j <= piece.lastRow; j++)
			if( piece.grille[j * piece.size + i] && ( i + (int)floor(piece.x) < 0 || matrix[i + (int)floor(piece.x)][j + (int)piece.y] != EMPTY ) )
				return 1;

	if( piece.x != roundf(piece.x) && piece.y != roundf(piece.y) ){
		for(i = piece.firstCol; i <= piece.lastCol; i++)
			for(j = piece.firstRow; j <= piece.lastRow; j++)
				if( piece.grille[j * piece.size + i] && ( i + (int)floor(piece.x) + 1 < 0 || matrix[i + (int)floor(piece.x) + 1][j + (int)piece.y + 1] != EMPTY ) ) //Pas besoin de verifier si (int)piece.y + 1 n'est pas en dehors de la matrice car tooCloseFromWall est appelé avant
					return 1;
	}


	if( piece.x != roundf(piece.x) ){
		for(i = piece.firstCol; i <= piece.lastCol; i++)
			for(j = piece.firstRow; j <= piece.lastRow; j++)
				if( piece.grille[j * piece.size + i] && ( i + (int)floor(piece.x) + 1 < 0 || matrix[i + (int)floor(piece.x) + 1][j + (int)piece.y] != EMPTY ) )
					return 1;
	}

	if( piece.y != roundf(piece.y) ){

		for(i = piece.firstCol; i <= piece.lastCol; i++)
			for(j = piece.firstRow; j <= piece.lastRow; j++)
				if( piece.grille[j * piece.size + i] && ( i + (int)floor(piece.x) < 0 || matrix[i + (int)floor(piece.x)][j + (int)piece.y + 1] != EMPTY ) )
					return 1;
	}

	return 0;
}

int getFirstColumn(Piece piece){
	for(int j = 0; j < piece.size; j++)
		for(int i = 0; i < piece.size; i++)
			if(piece.grille[i * piece.size + j])
				return j;

	return 0;
}

int getLastColumn(Piece piece){
	for(int j = piece.size - 1; j >= 0; j--)
		for(int i = 0; i < piece.size; i++)
			if(piece.grille[i * piece.size + j])
				return j;

	return 0;
}

int getFirstRow(Piece piece){
	for(int j = 0; j < piece.size; j++)
		for(int i = 0; i < piece.size; i++)
			if(piece.grille[j * piece.size + i])
				return j;

	return 0;
}

int getLastRow(Piece piece){
	for(int j = piece.size - 1; j >= 0; j--)
		for(int i = 0; i < piece.size; i++)
			if(piece.grille[j * piece.size + i])
				return j;

	return 0;
}

void updateGrille(Piece *piece){
	for(int i = 0; i < piece->size; i++)
		for(int j = 0; j < piece->size; j++)
			(piece->grille)[i * piece->size + j] = PIECES[piece->giant][piece->id][(piece->rota) % NB_ROTA][i][j];
}


void getColRawInfos(Piece *piece){
	piece->firstCol = getFirstColumn(*piece);
	piece->lastCol = getLastColumn(*piece);
	piece->firstRow = getFirstRow(*piece);
	piece->lastRow = getLastRow(*piece);
}

void getNewPiece(Piece *piece, int giant){
	piece->rota = 0;
	piece->size = PIECE_SIZE * (giant ? RATIO_GIANT : 1);
	if(piece->giant != giant)
		piece->grille = realloc( piece->grille, piece->size * piece->size * sizeof(int));

	piece->giant = giant;
	piece->id = rand() % NB_PIECES;

	//handle bonus
	piece->bonus = LASER;//rand() % (NB_BONUSES+1);

	updateGrille(piece);

	getColRawInfos(piece);

}

int almostRound( float f ){
	return fabs(f - roundf(f)) < ROUNDABLE;
}

int putAtTop(Piece *piece,  int matrix[GRILLE_W][GRILLE_H], int frameToGo){
	piece->x = roundf((GRILLE_W - piece->size) / 2);
	piece->y = 0 - piece->firstRow;
	piece->frameToGo = frameToGo;
	piece->frameDir = 0;
	piece->dir = NO_MOVE;
	piece->frameStop = 0;

	if(tooCloseFromMatrix(*piece, matrix)){
		piece->y = UNDEFINED.y;
		piece->frameToGo = 9999;
		return COULDNT_PUT;
	}

	return COULD_PUT;
}

void putAtNextPiece(Piece *piece){
	piece->x = GRILLE_W;
	piece->y = 0;
}

void drawPiece(SDL_Renderer *renderer,SDL_Texture* brickTexture, SDL_Texture *bonusTexture, Piece piece){
	SDL_Rect src = BRICK_SRC;
	SDL_Rect dest = BRICK_DEST;
	for(int i = 0; i < piece.size; i++)
		for(int j = 0; j < piece.size; j++)
			if((piece.grille)[i * piece.size + j]){
				src.x = piece.id * BRICK_SRC.w;
				dest.x = (j + piece.x) * CASE_SIZE + MATRIX.x;
				dest.y = (i + piece.y) * CASE_SIZE + MATRIX.y;
				SDL_RenderCopy(renderer, brickTexture, &src, &dest);
				if( (piece.grille)[i * piece.size + j] == 2 && piece.bonus  != NO_BONUS){
					src.x = (piece.bonus - 1) * BRICK_SRC.h;
					SDL_RenderCopy(renderer, bonusTexture, &src, &dest);
				}
			}
}

int tooCloseFromWall(Piece piece){
	return (piece.x + piece.firstCol < -ROUNDABLE || piece.x + piece.lastCol > GRILLE_W - 1 || piece.y + piece.firstRow < -ROUNDABLE || piece.y + piece.lastRow > GRILLE_H - 1);
}

void rotatePiece(Piece *piece, int rotateSens, int matrix[GRILLE_W][GRILLE_H]){
	if( rotateSens ){
		piece->rota += rotateSens;
		updateGrille(piece);
		getColRawInfos(piece);

		if( tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix)){
			piece->rota -= rotateSens;
			updateGrille(piece);
			getColRawInfos(piece);
		}
	}
}

void moveSide(Piece *piece, int matrix[GRILLE_W][GRILLE_H], float distanceLateral){
	//Attention si on ajoute des cases dans la matrice en même temps qu'on bouge, on pourrait avoir des pièces qui s'arrêtent entre deux collones
	if( piece->dir ){
		piece->x += piece->dir * distanceLateral;
		piece->frameDir--;

		if(almostRound(piece->x))
			piece->x = roundf(piece->x);

		if(piece->frameDir == 0)
			piece->dir = NO_MOVE;

		if( tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix)){
			piece->x -= piece->dir * distanceLateral;
			piece->dir = NO_MOVE;
			piece->frameDir = 0;
		}
	}
}

int moveDown(Piece *piece, int accelerate, int matrix[GRILLE_W][GRILLE_H], float distanceDown, int frameStop){
	float distanceMoveDown = distanceDown * accelerate;
	if(distanceMoveDown >= 2)
		distanceMoveDown = 1.99;

	piece->y += distanceMoveDown;
	if(almostRound(piece->x))
		piece->x = roundf(piece->x);
	if(almostRound(piece->y))
		piece->y = roundf(piece->y);

	if( tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix)){
		piece->y = floor(piece->y);
		if( tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix)){
			piece->y -= floor(distanceMoveDown);
			printf("%f\n",distanceMoveDown );
		}
		if(accelerate == ACCELERATE)
			piece->frameStop = frameStop;

		if(piece->frameStop != frameStop)
			piece->frameStop ++;
		else
			return STOPPED;
	}
	else
		piece->frameStop = 0; //abusable ?

	return 0;

}

void changeDir(Piece *piece, int lateralMove, int frameLateral){
	if(lateralMove != NO_MOVE){
		if( piece->dir != lateralMove && piece->dir != NO_MOVE)
			piece->frameDir = frameLateral - piece->frameDir;
		else if(piece->frameDir == 0)
			piece->frameDir += frameLateral;

		piece->dir = lateralMove;
	}
}

int getPieceId(int n){
	return n%10;
}

void completeLine(int matrix[GRILLE_W][GRILLE_H], int frameCompleteLine[GRILLE_H], int line, int bonusActivate[NB_BONUSES], int getBonuses);

void useBonus(int bonusId, int frameLaser[GRILLE_H], int *framePassed, int nbUse){
	switch (bonusId) {
		case FILL:
			break;

		case POINTS:
			break;

		case LASER:
			printf("laser\n");
			for(int i=GRILLE_H-1; i > GRILLE_H - (NB_LINES_LASER * nbUse) - 1 && i > 0; i--)
				frameLaser[i] = LASER_FRAME;
			break;

		case SLOW:
			printf("slow\n");
			*framePassed -= nbUse * SLOW_AMMOUNT;
			break;
	}
}

void activateBonuses( int bonusActivate[NB_BONUSES],int frameLaser[GRILLE_H], int *framePassed ){
	for(int i=0; i<NB_BONUSES; i++)
		if(bonusActivate[i])
			useBonus(i+1, frameLaser, framePassed, bonusActivate[i]);
}

int getBonusId(int rectId){
	int bonusId = 0;

	while(rectId >= 10){
		rectId-=10;
		bonusId++;
	}

	return bonusId;
}

float getShift(int line, ShiftDown* shifts){
	return 0;
}


void drawMatrix(SDL_Renderer *renderer, SDL_Texture *brickTexture, SDL_Texture *bonusTexture, int matrix[GRILLE_W][GRILLE_H], int frameCompleteLine[GRILLE_H], ShiftDown* shifts){
	SDL_Rect src = BRICK_SRC;
	SDL_Rect dest = BRICK_DEST;

	SDL_SetRenderDrawColor(renderer, 180,180,180,255);
	SDL_RenderFillRect(renderer, &MATRIX);

	for(int i = 0; i < GRILLE_W; i++){
		for(int j = 0; j < GRILLE_H; j++){
			dest.x = i * CASE_SIZE + MATRIX.x;
			dest.y = j * CASE_SIZE + MATRIX.y + getShift(j, shifts);

			if(matrix[i][j] != EMPTY){ //piece
				src.x = getPieceId(matrix[i][j]) * BRICK_SRC.w;

				if(frameCompleteLine[j] >= 0){
					printf("%d\n", frameCompleteLine[j] );
					SDL_SetTextureColorMod(brickTexture, 255 / (FRAME_COMPLETE_LINE - frameCompleteLine[j] + 1), 255 / (FRAME_COMPLETE_LINE - frameCompleteLine[j] + 1), 255 / (FRAME_COMPLETE_LINE - frameCompleteLine[j] + 1));
				}
				else{
					SDL_SetTextureColorMod(brickTexture, 255, 255, 255);
				}
				printf("dezda %d\n", getBonusId(matrix[i][j]));


				SDL_RenderCopy(renderer, brickTexture, &src, &dest);

				if(matrix[i][j] >= 10){
					src.x = (getBonusId(matrix[i][j]) - 1) * BRICK_SRC.h;
					SDL_RenderCopy(renderer, bonusTexture, &src, &dest);
				}

			}
			else{
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				SDL_RenderDrawRect(renderer, &dest);
			} //grille


		}
	}
	SDL_SetTextureColorMod(brickTexture, 255, 255, 255);
}

void drawAnims(SDL_Renderer *renderer, SDL_Texture *laserTexture, int frameLaser[GRILLE_H]){
	//laser
	SDL_Rect anim = LASER_SRC;
	SDL_Rect dest = LASER_DEST;
	for(int i=0; i<GRILLE_H; i++){
		if(frameLaser[i] >= 0){
			int frame = (LASER_FRAME - frameLaser[i] - LASER_START_SHOOT < 0 ? 0 : LASER_FRAME - frameLaser[i] - LASER_START_SHOOT);

			printf("f : %d\n", frame);


			if (frame>=15){
				dest.x = LASER_END_POS - (frame - 14) * DIST_PER_FRAME_LASER_DISAPEAR;
				frame=0;
			}
			else if(frame >= LASER_RECOIL &&  frame < LASER_RECOIL + LASER_RECOIL_DURATION){
				dest.x = LASER_END_POS - LASER_RECOIL_DIST[frame-LASER_RECOIL];
				printf("recoil\n");
			}
			else if(LASER_FRAME - frameLaser[i] < LASER_START_SHOOT)
				dest.x = HUD_X +( LASER_FRAME - frameLaser[i]) * DIST_PER_FRAME_LASER; // add selon frameLaser[i]
			else
				dest.x = LASER_END_POS;

			dest.y = i * CASE_SIZE + MATRIX.y;

			anim.y = frame * LASER_SRC.h;
			SDL_RenderCopy(renderer, laserTexture, &anim, &dest);
		}
	}
	SDL_SetRenderDrawColor(renderer,255,255,255,255);
}

void savePiece(Piece piece, int matrix[GRILLE_W][GRILLE_H]){
	piece.y = roundf(piece.y);
	piece.x = roundf(piece.x);

	for(int i = 0; i < piece.size; i++){
		for(int j = 0; j < piece.size; j++){
			if(piece.grille[j * piece.size + i] == 2)
				matrix[i + (int)piece.x][j + (int)piece.y] = piece.id + BONUS_TRI*piece.bonus;
			else if(piece.grille[j * piece.size + i])
				matrix[i + (int)piece.x][j + (int)piece.y] = piece.id ;
		}
	}
}

void completeLine(int matrix[GRILLE_W][GRILLE_H], int frameCompleteLine[GRILLE_H], int line, int bonusActivate[NB_BONUSES], int getBonuses){

	frameCompleteLine[line] = FRAME_COMPLETE_LINE;

	if(getBonuses)
		for(int i = 0; i < GRILLE_W; i++)
			if(matrix[i][line] >= BONUS_TRI )
				bonusActivate[getBonusId(matrix[i][line])-1]++;
}

void eraseLine(int matrix[GRILLE_W][GRILLE_H], int line, int frameLaser[GRILLE_H], int frameCompleteLine[GRILLE_H]){
	for(int i = line; i > 0; i-- )
		for(int j=0; j<GRILLE_W; j++){
			matrix[j][i] = matrix[j][i-1];
			//frameLaser[i] = frameLaser[i-1];
			//frameCompleteLine[i] = frameCompleteLine [i-1];
		}


	for(int j=0; j<GRILLE_W; j++)
		matrix[j][0] = EMPTY;

	frameLaser[0] = -1;
	frameCompleteLine[0] = -1;
}

void checkLines(int matrix[GRILLE_W][GRILLE_H], int frameCompleteLine[GRILLE_H], int bonusActivate[NB_BONUSES]){
	for(int i=0; i<GRILLE_H; i++){
		int j;
		for(j=0; j<GRILLE_W && matrix[j][i] != EMPTY; j++);
		if(j == GRILLE_W)
			completeLine(matrix, frameCompleteLine, i, bonusActivate, SDL_TRUE);
	}
}

void clearIntTab( int *tab, int size){
	for(int i=0; i<size; i++)
		tab[i] = 0;
}

void transfertNextPiece(Piece *currentPiece, Piece nextPiece){
 	//handle giant (giant, size)
	currentPiece->id = nextPiece.id;
	currentPiece->bonus = nextPiece.bonus;
	currentPiece->rota = 0;
	currentPiece->firstCol = nextPiece.firstCol;
	currentPiece->lastCol = nextPiece.lastCol;
	currentPiece->firstRow = nextPiece.firstRow;
	currentPiece->lastRow = nextPiece.lastRow;
	updateGrille(currentPiece);
}

void updateFrames(int *framePassed, int frameLaser[GRILLE_H], int frameCompleteLine[GRILLE_H], int matrix[GRILLE_W][GRILLE_H]){
	(*framePassed)++;
	for(int i=0; i<GRILLE_H; i++){
		if(frameLaser[i] == LASER_FRAME - LASER_START_COMPLETE)
			frameCompleteLine[i] = FRAME_COMPLETE_LINE + 1;

		if(frameLaser[i] >= 0)
			frameLaser[i]--;

		if(frameCompleteLine[i] == 0)
			eraseLine(matrix, i, frameLaser, frameCompleteLine);

		if(frameCompleteLine[i] >= 0)
			frameCompleteLine[i]--;

	}
}

void shiftMatrixDown(int matrix[GRILLE_W][GRILLE_H], ShiftDown * shift){

}

// int launchSnake(SDL_Window *myWindow, SDL_Renderer* renderer, char *identifiant, char *token){
int main(){
/////////////////////
/// MISE EN PLACE ///``
/////////////////////
	myInit();

	////////////
	/// Vars ///`
	////////////

	//move
	int accelerate = NO_ACCELERATE;
	int lateralMove = NO_MOVE;
	int rotate = SDL_FALSE;
	int rdyToRotate[2] = {SDL_TRUE, SDL_TRUE};
	int cantMoveSide = SDL_FALSE;


	//frames and distances
	int framePassed = 0;
	float frame[NB_FRAMES];
	frame[LATERAL] = FRAME_LATERAL;
	frame[DOWN] = FRAME_DOWN;
	frame[TO_GO] = FRAME_TO_GO;
	frame[STOP] = FRAME_STOP;
	float distances[NB_DISTANCES];
	updateDistances(frame, distances, &framePassed);


	//Bonuses
	int bonusActivate[NB_BONUSES];
	int frameLaser[GRILLE_H];
	for(int i=0; i < GRILLE_H; i++)
		frameLaser[i] = -1;


	//pieces
	Piece currentPiece, nextPiece;


	//matrice
	int matrix[GRILLE_W][GRILLE_H];
	int frameCompleteLine[GRILLE_H];
	for(int i=0; i < GRILLE_H; i++)
		frameCompleteLine[i] = -1;
	ShiftDown* shifts = malloc(sizeof(ShiftDown));
	shifts[0].lineDown = -1;
	shifts[0].shift = 0;

	//Keyboard
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);

	//Time
	unsigned int lastTime = 0, currentTime;

	//Fonts
	/*TTF_Font* scoreFont = TTF_OpenFont("./fonts/flappy.ttf", OPEN_FONT_SIZE);
	if( scoreFont == NULL ){
		printf("TTF_OpenFont() Failed: %s\n", TTF_GetError());
		return EXIT_FAILURE;
	}*/

	//audio
	/*Mix_Chunk *flap_wav = Mix_LoadWAV( "../3_flappy_bird/Sounds/flap.wav" );
	if( !flap_wav)
		printf("Erreur chargement des sons\n");*/

	//Window and renderer
	float ratioWindowSize = 1;
	SDL_Rect displayBounds;
	SDL_GetDisplayBounds(0, &displayBounds);
	SDL_Point maxWindowSize = maximizeWindow(displayBounds, &ratioWindowSize);

	SDL_Window *myWindow = SDL_CreateWindow("Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, maxWindowSize.x ,maxWindowSize.y, WINDOW_FLAG);
	if( myWindow == NULL ){
		printf("Erreur lors de la creation de la fenêtre : %s", SDL_GetError());
		return EXIT_FAILURE;
	}
	printf("%d, %d\n", maxWindowSize.x ,maxWindowSize.y );

	SDL_Renderer *renderer = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED);
	if( renderer == NULL ){
		printf("Erreur lors de la creation d'un renderer : %s", SDL_GetError());
		return EXIT_FAILURE;
	}

	//Views
	SDL_Rect playgroundView = {HUD_W/ratioWindowSize, HUD_H/ratioWindowSize, PLAYGROUND_SIZE_W, PLAYGROUND_SIZE_H};
	SDL_Rect hudView = {0, 0, (PLAYGROUND_SIZE_W + 2 * HUD_W), (PLAYGROUND_SIZE_H + 2 * HUD_H)};
	SDL_Rect hudDraw = {0, 0, (PLAYGROUND_SIZE_W + 2 * HUD_W)/ratioWindowSize, (PLAYGROUND_SIZE_H + 2 * HUD_H)/ratioWindowSize};

	//hud and menus
	int paused = SDL_FALSE;
	int rdyToPause = SDL_TRUE;


	//mouse
	SDL_Point mouseCoor;

	//Textures
	SDL_Texture *hudTexture = IMG_LoadTexture(renderer, DIR_HUD);
	SDL_Texture *laserTexture = IMG_LoadTexture(renderer, "./Textures/laserAnim.png");
	SDL_Texture *brickTexture = IMG_LoadTexture(renderer, "./Textures/bricks.png");
	SDL_Texture *bonusTexture = IMG_LoadTexture(renderer, "./Textures/bonus.png");
	if( hudTexture == NULL ){
		printf("Erreur lors de la creation de texture");
		return EXIT_FAILURE;
	}

	///////////////////////
	/// Initialize vars ///`
	///////////////////////
	initMatrix(matrix);
	initPiece(&currentPiece);
	initPiece(&nextPiece);
	getNewPiece(&currentPiece, 0);
	getNewPiece(&nextPiece, 0);
	putAtTop(&currentPiece, matrix, (int)frame[TO_GO]);
	putAtNextPiece(&nextPiece);

/////////////////////
/// BOUCLE DU JEU ///``
/////////////////////

	while( 1 ){

		// Init input
		SDL_GetMouseState(&(mouseCoor.x), &(mouseCoor.y));
		accelerate = NO_ACCELERATE;
		lateralMove = NO_MOVE;
		rotate = SDL_FALSE;

	////////////
	// Events //`
	////////////
		SDL_Event event;
		while( SDL_PollEvent(&event) ){
			switch( event.type ){
				case SDL_QUIT:
					// fermer
					return 0;
					break;

				case SDL_KEYUP:
					if ( event.key.keysym.sym == SDLK_ESCAPE )
						rdyToPause = SDL_TRUE;
					else if(event.key.keysym.sym == SDLK_a)
						rdyToRotate[0] = SDL_TRUE;
					else if( event.key.keysym.sym == SDLK_e)
						rdyToRotate[1] = SDL_TRUE;
					break;

				case SDL_KEYDOWN:
					if ( event.key.keysym.sym == SDLK_ESCAPE && rdyToPause ){
						paused = !paused;
						rdyToPause = SDL_FALSE;
					}
					break;
			}
		}


	////////////////////////////
	// Handle Keyboard inputs //`
	////////////////////////////
		SDL_PumpEvents();

		if( keystate[SDL_SCANCODE_DOWN] )
			accelerate = ACCELERATE;


		if( keystate[SDL_SCANCODE_RIGHT] )
			lateralMove = MOVE_RIGHT;

		else if( keystate[SDL_SCANCODE_LEFT] )
			lateralMove = MOVE_LEFT;



		if( keystate[SDL_SCANCODE_Q] && rdyToRotate[0] ){
			rotate = 1;
			rdyToRotate[0] = SDL_FALSE;
		}
		else if( keystate[SDL_SCANCODE_E] && rdyToRotate[1] ){
			rotate = -1;
			rdyToRotate[1] = SDL_FALSE;
		}


		//gérer utilisation bonus ?

	//////////////
	// Gameplay //`
	//////////////


		rotatePiece(&currentPiece, rotate, matrix);

		if(!cantMoveSide)
			changeDir(&currentPiece, lateralMove, (int)frame[LATERAL]);

		moveSide(&currentPiece, matrix, distances[LATERAL]);


		cantMoveSide = SDL_FALSE;
		if(!currentPiece.frameToGo){
			if( moveDown(&currentPiece, accelerate, matrix, distances[DOWN], (int)frame[STOP]) == STOPPED ){
				cantMoveSide = SDL_TRUE;
				if(currentPiece.frameDir == 0){
					//Piece is saved and remplaced
					savePiece(currentPiece, matrix);
					clearIntTab(bonusActivate, NB_BONUSES);
					checkLines(matrix, frameCompleteLine, bonusActivate);
					activateBonuses(bonusActivate, frameLaser, &framePassed);
					updateDistances(frame, distances, &framePassed);
					transfertNextPiece(&currentPiece,nextPiece);
					getNewPiece(&nextPiece, 0);
					if( putAtTop(&currentPiece, matrix, (int)frame[TO_GO]) == COULDNT_PUT )
						break;
				}


			}
		}
		else
			currentPiece.frameToGo--;
	///////////////////
	// Check hitboxs //`
	///////////////////

	//////////
	// Draw //`
	//////////
		drawMatrix(renderer, brickTexture, bonusTexture, matrix, frameCompleteLine, shifts);
		drawPiece(renderer, brickTexture, bonusTexture, currentPiece);
		drawPiece(renderer, brickTexture, bonusTexture, nextPiece);
		drawAnims(renderer, laserTexture, frameLaser);
		//drawNextPiece(nextPiece);

		//hud
		SDL_RenderSetScale(renderer, 1, 1);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderSetViewport(renderer, &hudView);
		SDL_RenderCopy(renderer, hudTexture, &hudView, &hudDraw);
		SDL_RenderSetViewport(renderer, &playgroundView);

		//afficher
		SDL_RenderSetScale(renderer, 1. / ratioWindowSize, 1. / ratioWindowSize);
		SDL_RenderPresent(renderer);


	////////////////
	// Next frame //`
	////////////////

		//regulateFPS
		currentTime = SDL_GetTicks();
		while( currentTime - lastTime < FRAME_TIME )
			currentTime = SDL_GetTicks();

		if( currentTime - lastTime > FRAME_TIME )
			printf(" TIME FRAME : %d\n", currentTime - lastTime);

		lastTime = currentTime;

		//Actualise frames
		updateFrames(&framePassed, frameLaser, frameCompleteLine, matrix);


		// On efface
		SDL_SetRenderDrawColor(renderer, 0, 40, 200, 255);
		SDL_RenderClear(renderer);

	}


	printf("Waw t'es nul\n");
	return 0;
}
