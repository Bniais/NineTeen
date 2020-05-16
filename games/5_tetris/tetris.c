/**
*\file tetris.c
*\brief Jeu tetris avec bonus
*\author Hugo Lecomte
*\version 1.0
*\date 03/04/2020
*/

#include "../../include/communFunctions.h"
#include "../../include/define.h"
#include "../../include/hashage.h"
#include "../../include/libWeb.h"
#include "config.h"
#include <time.h>


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


/**
*\fn void myInit()
*\brief Initialise l'environement SDL, TTF et rand
*/
void myInit(){
	//rand
	srand(time(NULL));
	Mix_Volume(5, 10);//rotate
	Mix_Volume(6, 30);//fill
	Mix_Volume(7, 25);//hit
	Mix_Volume(8, 30);//line
	Mix_Volume(9, 30);//laser
	Mix_Volume(10, 128);//gameover

}

/**
*\fn void initMatrix(int matrix[GRILLE_W][GRILLE_H], int init)
*\brief Initialise les cases de la matrice
*\param matrix La matrice à initialiser
*\param init La valeur d'initialisation
*/
void initMatrix(int matrix[GRILLE_W][GRILLE_H], int init){
	for(int i=0; i<GRILLE_W; i++)
		for(int j=0; j<GRILLE_H; j++)
			matrix[i][j] = init;
}

/**
*\fn void initPiece(Piece *piece)
*\brief Initialise une pièce
*\param piece La pièce à initialiser
*/
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

/**
*\fn void clearCombo(Score * scoreToClear)
*\brief Efface les attributs combo d'un score
*\param scoreToClear Le score à effacer
*/
void clearCombo(Score * scoreToClear){
	scoreToClear->flat = 0;
	scoreToClear->flatDest = 0;
	scoreToClear->combo = 1;
	scoreToClear->multi = 1;
	scoreToClear->frameCombo = 0;
	scoreToClear->sameColor = -1;
}

/**
*\fn void clearScore(Score * scoreToClear)
*\brief Efface les attributs score d'un score
*\param scoreToClear Le score à effacer
*/
void clearScore(Score * scoreToClear){
	scoreToClear->score = 0;
	scoreToClear->scoreDest = 0;
	scoreToClear->frame = 0;
}

/**
*\fn void initScore(Score * score)
*\brief Initialise un tableau de scores
*\param score Le tableau de scores à initialiser
*/
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

/**
*\fn void updateDistances(float frame[NB_FRAMES], float distances[NB_DISTANCES], int *framePassed, int *frameDestJauge, long int * frameTotalSpeed)
*\brief Met à jour les distances et frames de déplacement, d'arrêt et de départ des pièces selon le nombre de frames qui se sont passées depuis le dernier appel
*\param frame Les frames de déplacement, d'arrêt et de départ
*\param distances Les distances de déplacement
*\param framePassed Le nombre de frame depuis le dernier appel
*\param frameDestJauge Les frames pour l'animation d'augmentation de la jauge de vitesse
*\param frameTotalSpeed Le nombre de frame théorique (avec bonus) depuis le début (pour la jauge)
*/
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

/**
*\fn int tooCloseFromMatrix(Piece piece, int matrix[GRILLE_W][GRILLE_H])
*\brief Teste si une pièce est en collision avec les pièces déjà inscrites dans la grille
*\param piece La pièce à tester
*\param matrix La grille
*\return Vrai si la piece à une collision avec les éléments de la grille, sinon faux
*/
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
				return SDL_TRUE;

	//Pièce à son emplacement haut droite
	if( piece.x != roundf(piece.x) && piece.y != roundf(piece.y) ){
		for(i = piece.firstCol; i <= piece.lastCol; i++)
			for(j = piece.firstRow; j <= piece.lastRow; j++)
				if( piece.grille[j * piece.size + i] && ( i + floorX + 1 < 0 || matrix[i + floorX + 1][j + floorY + 1] != EMPTY ) ) //Pas besoin de verifier si (int)piece.y + 1 n'est pas en dehors de la matrice car tooCloseFromWall est appelé avant
					return SDL_TRUE;
	}

	//Pièce à son emplacement haut gauche
	if( piece.x != roundf(piece.x) ){
		for(i = piece.firstCol; i <= piece.lastCol; i++)
			for(j = piece.firstRow; j <= piece.lastRow; j++)
				if( piece.grille[j * piece.size + i] && ( i +floorX + 1 < 0 || matrix[i + floorX + 1][j + floorY] != EMPTY ) )
					return SDL_TRUE;
	}

	//Pièce à son emplacement bas droite
	if( piece.y != roundf(piece.y) ){
		for(i = piece.firstCol; i <= piece.lastCol; i++)
			for(j = piece.firstRow; j <= piece.lastRow; j++){
				if( piece.grille[j * piece.size + i] && ( i + floorX < 0 || matrix[i + floorX][j + floorY + 1] != EMPTY ) )
					return SDL_TRUE;
			}

	}

	return SDL_FALSE;
}

/**
*\fn int getFirstColumn(Piece piece)
*\brief Calcule l'indice de la première collone d'une pièce
*\param piece La pièce
*\return L'indice de la première collone ou -1 en cas d'erreur
*/
int getFirstColumn(Piece piece){
	for(int j = 0; j < piece.size; j++)
		for(int i = 0; i < piece.size; i++)
			if(piece.grille[i * piece.size + j])
				return j;

	return -1;
}

/**
*\fn int getLastColumn(Piece piece)
*\brief Calcule l'indice de la dernière collone d'une pièce
*\param piece La pièce
*\return L'indice de la dernière collone ou -1 en cas d'erreur
*/
int getLastColumn(Piece piece){
	for(int j = piece.size - 1; j >= 0; j--)
		for(int i = 0; i < piece.size; i++)
			if(piece.grille[i * piece.size + j])
				return j;

	return -1;
}

/**
*\fn int getFirstRow(Piece piece)
*\brief Calcule l'indice de la première ligne d'une pièce
*\param piece La pièce
*\return L'indice de la première ligne ou -1 en cas d'erreur
*/
int getFirstRow(Piece piece){
	for(int j = 0; j < piece.size; j++)
		for(int i = 0; i < piece.size; i++)
			if(piece.grille[j * piece.size + i])
				return j;

	return -1;
}

/**
*\fn int getLastRow(Piece piece)
*\brief Calcule l'indice de la dernière ligne d'une pièce
*\param piece La pièce
*\return L'indice de la dernière ligne ou -1 en cas d'erreur
*/
int getLastRow(Piece piece){
	for(int j = piece.size - 1; j >= 0; j--)
		for(int i = 0; i < piece.size; i++)
			if(piece.grille[j * piece.size + i])
				return j;

	return -1;
}

/**
*\fn void getColRawInfos(Piece *piece)
*\brief Met à jour les informations de premières et dernière ligne et collone d'une pièce
*\param piece La pièce
*/
void getColRawInfos(Piece *piece){
	piece->firstCol = getFirstColumn(*piece);
	piece->lastCol = getLastColumn(*piece);
	piece->firstRow = getFirstRow(*piece);
	piece->lastRow = getLastRow(*piece);
}

/**
*\fn void updateGrille(Piece *piece, int hardcore)
*\brief Met à jour les informations de la grille de la pièce selon son id, sa rotation et le fait qu'elle soit géante ou non
*\param piece La pièce
*\param hardcore Indique si le mode de jeu est facile ou difficile
*/
void updateGrille(Piece *piece, int hardcore){
	for(int i = 0; i < piece->size; i++)
		for(int j = 0; j < piece->size; j++)
			(piece->grille)[i * piece->size + j] = PIECES[hardcore][piece->giant][piece->id][(piece->rota) % NB_ROTA][i][j];
}

/**
*\fn void getNewPiece(Piece *piece, int giant, int hardcore)
*\brief Obtient une nouvelle pièce aléatoire
*\param piece La pièce où stocker cette nouvelle pièce
*\param giant Indique si cette nouvelle pièce sera géante ou non
*\param hardcore Indique si le mode de jeu est facile ou difficile
*/
void getNewPiece(Piece *piece, int giant, int hardcore){
	piece->rota = 0;
	piece->size = PIECE_SIZE * (giant ? RATIO_GIANT : 1);

	piece->grille = realloc( piece->grille, piece->size * piece->size * sizeof(int));

	if(giant)
		piece->giant = SDL_TRUE;
	else
		piece->giant = SDL_FALSE;

	piece->id = rand() % NB_PIECES;

	//handle bonus
	if(rand()%PROBA_BONUS == 0)
		piece->bonus = rand() % (NB_BONUSES+1);
	else
		piece->bonus = NO_BONUS;

	updateGrille(piece, hardcore);

	getColRawInfos(piece);

}

/**
*\fn int almostRound( float f )
*\brief Determine si un float est proche d'un entier ou non
*\param f Le float
*\return Vrai si il est proche d'un entier, sinon faux
*/
int almostRound( float f ){
	return fabs(f - roundf(f)) < ROUNDABLE;
}

/**
*\fn int putAtTop(Piece *piece,  int matrix[GRILLE_W][GRILLE_H], int frameToGo, int hardcore)
*\brief Met une pièce en haut de la grille en vérifiant que c'est possible
*\param piece La pièce à mettre en haut
*\param matrix La grille
*\param frameToGo Le nombre de frame avant que la pièce commence à tomber
*\param hardcore Le mode de jeu
*\return Vrai si la pièce a pu être placée, sinon faux
*/
int putAtTop(Piece *piece,  int matrix[GRILLE_W][GRILLE_H], int frameToGo, int hardcore){
	piece->x = roundf((GRILLE_W - piece->size) / 2);
	 //Ajustements
	if(!hardcore && piece->id <= 1 )
		(piece->x) ++;
	else if(hardcore && (piece->id == 2 || piece->id == 3) )
	(piece->x) --;

	piece->y = 0 - piece->firstRow;
	piece->frameToGo = frameToGo;
	piece->frameDir = 0;
	piece->dir = NO_MOVE;
	piece->frameStop = 0;
	if(tooCloseFromMatrix(*piece, matrix)){
		piece->x = UNDEFINED.x;
		piece->y = UNDEFINED.y;
		piece->frameToGo = 32000;
		return SDL_FALSE;
	}

	return SDL_TRUE;
}

