#include "../../define/define.h"
#include "config.h"
#include <time.h>


#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_image.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

void initMatrix(int matrix[GRILLE_W][GRILLE_H], int init){
	for(int i=0; i<GRILLE_W; i++)
		for(int j=0; j<GRILLE_H; j++)
			matrix[i][j] = init;
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

void clearCombo(Score * scoreToClear){
	scoreToClear->flat = 0;
	scoreToClear->flatDest = 0;
	scoreToClear->combo = 1;
	scoreToClear->multi = 1;
	scoreToClear->frameCombo = 0;
	scoreToClear->sameColor = -1;
}

void clearScore(Score * scoreToClear){
	scoreToClear->score = 0;
	scoreToClear->scoreDest = 0;
	scoreToClear->frame = 0;
}

void initScore(Score * score){
	for(int i=0; i<GRILLE_H; i++){
		score[i].score = 0;
		score[i].scoreDest = 0;
		score[i].frame = 0;

		score[i].flat = 0;
		score[i].flatDest = 0;
		score[i].combo = 1;
		score[i].multi = 1;
		score[i].frameCombo = 0;
		score[i].sameColor = -1;
	}
}

void updateDistances(float frame[NB_FRAMES], float distances[NB_DISTANCES], int *framePassed, int *frameDestJauge, long int * frameTotalSpeed){
	*frameTotalSpeed += *framePassed;
	if(*frameTotalSpeed < 0)
		*frameTotalSpeed = 0;
	*framePassed = 0;

	for(int i=0; i<NB_FRAMES; i++){
		frame[i] = FRAME_MAX[i] * pow(GROW_RATE[i], *frameTotalSpeed);

		if(frame[i] < FRAME_MIN[i])
			frame[i] = FRAME_MIN[i];
		else if(frame[i] > FRAME_MAX[i])
			frame[i] = FRAME_MAX[i];
	}

	distances[DOWN] = 1. / (int)frame[DOWN];
	distances[LATERAL] = 1. / (int)frame[LATERAL];

	*frameDestJauge = FRAME_DEST_JAUGE;
}

int tooCloseFromMatrix(Piece piece, int matrix[GRILLE_W][GRILLE_H]){
	//Pas besoin de verifier si i + (int)piece.x < GRILLE_W car on le vérifie dans tooCloseFromWall
	int i;
	int j;

	int floorY = floor(piece.y);
	int floorX = floor(piece.x);

	//Pièce à son emplacement  bas gauche
	for(i = piece.firstCol; i <= piece.lastCol; i++)
		for(j = piece.firstRow; j <= piece.lastRow; j++)
			if( piece.grille[j * piece.size + i] && ( i + floorX < 0 || matrix[i + floorX][j + floorY] != EMPTY ) )
				return 1;

	//Pièce à son emplacement haut droite
	if( piece.x != roundf(piece.x) && piece.y != roundf(piece.y) ){
		for(i = piece.firstCol; i <= piece.lastCol; i++)
			for(j = piece.firstRow; j <= piece.lastRow; j++)
				if( piece.grille[j * piece.size + i] && ( i + floorX + 1 < 0 || matrix[i + floorX + 1][j + floorY + 1] != EMPTY ) ) //Pas besoin de verifier si (int)piece.y + 1 n'est pas en dehors de la matrice car tooCloseFromWall est appelé avant
					return 1;
	}

	//Pièce à son emplacement haut gauche
	if( piece.x != roundf(piece.x) ){
		for(i = piece.firstCol; i <= piece.lastCol; i++)
			for(j = piece.firstRow; j <= piece.lastRow; j++)
				if( piece.grille[j * piece.size + i] && ( i +floorX + 1 < 0 || matrix[i + floorX + 1][j + floorY] != EMPTY ) )
					return 1;
	}

	//Pièce à son emplacement bas droite
	if( piece.y != roundf(piece.y) ){
		for(i = piece.firstCol; i <= piece.lastCol; i++)
			for(j = piece.firstRow; j <= piece.lastRow; j++){
				if( piece.grille[j * piece.size + i] && ( i + floorX < 0 || matrix[i + floorX][j + floorY + 1] != EMPTY ) )
					return 1;
			}

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

	piece->grille = realloc( piece->grille, piece->size * piece->size * sizeof(int));

	if(giant)
		piece->giant = SDL_TRUE;
	else
		piece->giant = SDL_FALSE;

	piece->id = 0;// rand() % NB_PIECES;

	//handle bonus
	piece->bonus = FILL;//rand() % (NB_BONUSES+1);

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
		piece->frameToGo = 32000;
		return COULDNT_PUT;
	}

	return COULD_PUT;
}

void putAtNextPiece(Piece *piece){
	piece->x = GRILLE_W;
	piece->y = 0;
}

void drawPiece(SDL_Renderer *renderer,SDL_Texture* brickTexture, SDL_Texture *bonusTexture, Piece piece, int isNextPiece){
	int caseSize = CASE_SIZE;
	SDL_Point shiftCenterNext={0,0};
	if(isNextPiece && piece.giant)
		caseSize = CASE_SIZE / 2;

	if(isNextPiece){
		shiftCenterNext.x+=SHIFT_RIGHT_NEXT;
		shiftCenterNext.y+=SHIFT_TOP_NEXT;
		if((piece.lastCol-piece.firstCol)%2 == 0 || (piece.giant && ((piece.lastCol-piece.firstCol)/2)%2 == 0 ))
			shiftCenterNext.x -= CASE_SIZE / 2;

		if((piece.lastRow-piece.firstRow)%2 == 0)
			shiftCenterNext.y -= CASE_SIZE / 2;
		if((piece.lastRow-piece.firstRow) == 0)
			shiftCenterNext.y += CASE_SIZE;

		if(piece.giant)
			shiftCenterNext.x+=(piece.x) * caseSize;
	}

	SDL_Rect src = BRICK_SRC;
	SDL_Rect dest = BRICK_DEST;
	dest.w = caseSize;
	dest.h = caseSize;
	for(int i = 0; i < piece.size; i++)
		for(int j = 0; j < piece.size; j++)
			if((piece.grille)[i * piece.size + j]){
				src.x = piece.id * BRICK_SRC.w;
				dest.x = (j + piece.x) * caseSize + MATRIX.x + shiftCenterNext.x;
				dest.y = (i + piece.y) * caseSize + MATRIX.y + shiftCenterNext.y;
				SDL_RenderCopy(renderer, brickTexture, &src, &dest);
				if( (piece.grille)[i * piece.size + j] == 2 && piece.bonus  != NO_BONUS){
					src.x = (piece.bonus - 1) * BRICK_SRC.h;
					SDL_RenderCopy(renderer, bonusTexture, &src, &dest);
				}
			}
}

int tooCloseFromWall(Piece piece){
	return (piece.y + piece.firstRow < -ROUNDABLE || piece.x + piece.firstCol < -ROUNDABLE || piece.x + piece.lastCol > GRILLE_W - 1 || piece.y + piece.lastRow > GRILLE_H - 1);
}

int tryShift(int shiftInit, int maxShift, Piece * piece, float * pieceCoor, int sign, int matrix[GRILLE_W][GRILLE_H]){
	*pieceCoor += sign * shiftInit;
	int shift;

	for(shift=shiftInit; shift <= maxShift ; shift++, (*pieceCoor) += sign)
		if( !(tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix) ) )
			return SDL_TRUE;

	//revert
	piece->x -= sign * shift;
	return SDL_FALSE;
}

