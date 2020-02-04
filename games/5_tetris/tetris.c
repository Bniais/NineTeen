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
		 matrix[i][j] = 0;
}

void initPiece(Piece *piece){
	piece->rota = 0;
	piece->id = rand() % NB_PIECES;
	printf("%d\n", piece->id);
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

void getNewPiece(Piece *piece, int giant){

	piece->size = PIECE_SIZE * (giant ? RATIO_GIANT : 1);
	if(piece->giant != giant)
		piece->grille = realloc( piece->grille, piece->size * piece->size * sizeof(int));

	piece->giant = giant;
	piece->id = rand() % NB_PIECES;

	for(int i = 0; i < piece->size; i++)
		for(int j = 0; j < piece->size; j++)
			(piece->grille)[i * piece->size + j] = PIECES[piece->giant][piece->id][piece->rota][i][j];

	piece->firstCol = getFirstColumn(*piece);
	piece->lastCol = getLastColumn(*piece);
	piece->firstRow = getFirstRow(*piece);
	piece->lastRow = getLastRow(*piece);

	printf("col : %d %d , row : %d %d\n",piece->firstCol, piece->lastCol, piece->firstRow, piece->lastRow );

}

void putAtTop(Piece *piece){
	piece->x = (GRILLE_W - piece->size) / 2;
	piece->y = 1;
	piece->frameToGo = FRAME_TO_GO_SPAWN;
}

void drawPiece(SDL_Renderer *renderer, Piece piece){
	SDL_Rect pieceRect = {0, 0, CASE_SIZE, CASE_SIZE};
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	for(int i = 0; i < piece.size; i++)
		for(int j = 0; j < piece.size; j++)
			if((piece.grille)[i * piece.size + j]){
				pieceRect.x = (j + piece.x) * CASE_SIZE + 100;
				pieceRect.y = (i + piece.y) * CASE_SIZE + 100;
				SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
				SDL_RenderFillRect(renderer, &pieceRect);
				SDL_SetRenderDrawColor(renderer, 255, 5, 255, 255);
				SDL_RenderDrawRect(renderer, &pieceRect);
			}
}

int tooCloseFromWall(Piece piece){
	return (piece.x + piece.firstCol < 0 || piece.x + piece.lastCol > GRILLE_W - 1 || piece.y + piece.firstRow < 0 || piece.y + piece.lastRow > GRILLE_H - 1);
}

int tooCloseFromMatrix(Piece piece, int matrix[GRILLE_W][GRILLE_H]){
	//Pas besoin de verifier si i + (int)piece.x < GRILLE_W car on le vérifie dans tooCloseFromWall

	for(int i = piece.firstCol; i < piece.lastCol; i++)
		for(int j = piece.firstRow; j < piece.lastRow; j++)
			if( piece.grille[i * piece.size + j] && ( matrix[i + (int)piece.x][j + (int)piece.y] ) )
				return 1;

	if( piece.x != (int)piece.x && piece.y != (int)piece.y )
		for(int i = piece.firstRow; i < piece.lastRow; i++)
			for(int j = piece.firstCol; j < piece.lastCol; j++)
				if( piece.grille[i * piece.size + j] && ( matrix[i + (int)piece.x + 1][j + (int)piece.y + 1] ) ) //Pas besoin de verifier si (int)piece.y + 1 n'est pas en dehors de la matrice car tooCloseFromWall est appelé avant
					return 1;

	if( piece.x != (int)piece.x )
		for(int i = piece.firstRow; i < piece.lastRow; i++)
			for(int j = piece.firstCol; j < piece.lastCol; j++)
				if( piece.grille[i * piece.size + j] && ( matrix[i + (int)piece.x + 1][j + (int)piece.y] ) )
					return 1;

	if( piece.y != (int)piece.y )
		for(int i = piece.firstRow; i < piece.lastRow; i++)
			for(int j = piece.firstCol; j < piece.lastCol; j++)
				if( piece.grille[i * piece.size + j] && ( matrix[i + (int)piece.x][j + (int)piece.y + 1] ) )
					return 1;

	return 0;
}

void rotatePiece(Piece *piece, int rotateSens, int matrix[GRILLE_W][GRILLE_H]){
	if( rotateSens ){
		piece->rota += rotateSens;
		for(int i = 0; i < piece->size; i++)
			for(int j = 0; j < piece->size; j++)
				(piece->grille)[i * piece->size + j] = PIECES[piece->giant][piece->id][(piece->rota) % NB_ROTA][i][j];

		piece->firstCol = getFirstColumn(*piece);
		piece->lastCol = getLastColumn(*piece);
		piece->firstRow = getFirstRow(*piece);
		piece->lastRow = getLastRow(*piece);

		if( tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix)){
			printf("unrotate\n");
			piece->rota -= rotateSens;
			for(int i = 0; i < piece->size; i++)
				for(int j = 0; j < piece->size; j++)
					(piece->grille)[i * piece->size + j] = PIECES[piece->giant][piece->id][(piece->rota) % NB_ROTA][i][j];

			piece->firstCol = getFirstColumn(*piece);
			piece->lastCol = getLastColumn(*piece);
			piece->firstRow = getFirstRow(*piece);
			piece->lastRow = getLastRow(*piece);
		}
	}
}