/**
*\fn void putAtNextPiece(Piece *piece)
*\brief Met une pièce dans l'emplacement qui indique la prochaine pièce
*\param piece La pièce à mettre dans cet emplacement
*/
void putAtNextPiece(Piece *piece){
	piece->x = GRILLE_W;
	piece->y = 0;
}

/**
*\fn void drawPiece(SDL_Renderer *renderer, Piece piece, SDL_Texture *brickTexture, SDL_Texture *bonusTexture, int isNextPiece, int hardcore)
*\brief Dessine une pièce (en train de tomber ou à venir)
*\param renderer Le renderer où dessiner
*\param piece La pièce à dessiner
*\param brickTexture La texture des briques des pièces
*\param bonusTexture La texture des bonus
*\param isNextPiece Indique si c'est une pièce à venir ou qui est en train de tomber
*\param hardcore La difficulté de la partie
*/
void drawPiece(SDL_Renderer *renderer, Piece piece, SDL_Texture *brickTexture, SDL_Texture *bonusTexture, int isNextPiece, int hardcore){
	int caseSize = CASE_SIZE;
	SDL_Point shiftCenterNext={0,0};


	int largeur = piece.lastCol - piece.firstCol + 1;
	int hauteur = piece.lastRow - piece.firstRow + 1;

	if(isNextPiece){
		if(piece.giant)
			caseSize = CASE_SIZE / 2;

		if((!piece.giant && largeur>4)  || (piece.giant && largeur >8) )
			caseSize *= 0.8;

		//Placer en haut à gauche+
		shiftCenterNext.x+=SHIFT_RIGHT_NEXT - piece.firstCol*caseSize;
		shiftCenterNext.y+=SHIFT_TOP_NEXT- piece.firstRow*caseSize;

		//puis centrer
		float piece_size = PIECE_SIZE;
		if(piece.giant)
			piece_size*= RATIO_GIANT;

		//Ajustements
		if(!hardcore){
			if(piece.id == 0 ){
				shiftCenterNext.x += caseSize/4.;
				if(piece.giant){
					shiftCenterNext.x += caseSize/4.;
				}
			}
			if(piece.id == 1 ){
				shiftCenterNext.x -= caseSize/4.;
				if(piece.giant )
					shiftCenterNext.x -= caseSize/4.;
			}

			shiftCenterNext.y +=(piece.giant +1) * caseSize/4.;
		}


		shiftCenterNext.x+= (float)caseSize/2 * (float)(piece_size-largeur)/2;
		shiftCenterNext.y+=  (float)caseSize/2 * (float)(piece_size-hauteur)/2;
		if(piece.id == 1 && hardcore)
			shiftCenterNext.y+=  (float)caseSize/2 * (float)(piece_size-hauteur)/2;

	}

	//dessiner
	SDL_Rect src = BRICK_SRC;
	SDL_Rect dest = BRICK_DEST;
	dest.w = caseSize;
	dest.h = caseSize;
	for(int i = 0; i < piece.size; i++)
		for(int j = 0; j < piece.size; j++)
			if((piece.grille)[i * piece.size + j]){
				src.x = piece.id * BRICK_SRC.w;
				dest.x =piece.x*CASE_SIZE+ j * caseSize + MATRIX.x + shiftCenterNext.x;
				dest.y = (i + piece.y) * caseSize + MATRIX.y + shiftCenterNext.y;
				SDL_RenderCopy(renderer, brickTexture, &src, &dest);
				if( (piece.grille)[i * piece.size + j] == 2 && piece.bonus  != NO_BONUS){
					src.x = (piece.bonus - 1) * BRICK_SRC.h;
					SDL_RenderCopy(renderer, bonusTexture, &src, &dest);
				}
			}
}


/**
*\fn int tooCloseFromWall(Piece piece)
*\brief Détermine si une pièce dépasse des bords de la grille
*\param piece La pièce
*\return Vrai si elle dépasse sinon faux
*/
int tooCloseFromWall(Piece piece){
	return (piece.y + piece.firstRow < 0 || piece.x + piece.firstCol < 0 || piece.x + piece.lastCol > GRILLE_W - 1 || piece.y + piece.lastRow > GRILLE_H - 1);
}


/**
*\fn int tryShift(int shiftInit, int maxShift, Piece * piece, float * pieceCoor, int sign, int matrix[GRILLE_W][GRILLE_H])
*\brief Essaye de déplacer une pièce en vérifiant si elle est dans un emplacement valide
*\param shiftInit Le décallage de base (minimum)
*\param maxShift Le décallage maximum
*\param piece La pièce à tester
*\param pieceCoor La coordonnée (x ou y) de la pièce à tester
*\param sign Le sens de décallage
*\param matrix La matrice pour tester la validité de la pièce
*\return Vrai si un emplacement valide a été trouvé sinon faux
*/
int tryShift(int shiftInit, int maxShift, Piece * piece, float * pieceCoor, int sign, int matrix[GRILLE_W][GRILLE_H]){
	*pieceCoor += sign * shiftInit;
	int shift;

	for(shift=shiftInit; shift <= maxShift ; shift++, (*pieceCoor) += sign)
		if( !(tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix) ) )
			return SDL_TRUE;

	//revert
	(*pieceCoor) -= sign * shift;
	return SDL_FALSE;
}

/**
*\fn void rotatePiece(Piece *piece, int rotateSens, int matrix[GRILLE_W][GRILLE_H], int hardcore, Mix_Chunk ** sounds)
*\brief Essaye de tourner une pièce, en la déplaçant si besoin
*\param piece La pièce à tourner
*\param rotateSens Le sens de rotation
*\param matrix La matrice pour vérifier les collisions
*\param hardcore Indique si le mode de jeu est facile ou difficile
*\param sounds Les sons du jeu
*/
void rotatePiece(Piece *piece, int rotateSens, int matrix[GRILLE_W][GRILLE_H], int hardcore, Mix_Chunk ** sounds){
	if( rotateSens ){

		int originalFirstCol = piece->firstCol,
			originalLastCol = piece->lastCol,
			originalFirstRow = piece->firstRow,
			originalLastRow = piece->lastRow;

		piece->rota += rotateSens;

		updateGrille(piece, hardcore);
		getColRawInfos(piece);

		//try rotate shifts:
		int downShift=0, upShift = 0;
		int maxDownShift=originalFirstRow - piece->firstRow,
			maxUpShift = piece->lastRow -  originalLastRow,
			maxLeftShift=piece->lastCol - originalLastCol,
			maxRightShift= originalFirstCol - piece->firstCol;


		int found = SDL_FALSE;

		//try normal
		if( !(tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix) )  )
			found = SDL_TRUE;

		//left
		if(!found)
			found = tryShift(0, maxLeftShift, piece, &(piece->x), -1, matrix);

		//right
		if(!found)
			found = tryShift(1, maxRightShift, piece, &(piece->x), 1, matrix);

		//try down
		if(!found){
			piece->y++;
			for(downShift=1; downShift <= maxDownShift && !found ; downShift++, piece->y++){
				if( !(tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix) )  )
					found = SDL_TRUE;

				//down left
				if(!found)
					found = tryShift(0, maxLeftShift, piece, &(piece->x), -1, matrix);

				//down right
				if(!found)
					found = tryShift(0, maxRightShift, piece, &(piece->x), 1, matrix);

				if(found)
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
				if( !(tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix) ) )
					found = SDL_TRUE;


				//up left
				if(!found)
					found = tryShift(0, maxLeftShift, piece, &(piece->x), -1, matrix);

				//up right
				if(!found)
					found = tryShift(0, maxRightShift, piece, &(piece->x), 1, matrix);

				if(found)
					break;

			}
		}
		if(!found){
			//revert up
			piece->y += upShift;

			//revert all
			piece->rota -= rotateSens;
			updateGrille(piece, hardcore);
			getColRawInfos(piece);
			Mix_PlayChannel(5, sounds[SOUND_CANT_ROTATE], 0);
		}
		else{
			Mix_PlayChannel(5, sounds[SOUND_ROTATE], 0);
		}
	}
}