void rotatePiece(Piece *piece, int rotateSens, int matrix[GRILLE_W][GRILLE_H]){
	if( rotateSens ){

		RowColInfos originalInfos = {piece->firstCol, piece->lastCol, piece->firstRow, piece->lastRow};

		piece->rota += rotateSens;

		updateGrille(piece);
		getColRawInfos(piece);

		//try rotate shifts:
		int downShift=0, upShift = 0;
		int maxDownShift=originalInfos.firstRow - piece->firstRow,
			maxUpShift = piece->lastRow -  originalInfos.lastRow,
			maxLeftShift=piece->lastCol - originalInfos.lastCol,
			maxRightShift= originalInfos.firstCol - piece->firstCol;

		int found = SDL_FALSE;

		//try normal
		if( !(tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix) )  )
			found = SDL_TRUE;

		//left
		if(!found)
			found = tryShift(0, maxLeftShift, piece, &(piece->x), -1, matrix);

		//right
		if(!found)
			found = tryShift(0, maxRightShift, piece, &(piece->x), 1, matrix);

		//try down
		if(!found){
			piece->y++;
			for(downShift=1; downShift <= maxDownShift && !found ; downShift++, piece->y++){
				if( !(tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix) )  ){
					found = SDL_TRUE;
					break;
				}

				//down left
				if(!found)
					found = tryShift(1, maxLeftShift, piece, &(piece->x), -1, matrix);
				else
					break;

				//down right
				if(!found)
					found = tryShift(1, maxRightShift, piece, &(piece->x), 1, matrix);
				else
					break;

			}
			//revert down
			if(!found){
				piece->y -= downShift;
			}
		}

		//try up
		if(!found){
			piece->y--;
			for(upShift=1; upShift <= maxUpShift && !found ; upShift++, piece->y--){
				if( !(tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix) )  ){
					found = SDL_TRUE;
					break;
				}

				//up left
				if(!found)
					found = tryShift(1, maxLeftShift, piece, &(piece->x), -1, matrix);
				else
					break;

				//up right
				if(!found)
					found = tryShift(1, maxRightShift, piece, &(piece->x), 1, matrix);
				else
					break;

			}
		}
		if(!found){
			//revert up
			piece->y += upShift;

			//revert all
			piece->rota -= rotateSens;
			updateGrille(piece);
			getColRawInfos(piece);
		}
	}
}

void moveSide(Piece *piece, int matrix[GRILLE_W][GRILLE_H], float distanceLateral, int maxDown){
	if(maxDown && piece->frameToGo == 0){
		piece->x += piece->dir;
		if(piece->dir > 0)
			piece->x = floor(piece->x);
		else if(piece-> dir < 0)
			piece->x = ceil(piece->x);

		if( tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix)){
			piece->x -= piece->dir;
		}
		piece->dir = NO_MOVE;
		piece->frameDir = 0;
	}
	else if( piece->dir ){
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

int moveDown(Piece *piece, int accelerate, int matrix[GRILLE_W][GRILLE_H], float distanceDown, int frameStop, int maxDown){
	float distanceMoveDown = distanceDown * accelerate;
	if(distanceMoveDown >= 2)
		distanceMoveDown = 5/3;

	piece->y += distanceMoveDown;
	if(almostRound(piece->x))
		piece->x = roundf(piece->x);
	if(almostRound(piece->y))
		piece->y = roundf(piece->y);

	if( tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix)){
		piece->y = floor(piece->y);
		if( tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix)){
			piece->y -= floor(distanceMoveDown);
		}
		if(accelerate == ACCELERATE)
			piece->frameStop += FRAME_STOP_ACCELERATE;

		if(piece->frameStop <= frameStop)
			piece->frameStop ++;
		else
			return STOPPED;
	}
	else
		piece->frameStop = 0; //abusable ?

	if(maxDown){
		return moveDown(piece,  accelerate, matrix,  distanceDown,  frameStop, maxDown);
	}
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

int isOnArray(int n, int array[], int size){
	for(int i = 0; i < size; i++)
		if(array[i] == n)
			return 1;
	return 0;
}

int compareInt_i(const int * a,const int * b){
	return(*a - *b);
}

int compareInt(const void *a,const void* b){
	return compareInt_i(a, b);
}

void getFillPlaces(int matrix[GRILLE_W][GRILLE_H], int matrixFill[GRILLE_W][GRILLE_H], int nbFill){
	int nbEmpty = 0;
	int firstRow = GRILLE_H-1;
	int isEmpty = SDL_FALSE;
	int nbEmptyLastLine = 0;
	int nbEmptyLine = 0;

	for(int row = GRILLE_H-1; row>=0 && !isEmpty; row--){
		isEmpty = SDL_TRUE;
		nbEmptyLastLine=nbEmptyLine;
		nbEmptyLine = 0;
		for(int col = 0; col < GRILLE_W; col++){
			if(matrix[col][row] != EMPTY)
				isEmpty = SDL_FALSE;
			else{
				nbEmpty++;
				nbEmptyLine++;
			}
		}
		if(!isEmpty)
			firstRow = row+1;
	}

	nbEmpty -= nbEmptyLastLine;
	if(firstRow != 0)
		nbEmpty -= GRILLE_W; //car la dernière ligne scannée aura GRILLE_W cases vides

	//printf(" empty %d\n",nbEmpty);

	int toFill[nbFill];
	for(int i=0; i<nbFill; i++)
		toFill[i] = 0;

	int n;
	if(nbFill<nbEmpty){
		for(int i=0; i<nbFill; i++){
			do{
				n = rand()%nbEmpty;
			}while(isOnArray(n, toFill, i));
			toFill[i] = n;
		}
		qsort(toFill, nbFill, sizeof(int), compareInt);

	}
	else {
		for(int i=0; i<nbEmpty; i++)
			toFill[i] = i;

		for(int i=nbEmpty; i<nbFill; i++)
			toFill[i] = -1;

		nbFill=nbEmpty;
	}


	n=0;
	int iToFill = 0;
	isEmpty = SDL_FALSE;
	for(int row = GRILLE_H-1; row>=firstRow && !isEmpty && iToFill<nbFill; row--){
		isEmpty = SDL_TRUE;
		for(int col = 0; col < GRILLE_W && iToFill<nbFill; col++){
			if(matrix[col][row] == EMPTY){
				if(n == toFill[iToFill]){
					matrixFill[col][row] = FRAME_FILL + iToFill * ESPACEMENT_FRAME_FILL;
					iToFill++;
				}
				n++;
			}
			else{
				isEmpty = SDL_FALSE;
			}
		}
	}
}