void moveSide(Piece *piece, int matrix[GRILLE_W][GRILLE_H]){
	//Attention si on ajoute des cases dans la matrice en même temps qu'on bouge, on pourrait avoir des pièces qui s'arrêtent entre deux collones

	if( piece->dir ){
		piece->x += piece->dir * MOVE_LATERAL;
		(piece->frameDir)--;

		if(piece->frameDir%FRAME_LATERAL == 0)
			piece->x = roundf(piece->x);

		if(piece->frameDir == 0)
			piece->dir = NO_MOVE;

		if( tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix)){
			printf("too close\n");
			piece->x -= piece->dir * MOVE_LATERAL;
			piece->dir = NO_MOVE;
			piece->frameDir = 0;
		}

	}
}

void changeDir(Piece *piece, int lateralMove){
	if(lateralMove != NO_MOVE){
		if( piece->dir != lateralMove && piece->dir != NO_MOVE)
			piece->frameDir = FRAME_LATERAL - piece->frameDir;
		else if(piece->frameDir == 0)
			piece->frameDir += FRAME_LATERAL;

		piece->dir = lateralMove;
	}
}

void drawGrille(SDL_Renderer *renderer, int matrix[GRILLE_W][GRILLE_H]){
	SDL_Rect pieceRect = {0, 0, CASE_SIZE, CASE_SIZE};
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

	for(int i = 0; i < GRILLE_W; i++)
		for(int j = 0; j < GRILLE_H; j++){
			pieceRect.x = i * CASE_SIZE + 100;
			pieceRect.y = j * CASE_SIZE + 100;
			SDL_RenderDrawRect(renderer, &pieceRect);
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
	int accelerate = SDL_FALSE;
	int lateralMove = NO_MOVE;
	int rotate = SDL_FALSE;
	int rdyToRotate = SDL_TRUE;


	//pieces
	Piece currentPiece, nextPiece;

	//matrice
	int matrix[GRILLE_W][GRILLE_H];


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
	putAtTop(&currentPiece);

/////////////////////
/// BOUCLE DU JEU ///``
/////////////////////

	while( 1 ){

		// Init input
		SDL_GetMouseState(&(mouseCoor.x), &(mouseCoor.y));
		accelerate = SDL_FALSE;
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

				case SDL_MOUSEBUTTONDOWN:
					// gestion souris
					if( event.button.button == SDL_BUTTON_LEFT ){
						if( (SDL_GetWindowFlags(myWindow) | SDL_WINDOW_FULLSCREEN) != SDL_GetWindowFlags(myWindow)){
							SDL_SetWindowFullscreen( myWindow , SDL_TRUE);
						}
						else{
							SDL_SetWindowFullscreen( myWindow , SDL_FALSE);
						}
					}
					break;

				case SDL_KEYUP:
					if ( event.key.keysym.sym == SDLK_ESCAPE )
						rdyToPause = SDL_TRUE;
					else if( (event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_UP))
						rdyToRotate = SDL_TRUE;
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
			accelerate = SDL_TRUE;


		if( keystate[SDL_SCANCODE_RIGHT] )
			lateralMove = MOVE_RIGHT;

		else if( keystate[SDL_SCANCODE_LEFT] )
			lateralMove = MOVE_LEFT;


		if(rdyToRotate){
			if( keystate[SDL_SCANCODE_UP] ){
				rotate = 1;
				rdyToRotate = SDL_FALSE;
			}
			else if( keystate[SDL_SCANCODE_SPACE] ){
				rotate = -1;
				rdyToRotate = SDL_FALSE;
			}
		}


		//gérer utilisation bonus ?

	//////////////
	// Gameplay //`
	//////////////
		changeDir(&currentPiece, lateralMove);

		rotatePiece(&currentPiece, rotate, matrix);

		moveSide(&currentPiece, matrix);

		//moveDown(&currentPiece, accelerate, matrix);


	///////////////////
	// Check hitboxs //`
	///////////////////
		//checkLines(matrix);

	//////////
	// Draw //`
	//////////
		drawGrille(renderer, matrix);
		drawPiece(renderer, currentPiece);
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

		// On efface
		SDL_SetRenderDrawColor(renderer, 0, 40, 200, 255);
		SDL_RenderClear(renderer);
	}
	//printf("Waw t'es nul, %d\n", score);
	return 0;
}
