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


void initScore(Score * score){
	for(int i=0; i<GRILLE_H; i++){
		score[i].score = 0;
		score[i].frame = 0;
		score[i].scoreDest = 0;
		score[i].combo = 0;
		score[i].flat = 0;
		score[i].multi = 0;
	}
}

void updateDistances(float frame[NB_FRAMES], float distances[NB_DISTANCES], int *framePassed){
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

	int floorY = floor(piece.y);
	int floorX = floor(piece.x);

	for(i = piece.firstCol; i <= piece.lastCol; i++)
		for(j = piece.firstRow; j <= piece.lastRow; j++)
			if( piece.grille[j * piece.size + i] && ( i + floorX < 0 || matrix[i + floorX][j + floorY] != EMPTY ) )
				return 1;

	if( piece.x != roundf(piece.x) && piece.y != roundf(piece.y) ){
		for(i = piece.firstCol; i <= piece.lastCol; i++)
			for(j = piece.firstRow; j <= piece.lastRow; j++)
				if( piece.grille[j * piece.size + i] && ( i + floorX + 1 < 0 || matrix[i + floorX + 1][j + floorY + 1] != EMPTY ) ) //Pas besoin de verifier si (int)piece.y + 1 n'est pas en dehors de la matrice car tooCloseFromWall est appelé avant
					return 1;
	}


	if( piece.x != roundf(piece.x) ){
		for(i = piece.firstCol; i <= piece.lastCol; i++)
			for(j = piece.firstRow; j <= piece.lastRow; j++)
				if( piece.grille[j * piece.size + i] && ( i +floorX + 1 < 0 || matrix[i + floorX + 1][j + floorY] != EMPTY ) )
					return 1;
	}

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
	if(piece->giant != giant)
		piece->grille = realloc( piece->grille, piece->size * piece->size * sizeof(int));

	piece->giant = giant;
	piece->id = rand() % NB_PIECES;

	//handle bonus
	piece->bonus = FILL;//rand() % (NB_BONUSES+1);

	updateGrille(piece);

	getColRawInfos(piece);

}

int almostRound( float f ){
	return fabs(f - roundf(f)) < ROUNDABLE;
}