void completeLine(int matrix[GRILLE_W][GRILLE_H], int frameCompleteLine[GRILLE_H], int line, int lastLine, int bonusActivate[NB_BONUSES], int getBonuses, Score scoreAdd[GRILLE_H], int comboLine);

void useBonus(int bonusId, int frameLaser[GRILLE_H], int *framePassed, int nbUse, int matrix[GRILLE_W][GRILLE_H], int matrixFill[GRILLE_W][GRILLE_H], int* nextIsGiant){
	switch (bonusId) {
		case FILL:
			getFillPlaces(matrix, matrixFill, nbUse*NB_FILL);
			break;

		case LASER:
			printf("laser\n");
			for(int i=GRILLE_H-1; i > GRILLE_H - (NB_LINES_LASER * nbUse) - 1 && i > 0 && i > GRILLE_H - MAX_HEIGHT_LASER -1; i--)
				frameLaser[i] = LASER_FRAME;
			break;

		case SLOW:
			printf("slow\n");
			*framePassed -= nbUse * SLOW_AMMOUNT;
			break;

		case SPEED:
			printf("speed\n");
			*framePassed += nbUse * SPEED_AMMOUNT;
			break;

		case GIANT:
			*nextIsGiant += nbUse;
			break;
	}
}

void activateBonuses( int bonusActivate[NB_BONUSES],int frameLaser[GRILLE_H], int *framePassed, int matrix[GRILLE_W][GRILLE_H], int matrixFill[GRILLE_W][GRILLE_H], int* nextIsGiant ){
	for(int i=0; i<NB_BONUSES; i++)
		if(bonusActivate[i])
			useBonus(i+1, frameLaser, framePassed, bonusActivate[i], matrix, matrixFill, nextIsGiant);
}

int getBonusId(int rectId){
	int bonusId = 0;

	while(rectId >= BONUS_TRI){
		rectId -= BONUS_TRI;
		bonusId++;
	}

	return bonusId;
}

void drawMatrix(SDL_Renderer *renderer, SDL_Texture *brickTexture, SDL_Texture *bonusTexture, int matrix[GRILLE_W][GRILLE_H], int frameCompleteLine[GRILLE_H], Piece currentPiece){
	SDL_Rect src = BRICK_SRC;
	SDL_Rect dest = BRICK_DEST;

	SDL_Rect col_piece = {(roundf(currentPiece.x) + currentPiece.firstCol) * CASE_SIZE + MATRIX.x, MATRIX.y, (currentPiece.lastCol-currentPiece.firstCol+1) * CASE_SIZE ,GRILLE_H * CASE_SIZE};

	SDL_SetRenderDrawColor(renderer, CORPS_GRILLE.r, CORPS_GRILLE.g, CORPS_GRILLE.b, CORPS_GRILLE.a);
	SDL_RenderFillRect(renderer, &MATRIX);
	SDL_SetRenderDrawColor(renderer, COL_PIECE.r, COL_PIECE.g, COL_PIECE.b, COL_PIECE.a);
	SDL_RenderFillRect(renderer, &col_piece);


	for(int i = 0; i < GRILLE_W; i++){
		for(int j = 0; j < GRILLE_H; j++){
			dest.x = i * CASE_SIZE + MATRIX.x;
			dest.y = j * CASE_SIZE + MATRIX.y;

			if(matrix[i][j] != EMPTY){ //piece
				src.x = getPieceId(matrix[i][j]) * BRICK_SRC.w;

				if(frameCompleteLine[j] >= 0){
					SDL_SetTextureColorMod(brickTexture, 255 * (frameCompleteLine[j]) / FRAME_COMPLETE_LINE , 255 * (frameCompleteLine[j]) / FRAME_COMPLETE_LINE, 255 * (frameCompleteLine[j]) / FRAME_COMPLETE_LINE);
				}
				else{
					SDL_SetTextureColorMod(brickTexture, 255, 255, 255);
				}

				SDL_RenderCopy(renderer, brickTexture, &src, &dest);

				if(matrix[i][j] >= 10){
					src.x = (getBonusId(matrix[i][j]) -1) * BRICK_SRC.w;
					SDL_RenderCopy(renderer, bonusTexture, &src, &dest);
				}

			}
			else{
				SDL_SetRenderDrawColor(renderer, CONTOUR_GRILLE.r, CONTOUR_GRILLE.g, CONTOUR_GRILLE.b, CONTOUR_GRILLE.a);
				SDL_RenderDrawRect(renderer, &dest);
			} //grille


		}
	}
	SDL_SetTextureColorMod(brickTexture, 255, 255, 255);
}