/**
*\fn void moveSide(Piece *piece, int matrix[GRILLE_W][GRILLE_H], float distanceLateral)
*\brief Déplace la pièce sur le côté
*\param piece La pièce à déplacer
*\param matrix La matrice pour vérifier les collisions
*\param distanceLateral La distance de déplacement
*/
void moveSide(Piece *piece, int matrix[GRILLE_W][GRILLE_H], float distanceLateral){
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


/**
*\fn int moveDown(Piece *piece, int accelerate, int matrix[GRILLE_W][GRILLE_H], float distanceDown, int frameStop, int maxDown)
*\brief Déplace la pièce vers le bas
*\param piece La pièce à déplacer
*\param accelerate Indique si la pièce est accélérée par le joueur
*\param matrix La matrice pour vérifier les collisions
*\param distanceDown La distance de déplacement
*\param frameStop Le nombre de frame maximum avant que la pièce ne se fasse inscrire dans la grille
*\param maxDown Indique si la pièce doit être descendue jusqu'en bas
*\return STOPPED si la pièce s'est arrêtée et doit être inscite dans la matrice sinon 0
*/
int moveDown(Piece *piece, int accelerate, int matrix[GRILLE_W][GRILLE_H], float distanceDown, int frameStop, int maxDown){
	float distanceMoveDown = distanceDown * accelerate;
	if(distanceMoveDown >= 2)
		distanceMoveDown = 5/3;

	if(maxDown){
		piece->x = roundf(piece->x);
		piece->frameDir = 0;
	}

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

/**
*\fn void changeDir(Piece *piece, int lateralMove, int frameLateral)
*\brief Change la direction horizontale d'une pièce
*\param piece La pièce à modifer la direction
*\param lateralMove La direction à attribuer
*\param frameLateral Le nombre de frame pour parcourir une case en horizontal
*/
void changeDir(Piece *piece, int lateralMove, int frameLateral){
	if(lateralMove != NO_MOVE){
		if( piece->dir != lateralMove && piece->dir != NO_MOVE)
			piece->frameDir = frameLateral - piece->frameDir;
		else if(piece->frameDir == 0)
			piece->frameDir += frameLateral;

		piece->dir = lateralMove;
	}
}


/**
*\fn int getPieceId(int n)
*\brief Obtient l'id d'une case de la matrice
*\param n La valeur de la case de la matrice
*\return L'id de cette case
*/
int getPieceId(int n){
	return n%BONUS_TRI;
}

/**
*\fn int compareInt_i(const int * a,const int * b)
*\brief Fonction de comparaison de deux entiers
*\param a Le premier entier
*\param b Le deuxième entier
*\return Une valeur positive si a>b et négative si b>a
*/
int compareInt_i(const int * a,const int * b){
	return(*a - *b);
}

/**
*\fn int compareInt(const void * a,const void * b)
*\brief Fonction de comparaison inversée de deux entiers avec types adaptés pour qsort
*\param a Le premier entier
*\param b Le deuxième entier
*\return Une valeur positive si a>b et négative si b>a
*/
int compareInt(const void * a,const void * b){
	return compareInt_i(a, b);
}

/**
*\fn int isOnArray(int n, int array[], int size)
*\brief Détermine si un nombre n est présent dans un tableau
*\param n Le nombre à chercher
*\param array Le tableau
*\param size La taille du tableau
*\return Vrai si l'entier est présent, sinon faux
*/
int isOnArray(int n, int array[], int size){
	for(int i = 0; i < size; i++)
		if(array[i] == n)
			return 1;
	return 0;
}

/**
*\fn void getFillPlaces(int matrix[GRILLE_W][GRILLE_H], int matrixFill[GRILLE_W][GRILLE_H], int nbFill)
*\brief Détermine aléatoirement quelles cases de la matrices doivent être replies par le bonus FILL et enclenche l'animation
*\param matrix La grille
*\param matrixFill Les frames d'animation de FILL pour chaque case
*\param nbFill Le nombre de cases à remplir
*/
void getFillPlaces(int matrix[GRILLE_W][GRILLE_H], int matrixFill[GRILLE_W][GRILLE_H], int nbFill){
	int nbFilled = 0;
	for(int row = GRILLE_H-1; row>=0 && nbFilled<nbFill; row--){
		for(int col = 0; col < GRILLE_W  && nbFilled<nbFill ; col++){
			if(matrix[col][row] == EMPTY){
				matrixFill[col][row] = FRAME_FILL + nbFilled * ESPACEMENT_FRAME_FILL;
				nbFilled++;
			}
		}
	}
}


/**
*\fn int getBonusId(int n)
*\brief Détermine l'id du bonus d'une case de la grille
*\param n La valeur de la case de la grille
*\return L'id du bonus correspondant
*/
int getBonusId(int n){
	int bonusId = 0;

	while(n >= BONUS_TRI){
		n -= BONUS_TRI;
		bonusId++;
	}

	return bonusId;
}

/**
*\fn int completeLine(int matrix[GRILLE_W][GRILLE_H], int frameCompleteLine[GRILLE_H], int line, int lastLine, int bonusActivate[NB_BONUSES], int getBonuses, Score scoreAdd[GRILLE_H], int comboLine, ScoreTotal *scoreTotal, long long *score_hash, long keys[4], Mix_Chunk * soundLine)
*\brief Enclenche l'animation de destruction de ligne et comptabilise les points rapportés et les bonus gagnés
*\param matrix La grille
*\param frameCompleteLine Les frames d'animation de complétion de ligne
*\param line La ligne à compléter
*\param lastLine L'indice de la dernière ligne complétée cette frame
*\param bonusActivate Le nombre de chaque bonus gagnés cette frame
*\param getBonuses Indique si la complétion de la ligne doit faire ganger les bonus présents
*\param scoreAdd Le tableau du nombre de points et combos rapportés cette frame pour chaque ligne
*\param comboLine Le nombre de ligne déjà complétées cette frame
*\param scoreTotal Le score total
*\param score_hash Le scroe hashé
*\param keys Les clés de hashage
*\param soundLine Le son de complétion de ligne
*\return Vrai si pas de problème, faux si problème dans le hashage
*/
int completeLine(int matrix[GRILLE_W][GRILLE_H], int frameCompleteLine[GRILLE_H], int line, int lastLine, int bonusActivate[NB_BONUSES], int getBonuses, Score scoreAdd[GRILLE_H], int comboLine, ScoreTotal *scoreTotal, long long *score_hash, long keys[4], Mix_Chunk * soundLine){
	Mix_PlayChannel(8, soundLine,0);
	int bonusGet = -1;
	if( frameCompleteLine[line] == -1 ){
		frameCompleteLine[line] = FRAME_COMPLETE_LINE;
		if(comboLine){
			scoreAdd[line].score += scoreAdd[lastLine].score  * RATIO_COMBO_LINE;
			scoreAdd[line].combo = pow(2, comboLine);
		}
		else{
			scoreAdd[line].score += SCORE_BASE;
		}

		if(getBonuses){
			int sameColor = getPieceId(matrix[0][line]);
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

			scoreAdd[line].score *= scoreAdd[line].multi;

			if(!changeProtectedVar(score_hash, &(scoreTotal->score), (scoreTotal->score) + scoreAdd[line].score, keys))
				return SDL_FALSE;


			if(!changeProtectedVar(score_hash, &(scoreTotal->score), (scoreTotal->score)+scoreAdd[line].flat * NB_FLAT_POINT, keys))
				return SDL_FALSE;


		}
	}
	return SDL_TRUE;
}

/**
*\fn void useBonus(int bonusId, int frameLaser[GRILLE_H], int *framePassed, int nbUse, int matrix[GRILLE_W][GRILLE_H], int matrixFill[GRILLE_W][GRILLE_H], int *nextIsGiant)
*\brief Effectue l'action d'un bonus
*\param bonusId L'identifiant du bonus
*\param frameLaser Les frames d'animation du bonus LASER
*\param framePassed Le nombre de frames déroules (pour SLOW et SPEED)
*\param nbUse Le nombre d'exemplaires de ce bonus gagnés
*\param matrix La grille
*\param matrixFill Les frames d'animation du bonus FILL
*\param nextIsGiant Indique si la prochaine pièce est géante
*/
void useBonus(int bonusId, int frameLaser[GRILLE_H], int *framePassed, int nbUse, int matrix[GRILLE_W][GRILLE_H], int matrixFill[GRILLE_W][GRILLE_H], int *nextIsGiant){
	switch (bonusId) {
		case FILL:
			getFillPlaces(matrix, matrixFill, nbUse*NB_FILL);
			break;

		case LASER:
			for(int i=GRILLE_H-1; i > GRILLE_H - (NB_LINES_LASER * nbUse) - 1 && i > GRILLE_H - MAX_HEIGHT_LASER -1; i--)
				frameLaser[i] = LASER_FRAME;
			break;

		case SLOW:
			*framePassed -= nbUse * SLOW_AMMOUNT;
			break;

		case SPEED:
			*framePassed += nbUse * SPEED_AMMOUNT;
			break;

		case GIANT:
			*nextIsGiant += nbUse;
			break;
	}
}


/**
*\fn void drawMatrix(SDL_Renderer *renderer, int matrix[GRILLE_W][GRILLE_H], int frameCompleteLine[GRILLE_H], Piece currentPiece, SDL_Texture *brickTexture, SDL_Texture *bonusTexture){
*\brief Dessine la grille et son contenu
*\param renderer Le renderer où dessiner
*\param matrix La grille à dessiner
*\param frameCompleteLine Les frames d'animation de complétion de ligne
*\param currentPiece La pièce qui est en train de tomber (pour dessiner la collone où elle se trouve plus claire)
*\param brickTexture La texture des briques des pièces
*\param bonusTexture La texture des bonus
*/
void drawMatrix(SDL_Renderer *renderer, int matrix[GRILLE_W][GRILLE_H], int frameCompleteLine[GRILLE_H], Piece currentPiece, SDL_Texture *brickTexture, SDL_Texture *bonusTexture){
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

/**
*\fn void drawLaser(SDL_Renderer *renderer, int frameLaser[GRILLE_H], SDL_Texture *laserTexture, Mix_Chunk * soundLaser){
*\brief Dessine l'animation du bonus LASER
*\param renderer Le renderer où dessiner
*\param frameLaser Les frames d'animation du bonus LASER
*\param laserTexture La texture du laser
*\param soundLaser Le son du laser
*/
void drawLaser(SDL_Renderer *renderer, int frameLaser[GRILLE_H], SDL_Texture *laserTexture, Mix_Chunk * soundLaser){
	//laser
	SDL_Rect anim = LASER_DIM;
	SDL_Rect dest = LASER_DIM;

	for(int i=0; i<GRILLE_H; i++){
		if(frameLaser[i] == LASER_START_SHOOT){
			Mix_PlayChannel(9, soundLaser,0);
			break;
		}
	}

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

/**
*\fn void drawFill(SDL_Renderer* renderer, int matrixFill[GRILLE_W][GRILLE_H], SDL_Texture *brickTexture){
*\brief Dessine l'animation du bonus FILL
*\param renderer Le renderer où dessiner
*\param matrixFill Les frames d'animation du bonus FILL
*\param brickTexture La texture des briques
*/
void drawFill(SDL_Renderer* renderer, int matrixFill[GRILLE_W][GRILLE_H], SDL_Texture *brickTexture){
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

/**
*\fn void savePiece(Piece piece, int matrix[GRILLE_W][GRILLE_H])
*\brief Inscrit une pièce dans la matrice
*\param piece La pièce à inscrire
*\param matrix La matrice
*/
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


/**
*\fn void eraseLine(int matrix[GRILLE_W][GRILLE_H], int line, int matrixFill[GRILLE_W][GRILLE_H], int frameLaser[GRILLE_H], int frameCompleteLine[GRILLE_H], Score * scoreAffichage)
*\brief Supprime une ligne de la matrice et fait descendre celles au desus
*\param matrix La matrice
*\param line La ligne à supprimer
*\param matrixFill Les frames d'animations du bonus FILL pour chaque ligne
*\param frameLaser  Les frames d'animations du bonus LASER pour chaque ligne
*\param frameCompleteLine Les frames d'animations de complétion de ligne
*\param scoreAffichage Les scores de chaque ligne
*/
void eraseLine(int matrix[GRILLE_W][GRILLE_H], int line, int matrixFill[GRILLE_W][GRILLE_H], int frameLaser[GRILLE_H], int frameCompleteLine[GRILLE_H], Score * scoreAffichage){
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

/**
*\fn int checkLines(int matrix[GRILLE_W][GRILLE_H], int frameCompleteLine[GRILLE_H], int bonusActivate[NB_BONUSES], int getBonus, Score scoreAdd[GRILLE_H], ScoreTotal *scoreTotal, long long * score_hash, long keys[4], Mix_Chunk * soundLine)
*\brief Vérifie si des lignes sont complétées
*\param matrix La matrice
*\param frameCompleteLine Les frames d'animation de complétion de ligne
*\param bonusActivate Le tableau des bonus obtenus cette frame
*\param getBonus Determine si les bonus des lignes complétés sont à donner au joueur
*\param scoreAdd Le tableau de score par frame et par ligne
*\param scoreTotal Le score total
*\param score_hash Le score hashé
*\param keys Les clés de hashage
*\param soundLine Le son de complétion de ligne
*\return Vrai si pas de problème, faux si problème dans le hashage
*/
int checkLines(int matrix[GRILLE_W][GRILLE_H], int frameCompleteLine[GRILLE_H], int bonusActivate[NB_BONUSES], int getBonus, Score scoreAdd[GRILLE_H], ScoreTotal *scoreTotal, long long * score_hash, long keys[4], Mix_Chunk * soundLine){

	int comboLine = 0; //Le nombre de ligne complétées cette frame
	int j;
	int lastLine = -1; //La dernière ligne complétée cette frame

	for(int i=0; i<GRILLE_H; i++){
		for(j=0; j<GRILLE_W && matrix[j][i] != EMPTY; j++);
		if(j == GRILLE_W){
			if(!completeLine(matrix, frameCompleteLine, i, lastLine, bonusActivate, getBonus, scoreAdd, comboLine, scoreTotal, score_hash, keys, soundLine))
				return SDL_FALSE;
			if(getBonus){
				lastLine = i;
				comboLine++;
			}
		}
	}
	return SDL_TRUE;
}

/**
*\fn void clearIntTab( int *tab, int size)
*\brief Vide un tableau d'entier (l'initialise à 0)
*\param tab Le tableau à initialiser
*\param size La taille du tableau
*/
void clearIntTab( int *tab, int size){
	for(int i=0; i<size; i++)
		tab[i] = 0;
}

/**
*\fn void transfertNextPiece(Piece *currentPiece, Piece nextPiece, int hardcore)
*\brief Copie une pièce dans une autre
*\param currentPiece La pièce destinatrice
*\param nextPiece La pièce source
*\param hardcore La difficulté du jeu
*/
void transfertNextPiece(Piece *currentPiece, Piece nextPiece, int hardcore){
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
	updateGrille(currentPiece, hardcore);
}

/**
*\fn void updateScoreAffichage(Score * scoreAffichage, Score scoreAdd[GRILLE_H], ScoreTotal *scoreTotal)
*\brief Rajoute les scores et combos obtenus cette frame dans le tableau de score à afficher pour toutes les lignes
*\param scoreAffichage Les scores des lignes
*\param scoreAdd Les scores et combo de lignes à ajouter
*\param scoreTotal Le score total
*/

void updateScoreAffichage(Score * scoreAffichage, Score scoreAdd[GRILLE_H], ScoreTotal *scoreTotal){
	for(int i=0; i<GRILLE_H; i++){
		if(scoreAdd[i].score){ //Si un score est à rajouter

			//Lancer anim score total
			scoreTotal->frameToDest = FRAME_DEST_SCORE_TOTAL;


			scoreAffichage[i].scoreDest += scoreAdd[i].score;
			scoreAffichage[i].flatDest += scoreAdd[i].flat;


			if(scoreAdd[i].combo>1)
				scoreAffichage[i].combo = scoreAdd[i].combo;

			if(scoreAdd[i].multi>1)
				scoreAffichage[i].multi = scoreAdd[i].multi;

			if(scoreAdd[i].sameColor != -1)
				scoreAffichage[i].sameColor = scoreAdd[i].sameColor;

			//Lancer ou reset animation des scores de ligne
			if(scoreAffichage[i].frame == 0){
				scoreAffichage[i].score += scoreAdd[i].score;
				scoreAffichage[i].frame = SCORE_TTL;
			}
			else if(scoreAffichage[i].frame < RESET_ANIM){
				scoreAffichage[i].frame = RESET_ANIM;
			}


			//Si un combo est à rajouter
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

	//Remmetre les scores de cette frame à 0
	initScore(scoreAdd);
}


/**
*\fn int lineEmpty(int matrix[GRILLE_W][GRILLE_H], int line )
*\brief Determine si une ligne est vide
*\param matrix La matrice
*\param line La ligne à vérifier
*\return Vrai si la ligne est vide sinon faux
*/
int lineEmpty(int matrix[GRILLE_W][GRILLE_H], int line ){
	for(int i=0;i<GRILLE_W; i++)
		if(matrix[i][line] != EMPTY)
			return 0;

	return 1;
}

/**
*\fn int updateFrames(int frameLaser[GRILLE_H], int frameCompleteLine[GRILLE_H], int matrix[GRILLE_W][GRILLE_H], int matrixFill[GRILLE_W][GRILLE_H], int bonusActivate[NB_BONUSES], Score *scoreAffichage, Score *scoreAdd, ScoreTotal *scoreTotal,int *frameDestJauge,long int frameTotalSpeed,long int *frameTotalShow, long long *score_hash, long keys[4], Mix_Chunk * soundLine, Mix_Chunk * soundFill){

*\brief Actualise toutes les animations et réagis lorsqu'elles arrivent à certains moments
*\remark De nombreuses variables et choses différentes se passent dans cette fonction, mais pour beaucoup elles dépendent les unes des autres donc décomposer en fonction ne ferait qu'ajouter des appels lourds
*\param frameLaser Les animations de LASER
*\param frameCompleteLine Les animations de complétion de ligne
*\param matrix La matrice
*\param matrixFill Les animations de FILL
*\param bonusActivate Le tableau des bonus obtenus cette frame
*\param scoreAffichage Les animations de score et combo de lignes
*\param scoreAdd Les scores et combos obtenus cette frame
*\param scoreTotal Le score total
*\param frameDestJauge Les frames de l'animation de remplissage de la jauge
*\param frameTotalSpeed Le nombre de frame total compabilisé avec bonus
*\param frameTotalShow Le nombre de frame total qui tend vers frameTotalSpeed pour l'animation d'update score total
*\param score_hash le score hashé
*\param keys les clés de hashage
*\param soundLine Le son de complétion de ligne
*\param soundFill Le son du bonus FILL
*\return Faux si il y a une erreur de sécurité hashage, sinon vrai
*/
int updateFrames(int frameLaser[GRILLE_H], int frameCompleteLine[GRILLE_H], int matrix[GRILLE_W][GRILLE_H], int matrixFill[GRILLE_W][GRILLE_H], int bonusActivate[NB_BONUSES], Score *scoreAffichage, Score *scoreAdd, ScoreTotal *scoreTotal,int *frameDestJauge,long int frameTotalSpeed,long int *frameTotalShow, long long *score_hash, long keys[4], Mix_Chunk * soundLine, Mix_Chunk * soundFill){


	if(scoreTotal->frameToDest){
		scoreTotal->frameToDest--;
		if(scoreTotal->score != scoreTotal->scoreShow)
			scoreTotal->scoreShow += (scoreTotal->score-scoreTotal->scoreShow)/ (scoreTotal->frameToDest);

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

		for(int j=0; j<GRILLE_W; j++){
			if(matrixFill[j][i] == FRAME_FILL)
				Mix_PlayChannel(6, soundFill, 0);
			if(matrixFill[j][i]){
				matrixFill[j][i]--;
				if(matrixFill[j][i] == 0){
					matrix[j][i] = NB_PIECES;
					if(!checkLines(matrix, frameCompleteLine, bonusActivate, SDL_FALSE, scoreAdd, scoreTotal, score_hash, keys, soundLine))
						return SDL_FALSE;

				}
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

		//combo
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
			eraseLine(matrix, i, matrixFill, frameLaser, frameCompleteLine, scoreAffichage);
		}
	}

	//animJauge
	if(*frameDestJauge){
		*frameTotalShow += (float)(frameTotalSpeed - *frameTotalShow )/ ( *frameDestJauge );
		(*frameDestJauge)--;
	}
	return SDL_TRUE;
}

/**
*\fn void afficherScores(SDL_Renderer *renderer , SDL_Texture *scoreTexture, Score scoreAffichage, int iScore)
*\brief Affiche les animations de score d'une ligne
*\param renderer Le renderer
*\param scoreTexture La texture des chiffres
*\param scoreAffichage L'animation de score d'une ligne
*\param iScore L'indice de cette ligne
*/
void afficherScores(SDL_Renderer *renderer , SDL_Texture *scoreTexture, Score scoreAffichage, int iScore)
{
	if(scoreAffichage.frame && scoreAffichage.frame<=SCORE_TTL){
		int len, size;
		SDL_Rect src, dest;

		len = lenNum(scoreAffichage.score);
		src = SCORE_SRC;
		size = SIZE_SCORE;

		//Centrer le texte
		dest = (SDL_Rect){SCORE_DEST + (size*len)/2 , MATRIX_Y+iScore*CASE_SIZE + (FONT_HEIGHT_RATIO*size) / 2, size, FONT_HEIGHT_RATIO*size};
		dest.x -= dest.w/2;
		dest.y -= dest.h/2;
		if(dest.h < CASE_SIZE)
			dest.y += (CASE_SIZE-dest.h) / 2;


		for(int j=0 ; j < len; j++) //pour chaque chiffre en partant de la droite
		{
			SDL_SetTextureAlphaMod(scoreTexture, ALPHA_SCORE[SCORE_TTL -scoreAffichage.frame]);

			src.x = SCORE_SRC.w * (scoreAffichage.score%10);
			SDL_RenderCopy(renderer,scoreTexture,&src,&dest);

			scoreAffichage.score /=10;
			dest.x -= size;
		}
	}
}


/**
*\fn void drawComboText(SDL_Renderer *renderer, char * msgCombo, TTF_Font * font, Score scoreAffichage, SDL_Rect * dest, SDL_Color comboColor, float size_score)
*\brief Affiche un texte de combo
*\param renderer Le renderer
*\param msgCombo Le message à afficher
*\param font La police d'écriture du combo
*\param scoreAffichage Les informations du combo
*\param dest L'endroit où l'afficher
*\param comboColor La couleur d'écriture
*\param size_score La taille d'écriture
*/
void drawComboText(SDL_Renderer *renderer, char * msgCombo, TTF_Font * font, Score scoreAffichage, SDL_Rect * dest, SDL_Color comboColor, float size_score){

	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, msgCombo, comboColor);
	SDL_SetSurfaceAlphaMod(surfaceMessage, ALPHA_SCORE[SCORE_TTL -scoreAffichage.frameCombo]);
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	SDL_QueryTexture(Message,NULL,(int*)SDL_TEXTUREACCESS_STATIC,&(dest->w), &(dest->h) );
	dest->w /= (OPEN_FONT_SIZE / size_score);
	dest->h /= (OPEN_FONT_SIZE / size_score);
	dest->y += (CASE_SIZE - dest->h)/2;

	SDL_RenderCopy(renderer, Message, NULL, dest);
	dest->y -= (CASE_SIZE - dest->h)/2;
	dest->x += dest->w;

	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
}

/**
*\fn float getDrawSize(SDL_Renderer *renderer, char * msgTotal, TTF_Font * font)
*\brief Determine la taille d'écriture d'un message
*\param renderer Le renderer
*\param msgTotal Le message à mesurer
*\param font La police d'écriture du combo
*\return La taille d'écriture maximale de ce message
*/
float getDrawSize(SDL_Renderer *renderer, char * msgTotal, TTF_Font * font){

	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, msgTotal, (SDL_Color){255,255,255});
	SDL_Rect dest = {0,0,0,0};
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	float size_score = SIZE_COMBO+1;
	do{
		size_score--;
		SDL_QueryTexture(Message,NULL,(int*)SDL_TEXTUREACCESS_STATIC,&(dest.w), &(dest.h) );
		dest.w /= (OPEN_FONT_SIZE / size_score);
	}while(dest.w > WIDTH_DRAW_COMBO);

	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);

	return size_score;
}

/**
*\fn int intlog(double x, double base)
*\brief Calcule le logarithme d'un nombre à une certaine base
*\param x Le nombre
*\param base La base
*\return Le logarithme de x à la base "base"
*/
int intlog(double x, double base) {
    return (int)(log(x) / log(base));
}


/**
*\fn void drawTotalScore(SDL_Renderer * renderer, TTF_Font *font, ScoreTotal score)
*\brief Affiche le score total du joueur
*\param renderer Le renderer où afficher
*\param font La police d'écriture
*\param score Le score du joueur
*/
void drawTotalScore(SDL_Renderer * renderer, TTF_Font *font, ScoreTotal score){

	char msgScore[MAX_APPEND_LENGHT];
	sprintf(msgScore, "%d", (int)score.scoreShow);
	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, msgScore, SCORE_TOTAL_COLOR);
	SDL_Rect dest = SCORE_TOTAL_DEST;
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

	float size_score = SIZE_SCORE_TOTAL+1;
	do{
		size_score--;
		SDL_QueryTexture(Message,NULL,(int*)SDL_TEXTUREACCESS_STATIC,&(dest.w), &(dest.h) );
		dest.w /= (OPEN_FONT_SIZE / size_score);
	}while(dest.w > WIDTH_SCORE_TOTAL);


	dest.h /= (OPEN_FONT_SIZE / size_score);
	dest.y += (SIZE_SCORE_TOTAL - dest.h)/2;
	dest.x += (WIDTH_SCORE_TOTAL - dest.w)/2;

	SDL_RenderCopy(renderer, Message, NULL, &dest);

	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
}

/**
*\fn void drawHelpText(SDL_Renderer *renderer, TTF_Font *font, SDL_Texture *flecheTexture, SDL_Texture *turnTexture, SDL_Texture *bonusTexture, SDL_Texture *brickTexture)
*\brief Affiche les commandes et explications des bonus
*\param renderer Le renderer où afficher
*\param font La police d'écriture
*\param flecheTexture La texture des flèches directionelles
*\param turnTexture La texture des flèches rotatives
*\param bonusTexture La texture des bonus
*\param brickTexture La texture des briques
*/
void drawHelpText(SDL_Renderer *renderer, TTF_Font *font, SDL_Texture *flecheTexture, SDL_Texture *turnTexture, SDL_Texture *bonusTexture, SDL_Texture *brickTexture){

	//bonus
	for(int i=0; i<NB_BONUSES; i++){
		//brick
		SDL_Rect brickSrc = BRICK_SRC;
		SDL_Rect brickDest = BRICK_HELP_BONUS_DEST;
		brickSrc.x = ID_HELP_BONUS * brickSrc.w;
		brickDest.y += i * ESPACEMENT_HINT_BONUS;
		SDL_RenderCopy(renderer, brickTexture, &brickSrc, &brickDest);
		brickSrc.x = i * brickSrc.w;
		SDL_RenderCopy(renderer, bonusTexture, &brickSrc, &brickDest);

		char * text = BONUS_TEXT[i];

		SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, JAUGE_COLOR);
		SDL_Rect destMsg = BONUS_HINT_DEST;

		destMsg.y += i * ESPACEMENT_HINT_BONUS;

		SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
		SDL_QueryTexture(Message,NULL,(int*)SDL_TEXTUREACCESS_STATIC,&(destMsg.w), &(destMsg.h) );

		destMsg.w /= (OPEN_FONT_SIZE / SIZE_HELP_BONUS);
		destMsg.h /= (OPEN_FONT_SIZE / SIZE_HELP_BONUS);

		SDL_RenderCopy(renderer, Message, NULL, &destMsg);
		SDL_FreeSurface(surfaceMessage);
		SDL_DestroyTexture(Message);
	}

	//commandes
	for(int i=0; i<NB_HELP; i++){
		char * text = HELP_TEXT[i];

		SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, COL_PIECE);
		SDL_Rect dest = HELP_DEST[i];
		SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
		SDL_QueryTexture(Message,NULL,(int*)SDL_TEXTUREACCESS_STATIC,&(dest.w), &(dest.h) );
		dest.w /= (OPEN_FONT_SIZE / SIZE_HELP);
		dest.h /= (OPEN_FONT_SIZE / SIZE_HELP);

		SDL_RenderCopy(renderer, Message, NULL, &dest);
		SDL_FreeSurface(surfaceMessage);
		SDL_DestroyTexture(Message);
	}
	SDL_SetTextureColorMod(flecheTexture, COL_PIECE.r, COL_PIECE.g, COL_PIECE.b);

	SDL_RenderCopy(renderer, flecheTexture, &FLECHE_SRC, &FLECHE_DEST[0]);
	SDL_RenderCopyEx(renderer, flecheTexture, &FLECHE_SRC, &FLECHE_DEST[1], 0, NULL, SDL_FLIP_HORIZONTAL);
	SDL_RenderCopyEx(renderer, flecheTexture, &FLECHE_SRC, &FLECHE_DEST[2], -90, NULL, SDL_FLIP_NONE);


	SDL_SetTextureColorMod(turnTexture, COL_PIECE.r, COL_PIECE.g, COL_PIECE.b);

	SDL_RenderCopy(renderer, turnTexture, &TURN_SRC, &TURN_DEST[0]);
	SDL_RenderCopyEx(renderer, turnTexture, &TURN_SRC, &TURN_DEST[1], 0, NULL, SDL_FLIP_HORIZONTAL);

}

/**
*\fn void drawCombo(SDL_Renderer *renderer, Score scoreAffichage, int line, TTF_Font *font )
*\brief Determine les textes de combos et leurs attributs et les affiche
*\param renderer Le renderer où afficher
*\param scoreAffichage Le combo à afficher
*\param line La ligne du combo
*\param font La police d'écriture à utiliser
*/
void drawCombo(SDL_Renderer *renderer, Score scoreAffichage, int line, TTF_Font *font ){

	if(scoreAffichage.frameCombo && scoreAffichage.frameCombo<=SCORE_TTL){
		char msgCombo[MAX_APPEND_LENGHT]= "";
		char msgFlat[MAX_APPEND_LENGHT]= "";
		char msgMulti[MAX_APPEND_LENGHT]= "" ;
		char msgColor[MAX_APPEND_LENGHT]= "";
		char msgTotal[4*MAX_APPEND_LENGHT] = "";
		SDL_Color comboColor;

		SDL_Rect dest = {COMBO_DRAW_X,MATRIX_Y + line * CASE_SIZE ,0,0};

		//creations messages
		//Combo color
		if(scoreAffichage.sameColor != -1 )
			sprintf(msgColor, "Color!  x%d  ", RATIO_SAME_COLOR);

		//Combo basique
		if(scoreAffichage.combo > 1)
			sprintf(msgCombo, "Combo!  x%d  ",scoreAffichage.combo);

		//Combo bonus multiple
		if(scoreAffichage.multi > 1)
			sprintf(msgMulti, "x%d ",scoreAffichage.multi);

		//Combo bonus flat
		if(scoreAffichage.flat)
			sprintf(msgFlat, "+ %d",scoreAffichage.flat * NB_FLAT_POINT);

		//calcul taille max
		strcat(msgTotal,msgColor);
		strcat(msgTotal,msgCombo);
		strcat(msgTotal,msgMulti);
		strcat(msgTotal,msgFlat);
		float size_score = getDrawSize(renderer, msgTotal, font);


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
	}
}

/**
*\fn void drawJauge(SDL_Renderer * renderer, SDL_Texture * jaugeTexture, long int frameTotalShow)
*\brief Affiche la jauge de vitesse en fonction de l'avancement de la partie
*\param renderer Le renderer où afficher
*\param jaugeTexture La texture de la jauge
*\param frameTotalShow Le total de frame à utiliser
*/
void drawJauge(SDL_Renderer * renderer, SDL_Texture * jaugeTexture, long int frameTotalShow){

	//Calcule le ratio de frame actuelles / frame max
	float ratioVitesse =  (float)frameTotalShow / TIME_TO_REACH_MAX ;
	if(ratioVitesse>1)
		ratioVitesse = 1;
	else if(ratioVitesse<0)
		ratioVitesse = 0;

	SDL_Rect jauge = JAUGE_SPEED_DEST;
	jauge.y++; //Pour eviter les débordements quand la fenetre est redimensionnées
	jauge.h--;

	//Dessine l'arrière plan
	SDL_SetRenderDrawColor(renderer, JAUGE_COLOR_BACKGROUND.r, JAUGE_COLOR_BACKGROUND.g, JAUGE_COLOR_BACKGROUND.b, 255);
	SDL_RenderFillRect(renderer, &jauge);
	jauge.y--;
	//Dessiner le taux de vitesse dans la jauge
	jauge.h *= ratioVitesse;
	jauge.y += JAUGE_SPEED_DEST.h - jauge.h;
	jauge.y--;
	SDL_SetRenderDrawColor(renderer, JAUGE_COLOR.r, JAUGE_COLOR.g, JAUGE_COLOR.b, 255);
	SDL_RenderFillRect(renderer, &jauge);

	//dessiner la texture de jauge pour recouvrir
	jauge = JAUGE_SPEED_DEST;
	SDL_RenderCopy(renderer, jaugeTexture, &JAUGE_SPEED_SRC, &JAUGE_SPEED_DEST);
}


/**
*\fn int linesInCompletion(int matrixFill[GRILLE_W][GRILLE_H], int frameLaser[GRILLE_H], int frameCompleteLine[GRILLE_H])
*\brief Détermine si une ligne va potentiellement bientôt être complétée
*\param matrixFill Les animations du bonus FILL
*\param frameLaser Les animations du bonus LASER
*\param frameCompleteLine Les animations de complétion de ligne
*/
int linesInCompletion(int matrixFill[GRILLE_W][GRILLE_H], int frameLaser[GRILLE_H], int frameCompleteLine[GRILLE_H]){
	for(int line=0; line<GRILLE_H; line++){

		//checkLaser and complete anim
		if(frameLaser[line] != -1 || frameCompleteLine[line] != -1)
			return SDL_TRUE;

		//Check Fill anim
		for(int col=0; col<GRILLE_W; col++)
			if(matrixFill[col][line])
				return SDL_TRUE;

	}

	return SDL_FALSE;
}

/**
*\fn void deathAnimInit(int *gameOver,DeadPiece **deadPieces,int *nbDeadPieces, int matrix[GRILLE_W][GRILLE_H])
*\brief Met en place et enclenche l'animation de fin de partie en scannant la matrice et créant les pièces mortes
*\param gameOver Determine si la partie est finie
*\param deadPieces Le tableau de pièce mortes
*\param nbDeadPieces Le nombre de pièces mortes
*\param matrix La matrice
*/
void deathAnimInit(int *gameOver,DeadPiece **deadPieces,int *nbDeadPieces, int matrix[GRILLE_W][GRILLE_H]){

	if(!deadPieces)
		(*deadPieces) = malloc(GRILLE_W * GRILLE_H * sizeof(DeadPiece));
	(*nbDeadPieces) = 0;

	for(int col = 0; col<GRILLE_W; col++){
		for(int line=0; line<GRILLE_H; line++){
			if(matrix[col][line] != EMPTY){ //Si il y a une brique

				(*deadPieces)[(*nbDeadPieces)++] = (DeadPiece){
					(Vector2f){(rand()%INTERVALE_DEAD_SPEED.x)/PRECISION + BASE_DEAD_SPEED.x, (rand()%INTERVALE_DEAD_SPEED.y)/PRECISION + BASE_DEAD_SPEED.y + COEF_LINE * line}, //speed
					MATRIX_X + col * CASE_SIZE, //x
					MATRIX_Y + line * CASE_SIZE, //y
					0, //angle
					rand()%(int)INTERVALE_ROTA_SPEED/PRECISION + BASE_ROTA_SPEED,//rotaSpeed
					matrix[col][line]//id
				};

				while(fabsf( (*deadPieces)[(*nbDeadPieces)-1].rotaSpeed ) > MIN_ROTA )
					(*deadPieces)[(*nbDeadPieces)-1].rotaSpeed = rand()%(int)INTERVALE_ROTA_SPEED/PRECISION + BASE_ROTA_SPEED; //Veille à ce que la valeur de rotation soit dans les normes

			}
			matrix[col][line] = EMPTY; //vide la case de la matrice
		}
	}
	//(*deadPieces) = realloc(*deadPieces, (*nbDeadPieces) * sizeof(DeadPiece));
}

/**
*\fn void afficherDeadPiece(SDL_Renderer *renderer, DeadPiece deadPiece, SDL_Texture * brickTexture, SDL_Texture * bonusTexture)
*\brief Affiche une pièce morte de l'animation de fin de partie
*\param renderer Le renderer où afficher
*\param deadPiece La pièce morte à afficher
*\param brickTexture La texture des briques
*\param bonusTexture La texture des bonus
*/
void afficherDeadPiece(SDL_Renderer *renderer, DeadPiece deadPiece, SDL_Texture * brickTexture, SDL_Texture * bonusTexture){

	SDL_Rect src = BRICK_SRC;
	SDL_Rect dest = BRICK_DEST;

	//positionner la brique
	dest.x = deadPiece.x;
	dest.y = deadPiece.y;
	src.x = getPieceId(deadPiece.id) * BRICK_SRC.w;

	//afficher brique
	SDL_RenderCopyEx(renderer, brickTexture, &src, &dest,deadPiece.rota,NULL,SDL_FLIP_NONE);

	//afficher bonus
	if(deadPiece.id >= 10){
		src.x = (getBonusId(deadPiece.id) -1) * BRICK_SRC.w;
		SDL_RenderCopyEx(renderer, bonusTexture, &src, &dest,deadPiece.rota,NULL,SDL_FLIP_NONE);
	}
}

/**
*\fn void moveDeadPiece(DeadPiece *deadPiece)
*\brief Déplace une pièce morte en fonction de ses attributs
*\param deadPiece La pièce morte à afficher
*/
void moveDeadPiece(DeadPiece *deadPiece){
	deadPiece->x += deadPiece->speed.x;
	deadPiece->y -= deadPiece->speed.y;

	deadPiece->speed.y -= GRAVITE;
	if(deadPiece->speed.y < MIN_DEAD_SPEED)
		deadPiece->speed.y = MIN_DEAD_SPEED;
	deadPiece->rota += deadPiece->rotaSpeed;
}

/**
*\fn void myFrees(Piece * currentPiece, Piece * nextPiece, DeadPiece ** deadPieces, SDL_Texture * textures[NB_TETRIS_TEXTURES], TTF_Font * fonts[NB_TETRIS_FONTS], SDL_Thread ** thread)
*\brief Libère la mémoire
*\param currentPiece La pièce actuelle
*\param nextPiece La pièce à venir
*\param deadPieces Les briques de l'animation de fin
*\param textures Les textures
*\param fonts Les polices
*\param thread Le thread d'envoi de score
*/
static void myFrees(Piece * currentPiece, Piece * nextPiece, DeadPiece ** deadPieces, SDL_Texture * textures[NB_TETRIS_TEXTURES], TTF_Font * fonts[NB_TETRIS_FONTS], SDL_Thread ** thread){
	if(currentPiece->grille){
		free(currentPiece->grille);
		currentPiece->grille = NULL;
	}

	if(nextPiece->grille){
		free(nextPiece->grille);
		nextPiece->grille = NULL;
	}

	if(*deadPieces){
		free(*deadPieces);
		*deadPieces = NULL;
	}

	/*for(int i=0; i<NB_TETRIS_TEXTURES; i++)
		if(textures[i]){
			SDL_DestroyTexture(textures[i]);
			textures[i] = NULL;
		}
	*/
	for(int i=0; i<NB_TETRIS_FONTS; i++)
		if(fonts[i]){
			TTF_CloseFont(fonts[i]);
			fonts[i] = NULL;
		}

	if(*thread){
		SDL_WaitThread(*thread, NULL);
		*thread = NULL;
	}
}



extern int updateEnded;
/**
*\fn int tetris( SDL_Renderer *renderer ,int highscore, int WinWidth, int WinHeight, char *token, int hardcore, SDL_Texture ** textures){

*\brief La fonction principale : Lance et fait tourner le jeu tetris
*\param renderer Le renderer
*\param highscore Le meilleur score personnel
*\param WinWidth La largeur de la fenêtre
*\param WinHeight La hauteur de la fenêtre
*\param token Le token pour l'envoi des scores
*\param hardcore  Le difficulté du jeu
*\return 0 en cas de retour normal
*/
int tetris( SDL_Renderer *renderer ,int highscore, int WinWidth, int WinHeight, char *token, int hardcore, SDL_Texture ** textures){


// // // // // // //
// MISE EN PLACE   //``
// // // // // // //
	myInit();

	float ratioWindowSize = (float)WinWidth/BASE_WINDOW_W;

	/*SDL_Rect playgroundView = {HUD_W*ratioWindowSize, HUD_H*ratioWindowSize, PLAYGROUND_SIZE_W*ratioWindowSize, PLAYGROUND_SIZE_H*ratioWindowSize};
	SDL_RenderSetViewport(renderer, &playgroundView);*/
	SDL_RenderSetScale(renderer, ratioWindowSize, ratioWindowSize);
	SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b, 255);
	SDL_RenderClear(renderer);

	SDL_Thread *thread = NULL;
	char buffer[10];
	EnvoiScore envoiScore;
	updateEnded = SDL_FALSE;
	int retour = EXIT_FAILURE;
	int frameRetour = 0;
	int frame_anim_loading = 0;

	DeadPiece *deadPieces = malloc(GRILLE_W * GRILLE_H * sizeof(DeadPiece));


	//Fonts
	TTF_Font* fonts[NB_TETRIS_FONTS];
	for(int i=0; i< NB_TETRIS_FONTS; i++){
		 fonts[i] =  TTF_OpenFont(DIR_FONTS_TETRIS[i], OPEN_FONT_SIZE);
		 if( fonts[i] == NULL ){
			printf("Erreur lors de la creation de font %s", TTF_GetError());
			return EXIT_FAILURE;
		}
	}

	//audio
	Mix_Chunk* sounds[NB_TETRIS_SOUNDS];

	for(int i=0; i< NB_TETRIS_SOUNDS; i++){
		 sounds[i] = Mix_LoadWAV(DIR_SOUNDS_TETRIS[i]);
		 if( sounds[i] == NULL ){
			printf("Erreur lors de la creation de son %s %s", DIR_SOUNDS_TETRIS[i], Mix_GetError());
			return EXIT_FAILURE;
		}
	}

	SDL_RenderSetScale(renderer, ratioWindowSize, ratioWindowSize);
	int firstframe = SDL_TRUE;
	int quit = SDL_FALSE;
	while(!quit){

   		 // // // //
		// Vars   //`
		 // // // //
		int gameOver = SDL_FALSE;
		//move
		int accelerate = NO_ACCELERATE;
		int maxDown = SDL_FALSE;
		int lateralMove = NO_MOVE;
		int rotate = SDL_FALSE;
		int rdyToRotate[2] = {SDL_TRUE, SDL_TRUE};
		int rdyToSpace = SDL_TRUE;
		int cantMoveSide = SDL_FALSE;
		int nextIsGiant = SDL_FALSE;

		//int doGameplay = SDL_TRUE;
		int waitToPlace = SDL_FALSE;

		//frames and distances
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

		//hash setup
		long keys[4];
		initialisationConstantHashage(keys);
		long long score_hash = hashage(score.score, keys);

		//matrice
		int matrix[GRILLE_W][GRILLE_H];
		int matrixFill[GRILLE_W][GRILLE_H];
		int frameCompleteLine[GRILLE_H];
		for(int i=0; i < GRILLE_H; i++)
			frameCompleteLine[i] = -1;

		//death
		int nbDeadPieces = 0;

		//Keyboard
		const Uint8 *keystate = SDL_GetKeyboardState(NULL);

		//sound
		for(int i=0; i<NB_TETRIS_SOUNDS;i++)
			Mix_VolumeChunk(sounds[i], SOUND_VOLUMES[i]);

		//Time
		unsigned int lastTime = 0, currentTime;

		//hud and menus
		int rdyToPause = SDL_TRUE;

		//mouse
		SDL_Point mouseCoor;

		 // // // // // // // //
		//   Initialize vars   //
		 // // // // // // // //
		initScore(scoreAffichage);
		initScore(scoreAdd);
		initMatrix(matrix, EMPTY);
		initMatrix(matrixFill, 0);
		initPiece(&currentPiece);
		initPiece(&nextPiece);
		getNewPiece(&currentPiece, SDL_FALSE, hardcore);
		getNewPiece(&nextPiece, SDL_FALSE, hardcore);
		putAtTop(&currentPiece, matrix, (int)frame[TO_GO], hardcore);
		putAtNextPiece(&nextPiece);

	 // // // // // // //
	//   BOUCLE DU JEU  //``
	 // // // // // // //

		while( 1 ){
			if(firstframe){
				nbDeadPieces=1 ;
				deadPieces[0].y = Y_START_REPLAY + 500;
				gameOver = SDL_TRUE;
				firstframe=SDL_FALSE;
				nextPiece.x = UNDEFINED.x;
				currentPiece.x = UNDEFINED.x;
			}
			int sentScore = SDL_FALSE;
			// Init input
			SDL_GetMouseState(&(mouseCoor.x), &(mouseCoor.y));
			accelerate = NO_ACCELERATE;
			lateralMove = NO_MOVE;
			rotate = SDL_FALSE;
			maxDown = SDL_FALSE;

		// // // //
		// Events //`
		// // // //
			SDL_Event event;
			while( SDL_PollEvent(&event) ){
				switch( event.type ){
					case SDL_QUIT:
						// fermer
						myFrees(&currentPiece, &nextPiece,  &deadPieces, textures, fonts, &thread);
						return 0;
						break;

					case SDL_KEYUP:
						/*if ( event.key.keysym.sym == SDLK_ESCAPE )
							rdyToPause = SDL_TRUE;
						else*/
						if(event.key.keysym.sym == SDLK_a)
							rdyToRotate[0] = SDL_TRUE;
						else if( event.key.keysym.sym == SDLK_e)
							rdyToRotate[1] = SDL_TRUE;
						else if( event.key.keysym.sym == SDLK_SPACE)
							rdyToSpace = SDL_TRUE;
						break;

					case SDL_KEYDOWN:
						if ( event.key.keysym.sym == SDLK_ESCAPE && rdyToPause ){
							//doGameplay = !doGameplay;
							rdyToPause = SDL_FALSE;
						}
						break;
				}
			}


		 // // // // // // // // //
		// Handle Keyboard inputs //`
		 // // // // // // // // //
			SDL_PumpEvents();

			if( keystate[SDL_SCANCODE_DOWN] || keystate[SDL_SCANCODE_S] )
				accelerate = ACCELERATE;


			if( keystate[SDL_SCANCODE_SPACE] && rdyToSpace){
				maxDown = SDL_TRUE;
				rdyToSpace = SDL_FALSE;
				if(nbDeadPieces)
					break;
			}

			if( keystate[SDL_SCANCODE_ESCAPE] ){
				myFrees(&currentPiece, &nextPiece,  &deadPieces, textures, fonts, &thread);
				return 0;
			}

			if( keystate[SDL_SCANCODE_RIGHT] || keystate[SDL_SCANCODE_D] )
				lateralMove = MOVE_RIGHT;

			else if( keystate[SDL_SCANCODE_LEFT]  || keystate[SDL_SCANCODE_A])
				lateralMove = MOVE_LEFT;


			if( (keystate[SDL_SCANCODE_Q] && rdyToRotate[0]) ){
				rdyToRotate[0] = SDL_FALSE;
				rotate = 1;
			}
			else if( (keystate[SDL_SCANCODE_E] && rdyToRotate[1])){
				rdyToRotate[1] = SDL_FALSE;
				rotate = -1;
			}


		// // // // //
		// Gameplay //`
		// // // // //
			if(!gameOver && !waitToPlace){
				//rotate normal then left down right
				rotatePiece(&currentPiece, rotate, matrix, hardcore, sounds);



				if(!cantMoveSide)
					changeDir(&currentPiece, lateralMove, (int)frame[LATERAL]);

				moveSide(&currentPiece, matrix, distances[LATERAL]);


				cantMoveSide = SDL_FALSE;
				if(!currentPiece.frameToGo){
					if( moveDown(&currentPiece, accelerate, matrix, distances[DOWN], (int)frame[STOP], maxDown) == STOPPED){
						cantMoveSide = SDL_TRUE;
						if(currentPiece.frameDir == 0){
							//Piece is saved and remplaced
							Mix_PlayChannel(7, sounds[SOUND_HIT], 0);
							savePiece(currentPiece, matrix);
							clearIntTab(bonusActivate, NB_BONUSES);
							if(!checkLines(matrix, frameCompleteLine, bonusActivate, SDL_TRUE, scoreAdd, &score, &score_hash, keys,sounds[SOUND_LINE])){
								myFrees(&currentPiece, &nextPiece,  &deadPieces, textures, fonts,&thread);
								printf("U HACKER\n" );
								return HACKED;
							}


							for(int i=0; i<NB_BONUSES; i++)
								if(bonusActivate[i])
									useBonus(i+1, frameLaser, &framePassed, bonusActivate[i], matrix, matrixFill, &nextIsGiant);

							updateDistances(frame, distances, &framePassed, &frameDestJauge, &frameTotalSpeed);
							transfertNextPiece(&currentPiece,nextPiece, hardcore);

							getNewPiece(&nextPiece, nextIsGiant, hardcore);
							if(nextIsGiant)
								nextIsGiant--;

							if( !putAtTop(&currentPiece, matrix, (int)frame[TO_GO], hardcore) ){
								if(linesInCompletion(matrixFill, frameLaser, frameCompleteLine))
									waitToPlace = SDL_TRUE;
								else{
									nextPiece.x=UNDEFINED.x;
									frameTotalSpeed = TIME_START;
									deathAnimInit(&gameOver, &deadPieces, &nbDeadPieces, matrix);
									if(!sentScore){
										sentScore = SDL_TRUE;
										if (  score_hash == hashage(score.score, keys) )
										{
											frame_anim_loading = 0;
											//On atttend si jamais un envoi est en cours
											if(thread){
												printf("waiting for trhad 1\n" );
												SDL_WaitThread(thread, &retour);
												if( retour == EXIT_FAILURE){
													frameRetour = -2*FRAME_ANIM_RETOUR;
												}
												else{
													 frameRetour = FRAME_ANIM_RETOUR;
												}
												thread = NULL;
											}


											// CONVERTIR SCORE EN TEXT

											sprintf(buffer,"%d",score.score);

											if(hardcore)
												envoiScore = (EnvoiScore){"2", buffer, token};
											//updateScore("2",buffer,token);
											else
												envoiScore = (EnvoiScore){"11", buffer, token};
											//updateScore("11",buffer,token);

											printf("CONNEXION...\n" );
											thread = SDL_CreateThread(  (int(*)(void*))updateScore, NULL, &envoiScore );
											Mix_PlayChannel(10, sounds[SOUND_GAMEOVER],0);
										}
									}
								}
							}
						}
					}
				}
				else
					currentPiece.frameToGo--;
			}
			else if(waitToPlace){
				if(putAtTop(&currentPiece, matrix, 2*(int)frame[TO_GO], hardcore) ){
					waitToPlace = SDL_FALSE;
				}
				else if(!linesInCompletion(matrixFill, frameLaser, frameCompleteLine)){
					waitToPlace = SDL_FALSE;
					nextPiece.x=UNDEFINED.x;
					nextPiece.x=UNDEFINED.x;
					frameTotalSpeed = TIME_START;
					deathAnimInit(&gameOver, &deadPieces, &nbDeadPieces, matrix);

					if(!sentScore){
						sentScore = SDL_TRUE;
						if (  score_hash == hashage(score.score, keys) )
						{
							frame_anim_loading = 0;
							//On atttend si jamais un envoi est en cours
							if(thread){
								printf("waiting for trhad 2\n" );
								SDL_WaitThread(thread, &retour);
								if( retour == EXIT_FAILURE){
									frameRetour = -2*FRAME_ANIM_RETOUR;
								}
								else{
									 frameRetour = FRAME_ANIM_RETOUR;
								}
								thread = NULL;
							}


							// CONVERTIR SCORE EN TEXT

							sprintf(buffer,"%d",score.score);

							if(hardcore)
								envoiScore = (EnvoiScore){"2", buffer, token};
							//updateScore("5",buffer,token);
							else
								envoiScore = (EnvoiScore){"11", buffer, token};
							//updateScore("8",buffer,token);

							printf("CONNEXION...\n" );
							thread = SDL_CreateThread(  (int(*)(void*))updateScore, NULL, &envoiScore );
							Mix_PlayChannel(10, sounds[SOUND_GAMEOVER],0);
						}
					}


					//break;
				}
			}
			updateScoreAffichage(scoreAffichage, scoreAdd, &score);

			for(int i=0; i<nbDeadPieces; i++)
				moveDeadPiece(&(deadPieces[i]));


		 // // //
		// Draw //`
		 // // //
		 	drawHelpText(renderer, fonts[T_FONT_COMBO], textures[T_FLECHE], textures[T_TURN], textures[T_BONUS], textures[T_BRICKS]);
			SDL_RenderCopy(renderer, textures[T_HUD_GRILLE], NULL, &HUD_GRILLE_DIM);
			drawJauge(renderer, textures[T_SPEED_JAUGE], frameTotalShow);
			drawMatrix(renderer, matrix, frameCompleteLine, currentPiece, textures[T_BRICKS], textures[T_BONUS]);
			drawPiece(renderer, currentPiece, textures[T_BRICKS], textures[T_BONUS], SDL_FALSE, hardcore);
			drawPiece(renderer, nextPiece, textures[T_BRICKS], textures[T_BONUS], SDL_TRUE, hardcore);
			drawLaser(renderer, frameLaser, textures[T_LASER_ANIM], sounds[SOUND_LASER]);
			drawFill(renderer, matrixFill,textures[T_BRICKS]);
			drawQuit(renderer, fonts[T_FONT_COMBO], 1, JAUGE_COLOR);
			if(nbDeadPieces && deadPieces[0].y > Y_START_REPLAY)
				drawReplay(renderer, fonts[T_FONT_COMBO],0.57, BASE_WINDOW_W,BASE_WINDOW_H);

			//SDL_RenderFillRect(renderer, &SCORE_TOTAL_DEST);
			drawTotalScore(renderer,fonts[T_FONT_COMBO], score);
			//drawNextPiece(nextPiece);

			for(int i=0; i<GRILLE_H; i++){
				afficherScores(renderer, textures[T_CHIFFRE], scoreAffichage[i], i);
				drawCombo(renderer,  scoreAffichage[i], i, fonts[T_FONT_COMBO]);
			}
			for(int i=0; i<nbDeadPieces; i++)
				afficherDeadPiece(renderer, deadPieces[i], textures[T_BRICKS], textures[T_BONUS]);

			//drawretour
			if(thread && updateEnded){
				printf("waiting for trhad 3\n" );
				SDL_WaitThread(thread, &retour);
				thread = NULL;
				if(retour == EXIT_FAILURE){
					frameRetour = -2*FRAME_ANIM_RETOUR;
				}
				else{
					 frameRetour = FRAME_ANIM_RETOUR;
				}
			}
			else if(thread){
				afficherLoading(renderer, textures[T_LOADING], JAUGE_COLOR, 0, 0, frame_anim_loading++, BASE_WINDOW_W , BASE_WINDOW_H, BASE_WINDOW_W );
			}

			if(frameRetour){

				afficherRetour(renderer, textures[T_LOADING],fonts[T_FONT_COMBO], JAUGE_COLOR, 0, 0, frameRetour, BASE_WINDOW_W , BASE_WINDOW_H, BASE_WINDOW_W );
				if(frameRetour >0)
					frameRetour--;
				else
					frameRetour++;
			}

			SDL_RenderPresent(renderer);


		 // // // // //
		// Next frame //`
		 // // // // //

			//regulateFPS
			currentTime = SDL_GetTicks();
			while( currentTime - lastTime < 1000./FRAMES_PER_SECOND )
				currentTime = SDL_GetTicks();

			lastTime = currentTime;

			//Actualise frames
			framePassed++;
			if(!gameOver)
			if(!updateFrames(frameLaser, frameCompleteLine, matrix, matrixFill, bonusActivate, scoreAffichage, scoreAdd, &score, &frameDestJauge, frameTotalSpeed, &frameTotalShow, &score_hash, keys, sounds[SOUND_LINE], sounds[SOUND_HIT]))
			{
				printf("U HACKER\n" );
				myFrees(&currentPiece, &nextPiece,  &deadPieces, textures, fonts, &thread);
				return HACKED;
			}


			// On efface
			SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b, 255);
			SDL_RenderClear(renderer);

		}

	}
	return 0;
}


/*int main(){ // pour tester tetris
	myInit();


	SDL_Rect displayBounds;
	if (SDL_GetDisplayBounds(0, &displayBounds) != 0) {
		SDL_Log("SDL_GetDisplayBounds failed: %s", SDL_GetError());
		return 1;
	}


	SDL_Window *myWindow = SDL_CreateWindow("Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 0.9*displayBounds.w, 0.9*displayBounds.h, WINDOW_FLAG);
	if( myWindow == NULL ){
		printf("Erreur lors de la creation de la fenêtre : %s", SDL_GetError());
		return EXIT_FAILURE;
	}


	float ratioWindowSize = BASE_WINDOW_W/(0.9*displayBounds.w);

	SDL_Renderer *renderer = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED);
	if( renderer == NULL ){
		printf("Erreur lors de la creation d'un renderer : %s", SDL_GetError());
		return EXIT_FAILURE;
	}



	tetris(renderer, 0, ratioWindowSize, NULL, SDL_FALSE);
}*/