int putAtTop(Piece *piece,  int matrix[GRILLE_W][GRILLE_H], int frameToGo, int *remindRotate){
	piece->x = roundf((GRILLE_W - piece->size) / 2);
	piece->y = 0 - piece->firstRow;
	piece->frameToGo = frameToGo;
	piece->frameDir = 0;
	piece->dir = NO_MOVE;
	piece->frameStop = 0;
	*remindRotate = 0;
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

void drawPiece(SDL_Renderer *renderer,SDL_Texture* brickTexture, SDL_Texture *bonusTexture, Piece piece, int isNextPiece){
	int caseSize = CASE_SIZE;
	SDL_Point shiftCenterNext={0,0};
	if(isNextPiece && piece.giant)
		caseSize = CASE_SIZE / 2;

	if(isNextPiece){
		shiftCenterNext.x+=SHIFT_RIGHT_NEXT;
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

void rotatePiece(Piece *piece, int rotateSens, int matrix[GRILLE_W][GRILLE_H],int *remindRotate, int rotatePressed){
	if( rotateSens ){
		piece->rota += rotateSens;
		updateGrille(piece);
		getColRawInfos(piece);
		int closeWall = tooCloseFromWall(*piece);
		if( closeWall || tooCloseFromMatrix(*piece, matrix)){

			piece->rota -= rotateSens;
			updateGrille(piece);
			getColRawInfos(piece);
			//printf("close : %d %d\n",closeWall, rotateSens);
			if(closeWall && rotatePressed)
				*remindRotate = rotateSens * FRAME_REMIND_ROTATE;

		}
		else{
			*remindRotate = 0;
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

void completeLine(int matrix[GRILLE_W][GRILLE_H], int frameCompleteLine[GRILLE_H], int line, int bonusActivate[NB_BONUSES], int getBonuses, Score scoreAdd[GRILLE_H], int comboLine);

void useBonus(int bonusId, int frameLaser[GRILLE_H], int *framePassed, int nbUse, int matrix[GRILLE_W][GRILLE_H], int matrixFill[GRILLE_W][GRILLE_H]){
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
	}
}

void activateBonuses( int bonusActivate[NB_BONUSES],int frameLaser[GRILLE_H], int *framePassed, int matrix[GRILLE_W][GRILLE_H], int matrixFill[GRILLE_W][GRILLE_H] ){
	for(int i=0; i<NB_BONUSES; i++)
		if(bonusActivate[i])
			useBonus(i+1, frameLaser, framePassed, bonusActivate[i], matrix, matrixFill);
}

int getBonusId(int rectId){
	int bonusId = -1;

	while(rectId >= 10){
		rectId-=10;
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
					src.x = (getBonusId(matrix[i][j])) * BRICK_SRC.h;
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
					dest.x = SPAWN_FILL.x + ((col * CASE_SIZE + MATRIX.x) - SPAWN_FILL.x ) * (FRAME_FILL - matrixFill[col][row] + 1) / (float)(FRAME_DEPLACEMENT_FILL );
					dest.y = SPAWN_FILL.y + ((row * CASE_SIZE + MATRIX.y) - SPAWN_FILL.y ) * (FRAME_FILL - matrixFill[col][row] + 1) / (float)(FRAME_DEPLACEMENT_FILL );

					if(dest.x < INNER_RIGHT){
						if(INNER_RIGHT-dest.x<CASE_SIZE)
							dest.w=INNER_RIGHT-dest.x;
						else
							dest.w=BRICK_DEST.w;
						SDL_RenderCopy(renderer, brickTexture, &src, &dest);
						SDL_SetTextureColorMod(brickTexture, 255, 255, 255);
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

void completeLine(int matrix[GRILLE_W][GRILLE_H], int frameCompleteLine[GRILLE_H], int line, int bonusActivate[NB_BONUSES], int getBonuses, Score scoreAdd[GRILLE_H], int comboLine){
	int bonusGet = -1;
	if( frameCompleteLine[line] == -1 )
		frameCompleteLine[line] = FRAME_COMPLETE_LINE;

	if(comboLine){

		scoreAdd[line].score += scoreAdd[line-1].score  * RATIO_COMBO_LINE;
		printf("score after combo ! %d\n",scoreAdd[line].score );
		scoreAdd[line].combo ++;
	}
	else{

		scoreAdd[line].score += SCORE_BASE;
		printf("score after base ! %d\n",scoreAdd[line].score );
	}


	if(getBonuses){
		for(int i = 0; i < GRILLE_W; i++){
			if(matrix[i][line] >= BONUS_TRI ){
				bonusGet = getBonusId(matrix[i][line]);
				if(bonusGet == FLAT_POINT ){
					scoreAdd[line].score += NB_FLAT_POINT;
					printf("score after flat bonus ! %d\n",scoreAdd[line].score );
					scoreAdd[line].flat ++;
				}
				else if(bonusGet == MULTI_POINT){
					scoreAdd[line].score *= RATIO_MULTI_POINT;
					printf("score after MULTI_POINT bonus ! %d\n",scoreAdd[line].score );
					scoreAdd[line].multi ++;
				}
				else
					bonusActivate[bonusGet]++;
			}
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

	int comboLine = SDL_FALSE;
	int j;

	for(int i=0; i<GRILLE_H; i++){
		for(j=0; j<GRILLE_W && matrix[j][i] != EMPTY; j++);
		if(j == GRILLE_W){
			printf("combo : %d\n", comboLine );
			completeLine(matrix, frameCompleteLine, i, bonusActivate, getBonus, scoreAdd, comboLine);
			if(getBonus)
				comboLine++;
		}
		else
			comboLine = 0;
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

void updateScore(Score * scoreAffichage, Score scoreAdd[GRILLE_H]){
	for(int i=0; i<GRILLE_H; i++){
		if(scoreAdd[i].score){
			printf("\n score to add : %d\n", scoreAdd[i].score);
			printf("score dest before update : %d \n",scoreAffichage[i].scoreDest);
			scoreAffichage[i].scoreDest += scoreAdd[i].score;
			printf("score dest after update : %d \n",scoreAffichage[i].scoreDest);
			scoreAffichage[i].combo += scoreAdd[i].combo;
			scoreAffichage[i].flat += scoreAdd[i].flat;
			scoreAffichage[i].multi += scoreAdd[i].multi;
			printf("score before update : %d\n",scoreAffichage[i].score);
			if(scoreAffichage[i].frame == 0){
				scoreAffichage[i].score += scoreAdd[i].score;
				scoreAffichage[i].frame = SCORE_TTL;
			}
			else if(scoreAffichage[i].frame < RESET_ANIM){
				scoreAffichage[i].frame = RESET_ANIM;
			}
			printf("score after update :  %d\n\n",scoreAffichage[i].score);
		}
	}
	initScore(scoreAdd);
}

void clearScore(Score * scoreToClear){
	scoreToClear->score = 0;
	scoreToClear->scoreDest = 0;
	scoreToClear->combo = 0;
	scoreToClear->flat = 0;
	scoreToClear->multi = 0;
}

void updateFrames(int *framePassed, int frameLaser[GRILLE_H], int frameCompleteLine[GRILLE_H], int matrix[GRILLE_W][GRILLE_H], int matrixFill[GRILLE_W][GRILLE_H], int bonusActivate[NB_BONUSES], int *remindRotate, Score * scoreAffichage, Score * scoreAdd){
	(*framePassed)++;

	if(*remindRotate<0)
		(*remindRotate)++;
	else if(*remindRotate>0)
		(*remindRotate)--;

	for(int i=0; i<GRILLE_H; i++){
		if(frameLaser[i] == LASER_FRAME - LASER_START_COMPLETE)
			frameCompleteLine[i] = FRAME_COMPLETE_LINE + 1;

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

		if(frameCompleteLine[i] == 0){
			eraseLine(matrix, matrixFill, i, frameLaser, frameCompleteLine, scoreAffichage);
		}
	}
}

//score

int scoreSize(float score){
	int size = 0.15 + 3.65 * log(score);

	if( size < MIN_SIZE_SCORE )
		size = MIN_SIZE_SCORE;
	else if( size > MAX_SIZE_SCORE )
		size = MAX_SIZE_SCORE;

	return size;
}

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
		size = scoreSize(scoreAffichage.score);
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
	int remindRotate = 0;
	int rotatePressed;


	//frames and distances
	int totalFrame = 0;
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

	//score
	Score scoreAffichage[GRILLE_H];
	Score scoreAdd[GRILLE_H];

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

	SDL_Rect background_src = BACKGROUND_SRC;

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
	SDL_Texture *grilleHudTexture = IMG_LoadTexture(renderer, "./Textures/hud_grille.png");
	SDL_Texture *backgroundTexture = IMG_LoadTexture(renderer, "./Textures/background.png");
	SDL_Texture *scoreTexture = IMG_LoadTexture(renderer, "./Textures/chiffre.png");
	if( scoreTexture == NULL ){
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
	putAtTop(&currentPiece, matrix, (int)frame[TO_GO], &remindRotate);
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
		rotatePressed = SDL_FALSE;

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


		if( (keystate[SDL_SCANCODE_Q] && rdyToRotate[0]) || remindRotate>0 ){
			if(keystate[SDL_SCANCODE_Q] && rdyToRotate[0]){
				rotatePressed = SDL_TRUE;
				rdyToRotate[0] = SDL_FALSE;
			}

			rotate = 1;


		}
		else if( (keystate[SDL_SCANCODE_E] && rdyToRotate[1]) || remindRotate<0 ){
			if(keystate[SDL_SCANCODE_E] && rdyToRotate[1]){
				rotatePressed = SDL_TRUE;
				rdyToRotate[1] = SDL_FALSE;
			}

			rotate = -1;


		}
		//printf("Q %d E %d rdyQ %d rdyE %d\n", keystate[SDL_SCANCODE_Q],keystate[SDL_SCANCODE_E], rdyToRotate[0], rdyToRotate[1]);


		//gérer utilisation bonus ?

	//////////////
	// Gameplay //`
	//////////////

		rotatePiece(&currentPiece, rotate, matrix, &remindRotate, rotatePressed);


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
					checkLines(matrix, frameCompleteLine, bonusActivate, SDL_TRUE, scoreAdd);
					activateBonuses(bonusActivate, frameLaser, &framePassed, matrix, matrixFill);
					updateDistances(frame, distances, &framePassed);
					transfertNextPiece(&currentPiece,nextPiece);
					getNewPiece(&nextPiece, SDL_FALSE);
					if( putAtTop(&currentPiece, matrix, (int)frame[TO_GO], &remindRotate) == COULDNT_PUT )
						break;
				}


			}
		}
		else
			currentPiece.frameToGo--;

		updateScore(scoreAffichage, scoreAdd);
	///////////////////
	// Check hitboxs //`
	///////////////////

	//////////
	// Draw //`
	//////////


		SDL_RenderCopy(renderer, backgroundTexture, &background_src, &hudView);

		SDL_RenderCopy(renderer, grilleHudTexture, NULL, &HUD_GRILLE_DIM);
		drawMatrix(renderer, brickTexture, bonusTexture, matrix, frameCompleteLine, currentPiece);
		drawPiece(renderer, brickTexture, bonusTexture, currentPiece, SDL_FALSE);
		drawPiece(renderer, brickTexture, bonusTexture, nextPiece, SDL_TRUE);
		drawLaser(renderer, laserTexture, frameLaser);
		drawFill(renderer,brickTexture, matrixFill);
		//drawNextPiece(nextPiece);

		for(int i=0; i<GRILLE_H; i++)
			afficherScores(renderer, scoreTexture, scoreAffichage[i], i);

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
		updateFrames(&framePassed, frameLaser, frameCompleteLine, matrix, matrixFill, bonusActivate, &remindRotate, scoreAffichage, scoreAdd);
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