void drawLaser(SDL_Renderer *renderer, SDL_Texture *laserTexture, int frameLaser[GRILLE_H]){
	//laser
	SDL_Rect anim = LASER_DIM;
	SDL_Rect dest = LASER_DIM;
	for(int i=0; i<GRILLE_H; i++){

		if(frameLaser[i] >= 0){
			int frameDraw = LASER_START_SHOOT - frameLaser[i] ;
			if(frameDraw < 0 || frameDraw>DURATION_SHOOT)
				frameDraw = 0;
			if (frameLaser[i]>=LASER_ARRIVED){ // deplacement
				dest.x = SPAWN_LASER + (LASER_FRAME - frameLaser[i]) * (LASER_END_POS-SPAWN_LASER)/(LASER_FRAME-LASER_ARRIVED);
				frameDraw=0;
			}
			else if(frameLaser[i] >= LASER_RECOIL &&  frameLaser[i] < LASER_RECOIL + LASER_RECOIL_DURATION){ //recoil
				dest.x = LASER_END_POS - LASER_RECOIL_DIST[frameLaser[i]-LASER_RECOIL];
			}
			else if(frameLaser[i] < LASER_RETREAT){
				dest.x = LASER_END_POS + (LASER_RETREAT - frameLaser[i]) * (UNSPAWN_LASER - LASER_END_POS)/(LASER_RETREAT);
			}
			else
				dest.x = LASER_END_POS;

			dest.y = i * CASE_SIZE + MATRIX.y;

			anim.y = frameDraw * LASER_DIM.h;

			if(dest.x + LASER_WIDTH > INNER_LEFT){
				if(dest.x < INNER_LEFT){
					anim.w = dest.x -INNER_LEFT +LASER_WIDTH;
					anim.x = INNER_LEFT - dest.x;

					dest.w = dest.x + LASER_WIDTH-INNER_LEFT;
					dest.x = INNER_LEFT;
				}
				SDL_RenderCopy(renderer, laserTexture, &anim, &dest);
			}
		}
	}
	SDL_SetRenderDrawColor(renderer,255,255,255,255);
}

void drawFill(SDL_Renderer* renderer, SDL_Texture *brickTexture, int matrixFill[GRILLE_W][GRILLE_H]){
	SDL_Rect src = BRICK_SRC;
	SDL_Rect dest = BRICK_DEST;


	src.x = NB_PIECES * BRICK_SRC.w;

	for(int row = 0; row<GRILLE_H; row++){
		for(int col = 0; col < GRILLE_W; col++){
			if(matrixFill[col][row] > 0 ){

				if( matrixFill[col][row] <= FRAME_FILL-FRAME_DEPLACEMENT_FILL){//cas apparition
					//src.y=0;
					dest.x = col * CASE_SIZE + MATRIX.x;
					dest.y = row * CASE_SIZE + MATRIX.y;
					float ratioColor = (FRAME_FILL - matrixFill[col][row] -FRAME_DEPLACEMENT_FILL)/(float)(FRAME_FILL-FRAME_DEPLACEMENT_FILL);
					SDL_SetTextureColorMod(brickTexture, 255 *ratioColor, 255 * ratioColor, 255  * ratioColor);
					SDL_RenderCopy(renderer, brickTexture, &src, &dest);
					SDL_SetTextureColorMod(brickTexture, 255, 255, 255);

				}
				else if(matrixFill[col][row] <= FRAME_FILL){ //cas deplacement
					//src.y=BRICK_SRC.h;
					dest.x = SPAWN_FILL_X + ((col * CASE_SIZE + MATRIX.x) - SPAWN_FILL_X ) * (FRAME_FILL - matrixFill[col][row] + 1) / (float)(FRAME_DEPLACEMENT_FILL );
					dest.y = SPAWN_FILL_Y + ((row * CASE_SIZE + MATRIX.y) - SPAWN_FILL_Y ) * (FRAME_FILL - matrixFill[col][row] + 1) / (float)(FRAME_DEPLACEMENT_FILL );



					if(dest.x + CASE_SIZE > INNER_LEFT){
						if(dest.x < INNER_LEFT){
							dest.w = dest.x + CASE_SIZE-INNER_LEFT;
							dest.x = INNER_LEFT;
						}
						SDL_RenderCopy(renderer, brickTexture, &src, &dest);
					}
				}
			}
		}
	}



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

/*int isLineRainbow(int matrix[GRILLE_W][GRILLE_H],int line){
	int colorCheck[NB_PIECES];

	for (int i = 0; i< NB_PIECES; i++)
		colorCheck[i] = SDL_FALSE;

	for(int i = 0; i < GRILLE_W; i++)
		colorCheck[getPieceId(matrix[i][line])] = SDL_TRUE;

	for (int i = 0; i< NB_PIECES; i++)
		if(!colorCheck[i])
			return SDL_FALSE;

	return SDL_TRUE;
}*/

void completeLine(int matrix[GRILLE_W][GRILLE_H], int frameCompleteLine[GRILLE_H], int line, int lastLine, int bonusActivate[NB_BONUSES], int getBonuses, Score scoreAdd[GRILLE_H], int comboLine){
	int bonusGet = -1;
	if( frameCompleteLine[line] == -1 ){
		frameCompleteLine[line] = FRAME_COMPLETE_LINE;
		printf("c%d\n",line );
		if(comboLine){

			scoreAdd[line].score += scoreAdd[lastLine].score  * RATIO_COMBO_LINE;
			scoreAdd[line].combo = pow(2, comboLine);
		}
		else{

			scoreAdd[line].score += SCORE_BASE;
			//printf("score after base ! %d\n",scoreAdd[line].score );
		}

		if(getBonuses){
			int sameColor = getPieceId(matrix[0][line]);
			//int rainbow = isLineRainbow(matrix, line);
			int idPiece;
			for(int i = 0; i < GRILLE_W; i++){
				idPiece = getPieceId(matrix[i][line]);
				if(idPiece == NB_PIECES || (i > 0 && idPiece != getPieceId(matrix[i-1][line])) )
					sameColor = -1;

				if(matrix[i][line] >= BONUS_TRI ){
					//gestion bonus
					bonusGet = getBonusId(matrix[i][line]);

					if(bonusGet == FLAT_POINT )
						scoreAdd[line].flat ++;
					else if(bonusGet == MULTI_POINT)
						scoreAdd[line].multi *= RATIO_MULTI_POINT;
					else
						bonusActivate[bonusGet-1]++;

				}
			}

			if(sameColor != -1){
				scoreAdd[line].sameColor = sameColor;
				scoreAdd[line].score *= RATIO_SAME_COLOR;
			}
			/*else if(rainbow){
				scoreAdd[line].rainbow = SDL_TRUE;
				scoreAdd[line].score *= RATIO_RAINBOW;
			}*/

			scoreAdd[line].score *= scoreAdd[line].multi;
			//printf("score after MULTI_POINT bonus ! %d\n",scoreAdd[line].score );

			scoreAdd[line].score += scoreAdd[line].flat * NB_FLAT_POINT;
			//printf("score after flat bonus ! %d\n",scoreAdd[line].score );

		}
	}



}

void eraseLine(int matrix[GRILLE_W][GRILLE_H], int matrixFill[GRILLE_W][GRILLE_H], int line, int frameLaser[GRILLE_H], int frameCompleteLine[GRILLE_H], Score * scoreAffichage){
	for(int i = line; i > 0; i-- ){
		frameCompleteLine[i] = frameCompleteLine [i-1];
		for(int j=0; j<GRILLE_W; j++){
			matrix[j][i] = matrix[j][i-1];
			matrixFill[j][i] = matrixFill[j][i-1];
			//frameLaser[i] = frameLaser[i-1];
		}
	}



	for(int j=0; j<GRILLE_W; j++){
		matrix[j][0] = EMPTY;
		matrixFill[j][0] = 0;
	}


	//frameLaser[0] = -1;
	frameCompleteLine[0] = -1;

}

void checkLines(int matrix[GRILLE_W][GRILLE_H], int frameCompleteLine[GRILLE_H], int bonusActivate[NB_BONUSES], int getBonus, Score scoreAdd[GRILLE_H]){

	int comboLine = 0;
	int j;
	int lastLine = -1;

	for(int i=0; i<GRILLE_H; i++){
		for(j=0; j<GRILLE_W && matrix[j][i] != EMPTY; j++);
		if(j == GRILLE_W){
			completeLine(matrix, frameCompleteLine, i, lastLine, bonusActivate, getBonus, scoreAdd, comboLine);
			if(getBonus){
				lastLine = i;
				comboLine++;
			}


		}

	}

}

void clearIntTab( int *tab, int size){
	for(int i=0; i<size; i++)
		tab[i] = 0;
}

void transfertNextPiece(Piece *currentPiece, Piece nextPiece){
	currentPiece->size = PIECE_SIZE * (nextPiece.giant ? RATIO_GIANT : 1);
	if(currentPiece->giant != nextPiece.giant)
		currentPiece->grille = realloc( currentPiece->grille, currentPiece->size * currentPiece->size * sizeof(int));

	currentPiece->giant = nextPiece.giant;
	currentPiece->id = nextPiece.id;
	currentPiece->bonus = nextPiece.bonus;
	currentPiece->rota = 0;
	currentPiece->firstCol = nextPiece.firstCol;
	currentPiece->lastCol = nextPiece.lastCol;
	currentPiece->firstRow = nextPiece.firstRow;
	currentPiece->lastRow = nextPiece.lastRow;
	updateGrille(currentPiece);
}

void updateScore(Score * scoreAffichage, Score scoreAdd[GRILLE_H], ScoreTotal *scoreTotal){
	for(int i=0; i<GRILLE_H; i++){
		if(scoreAdd[i].score){
			scoreTotal->score += scoreAdd[i].score;
			scoreTotal->frameToDest = FRAME_DEST_SCORE_TOTAL;

			scoreAffichage[i].scoreDest += scoreAdd[i].score;
			scoreAffichage[i].flatDest += scoreAdd[i].flat;


			if(scoreAdd[i].combo>1)
				scoreAffichage[i].combo = scoreAdd[i].combo;

			if(scoreAdd[i].multi>1)
				scoreAffichage[i].multi = scoreAdd[i].multi;

			if(scoreAdd[i].sameColor != -1)
				scoreAffichage[i].sameColor = scoreAdd[i].sameColor;





			if(scoreAffichage[i].frame == 0){
				scoreAffichage[i].score += scoreAdd[i].score;
				scoreAffichage[i].frame = SCORE_TTL;
			}
			else if(scoreAffichage[i].frame < RESET_ANIM){
				scoreAffichage[i].frame = RESET_ANIM;
			}

			if((scoreAdd[i].combo > 1  ||  scoreAdd[i].multi>1 || scoreAdd[i].flat || scoreAdd[i].sameColor != -1 )){
				if(scoreAffichage[i].frameCombo == 0){
					scoreAffichage[i].flat += scoreAdd[i].flat;
					scoreAffichage[i].frameCombo = SCORE_TTL;
				}
				else if( scoreAffichage[i].frameCombo < RESET_ANIM){
					scoreAffichage[i].frameCombo = RESET_ANIM;
				}
			}
		}
	}
	initScore(scoreAdd);
}



int lineEmpty(int matrix[GRILLE_W][GRILLE_H], int line ){
	for(int i=0;i<GRILLE_W; i++)
		if(matrix[i][line] != EMPTY){
			printf("%d\n", matrix[i][line]);
			return 0;
		}
	printf("emty\n");
	return 1;
}

void updateFrames(int *framePassed, int frameLaser[GRILLE_H], int frameCompleteLine[GRILLE_H], int matrix[GRILLE_W][GRILLE_H], int matrixFill[GRILLE_W][GRILLE_H], int bonusActivate[NB_BONUSES], Score * scoreAffichage, Score * scoreAdd, ScoreTotal* scoreTotal,int *frameDestJauge,long int frameTotalSpeed,long int * frameTotalShow){
	(*framePassed)++;

	if(scoreTotal->frameToDest){
		scoreTotal->frameToDest--;
		if(scoreTotal->score != scoreTotal->scoreShow){
			scoreTotal->scoreShow += (scoreTotal->score-scoreTotal->scoreShow)/ (scoreTotal->frameToDest);
		}
	}

	for(int i=0; i<GRILLE_H; i++){
		if(frameLaser[i] == LASER_FRAME - LASER_START_COMPLETE){
			if(frameCompleteLine[i] == -1){
				frameCompleteLine[i] = FRAME_COMPLETE_LINE;
				if(!lineEmpty(matrix, i)){
					scoreAdd[i].score += SCORE_BASE;
				}
			}
		}


		if(frameLaser[i] >= 0)
			frameLaser[i]--;


		if(frameCompleteLine[i] >= 0)
			frameCompleteLine[i]--;

		for(int j=0; j<GRILLE_W; j++)
			if(matrixFill[j][i]){
				//printf("mf %d\n",matrixFill[j][i] );
				matrixFill[j][i]--;
				if(matrixFill[j][i] == 0){
					matrix[j][i] = NB_PIECES;
					checkLines(matrix, frameCompleteLine, bonusActivate, SDL_FALSE, scoreAdd);
				}
			}


		//score
		if(scoreAffichage[i].frame){
			scoreAffichage[i].frame--;
			if(!scoreAffichage[i].frame){
				clearScore(&(scoreAffichage[i]));
			}
			else if(scoreAffichage[i].frame > REACH_TARGET_SCORE){
				scoreAffichage[i].score += (scoreAffichage[i].scoreDest-scoreAffichage[i].score)/ (scoreAffichage[i].frame - REACH_TARGET_SCORE);
			}
		}

		if(scoreAffichage[i].frameCombo){
			scoreAffichage[i].frameCombo--;
			if(!scoreAffichage[i].frameCombo){
				clearCombo(&(scoreAffichage[i]));
			}
			else if(scoreAffichage[i].frameCombo > REACH_TARGET_SCORE){
				scoreAffichage[i].flat += (scoreAffichage[i].flatDest-scoreAffichage[i].flat)/ (scoreAffichage[i].frameCombo - REACH_TARGET_SCORE);
			}
		}

		if(frameCompleteLine[i] == 0){
			eraseLine(matrix, matrixFill, i, frameLaser, frameCompleteLine, scoreAffichage);
		}
	}

	if(*frameDestJauge){
		*frameTotalShow += (float)(frameTotalSpeed - *frameTotalShow )/ ( *frameDestJauge );
		(*frameDestJauge)--;
	}
}

//score

int len_num(int score)
{
	int count=1;

	while(score >= 10){
		score /=10;
		count++;
	}

	return count;
}

void afficherScores(SDL_Renderer *renderer , SDL_Texture *scoreTexture, Score scoreAffichage, int iScore)
{
	if(scoreAffichage.frame && scoreAffichage.frame<=SCORE_TTL){
		int len, size;
		SDL_Rect src, dest;


		len = len_num(scoreAffichage.score);
		src = SCORE_SRC;
		size = SIZE_SCORE;
		dest = (SDL_Rect){SCORE_DEST + (size*len)/2 , MATRIX_Y+iScore*CASE_SIZE + (FONT_HEIGHT_RATIO*size) / 2, size, FONT_HEIGHT_RATIO*size};

		dest.x -= dest.w/2;
		dest.y -= dest.h/2;
		if(dest.h < CASE_SIZE){
			dest.y += (CASE_SIZE-dest.h) / 2;
		}

		for(int j=0 ; j < len; j++)
		{
			src.x = SCORE_SRC.w * (scoreAffichage.score%10);
			SDL_SetTextureAlphaMod(scoreTexture, ALPHA_SCORE[SCORE_TTL -scoreAffichage.frame]);

			SDL_RenderCopy(renderer,scoreTexture,&src,&dest);

			scoreAffichage.score /=10;
			dest.x -= size;
		}
	}



}

void drawComboText(SDL_Renderer *renderer, char * msgCombo, TTF_Font * font, Score scoreAffichage, SDL_Rect * dest, SDL_Color comboColor, float size_score){

	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, msgCombo, comboColor);
	SDL_SetSurfaceAlphaMod(surfaceMessage, ALPHA_SCORE[SCORE_TTL -scoreAffichage.frameCombo]);
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	SDL_QueryTexture(Message,NULL,SDL_TEXTUREACCESS_STATIC,&(dest->w), &(dest->h) );
	dest->w /= (OPEN_FONT_SIZE / size_score);
	dest->h /= (OPEN_FONT_SIZE / size_score);
	dest->y += (CASE_SIZE - dest->h)/2;

	SDL_RenderCopy(renderer, Message, NULL, dest);
	dest->y -= (CASE_SIZE - dest->h)/2;
	dest->x += dest->w;

	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
}

float getDrawSize(SDL_Renderer *renderer, char * msgTotal, TTF_Font * font){

	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, msgTotal, (SDL_Color){255,255,255});
	SDL_Rect dest = {0,0,0,0};
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	float size_score = SIZE_COMBO+1;
	do{
		size_score--;
		SDL_QueryTexture(Message,NULL,SDL_TEXTUREACCESS_STATIC,&(dest.w), &(dest.h) );
		dest.w /= (OPEN_FONT_SIZE / size_score);
	}while(dest.w > SIZE_DRAW_COMBO);

	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);

	return size_score;
}

int intlog(double x, double base) {
    return (int)(log(x) / log(base));
}

void afficherScoreTotal(SDL_Renderer * renderer, TTF_Font *font, ScoreTotal score){

	char msgScore[MAX_APPEND_LENGHT];
	sprintf(msgScore, "%ld", score.score);
	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, msgScore, SCORE_TOTAL_COLOR);
	SDL_Rect dest = SCORE_TOTAL_DEST;
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	float size_score = SIZE_SCORE_TOTAL+1;
	do{
		size_score--;
		SDL_QueryTexture(Message,NULL,SDL_TEXTUREACCESS_STATIC,&(dest.w), &(dest.h) );
		dest.w /= (OPEN_FONT_SIZE / size_score);
	}while(dest.w > SIZE_DRAW_SCORE_TOTAL);


	dest.h /= (OPEN_FONT_SIZE / size_score);
	dest.y += (SIZE_SCORE_TOTAL - dest.h)/2;
	dest.x += (SIZE_DRAW_SCORE_TOTAL - dest.w)/2;

	SDL_RenderCopy(renderer, Message, NULL, &dest);

	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
}

void afficherCombo(SDL_Renderer *renderer, Score scoreAffichage, int line, TTF_Font *font ){

	if(scoreAffichage.frameCombo && scoreAffichage.frameCombo<=SCORE_TTL){
		char msgCombo[MAX_APPEND_LENGHT]= "";
		char msgFlat[MAX_APPEND_LENGHT]= "";
		char msgMulti[MAX_APPEND_LENGHT]= "" ;
		char msgColor[MAX_APPEND_LENGHT]= "";
		char msgTotal[4*MAX_APPEND_LENGHT] = "";
		SDL_Color comboColor;

		SDL_Rect dest = {COMBO_DRAW_X,MATRIX_Y + line * CASE_SIZE ,0,0};


		/*if(scoreAffichage.rainbow ){

			for(int i=0; i<NB_RAINBOW; i++){
				sprintf(msgCombo, "%c", RAINBOW_TEXT[i]);
				comboColor = RAINBOW_COLOR[i];
				drawComboText(renderer, msgCombo, font, scoreAffichage, &dest, comboColor);
			}
		}*/

		//creations messages
		if(scoreAffichage.sameColor != -1 )
			sprintf(msgColor, "Color!  x%d  ", RATIO_SAME_COLOR);

		if(scoreAffichage.combo > 1)
			sprintf(msgCombo, "Combo!  x%d  ",scoreAffichage.combo);

		if(scoreAffichage.multi > 1)
			sprintf(msgMulti, "x%d ",scoreAffichage.multi);

		if(scoreAffichage.flat)
			sprintf(msgFlat, "+ %d",scoreAffichage.flat * NB_FLAT_POINT);

		//calcul taille max

		strcat(msgTotal,msgColor);
		strcat(msgTotal,msgCombo);
		strcat(msgTotal,msgMulti);
		strcat(msgTotal,msgFlat);

		float size_score = getDrawSize(renderer, msgTotal, font);
		//drawComboText(renderer, msgTotal, font, scoreAffichage, &dest, (SDL_Color){255,255,255}, size_score);

		//affichage messages
		if(scoreAffichage.sameColor != -1 ){

			comboColor = BRICK_COLORS[scoreAffichage.sameColor];
			drawComboText(renderer, msgColor, font, scoreAffichage, &dest, comboColor, size_score);
		}
		if(scoreAffichage.combo>1){
			comboColor = COMBO_COLOR[ intlog(scoreAffichage.combo, RATIO_COMBO_LINE) - 1 ];
			drawComboText(renderer, msgCombo, font, scoreAffichage, &dest, comboColor, size_score);
		}
		if(scoreAffichage.multi > 1){
			comboColor = MULTI_COLOR[ intlog(scoreAffichage.multi, RATIO_MULTI_POINT) - 1 ];
			drawComboText(renderer, msgMulti, font, scoreAffichage, &dest, comboColor, size_score);
		}
		if(scoreAffichage.flat){
			comboColor = FLAT_COLOR[scoreAffichage.flat-1];
			drawComboText(renderer, msgFlat, font, scoreAffichage, &dest, comboColor, size_score);
		}


		/*if(strlen(msgCombo))
			printf("\n\ncombo phrase : %s\n", msgCombo);
		if(strlen(msgFlat))
			printf("combo phrase : %s\n", msgFlat);
		if(strlen(msgMulti))
			printf("combo phrase : %s\n", msgMulti);*/

	}

}

void drawJauge(SDL_Renderer * renderer, SDL_Texture * jaugeTexture, long int frameTotalShow){


	float ratioVitesse =  (float)frameTotalShow / TIME_TO_REACH_MAX ;
	if(ratioVitesse>1)
		ratioVitesse = 1;
	else if(ratioVitesse<0)
		ratioVitesse = 0;
	SDL_Rect jauge = JAUGE_SPEED_DEST;

	SDL_SetRenderDrawColor(renderer, JAUGE_COLOR_BACKGROUND.r, JAUGE_COLOR_BACKGROUND.g, JAUGE_COLOR_BACKGROUND.b, 255);
	SDL_RenderFillRect(renderer, &jauge);

	jauge.h *= ratioVitesse;
	jauge.y += JAUGE_SPEED_DEST.h - jauge.h;
	SDL_SetRenderDrawColor(renderer, JAUGE_COLOR.r, JAUGE_COLOR.g, JAUGE_COLOR.b, 255);
	SDL_RenderFillRect(renderer, &jauge);

	SDL_RenderCopy(renderer, jaugeTexture, &JAUGE_SPEED_SRC, &JAUGE_SPEED_DEST);
}

int linesInCompletion(int matrixFill[GRILLE_W][GRILLE_H], int frameLaser[GRILLE_H], int frameCompleteLine[GRILLE_H]){
	for(int line=0; line<GRILLE_H; line++){
	//	printf("fl %d\n",frameCompleteLine[line] );
		if(frameLaser[line] != -1 || frameCompleteLine[line] != -1){
			printf("fl %d\n",frameLaser[line] );
			return SDL_TRUE;
		}


		for(int col=0; col<GRILLE_W; col++){
			printf("ff %d\n",matrixFill[col][line]);
			if(matrixFill[col][line])
				return SDL_TRUE;
		}
	}
	printf("no line in c\n");
	return SDL_FALSE;
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
	int maxDown = SDL_FALSE;
	int lateralMove = NO_MOVE;
	int rotate = SDL_FALSE;
	int rdyToRotate[2] = {SDL_TRUE, SDL_TRUE};
	int rdyToSpace = SDL_TRUE;
	int cantMoveSide = SDL_FALSE;
	int nextIsGiant = 0;

	int doGameplay = SDL_TRUE;
	int waitToPlace = SDL_FALSE;

	//frames and distances
	int totalFrame = 0;
	int framePassed = 0;

	long int frameTotalSpeed = TIME_START;
	long int frameTotalShow = frameTotalSpeed;//used to show smooth jauge
	int frameDestJauge = 0;

	float frame[NB_FRAMES];
	frame[LATERAL] = FRAME_START[LATERAL];
	frame[DOWN] = FRAME_START[DOWN];
	frame[TO_GO] = FRAME_START[TO_GO];
	frame[STOP] = FRAME_START[STOP];
	float distances[NB_DISTANCES];




	updateDistances(frame, distances, &framePassed, &frameDestJauge, &frameTotalSpeed);

	//Bonuses
	int bonusActivate[NB_BONUSES];
	int frameLaser[GRILLE_H];
	for(int i=0; i < GRILLE_H; i++)
		frameLaser[i] = -1;


	//pieces
	Piece currentPiece, nextPiece;

	//score
	Score scoreAffichage[GRILLE_H];
	Score scoreAdd[GRILLE_H];
	ScoreTotal score = {0,0,0};

	//matrice
	int matrix[GRILLE_W][GRILLE_H];
	int matrixFill[GRILLE_W][GRILLE_H];
	int frameCompleteLine[GRILLE_H];
	for(int i=0; i < GRILLE_H; i++)
		frameCompleteLine[i] = -1;

	//Keyboard
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);

	//Time
	unsigned int lastTime = 0, currentTime;

	//Fonts
	TTF_Font* comboFont = TTF_OpenFont("./Fonts/zorque.ttf", OPEN_FONT_SIZE);
	if( comboFont == NULL ){
		printf("TTF_OpenFont() Failed: %s\n", TTF_GetError());
		return EXIT_FAILURE;
	}

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

	SDL_Rect background_src = BACKGROUND_SRC;

	//hud and menus
	int rdyToPause = SDL_TRUE;


	//mouse
	SDL_Point mouseCoor;

	//Textures
	SDL_Texture *hudTexture = IMG_LoadTexture(renderer, DIR_HUD);
	SDL_Texture *laserTexture = IMG_LoadTexture(renderer, "./Textures/laserAnim.png");
	SDL_Texture *brickTexture = IMG_LoadTexture(renderer, "./Textures/bricks.png");
	SDL_Texture *bonusTexture = IMG_LoadTexture(renderer, "./Textures/bonus.png");
	SDL_Texture *grilleHudTexture = IMG_LoadTexture(renderer, "./Textures/hud_grille.png");
	SDL_Texture *backgroundTexture = IMG_LoadTexture(renderer, "./Textures/background.png");
	SDL_Texture *scoreTexture = IMG_LoadTexture(renderer, "./Textures/chiffre.png");
	SDL_Texture *jaugeTexture = IMG_LoadTexture(renderer, "./Textures/speedJauge.png");
	if( jaugeTexture == NULL ){
		printf("Erreur lors de la creation de texture%s", SDL_GetError());
		return EXIT_FAILURE;
	}

	///////////////////////
	/// Initialize vars ///`
	///////////////////////
	initScore(scoreAffichage);
	initScore(scoreAdd);
	initMatrix(matrix, EMPTY);
	initMatrix(matrixFill, 0);
	initPiece(&currentPiece);
	initPiece(&nextPiece);
	getNewPiece(&currentPiece, SDL_FALSE);
	getNewPiece(&nextPiece, SDL_FALSE);
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
		maxDown = SDL_FALSE;

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
					else if( event.key.keysym.sym == SDLK_SPACE)
						rdyToSpace = SDL_TRUE;
					break;

				case SDL_KEYDOWN:
					if ( event.key.keysym.sym == SDLK_ESCAPE && rdyToPause ){
						doGameplay = !doGameplay;
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


		if( keystate[SDL_SCANCODE_SPACE] && rdyToSpace){
			maxDown = SDL_TRUE;
			rdyToSpace = SDL_FALSE;
		}

		if( keystate[SDL_SCANCODE_RIGHT] )
			lateralMove = MOVE_RIGHT;

		else if( keystate[SDL_SCANCODE_LEFT] )
			lateralMove = MOVE_LEFT;


		if( (keystate[SDL_SCANCODE_Q] && rdyToRotate[0]) ){
			rdyToRotate[0] = SDL_FALSE;
			rotate = 1;
		}
		else if( (keystate[SDL_SCANCODE_E] && rdyToRotate[1])){
			rdyToRotate[1] = SDL_FALSE;
			rotate = -1;
		}


		//printf("Q %d E %d rdyQ %d rdyE %d\n", keystate[SDL_SCANCODE_Q],keystate[SDL_SCANCODE_E], rdyToRotate[0], rdyToRotate[1]);


		//gérer utilisation bonus ?

	//////////////
	// Gameplay //`
	//////////////
		if(doGameplay && !waitToPlace){
			//rotate normal then left down right
			rotatePiece(&currentPiece, rotate, matrix);



			if(!cantMoveSide)
				changeDir(&currentPiece, lateralMove, (int)frame[LATERAL]);

			moveSide(&currentPiece, matrix, distances[LATERAL], maxDown);


			cantMoveSide = SDL_FALSE;
			if(!currentPiece.frameToGo){
				if( moveDown(&currentPiece, accelerate, matrix, distances[DOWN], (int)frame[STOP], maxDown) == STOPPED){
					cantMoveSide = SDL_TRUE;
					if(currentPiece.frameDir == 0){
						//Piece is saved and remplaced
						savePiece(currentPiece, matrix);
						clearIntTab(bonusActivate, NB_BONUSES);
						checkLines(matrix, frameCompleteLine, bonusActivate, SDL_TRUE, scoreAdd);
						activateBonuses(bonusActivate, frameLaser, &framePassed, matrix, matrixFill, &nextIsGiant);
						updateDistances(frame, distances, &framePassed, &frameDestJauge, &frameTotalSpeed);
						transfertNextPiece(&currentPiece,nextPiece);

						getNewPiece(&nextPiece, nextIsGiant);
						if(nextIsGiant)
							nextIsGiant--;

						if( putAtTop(&currentPiece, matrix, (int)frame[TO_GO]) == COULDNT_PUT ){
							if(linesInCompletion(matrixFill, frameLaser, frameCompleteLine))
								waitToPlace = SDL_TRUE;
							else
								break;//lose
						}
					}
				}
			}
			else
				currentPiece.frameToGo--;
		}
		else if(waitToPlace){
			if(putAtTop(&currentPiece, matrix, 2*(int)frame[TO_GO]) != COULDNT_PUT){
				waitToPlace = SDL_FALSE;
			}
			else if(!linesInCompletion(matrixFill, frameLaser, frameCompleteLine)){
				break;
			}
		}
		updateScore(scoreAffichage, scoreAdd, &score);
	///////////////////
	// Check hitboxs //`
	///////////////////

	//////////
	// Draw //`
	//////////


		SDL_RenderCopy(renderer, backgroundTexture, &background_src, &hudView);

		SDL_RenderCopy(renderer, grilleHudTexture, NULL, &HUD_GRILLE_DIM);
		drawJauge(renderer, jaugeTexture, frameTotalShow);
		drawMatrix(renderer, brickTexture, bonusTexture, matrix, frameCompleteLine, currentPiece);
		drawPiece(renderer, brickTexture, bonusTexture, currentPiece, SDL_FALSE);
		drawPiece(renderer, brickTexture, bonusTexture, nextPiece, SDL_TRUE);
		drawLaser(renderer, laserTexture, frameLaser);
		drawFill(renderer,brickTexture, matrixFill);



		//SDL_RenderFillRect(renderer, &SCORE_TOTAL_DEST);
		afficherScoreTotal(renderer,comboFont, score);
		//drawNextPiece(nextPiece);

		for(int i=0; i<GRILLE_H; i++){
			afficherScores(renderer, scoreTexture, scoreAffichage[i], i);
			afficherCombo(renderer,  scoreAffichage[i], i, comboFont);
		}


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
		if(doGameplay){
			updateFrames(&framePassed, frameLaser, frameCompleteLine, matrix, matrixFill, bonusActivate, scoreAffichage, scoreAdd, &score, &frameDestJauge, frameTotalSpeed, &frameTotalShow);
		}

		totalFrame++;
		background_src.x = BACKGROUND_SRC.w * ((totalFrame/3)%BACKGROUND_COL);
		background_src.y = BACKGROUND_SRC.h * ((totalFrame/(3*BACKGROUND_COL))%BACKGROUND_ROW);

		// On efface
		SDL_SetRenderDrawColor(renderer, 0, 40, 200, 255);
		SDL_RenderClear(renderer);

	}


	printf("Waw t'es nul\n");
	return 0;
}
