/**
*\file snake.c
*\brief Jeu snake avec bonus
*\author Hugo Lecomte
*\version 1.0
*\date 25/04/2020
*/

#include "../../include/define.h"
#include "../../include/hashage.h"
#include "../../include/libWeb.h"
#include "../../include/communFunctions.h"
#include "config.h"
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>

#include <stdlib.h>
#include <stdio.h>

#include <math.h>


/**
*\fn void snakeInit()
*\brief Initialise l'environement de snake
*/
void snakeInit(){
	int nbZero;

	// FRUIT_ADD_RADIUSa
	//Générer courbe des "ventres"
	for( int i = 0; i < NB_FRUITS + NB_BONUSES; i++ ){
		for( int j = 0; j < SIZE_PRE_RADIUS / 2; j++ ){
			float t = (j - (1.2 / j)) * FRUIT_PROPRIETES[i][DIGESTION_SIZE] / (SIZE_PRE_RADIUS / 2);
			if( t >= 0 ){
				FRUIT_ADD_RADIUS[i][SIZE_PRE_RADIUS - 1 - j] = t;
				FRUIT_ADD_RADIUS[i][j] = t;
			}
			else{
				FRUIT_ADD_RADIUS[i][j] = 0;
				FRUIT_ADD_RADIUS[i][SIZE_PRE_RADIUS - 1 - j] = 0;
			}
		}
		FRUIT_ADD_RADIUS[i][(SIZE_PRE_RADIUS / 2)] = FRUIT_PROPRIETES[i][DIGESTION_SIZE];
	}

	//Coller à droite
	for( int i = 0; i < NB_FRUITS + NB_BONUSES; i++ ){
		nbZero = 0;

		for( int j = SIZE_PRE_RADIUS - 1; j >= 0  &&  !FRUIT_ADD_RADIUS[i][j]; j-- )
			nbZero++;

		if( nbZero ){
			for( int k = SIZE_PRE_RADIUS - 1 - nbZero; k >= 0; k-- ){
				FRUIT_ADD_RADIUS[i][k + nbZero] = FRUIT_ADD_RADIUS[i][k];
				FRUIT_ADD_RADIUS[i][k] = 0;
			}
		}
	}

	// FRUIT_ADD_RADIUS_GIANT
	//Générer courbe des "ventres"
	for( int i = 0; i < NB_FRUITS + NB_BONUSES ; i++ ){
		for( int j = 0; j < SIZE_PRE_RADIUS / 2; j++ ){
			float t = ((j - (1.2 / j)) * (FRUIT_PROPRIETES[i][DIGESTION_SIZE]  + GIANT_DIGESTION) / (SIZE_PRE_RADIUS / 2));

			if( t >= 0 ){
				FRUIT_ADD_RADIUS_GIANT[i][SIZE_PRE_RADIUS - 1 - j] = t;
				FRUIT_ADD_RADIUS_GIANT[i][j] = t;
			}
			else{
				FRUIT_ADD_RADIUS_GIANT[i][j] = 0;
				FRUIT_ADD_RADIUS_GIANT[i][SIZE_PRE_RADIUS - 1 - j] = 0;
			}
		}
		FRUIT_ADD_RADIUS_GIANT[i][(SIZE_PRE_RADIUS / 2)] = FRUIT_PROPRIETES[i][DIGESTION_SIZE] + GIANT_DIGESTION;
	}

	//Coller à droite
	for( int i = 0; i < NB_FRUITS + NB_BONUSES; i++ ){
		nbZero = 0;

		for( int j = SIZE_PRE_RADIUS - 1; j >= 0  &&  !FRUIT_ADD_RADIUS_GIANT[i][j]; j-- )
			nbZero++;

		if( nbZero ){
			for( int k = SIZE_PRE_RADIUS - 1 - nbZero; k >= 0; k-- ){
				FRUIT_ADD_RADIUS_GIANT[i][k + nbZero] = FRUIT_ADD_RADIUS_GIANT[i][k];
				FRUIT_ADD_RADIUS_GIANT[i][k] = 0;
			}
		}
	}

	// Settings
	Mix_Volume(5, 40);//appear
	Mix_Volume(6, 128);//diseapear
	Mix_Volume(7, 35);//get
	Mix_Volume(8, 128);//bonus
	Mix_Volume(9, 128);//gameOver

	//rand
	srand(time(NULL));
}


/**
*\fn void initFruit(Fruit** fruit)
*\brief Initialise le tableau dynamique de fruits
*\param fruit Le tableau dynamique de fruits
*/
void initFruit(Fruit** fruit){
	if(*fruit)
		free(*fruit);
	*fruit = malloc(sizeof(Fruit));
	(*fruit)->x = UNDEF.x;
	(*fruit)->y = UNDEF.x;
	(*fruit)->giant = 0;
	(*fruit)->id = NO_ID;
	(*fruit)->hitbox = 0;
	(*fruit)->frame = 0;
	(*fruit)->dest = (Vector2f){UNDEF.x, UNDEF.y};
	(*fruit)->coefRadius = 1;
}


/**
*\fn void initBonus(Fruit* bonus)
*\brief Initialise le bonus
*\param bonus Le bonus à initialise
*/
void initBonus(Fruit* bonus){
	bonus->x = UNDEF.x;
	bonus->y = UNDEF.x;
	bonus->giant = 0;
	bonus->id = 0;
	bonus->hitbox = 0;
	bonus->frame = FRUIT_TTL + NB_FRAME_DEATH_FRUIT + 1;
	bonus->dest = (Vector2f){UNDEF.x, UNDEF.y};
	bonus->coefRadius = 1;
}


/**
*\fn void turnLeft(double *angle)
*\brief Tourne le serpent vers la gauche
*\param angle La direction du serpent
*/
void turnLeft(double *angle){
	*angle -= TURN_AMMOUNT;

	if(*angle < 0){
		*angle += 2 * PI;
	}
}


/**
*\fn void turnRight(double *angle)
*\brief Tourne le serpent vers la droite
*\param angle La direction du serpent
*/
void turnRight(double *angle){
	*angle += TURN_AMMOUNT;
}


/**
*\fn double norme(Vector2f v)
*\brief Calcule la norme d'un vecteur
*\param v Le vecteur
*\return La norme de ce vecteur
*/
double norme(Vector2f v){
	return sqrt(v.x * v.x + v.y * v.y);
}


/**
*\fn void initBody(size_t *size, SnakePart** snake)
*\brief Initialise le corps du serpent
*\param size La taille du serpent
*\param snake Le coprs du serpent (tableau de parties de corps)
*/
void initBody(size_t *size, SnakePart** snake){
	if(*snake)
		free(*snake);

	*snake = malloc(sizeof(SnakePart) * (INIT_BODY + SIZE_PRE_RADIUS));

	for( int i = 0; i < INIT_BODY + SIZE_PRE_RADIUS; i++ )
		(*snake)[i] = (SnakePart){UNDEF.x, UNDEF.y, 0, 0};

	(*snake)[SIZE_PRE_RADIUS] = (SnakePart){INIT_HEAD.x, INIT_HEAD.y, 0, 0};

	for( int i = SIZE_PRE_RADIUS + 1; i < INIT_BODY + SIZE_PRE_RADIUS; i++ )
		(*snake)[i] = (SnakePart){(*snake)[i - 1].x + INIT_DECAL.x, (*snake)[i - 1].y + INIT_DECAL.y, 0, 0};

	*size = INIT_BODY + SIZE_PRE_RADIUS;
}


/**
*\fn void initToV(Vector2f *vTab, Vector2f v, int iStart, int iEnd)
*\brief Initialise une portion de tableau de vecteur
*\param vTab Le tableau de vecteur
*\param v Le vecteur pour l'initialisation
*\param iStart Le premier indice du tableau à initialiser (inclus)
*\param iEnd Le dernier indice du tableau à initialiser (exclus)
*/
void initToV(Vector2f *vTab, Vector2f v, int iStart, int iEnd){
	for( int i = iStart; i < iEnd; i++ )
		vTab[i] = v;
}


/**
*\fn void shiftRefresh(Vector2f pastBody[REMIND_BODY], int nbDecale)
*\brief Décale le tableau de positions anciennes du serpent pour en ajouter de nouvelles
*\param pastBody Le tableau de positiones anciennes
*\param nbDecale La quantité de décalage
*/
void shiftRefresh(Vector2f pastBody[REMIND_BODY], int nbDecale){
	for( int i = nbDecale; i < REMIND_BODY; i++ )
		pastBody[i - nbDecale] = pastBody[i];

	initToV(pastBody, UNDEF, REMIND_BODY - nbDecale, REMIND_BODY);
}


/**
*\fn int addBody(SnakePart **snake, size_t *size, Vector2f pastBody[REMIND_BODY], float *radiusRestant)
*\brief Ajoute des parties de corps au serpent
*\param snake Le tableau de parties de corps
*\param size La taille du serpent
*\param pastBody Les positions anciennes du serpent
*\param radiusRestant La taille excedante de la dernière partie de queue
*\return Le nombre de parties de corps ajoutées
*/
int addBody(SnakePart **snake, size_t *size, Vector2f pastBody[REMIND_BODY], float *radiusRestant){
	int nbAdd = *radiusRestant / RATIO_RADIUS_BODYADD;
	(*snake)[*size - 1].radius = 0;

	*snake = realloc(*snake, (*size + nbAdd) * sizeof(SnakePart));

	for( int i = 0; i < nbAdd && *size + i - 2>=0; i++ ){
		if( pastBody[i].x == UNDEF.x ){
			(*snake)[*size + i].x = (*snake)[*size + i - 1].x + (*snake)[*size + i - 1].x - (*snake)[*size + i - 2].x;
			(*snake)[*size + i].y = (*snake)[*size + i - 1].y + (*snake)[*size + i - 1].y - (*snake)[*size + i - 2].y;
		}
		else{
			(*snake)[*size + i].x = pastBody[i].x;
			(*snake)[*size + i].y = pastBody[i].y;
		}
	}

	for( int i = 0; i < nbAdd; i++ )
		(*snake)[*size + i].radius = 0;

	shiftRefresh(pastBody, nbAdd);

	*size += nbAdd;

	*radiusRestant -= nbAdd * RATIO_RADIUS_BODYADD;

	return nbAdd;
}


/**
*\fn void shiftFreeSpace(Vector2f* pastBody)
*\brief  Décale le tableau de positions anciennes du serpent pour libérer de la place
*\param pastBody Le tableau de positions anciennes
*/
void shiftFreeSpace(Vector2f* pastBody){
	for( int i = REMIND_BODY - 2; i >= 0; i-- )
		pastBody[i + 1] = pastBody[i];
	pastBody[0] = UNDEF;
}


/**
*\fn void shiftBomb(Vector2f* pastBody)
*\brief  Décale le tableau de positions anciennes du serpent lors d'une explosion de bombe
*\param pastBody Le tableau de positions anciennes
*/
void shiftBomb(Vector2f* pastBody){
	for( int i = 0; i < REMIND_BODY - 1; i++ )
		pastBody[i] = pastBody[i + 1];
	pastBody[REMIND_BODY - 1] = UNDEF;
}


/**
*\fn void rmBody(SnakePart **snake, size_t *size, int nbRm, Vector2f* pastBody, SnakePart **deadBodies, size_t *nbDeadBodies, int deadly, int ratioTexture, int initFrame)
*\brief Suprimme des parties du serpent
*\param snake Le serpent
*\param size La taille du serpent
*\param nbRm Le nombre de parties à supprimer
*\param pastBody Le tableau de positions anciennes
*\param deadBodies Le tableau de parties de serpent détruites
*\param nbDeadBodies Le nombre de parties de serpent détruites
*\param deadly Détermine si cette supression peut tuer le serpent
*\param ratioTexture Détermine tous les combien de parties supprimées il faut afficher l'animation
*\param initFrame Détermine combien de parties sont passées sans afficher d'animation de mort de partie de serpent (animation de mort)
*/
void rmBody(SnakePart **snake, size_t *size, int nbRm, Vector2f* pastBody, SnakePart **deadBodies, size_t *nbDeadBodies, int deadly, int ratioTexture, int initFrame){
	//check nbRm doesnt kill snake
	if( !deadly && nbRm >= *size - SIZE_PRE_RADIUS - MIN_BODY_PARTS )
		nbRm = *size - SIZE_PRE_RADIUS - MIN_BODY_PARTS - 1;

	if(nbRm < *size - SIZE_PRE_RADIUS){
		//update pastBody
		for( int i = 0; i < nbRm; i++ )
			shiftBomb(pastBody);

		for( int i = 0; i < nbRm; i++ )
			shiftFreeSpace(pastBody);

		for( int i = 0; i < nbRm && *size - nbRm + i >= 0 && *size - nbRm + i < REMIND_BODY; i++ )
			pastBody[i] = (Vector2f){(*snake)[*size - nbRm + i ].x, (*snake)[*size - nbRm + i ].y};

		//deadBodies
		*nbDeadBodies += nbRm;
		*deadBodies = realloc(*deadBodies, *nbDeadBodies * sizeof(SnakePart));
		for( int i = 0; i < nbRm; i++ ){
			(*deadBodies)[*nbDeadBodies - nbRm + i]=(SnakePart){
					(*snake)[*size - nbRm + i].x,
					(*snake)[*size - nbRm + i].y,
					FRUIT_TTL + initFrame + ((i % ratioTexture)?NB_FRAME_DEATH_BODY:0),
					BODY_RADIUS + (*snake)[*size - nbRm + i].radius
			};
		}
		*snake = realloc(*snake, (*size - nbRm) * sizeof(SnakePart));
	}
	else
		nbRm = *size - SIZE_PRE_RADIUS;

	*size -= nbRm;
}


/**
*\fn void drawBody(SDL_Renderer *renderer, SDL_Texture *snakeTexture, SnakePart *snake, size_t size, int frameUnkillable)
*\brief Affiche le serpent
*\param renderer Le renderer où afficher
*\param snakeTexture La texture du serpent
*\param snake Le serpent
*\param size La taille du serpent
*\param frameUnkillable Le nombre de frame où le serpent est invulnérable
*/
void drawBody(SDL_Renderer *renderer, SDL_Texture *snakeTexture, SnakePart *snake, size_t size, int frameUnkillable){
	SDL_Rect currentBody;

	//is blinking ?
	int blinking = 0;
	for( int i = 0; i < NB_BLINK_BODY; i++ )
		if( NB_FRAME_INVINCIBILITY - frameUnkillable == BLINK_FRAMES_BODY[i] )
			blinking = 1;

	int rangee;
	if(blinking)
		rangee = (RANGEE_BLINK_BODY - 1);
	else if(frameUnkillable)
		rangee = (RANGEE_INVICIBLE - 1);
	else
		rangee = 0;

	SDL_Rect src = {0, BODY_DIM.y * rangee , BODY_DIM.x, BODY_DIM.y};

	// Body
	for( int i = size - 1; i > SIZE_PRE_RADIUS; i-- ){
		currentBody = (SDL_Rect){roundf(snake[i].x) - (BODY_RADIUS + snake[i].radius), roundf(snake[i].y) - (BODY_RADIUS + snake[i].radius), (BODY_RADIUS + snake[i].radius) * 2, (BODY_RADIUS + snake[i].radius) * 2};

		SDL_RenderCopy(renderer, snakeTexture, &src, &currentBody);
	}

	// Head
	src.x += BODY_DIM.x;
	currentBody = (SDL_Rect){roundf(snake[SIZE_PRE_RADIUS].x) - (BODY_RADIUS + snake[SIZE_PRE_RADIUS].radius), roundf(snake[SIZE_PRE_RADIUS].y) - (BODY_RADIUS + snake[SIZE_PRE_RADIUS].radius), (BODY_RADIUS + snake[SIZE_PRE_RADIUS].radius) * 2, (BODY_RADIUS + snake[SIZE_PRE_RADIUS].radius) * 2};
	SDL_RenderCopy(renderer, snakeTexture, &src, &currentBody);
}


/**
*\fn int hitboxTail(SnakePart *snake, size_t size)
*\brief Détermine si la tête du serpent touche sa queue
*\param snake Le serpent
*\param size La taille du serpent
*\return Vrai si il touche sa queue, sinon faux
*/
int hitboxTail(SnakePart *snake, size_t size){
	for( int i = BODY_DEATH_HITBOX + SIZE_PRE_RADIUS; i < size; i++ )
		if( norme((Vector2f){snake[SIZE_PRE_RADIUS].x - snake[i].x, snake[SIZE_PRE_RADIUS].y - snake[i].y}) < snake[SIZE_PRE_RADIUS].radius + snake[i].radius + 2 * BODY_RADIUS - HITBOX_GENTILLE )
			return 1;

	return 0;
}


/**
*\fn int tooCloseFromHead(SnakePart head, Fruit fruit)
*\brief Détermine si un fruit est trop proche de la tête pour pouvoir apparaître
*\param head La tête du serpent
*\param fruit Un fruit
*\return Vrai si il est trop proche, sinon faux
*/
int tooCloseFromHead(SnakePart head, Fruit fruit){
	return (norme((Vector2f){fruit.dest.x - head.x, fruit.dest.y - head.y}) < DIST_HEAD_FRUIT + FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE) + BODY_RADIUS + head.radius);
}


/**
*\fn static int tooCloseFromWall(Vector2f fruit, int dist)
*\brief Détermine si un point est trop proche d'un mur par rapport à une distance donnée
*\param point Un point
*\param dist La distance maximale autorisée
*\return Vrai si il est trop proche, sinon faux
*/
static int tooCloseFromWall(Vector2f point, int dist){
	return (point.x < dist  ||  point.x > PLAYGROUND_SIZE_W - dist  ||  point.y < dist  ||  point.y > PLAYGROUND_SIZE_H - dist);
}


/**
*\fn int tooCloseFromCorner(Fruit fruit)
*\brief Détermine si un fruit est trop proche d'un coin pour pouvoir apparaître
*\param fruit Un fruit
*\return Vrai si il est trop proche, sinon faux
*/
int tooCloseFromCorner(Fruit fruit){
	return (norme((Vector2f){fruit.dest.x - 0, fruit.dest.y - 0}) < DIST_CORNER_FRUIT + FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE)
		|| norme((Vector2f){fruit.dest.x - PLAYGROUND_SIZE_W, fruit.dest.y - 0}) < DIST_CORNER_FRUIT + FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE)
		|| norme((Vector2f){fruit.dest.x - PLAYGROUND_SIZE_W, fruit.dest.y - PLAYGROUND_SIZE_H}) < DIST_CORNER_FRUIT + FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE)
		|| norme((Vector2f){fruit.dest.x - 0, fruit.dest.y - PLAYGROUND_SIZE_H}) < DIST_CORNER_FRUIT + FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE));
}


/**
*\fn int tooCloseFromFruit(Fruit* fruitRang, int rang, Fruit* fruitSearch, int nbFruitSearch)
*\brief Détermine si un fruit est trop d'un autre ou d'un bonus pour pouvoir apparaître
*\param fruitRang Le tableau de fruit
*\param rang Le rang du fruit
*\param fruitSearch Le tableau de fruit/bonus où vérifier la proximité
*\param nbFruitSearch Le nombre d'élément dans ce tableau
*\return Vrai si il est trop proche, sinon faux
*/
int tooCloseFromFruit(Fruit* fruitRang, int rang, Fruit* fruitSearch, int nbFruitSearch){
	for( int i = 0; i < nbFruitSearch; i++ )
		if( i != rang  &&  norme((Vector2f){fruitRang[rang].dest.x - fruitSearch[i].x, fruitRang[rang].dest.y - fruitSearch[i].y}) < FRUIT_PROPRIETES[fruitRang[rang].id][RADIUS] + FRUIT_PROPRIETES[fruitSearch[i].id][RADIUS] + DIST_FRUITS )
			return 1;

	return 0;
}


/**
*\fn void spawnFruit(SnakePart head, Fruit *fruit, int rang, int nbFruits, float nbFruitEaten, Fruit *bonus, Vector2f spawn, int spawnCondition, int retardFrame, Mix_Chunk *soundAppear, int hardcore)
*\brief Fait apparaître un fruit
*\param head La tête du serpent
*\param fruit Le tableau de fruit
*\param rang Le rang du fruit à faire apparaître
*\param nbFruits Le nombre de fruits mangés (réduit par les fruits expirés)
*\param nbFruitEaten Le nombre d'élément dans ce tableau
*\param bonus Le bonus
*\param spawn La position d'apparition si elle n'est pas aléatoire
*\param spawnCondition La condition d'apparition ( naturelle, coffre...)
*\param retardFrame Le nombre de frame avant que l'apparition du fruit commence
*\param soundAppear Le son d'apparition des fruits
*\param hardcore Le mode de jeu
*/
void spawnFruit(SnakePart head, Fruit *fruit, int rang, int nbFruits, float nbFruitEaten, Fruit *bonus, Vector2f spawn, int spawnCondition, int retardFrame, Mix_Chunk *soundAppear, int hardcore){
	//if(!hardcore)
		Mix_PlayChannel(6, soundAppear, 0);

	fruit[rang].x = UNDEF.x;
	fruit[rang].y = UNDEF.y;
	fruit[rang].giant = 0;
	fruit[rang].id = NO_ID;
	fruit[rang].hitbox = 0;
	fruit[rang].frame = -NB_FRAME_SPAWN_FRUIT - retardFrame;
	fruit[rang].coefRadius = 1;
	fruit[rang].dest.x = UNDEF.x;
	fruit[rang].dest.y = UNDEF.y;

	//fruit.id
	int nbRand = 0;

	for( int i = 0; i < NB_FRUITS; i++ )
		if( FRUIT_PROPRIETES[i][MIN_FRUIT_TO_APPEAR] <= nbFruitEaten )
			nbRand += FRUIT_PROPRIETES[i][PROBA];

	nbRand = rand() % nbRand;

	int i;
	for( i = 0; i < NB_FRUITS  &&  (nbRand >= FRUIT_PROPRIETES[i][PROBA]  ||  FRUIT_PROPRIETES[i][MIN_FRUIT_TO_APPEAR] > nbFruitEaten) ; i++ )
		if( FRUIT_PROPRIETES[i][MIN_FRUIT_TO_APPEAR] <= nbFruitEaten )
			nbRand -= FRUIT_PROPRIETES[i][PROBA];
	fruit[rang].id = i;

	//fruit.giant
	if( nbFruitEaten >= UNLOCK_GIANT ){
		fruit[rang].giant = rand() % GIANT_CHANCE;
		if( fruit[rang].giant != 1 )
			fruit[rang].giant = 0;
	}

	//fruit coordinates
	if( spawnCondition == FROM_NATURAL ){
		i = -1;
		do{
			fruit[rang].x = rand() % (int)(PLAYGROUND_SIZE_W);
			fruit[rang].y = rand() % (int)(PLAYGROUND_SIZE_H);
			i++;
		}while( i < MAX_TRIES_RAND  &&  (tooCloseFromFruit(fruit, rang, fruit, nbFruits)  ||  tooCloseFromFruit(fruit, rang, bonus, 1)  ||  tooCloseFromHead(head, fruit[rang])  ||  tooCloseFromWall((Vector2f){fruit[rang].dest.x, fruit[rang].dest.y}, DIST_WALL_FRUIT + FRUIT_PROPRIETES[fruit[rang].id][RADIUS] + (fruit[rang].giant * GIANT_SIZE))  ||  tooCloseFromCorner(fruit[rang])) );

		//Moins de conditions de spawn
		if( i == MAX_TRIES_RAND ){
			i = 0;
			do{
				fruit[rang].dest.x = rand() % (int)(PLAYGROUND_SIZE_W);
				fruit[rang].dest.y = rand() % (int)(PLAYGROUND_SIZE_H);
				i++;
			}while( i < MAX_TRIES_RAND  &&  ( tooCloseFromHead(head, fruit[rang]) || tooCloseFromWall((Vector2f){fruit[rang].dest.x, fruit[rang].dest.y}, DIST_WALL_FRUIT + FRUIT_PROPRIETES[fruit[rang].id][RADIUS] + (fruit[rang].giant * GIANT_SIZE)) || tooCloseFromCorner(fruit[rang])) );
		}

		fruit[rang].x = fruit[rang].dest.x;
		fruit[rang].y = fruit[rang].dest.y;
	}
	else{
		fruit[rang].x = spawn.x;
		fruit[rang].y = spawn.y;

		if( spawnCondition == FROM_COFFRE ){
			do{
				float angle = (rand() % 628) / 100;
				fruit[rang].dest.x = spawn.x + (rand() % COFFRE_SPAWN_RANGE + COFFRE_SPAWN_MIN) * cos(angle);
				fruit[rang].dest.y = spawn.y + (rand() % COFFRE_SPAWN_RANGE + COFFRE_SPAWN_MIN) * sin(angle);
			}while( i < MAX_TRIES_RAND  &&  ( tooCloseFromWall((Vector2f){fruit[rang].dest.x, fruit[rang].dest.y}, DIST_WALL_FRUIT + FRUIT_PROPRIETES[fruit[rang].id][RADIUS] + (fruit[rang].giant * GIANT_SIZE)) || tooCloseFromCorner(fruit[rang])) );
		}
		else{
			fruit[rang].dest.x = fruit[rang].x;
			fruit[rang].dest.y = fruit[rang].y;
		}
	}
}


/**
*\fn void spawnBonus(SnakePart head, Fruit* bonus, float nbFruitEaten, Fruit* fruit, int nbFruits)
*\brief Fait apparaître un bonus
*\param head La tête du serpent
*\param bonus Le bonus
*\param nbFruitEaten Le nombre d'élément dans ce tableau
*\param fruit Le tableau de fruits
*\param nbFruits Le nombre de fruits mangés (réduit par les fruits expirés)
*/
void spawnBonus(SnakePart head, Fruit* bonus, float nbFruitEaten, Fruit* fruit, int nbFruits){

	bonus->x = UNDEF.x;
	bonus->y = UNDEF.x;
	bonus->giant = 0;
	bonus->id = NO_ID;
	bonus->hitbox = 0;
	bonus->frame = -NB_FRAME_SPAWN_FRUIT;
	bonus->dest = (Vector2f){UNDEF.x, UNDEF.y};
	bonus->coefRadius = 1;

	//bonus.id
	int nbRand = 0;

	for( int i = NB_FRUITS; i < NB_FRUITS + NB_BONUSES; i++ )
		if( FRUIT_PROPRIETES[i][MIN_FRUIT_TO_APPEAR] <= nbFruitEaten )
			nbRand += FRUIT_PROPRIETES[i][PROBA];

	nbRand = rand() % nbRand;

	int i;
	for( i = NB_FRUITS; i < NB_FRUITS + NB_BONUSES  &&  (nbRand >= FRUIT_PROPRIETES[i][PROBA] || FRUIT_PROPRIETES[i][MIN_FRUIT_TO_APPEAR] > nbFruitEaten) ; i++ )
		if( FRUIT_PROPRIETES[i][MIN_FRUIT_TO_APPEAR] <= nbFruitEaten )
			nbRand -= FRUIT_PROPRIETES[i][PROBA];

	bonus->id = i;

	//bonus coordinates
	i = -1;
	do{
		bonus->dest.x = rand() % (int)(PLAYGROUND_SIZE_W);
		bonus->dest.y = rand() % (int)(PLAYGROUND_SIZE_H);
		i++;
	}while( i < MAX_TRIES_RAND  &&  (tooCloseFromFruit(bonus, 0,fruit, nbFruits)  ||  tooCloseFromHead(head, *bonus)  ||  tooCloseFromWall((Vector2f){bonus->dest.x, bonus->dest.y}, DIST_WALL_FRUIT + FRUIT_PROPRIETES[bonus->id][RADIUS] + (bonus->giant * GIANT_SIZE))  ||  tooCloseFromCorner(*bonus)) );

	//Moins de conditions de spawn
	if( i == MAX_TRIES_RAND ){
		i = 0;
		do{
			bonus->dest.x = rand() % (int)(PLAYGROUND_SIZE_W);
			bonus->dest.y = rand() % (int)(PLAYGROUND_SIZE_H);
			i++;
		}while( i < MAX_TRIES_RAND  &&  ( tooCloseFromHead(head, *bonus) || tooCloseFromWall((Vector2f){bonus->dest.x, bonus->dest.y}, DIST_WALL_FRUIT + FRUIT_PROPRIETES[bonus->id][RADIUS] + (bonus->giant * GIANT_SIZE)) || tooCloseFromCorner(*bonus)) );
	}

	bonus->x = bonus->dest.x;
	bonus->y = bonus->dest.y;
}


/**
*\fn void reachDest(Fruit* fruit, size_t nbFruits)
*\brief Déplace les fruits lors de l'animation de coffre
*\param fruit Le tableau de fruits
*\param nbFruits Le nombre de fruits mangés (réduit par les fruits expirés)
*/
void reachDest(Fruit* fruit, size_t nbFruits){
	for( int i=0; i < nbFruits; i++ )
		if( fruit[i].frame < 0  &&  fruit[i].frame >= -NB_FRAME_SPAWN_FRUIT  &&  fruit[i].dest.x != fruit[i].x ){
			fruit[i].coefRadius = 1 + (fabs(fabs(-fruit[i].frame - NB_FRAME_SPAWN_FRUIT/2.) - NB_FRAME_SPAWN_FRUIT/2.) / (3 * NB_FRAME_SPAWN_FRUIT/2.));
			fruit[i].x += ((NB_FRAME_SPAWN_FRUIT + (float)fruit[i].frame) / NB_FRAME_SPAWN_FRUIT) * (fruit[i].dest.x - fruit[i].x);
			fruit[i].y += ((NB_FRAME_SPAWN_FRUIT + (float)fruit[i].frame) / NB_FRAME_SPAWN_FRUIT) * (fruit[i].dest.y - fruit[i].y);
		}
}


/**
*\fn int hitboxFruit(Fruit fruit, SnakePart head)
*\brief Détermine si la tête du serpent est sur un fruit
*\param fruit Le fruit à tester
*\param head La tête du serpent
*\return Vrai si collision, sinon faux
*/
int hitboxFruit(Fruit fruit, SnakePart head){
	return (norme((Vector2f){fruit.x - head.x, fruit.y - head.y}) <= (FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE) + BODY_RADIUS + head.radius) * HITBOX_SECURITY);
}


/**
*\fn void drawFruit(SDL_Renderer *renderer, SDL_Texture *fruitTexture, SDL_Texture *spawnTexture, Fruit fruit, int hardcore)
*\brief Affiche un fruit
*\param renderer Le renderer où afficher
*\param fruitTexture La texture de fruits
*\param spawnTexture La texture d'animation d'apparition
*\param fruit Le fruit à afficher
*\param hardcore La difficulté du jeu
*/
void drawFruit(SDL_Renderer *renderer, SDL_Texture *fruitTexture, SDL_Texture *spawnTexture, Fruit fruit, int hardcore){
	SDL_Rect currentBody = (SDL_Rect){
		roundf(fruit.x) - (FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE)) * fruit.coefRadius,
		roundf(fruit.y) - (FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE)) * fruit.coefRadius,
		(FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE)) * 2 * fruit.coefRadius,
		(FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE)) * 2 * fruit.coefRadius
	};

	int blinking = 0;
	if( fruit.frame >= (hardcore? RATIO_TTL_HARDCORE : 1 ) * BLINK_FRAMES[0] )
		for( int i = 0; i < NB_BLINK; i++ )
			if( fruit.frame == (hardcore? RATIO_TTL_HARDCORE : 1 ) * BLINK_FRAMES[i] )
				blinking = 1;


	if( blinking ){ //blink
		SDL_Rect src = (SDL_Rect){fruit.id * FRUIT_DIM.x, FRUIT_DIM.y * (RANGEE_BLINK - 1), FRUIT_DIM.x, FRUIT_DIM.y};
		SDL_RenderCopy(renderer, fruitTexture, &src, &currentBody);
	}
	else if( fruit.frame >= FRUIT_TTL * (hardcore ? RATIO_TTL_HARDCORE : 1) ){ //die
		SDL_Rect src = (SDL_Rect){(fruit.frame - FRUIT_TTL * (hardcore ? RATIO_TTL_HARDCORE : 1)) / (NB_FRAME_DEATH_FRUIT / NB_ANIM_DEATH) * FRUIT_DIM.x, FRUIT_DIM.y, FRUIT_DIM.x, FRUIT_DIM.y};
		SDL_RenderCopy(renderer, spawnTexture, &src, &currentBody);
	}
	else if( fruit.frame >= 0 ){ //Normal
		SDL_Rect src = (SDL_Rect){fruit.id * FRUIT_DIM.x, 0, FRUIT_DIM.x, FRUIT_DIM.y};
		SDL_RenderCopy(renderer, fruitTexture, &src, &currentBody);
	}
	else if (fruit.frame > -NB_FRAME_SPAWN_FRUIT){ //spawn
		SDL_Rect src = (SDL_Rect){(fruit.frame + NB_FRAME_SPAWN_FRUIT) / (NB_FRAME_SPAWN_FRUIT / NB_ANIM_SPAWN) * FRUIT_DIM.x, 0, FRUIT_DIM.x, FRUIT_DIM.y};
		SDL_RenderCopy(renderer, spawnTexture, &src, &currentBody);
	}
}

/**
*\fn static void drawHelpText(SDL_Renderer *renderer, SDL_Texture *flecheTexture)
*\brief Affiche les commandes
*\param renderer Le renderer où afficher
*\param flecheTexture La texture des flèches
*/
static void drawHelpText(SDL_Renderer *renderer, SDL_Texture *flecheTexture){

	//direction
	SDL_SetTextureColorMod(flecheTexture, HUD_COLOR.r, HUD_COLOR.g, HUD_COLOR.b);
	SDL_RenderCopyEx(renderer, flecheTexture, &FLECHE_SRC, &FLECHE_DEST[0], 90, NULL, SDL_FLIP_NONE);
	SDL_RenderCopyEx(renderer, flecheTexture, &FLECHE_SRC, &FLECHE_DEST[1], 0, NULL, SDL_FLIP_NONE);
	SDL_RenderCopyEx(renderer, flecheTexture, &FLECHE_SRC, &FLECHE_DEST[2], 0, NULL, SDL_FLIP_HORIZONTAL);
}

/**
*\fn void drawPotions(SDL_Renderer* renderer, int nbPotion, SDL_Texture * fruitTexture, float ratioWindowSize)
*\brief Affiche les potions possédés ou non
*\param renderer Le renderer où afficher
*\param nbPotion Le nombre de potions
*\param fruitTexture La texture des fruits
*\param ratioWindowSize Le ratio tailleBase/tailleActuelle
*/
void drawPotions(SDL_Renderer* renderer, int nbPotion, SDL_Texture * fruitTexture, float ratioWindowSize){

	SDL_Rect src = {POTION_HITBOX * FRUIT_DIM.x, 0, FRUIT_DIM.x, FRUIT_DIM.y };
	SDL_Rect dest = SHOW_POTION_DEST;
	dest.w *= ratioWindowSize;
	dest.h *= ratioWindowSize;
	dest.x = HUD_W*(ratioWindowSize)/2 - dest.w/2;
	dest.y *= ratioWindowSize;
	for(int i=0; i<NB_POTION_MAX; i++){
		if(i+1 > nbPotion)
			SDL_SetTextureAlphaMod(fruitTexture, 50);
		SDL_RenderCopy(renderer, fruitTexture, &src, &dest);
		dest.y += ESPACEMENT_SHOW_POTION * ratioWindowSize;

	}
	SDL_SetTextureAlphaMod(fruitTexture, 255);
}

/**
*\fn static void drawJauge(SDL_Renderer *renderer, SDL_Texture *basketTexture, float ratioWindowSize, float jaugeValue)
*\brief Affiche la jauge
*\param renderer Le renderer où afficher
*\param basketTexture La texture de la jauge
*\param ratioWindowSize Le ratio tailleBase/tailleActuelle
*\param jaugeValue La valeur de la jauge
*/
static void drawJauge(SDL_Renderer *renderer, SDL_Texture *basketTexture, float ratioWindowSize, float jaugeValue){


	float ratioFruti = (jaugeValue+BASE_JAUGE) / (APPEAR_MAX+BASE_JAUGE);
	if(ratioFruti > 1 )
		ratioFruti = 1;

	//texture jauge
	SDL_Rect dest = BASKET_DIM;
	dest.w *= ratioWindowSize;
	dest.h *= ratioWindowSize;
	dest.x = (BASE_WINDOW_W - HUD_W/2)*ratioWindowSize - dest.w/2;
	dest.y = BASE_WINDOW_H*ratioWindowSize/2 - dest.h/2;

	//background
	SDL_Rect jauge = dest;
	jauge.x+= 0.2*jauge.w;
	jauge.w -= 0.4*jauge.w;
	jauge.y += 0.01 * jauge.h;
	jauge.h -= 0.02 * jauge.h;
	SDL_SetRenderDrawColor(renderer, COLOR_JAUGE_BACK.r, COLOR_JAUGE_BACK.g, COLOR_JAUGE_BACK.b, 255);
	SDL_RenderFillRect(renderer, &jauge);

	//foreground
	jauge.y += (jauge.h*(1-ratioFruti));
	jauge.h *= ratioFruti;
	SDL_SetRenderDrawColor(renderer, COLOR_JAUGE.r, COLOR_JAUGE.g, COLOR_JAUGE.b, 255);
	SDL_RenderFillRect(renderer, &jauge);

	SDL_RenderCopy(renderer, basketTexture, NULL, &dest);
}


/**
*\fn void drawDeadBody(SDL_Renderer *renderer, SDL_Texture *deadBodyTexture, SnakePart deadBody)
*\brief Affiche une animation de partie de serpent morte
*\param renderer Le renderer où afficher
*\param deadBodyTexture La texture d'animation de mort
*\param deadBody La partie morte à afficher
*/
void drawDeadBody(SDL_Renderer *renderer, SDL_Texture *deadBodyTexture, SnakePart deadBody){
	SDL_Rect currentBody = (SDL_Rect){
		roundf(deadBody.x) - deadBody.radius,
		roundf(deadBody.y) - deadBody.radius,
		deadBody.radius * 2,
		deadBody.radius * 2
	};

	SDL_Rect src = (SDL_Rect){(deadBody.frame - FRUIT_TTL) / (NB_FRAME_DEATH_BODY / NB_ANIM_DEATH_BODY) * FRUIT_DIM.x, FRUIT_DIM.y, FRUIT_DIM.x, FRUIT_DIM.y};
	SDL_RenderCopy(renderer, deadBodyTexture, &src, &currentBody);

}


/**
*\fn static void drawTotalScore(SDL_Renderer *renderer, TTF_Font *font, int scoreShow, float ratioWindowSize)
*\brief Affiche une animation de partie de serpent morte
*\param renderer Le renderer où afficher
*\param font La police d'écriture
*\param scoreShow Le score total à afficher
*\param ratioWindowSize Le ratio tailleBase/tailleActuelle
*/
static void drawTotalScore(SDL_Renderer *renderer, TTF_Font *font, int scoreShow, float ratioWindowSize){
	char msgScore[20];
	sprintf(msgScore, "%d", scoreShow);
	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, msgScore, TOTAL_SCORE_COLOR);
	SDL_Rect dest ;

	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	SDL_SetTextureAlphaMod(Message, 200);
	SDL_QueryTexture(Message,NULL,(int*)SDL_TEXTUREACCESS_STATIC,&(dest.w), &(dest.h) );
	dest.w *= (0.8*HUD_H*ratioWindowSize)/dest.h;
	dest.h *= (0.8*HUD_H*ratioWindowSize)/dest.h;

	dest.x = BASE_WINDOW_W*ratioWindowSize/2 - dest.w/2;
	dest.y = HUD_H*ratioWindowSize/2 - dest.h/2;

	SDL_RenderCopy(renderer, Message, NULL, &dest);

	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
}


/**
*\fn void shiftRadius(SnakePart **snake, size_t *size, Vector2f pastBody[REMIND_BODY], float *radiusRestant)
*\brief Décalle la taille des parties de serpents (digestion)
*\param snake Le serpent
*\param size La taille du serpent
*\param pastBody Les anciennes positions du serpent
*\param radiusRestant La taille de la dernière partie de la queue du serpent
*/
void shiftRadius(SnakePart **snake, size_t *size, Vector2f pastBody[REMIND_BODY], float *radiusRestant){
	int nbAdd = 0;

	*radiusRestant += (*snake)[*size - 1].radius;
	// Si la queue est grosse, on ralonge le serpent
	if( *radiusRestant >= RATIO_RADIUS_BODYADD )
		nbAdd = addBody(snake, size, pastBody, radiusRestant);

	// On décale tout
	for( int i = *size - 2; i >= 0; i-- )
		(*snake)[i + 1].radius = (*snake)[i].radius;

	(*snake)[0].radius = 0;

	// On décale le corps digéré jusqu'au bout de la queue
	if( nbAdd ){
		int i;

		for( i = *size - 2 - nbAdd / 3; !(*snake)[i].radius  &&  i >= 0; i-- );

		for( ; (*snake)[i].radius  &&  i >= 0; i-- )
			if(i + nbAdd - 1>=0 && i + nbAdd - 1 < *size)
				(*snake)[i + nbAdd - 1].radius = (*snake)[i].radius;

		for( int j = 0; j < nbAdd; j++ ){
			if(i + j- 1 >=0 && i + j- 1 < *size)
				(*snake)[i + j- 1].radius = 0;
		}

	}
}


/**
*\fn float moveSnake(SnakePart** snake, Vector2f pastBody[REMIND_BODY], size_t *size, float speed, double dir, Fruit *fruit, size_t nbFruits, Fruit *bonus, float *radiusRestant, int hardcore)
*\brief Décalle la taille des parties de serpents (digestion)
*\param snake Le serpent
*\param pastBody Les anciennes positions du serpent
*\param size La taille du serpent
*\param speed La vitesse du serpent
*\param dir La direction du serpent
*\param fruit Le tableau de fruits
*\param nbFruits Le nombre de fruits
*\param bonus Le bonus
*\param radiusRestant La taille de la dernière partie de la queue du serpent
*\param hardcore La difficulté du jeu
*\return La distance qui n'a pas été parcourue
*/
float moveSnake(SnakePart** snake, Vector2f pastBody[REMIND_BODY], size_t *size, float speed, double dir, Fruit *fruit, size_t nbFruits, Fruit *bonus, float *radiusRestant, int hardcore)
{
	// Verif speed not too high
	if( speed == SPEED_DECOMPOSITION ){
		// Se souvenir de l'emplacement qui va partir
		shiftFreeSpace(pastBody);
		pastBody[0].x = (*snake)[*size - 1].x;
		pastBody[0].y = (*snake)[*size - 1].y;

		// Bouger
		for( int i = *size - 1; i > SIZE_PRE_RADIUS; i-- ){
			(*snake)[i].x = (*snake)[i - 1].x;
			(*snake)[i].y = (*snake)[i - 1].y;
		}

		(*snake)[SIZE_PRE_RADIUS].x += cos(dir) * speed;
		(*snake)[SIZE_PRE_RADIUS].y += sin(dir) * speed;

		for( int i = 0; i<nbFruits; i++ )
			if( fruit[i].frame > 0  &&  fruit[i].frame < FRUIT_TTL * (hardcore ? RATIO_TTL_HARDCORE : 1)  &&  hitboxFruit(fruit[i], (*snake)[SIZE_PRE_RADIUS]) )
				fruit[i].hitbox = HIT;


		if( bonus->frame > 0  &&  bonus->frame < FRUIT_TTL * (hardcore ? RATIO_TTL_HARDCORE : 1)  &&  hitboxFruit(*bonus, (*snake)[SIZE_PRE_RADIUS]) )
			bonus->hitbox = HIT;

		return speed;
	}
	else{
		// On décompose le mouvement en mouvements de speed inférieurs
		int i=0;
		while( speed > SPEED_DECOMPOSITION ){
			i++;

			moveSnake(snake, pastBody, size, SPEED_DECOMPOSITION, dir, fruit, nbFruits, bonus, radiusRestant, hardcore);
			speed -= SPEED_DECOMPOSITION;

			shiftRadius(snake, size, pastBody, radiusRestant);
			if( i % 2 )
				shiftRadius(snake, size, pastBody, radiusRestant);
		}
	}
	return speed;
}



/**
*\fn int scoreSize(Fruit fruit)
*\brief Détermine la taille d'affichage du score d'un fruit
*\param fruit Le fruit
*\return Sa taille d'affichage de score
*/
int scoreSize(Fruit fruit){
	int size = 0.15 + 3.65 * log((int)(fruit.giant == 0 ? 1 : GIANT_SCORE) * 10 *  FRUIT_PROPRIETES[fruit.id][SCORE]);

	if( size < MIN_SIZE_SCORE )
		size = MIN_SIZE_SCORE;
	else if( size > MAX_SIZE_SCORE )
		size = MAX_SIZE_SCORE;

	return size;
}


/**
*\fn int eatFruit(SnakePart **snake, size_t *size, Fruit** fruitTab, size_t* nbFruits, Fruit fruit, float *speed, ScoreTotal *score, float *nbFruitEaten, int *frameJaugeAnim, Vector2f* pastBody, Fruit* bonus, SnakePart **deadBodies, size_t *nbDeadBodies, int* nbPotion, float* poisoned, Score** scoreAffichage, size_t* nbScoreAffichage, int *frameAcce, Mix_Chunk ** sounds, long long *score_hash, long keys[4], int hardcore)
*\brief Mange un fruit
*\param snake Le serpent
*\param size La taille du serpent
*\param fruitTab Les fruits
*\param nbFruits Le nombre de fruits
*\param fruit Le fruit à manger
*\param speed La vitesse du serpent
*\param score Le score total
*\param nbFruitEaten Le nombre de fruits mangés
*\param frameJaugeAnim Le nombre de frame pour l'animation de jauge
*\param pastBody Les anciennes positions du serpent
*\param bonus Le nonus
*\param deadBodies Les animations de mort des parties du serpent
*\param nbDeadBodies Le nombre d'animations de mort des parties du serpent
*\param nbPotion Le nombre de potions
*\param poisoned La quantité de poison ingéré
*\param scoreAffichage Les affichages de scores
*\param nbScoreAffichage Le nombre d'affichage de scores
*\param frameAcce Le nombre de frame où l'accélération est forcée
*\param flap_wav Le son de manger fruit
*\param score_hash Le score hashé
*\param keys Les clés de hashage
*\param hardcore La difficulté du jeu
*\return Faux en cas de problème dans le hashage, sinon vrai
*/
int eatFruit(SnakePart **snake, size_t *size, Fruit** fruitTab, size_t* nbFruits, Fruit fruit, float *speed, ScoreTotal *score, float *nbFruitEaten, int *frameJaugeAnim, Vector2f* pastBody, Fruit* bonus, SnakePart **deadBodies, size_t *nbDeadBodies, int* nbPotion, float* poisoned, Score** scoreAffichage, size_t* nbScoreAffichage, int *frameAcce, Mix_Chunk ** sounds, long long *score_hash, long keys[4], int hardcore){

	Mix_PlayChannel(7, sounds[SOUND_GET], 0);

	//manger
	if( fruit.giant ){
		for( int i = 0; i < SIZE_PRE_RADIUS; i++ )
			(*snake)[i].radius += (hardcore ? RATIO_RADIUS_HARDCORE : 1) * FRUIT_ADD_RADIUS_GIANT[fruit.id][i];
	}
	else{
		for( int i = 0; i < SIZE_PRE_RADIUS; i++ )
			(*snake)[i].radius += (hardcore ? RATIO_RADIUS_HARDCORE : 1) * FRUIT_ADD_RADIUS[fruit.id][i];
	}

	//accelerate
	*speed += (hardcore ? RATIO_SPEED_HARDCORE : 1) * (fruit.giant == 0 ? 1 : GIANT_SPEED) * FRUIT_PROPRIETES[fruit.id][ACCELERATION];

	if( *speed < MIN_SPEED )
		*speed = MIN_SPEED;

	//score
	int scoreFruit = (int)((hardcore && fruit.id != POTION_JAUNE ? RATIO_GET_FRUIT_HARDCORE : 1) * (fruit.giant == 0 ? 1 : GIANT_SCORE) * FRUIT_PROPRIETES[fruit.id][SCORE]);
	if(FRUIT_PROPRIETES[fruit.id][SCORE]){
		if(!changeProtectedVar(score_hash, &(score->score), (score->score) + scoreFruit, keys))
			return SDL_FALSE;
		score->frameToDest = FRAME_SCORE_ANIM;

		//afficher score
		(*nbScoreAffichage)++;
		*scoreAffichage = realloc(*scoreAffichage, *nbScoreAffichage * sizeof(Score) );
		int sizeAffichage = scoreSize(fruit);
		(*scoreAffichage)[*nbScoreAffichage - 1] = (Score){
			fruit.x,
			fruit.y,
			scoreFruit,
			0,
			sizeAffichage,
			SDL_FALSE
		};
	}

	//update nbEaten
	if( fruit.id < NB_FRUITS ){
		(*nbFruitEaten)+= (hardcore ? FRUIT_EATEN_HARDCORE : 1);
		if(*nbFruitEaten<0)
			*nbFruitEaten = 0;
		*frameJaugeAnim = FRAME_JAUGE_ANIM;
	}


	//check malus
	if( fruit.id == CAFE )
		*frameAcce += NB_FRAME_CAFE_ACCE;

	//check bonus
	else if( fruit.id >= NB_FRUITS ){
		switch ( fruit.id ) {
			case BOMBE:
				Mix_PlayChannel(8, sounds[SOUND_BOMB],0);
				if(*size<MAX_BOMB_SIZE)
					rmBody(snake, size, FLAT_RM_BOMB + RELATIVE_RM_BOMB * (*size - SIZE_PRE_RADIUS), pastBody, deadBodies, nbDeadBodies, 1, BOMB_TEXTURE_RATE, 0);
				else
					rmBody(snake, size, FLAT_RM_BOMB + RELATIVE_RM_BOMB * (MAX_BOMB_SIZE - SIZE_PRE_RADIUS), pastBody, deadBodies, nbDeadBodies, 1, BOMB_TEXTURE_RATE, 0);

				break;

			case PLUME:
				Mix_PlayChannel(8, sounds[SOUND_SLOW],0);
				*speed = *speed - RELATIVE_SLOW_FEATHER * (*speed - MIN_SPEED);
				break;

			case COFFRE:
				*fruitTab = realloc(*fruitTab, (*nbFruits + NB_SPAWN_COFFRE) * sizeof(Fruit));
				*nbFruits += NB_SPAWN_COFFRE;
				for( int i=0; i < NB_SPAWN_COFFRE; i++ )
					spawnFruit((*snake)[SIZE_PRE_RADIUS], *fruitTab, *nbFruits - NB_SPAWN_COFFRE + i, *nbFruits - NB_SPAWN_COFFRE + 1 + i, *nbFruitEaten + CHEST_BOOST_FRUIT_EATEN, bonus, (Vector2f){fruit.x, fruit.y}, FROM_COFFRE, FRAME_RETARD_COFFRE, sounds[SOUND_APPEAR], hardcore);
				break;

			case ARC_EN_CIEL:
				Mix_PlayChannel(8, sounds[SOUND_RAINBOW],0);
				*fruitTab = realloc(*fruitTab, (*nbFruits + NB_SPAWN_ARC_EN_CIEL) * sizeof(Fruit));
				*nbFruits += NB_SPAWN_ARC_EN_CIEL;

				Vector2f corner;
				if( fruit.y > PLAYGROUND_SIZE_H/2 )
					if( fruit.x > PLAYGROUND_SIZE_W/2 )
						corner = (Vector2f){PLAYGROUND_SIZE_W, 0};
					else
						corner = (Vector2f){0, 0};
				else{
					if( fruit.x > PLAYGROUND_SIZE_W/2 )
						corner = (Vector2f){PLAYGROUND_SIZE_W, PLAYGROUND_SIZE_H};
					else
						corner = (Vector2f){0, PLAYGROUND_SIZE_H};
				}

				float angle = 0;
				float dist = norme( (Vector2f){corner.x - fruit.x, corner.y - fruit.y} );

				while(tooCloseFromWall( (Vector2f){corner.x + cos(angle) * dist, corner.y + sin(angle) * dist}, DIST_WALL_FRUIT_RAINBOW))
					angle+=0.01;

				float minAngle = angle;

				while(!tooCloseFromWall((Vector2f){corner.x + cos(angle) * dist, corner.y + sin(angle) * dist}, DIST_WALL_FRUIT_RAINBOW))
					angle+=0.01;

				float rangeAngle = angle - minAngle;

				for( int i=0; i < NB_SPAWN_ARC_EN_CIEL; i++ ){
					spawnFruit((*snake)[SIZE_PRE_RADIUS], *fruitTab, *nbFruits - NB_SPAWN_ARC_EN_CIEL + i, *nbFruits - NB_SPAWN_ARC_EN_CIEL + 1 + i, *nbFruitEaten + RAINBOW_BOOST_FRUIT_EATEN, bonus, (Vector2f){corner.x + cos(minAngle) * dist, corner.y + sin(minAngle) * dist}, FROM_RAINBOW, i * FRAME_RETARD_RAINBOW, sounds[SOUND_APPEAR], hardcore);
					minAngle += rangeAngle / (NB_SPAWN_ARC_EN_CIEL - 1);
				}
				break;

			case POTION_HITBOX:
				(*nbPotion)++;
				break;

			case POTION_VERTE:
				(*poisoned) += POTION_POISON_ADD;
				break;
		}
	}
	return SDL_TRUE;
}


/**
*\fn static void drawScore(SDL_Renderer *renderer , SDL_Texture *scoreTexture, Score scoreAffichage)
*\brief Affiche un score de terrain
*\param renderer Le renderer où afficher
*\param scoreTexture La texture de score
*\param scoreAffichage Le score à afficher
*/
static void drawScore(SDL_Renderer *renderer , SDL_Texture *scoreTexture, Score scoreAffichage)
{
	int negative = SDL_FALSE;
	if (scoreAffichage.score < 0 ){
		scoreAffichage.score*=-1;
		negative = 1;
	}
	SDL_SetTextureAlphaMod(scoreTexture, scoreAffichage.fromTimeOut ? ALPHA_SCORE_TIMEOUT[scoreAffichage.frame] : ALPHA_SCORE[scoreAffichage.frame]);
	int taille = lenNum(scoreAffichage.score) + negative;

	SDL_Rect src = SCORE_SRC;
	SDL_Rect dest = {scoreAffichage.x + (scoreAffichage.size*taille)/2 , scoreAffichage.y - (FONT_HEIGHT_RATIO*scoreAffichage.size) / 2 , scoreAffichage.size, FONT_HEIGHT_RATIO*scoreAffichage.size };

	dest.x -= dest.w;

	//parcourt le score et l'affiche
	for(int i=0 ; i < taille-negative; i++)
	{
		src.x = SCORE_SRC.w * (scoreAffichage.score%10);

		SDL_RenderCopy(renderer,scoreTexture,&src,&dest);
		scoreAffichage.score /=10;
		dest.x -= scoreAffichage.size;
	}
	if(negative){
		src.x = SCORE_SRC.w * 10;
		SDL_RenderCopy(renderer,scoreTexture,&src,&dest);
	}
}

/**
*\fn static void myFrees(Fruit ** fruits, SnakePart ** deadBodies, SnakePart **snakeBody, Score ** scoreAffichage, Mix_Chunk ** sounds, SDL_Texture * textures[NB_SNAKE_TEXTURES], TTF_Font * fonts[NB_SNAKE_FONTS], SDL_Thread** thread)
*\brief Affiche un score de terrain
*\param fruits Le tableau de fruits
*\param deadBodies Le tableau de parties de serpent mortes
*\param snakeBody  Le tableau de parties de serpent
*\param scoreAffichage  Le tableau de score à afficher
*\param sound  Le tableau de sons
*\param textures  Le tableau de textures
*\param fonts Le tableau de polices
*\param thread Le thread
*/
static void myFrees(Fruit ** fruits, SnakePart ** deadBodies, SnakePart **snakeBody, Score ** scoreAffichage, Mix_Chunk ** sounds, SDL_Texture * textures[NB_SNAKE_TEXTURES], TTF_Font * fonts[NB_SNAKE_FONTS], SDL_Thread** thread){
	if(*fruits){
		free(*fruits);
		*fruits = NULL;
	}

	if(*snakeBody){
		free(*snakeBody);
		*snakeBody = NULL;
	}

	if(*deadBodies){
		free(*deadBodies);
		*deadBodies = NULL;
	}

	if(*scoreAffichage){
		free(*scoreAffichage);
		*scoreAffichage = NULL;
	}

	for(int i=0; i<NB_SNAKE_SOUNDS; i++){
		if(sounds[i]){
			Mix_FreeChunk(sounds[i]);
			sounds[i] = NULL;
		}
	}

	for(int i=0; i<NB_SNAKE_TEXTURES; i++){
		if(textures[i]){
			SDL_DestroyTexture(textures[i]);
			textures[i] = NULL;
		}
	}

	for(int i=0; i<NB_SNAKE_FONTS; i++){
		if(fonts[i]){
			TTF_CloseFont(fonts[i]);
			fonts[i] = NULL;
		}
	}

	if(*thread){
		SDL_WaitThread(*thread, NULL);
		*thread = NULL;
	}
}

extern int updateEnded; /** < Changée dans le thread pour savoir s'il est finit > */
/**
*\fn int snake(SDL_Renderer * renderer,int highscore, int WinWidth, int WinHeight, char *token, int hardcore, SDL_Texture ** textures)
*\brief Affiche un score de terrain
*\param renderer Le renderer
*\param highscore Le meilleur score personnel
*\param WinWidth La largeur de la fenêtre
*\param WinHeight La hauteur de la fenêtre
*\param token Le token pour l'envoi des scores
*\param hardcore  Le difficulté du jeu
*/
int snake(SDL_Renderer * renderer,int highscore, int WinWidth, int WinHeight, char *token, int hardcore, SDL_Texture ** textures){
/////////////////////
/// MISE EN PLACE ///``
/////////////////////
	snakeInit();

	float ratioWindowSize = (float)WinWidth/BASE_WINDOW_W;
	SDL_Rect playgroundView = {HUD_W*ratioWindowSize, HUD_H*ratioWindowSize, PLAYGROUND_SIZE_W*ratioWindowSize, PLAYGROUND_SIZE_H*ratioWindowSize};
	SDL_RenderSetViewport(renderer, &playgroundView);
	SDL_RenderSetScale(renderer, ratioWindowSize, ratioWindowSize);
	SDL_SetRenderDrawColor(renderer, HUD_COLOR.r, HUD_COLOR.g, HUD_COLOR.b, 255);
	SDL_RenderClear(renderer);

	//dimwindow
	int w, h;
	SDL_GetRendererOutputSize(renderer, &w, &h);

	//envoi score en thread
	SDL_Thread *thread = NULL;
	char buffer[10];
	EnvoiScore envoiScore;
	updateEnded = SDL_FALSE;
	int retour = EXIT_FAILURE;
	int frameRetour = 0;
	int frame_anim_loading = 0;

	//Textures
	SDL_SetTextureColorMod(textures[S_HUD], HUD_COLOR.r, HUD_COLOR.g, HUD_COLOR.b);

	//Fonts
	TTF_Font* fonts[NB_SNAKE_FONTS];
	for(int i=0; i< NB_SNAKE_FONTS; i++){
		 fonts[i] =  TTF_OpenFont(DIR_FONTS_SNAKE[i], OPEN_FONT_SIZE);
		 if( fonts[i] == NULL ){
			printf("Erreur lors de la creation de font %s", TTF_GetError());
			return EXIT_FAILURE;
		}
	}

	//audio
	Mix_Chunk* sounds[NB_SNAKE_SOUNDS];

	for(int i=0; i< NB_SNAKE_SOUNDS; i++){
		 sounds[i] = Mix_LoadWAV(DIR_SOUNDS_SNAKE[i]);
		 if( sounds[i] == NULL ){
			printf("Erreur lors de la creation de son %s %s", DIR_SOUNDS_SNAKE[i], Mix_GetError());
			return EXIT_FAILURE;
		}
	}

	const Uint8 *keystate = SDL_GetKeyboardState(NULL);

	//Fruits
	size_t nbFruits = 1;
	Fruit *fruit = NULL;

	//snake
	size_t snakeSize;
	SnakePart *snakeBody = malloc(sizeof(snakeBody));
	Vector2f pastBody[REMIND_BODY];
	SnakePart *deadBodies = NULL;
	size_t nbDeadBodies = 0;



	int firstframe = SDL_TRUE;
	while(1){
		////////////
		/// Vars ///`
		////////////
		//Keyboard

		int accelerate = ACCELERATE_DISABLED;

		//Time
		unsigned int lastTime = 0, currentTime;
		int rdyToSpace = SDL_TRUE;
		int rdyToUp = SDL_FALSE;
		int rdyToLeft = SDL_FALSE;
		int rdyToRight = SDL_FALSE;


		//Quantities of things gotten
		int nbPotion = 0;
		int frameUnkillable = 0;
		float nbFruitEaten = 0;
		float jaugeValue = 0;

		//fruits
		nbFruits = 1;
		initFruit(&fruit);

		//Snake
		snakeSize = 0;
		initBody(&snakeSize, &snakeBody);

		//dead bodies
		nbDeadBodies = 0;
		deadBodies = realloc(deadBodies, sizeof(SnakePart));

		initToV(pastBody, UNDEF, 0, REMIND_BODY);

		//attributs snake
		int turn;
		double dirAngle = BASE_ANGLE;
		float speedSnake = BASE_SPEED;
		double speedRestant = 0;
		float radiusRestant = 0;
		float poisoned = 0;
		float poison = 0;
		float finalDeath = 0;
		float finalDeathRate = FINAL_DEATH_RATE_INIT;
		int frameAcce = 0;

		float chance_spawn_hardcore = CHANCE_SPAWN_FRUIT_HARDCORE_INIT;

		//Bonus
		Fruit bonus;

		//Score
		ScoreTotal score = {0, 0, 0};
		long keys[4];
		initialisationConstantHashage(keys);
		long long score_hash = hashage(score.score, keys);
		int sentScore = SDL_FALSE;

		Score *scoreAffichage = malloc(sizeof(Score));
		size_t nbScoreAffichage = 0;
		int frameJaugeAnim = 0;
		int frameDead = 0;

		//sound
		for(int i=0; i<NB_SNAKE_SOUNDS;i++)
			Mix_VolumeChunk(sounds[i], SOUND_VOLUMES[i]);


		//mouse
		SDL_Point mouseCoor;

		//Bonus
		bonus = (Fruit){UNDEF.x, UNDEF.y, 0, 0, NO_HIT, FRUIT_TTL * (hardcore ? RATIO_TTL_HARDCORE : 1) + NB_FRAME_DEATH_FRUIT + 1};

		//Fruit
		if(!firstframe)
			spawnFruit(snakeBody[SIZE_PRE_RADIUS], &fruit[0], 0, nbFruits, nbFruitEaten, &bonus, UNDEF, FROM_NATURAL, 0, sounds[SOUND_APPEAR], hardcore);

		/////////////////////
		/// BOUCLE DU JEU ///``
		/////////////////////
		int done = 0, dead = 0;
		while( 1 ){
			if(firstframe){
				firstframe = SDL_FALSE;
				dead = 1;
				done = 1;
				sentScore = SDL_TRUE,
				frameDead = SHOW_HELP_FRAME+1;
			}
			// Init input
			turn = NO_TURN;
			accelerate = ACCELERATE_DISABLED;
			SDL_GetMouseState(&(mouseCoor.x), &(mouseCoor.y));

		////////////
		// Events //`
		////////////
			SDL_Event event;
			while( SDL_PollEvent(&event) ){
				switch( event.type ){
					case SDL_QUIT:
						myFrees(&fruit, &snakeBody,&deadBodies, &scoreAffichage, sounds, textures, fonts, &thread );
						return 0;// fermer
				}
			}

		////////////////////////////
		// Handle Keyboard inputs //`
		////////////////////////////
			SDL_PumpEvents();

			if(keystate[SDL_SCANCODE_ESCAPE]){
				myFrees(&fruit, &snakeBody,&deadBodies, &scoreAffichage, sounds, textures, fonts, &thread );
				return 0;
			}
			else if(done && ( ( rdyToSpace && keystate[SDL_SCANCODE_SPACE] ) || ( frameDead > SHOW_HELP_FRAME && ((rdyToUp && keystate[SDL_SCANCODE_UP]) || ( rdyToLeft && keystate[SDL_SCANCODE_LEFT] ) || ( rdyToRight && keystate[SDL_SCANCODE_RIGHT] ) ) ) ) ){
				break;
			}

			if(!keystate[SDL_SCANCODE_SPACE])
				rdyToSpace = SDL_TRUE;

			if(!keystate[SDL_SCANCODE_UP])
				rdyToUp = SDL_TRUE;
			else
				rdyToUp = SDL_FALSE;

			if(!keystate[SDL_SCANCODE_RIGHT])
				rdyToRight = SDL_TRUE;
			else
				rdyToRight = SDL_FALSE;

			if(!keystate[SDL_SCANCODE_LEFT])
				rdyToLeft = SDL_TRUE;
			else
				rdyToLeft = SDL_FALSE;

			if( keystate[SDL_SCANCODE_LEFT] )
				turn = LEFT;
			else if( keystate[SDL_SCANCODE_RIGHT] )
				turn = RIGHT;

			if( frameAcce  ||  keystate[SDL_SCANCODE_UP]  ||  keystate[SDL_SCANCODE_LSHIFT]  ||  keystate[SDL_SCANCODE_SPACE]  ){
				accelerate = ACCELERATE_ENABLED;
				if( keystate[SDL_SCANCODE_SPACE] )
					rdyToSpace = SDL_FALSE;
			}

			if( !done && !dead ){
		//////////////
		// Gameplay //`
		//////////////
				//poison
				poison += poisoned;
				if( poison >= 1){
					rmBody(&snakeBody, &snakeSize, (int)poison, pastBody, &deadBodies, &nbDeadBodies, 1, 1, 0);
					poison -= (int)poison;
					if(snakeSize == SIZE_PRE_RADIUS){
						done = 1;
						dead = 1;
					}
				}

				//Turn
				if( turn == LEFT )
					turnLeft(&dirAngle);
				else if( turn == RIGHT )
					turnRight(&dirAngle);

				//Move
				speedRestant = moveSnake(&snakeBody, pastBody, &snakeSize, accelerate * speedSnake + speedRestant, dirAngle, fruit, nbFruits, &bonus, &radiusRestant, hardcore);
				//spawn more fruits
				if( (hardcore && rand()%(int)(PRECISION_SPAWN*chance_spawn_hardcore) < PRECISION_SPAWN)  || rand() % (int)(CHANCE_SPAWN_FRUIT * nbFruits) == 0 ){
					fruit = realloc(fruit, sizeof(Fruit) * (++nbFruits));
					spawnFruit(snakeBody[SIZE_PRE_RADIUS], fruit, nbFruits - 1, nbFruits, nbFruitEaten, &bonus, UNDEF, FROM_NATURAL, 0, sounds[SOUND_APPEAR], hardcore);
				}


				//spawn more bonus
				if(( bonus.frame > FRUIT_TTL * (hardcore ? RATIO_TTL_HARDCORE : 1) + NB_FRAME_DEATH_FRUIT) && (rand() % (CHANCE_SPAWN_BONUS) == 0) )
					spawnBonus(snakeBody[SIZE_PRE_RADIUS], &bonus, nbFruitEaten, fruit, nbFruits);

				//Reach dest
				reachDest(fruit, nbFruits);

			///////////////////
			// Check hitboxs //`
			///////////////////
				//Head-Mur-queue
				if( tooCloseFromWall((Vector2f){snakeBody[SIZE_PRE_RADIUS].x, snakeBody[SIZE_PRE_RADIUS].y}, BODY_RADIUS + snakeBody[SIZE_PRE_RADIUS].radius - HITBOX_GENTILLE) )
					done = 1;

				if( frameUnkillable == 0  &&  hitboxTail(snakeBody, snakeSize)){
					if( nbPotion > 0 ){
						nbPotion--;
						frameUnkillable = NB_FRAME_INVINCIBILITY;
					}
					else
						done = 1;
				}

				//Head-fruits
				for( int i = 0; i<nbFruits; i++ ){
					if( fruit[i].hitbox == HIT ){
						fruit[i].hitbox = NO_HIT;
						if(!eatFruit(&snakeBody, &snakeSize, &fruit, &nbFruits, fruit[i], &speedSnake, &score, &nbFruitEaten, &frameJaugeAnim, pastBody, &bonus, &deadBodies, &nbDeadBodies, &nbPotion, &poisoned, &scoreAffichage, &nbScoreAffichage, &frameAcce, sounds, &score_hash, keys, hardcore)){
							myFrees(&fruit, &snakeBody,&deadBodies, &scoreAffichage, sounds, textures, fonts, &thread );
							return HACKED;
						}

						for( int j = i; j<nbFruits - 1; j++ )
							fruit[j] = fruit[j + 1];

						if( nbFruits == 1 )
							spawnFruit(snakeBody[SIZE_PRE_RADIUS], fruit, 0, nbFruits, nbFruitEaten, &bonus, UNDEF, FROM_NATURAL, 0, sounds[SOUND_APPEAR], hardcore);
						else
							fruit = realloc(fruit, sizeof(Fruit) * (--nbFruits));
					}
				}

				//Head-bonus
				if( bonus.hitbox == HIT ){

					bonus.hitbox = NO_HIT;

					if(!eatFruit(&snakeBody, &snakeSize, &fruit, &nbFruits, bonus, &speedSnake, &score, &nbFruitEaten, &frameJaugeAnim, pastBody, &bonus, &deadBodies, &nbDeadBodies, &nbPotion, &poisoned, &scoreAffichage, &nbScoreAffichage, &frameAcce, sounds, &score_hash, keys, hardcore))
					{
						myFrees(&fruit, &snakeBody,&deadBodies, &scoreAffichage, sounds, textures, fonts, &thread );
						return HACKED;
					}

					bonus.frame = FRUIT_TTL * (hardcore ? RATIO_TTL_HARDCORE : 1) + NB_FRAME_DEATH_FRUIT ;
					if(snakeSize == SIZE_PRE_RADIUS){
						done = 1;
						dead = 1;
					}
				}

			}
			else if( !dead ){ //death anim
				finalDeath += finalDeathRate;
				if( finalDeath >= 1){
					rmBody(&snakeBody, &snakeSize, (int)finalDeath, pastBody, &deadBodies, &nbDeadBodies, 1, 1, INIT_FRAME_DEATH);
					finalDeath -= (int)finalDeath;
					if(snakeSize == SIZE_PRE_RADIUS){
						done = 1;
						dead = 1;
					}
				}
				finalDeathRate *= FINAL_DEATH_RATE_GROW;
			}

			if(done && !sentScore){
				sentScore = SDL_TRUE;
				if (  score_hash == hashage(score.score, keys) )
				{
					frame_anim_loading = 0;
					//On atttend si jamais un envoi est en cours
					if(thread){
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
						envoiScore = (EnvoiScore){"5", buffer, token};
					else
						envoiScore = (EnvoiScore){"8", buffer, token};

					printf("CONNEXION...\n" );
					thread = SDL_CreateThread(  (int(*)(void*))updateScore, NULL, &envoiScore );
					Mix_PlayChannel(9, sounds[SOUND_GAMEOVER],0);
				}
				else{
					myFrees(&fruit, &snakeBody,&deadBodies, &scoreAffichage, sounds, textures, fonts, &thread );
					return HACKED;
				}

			}

			if(frameDead == SHOW_HELP_FRAME)
				initBody(&snakeSize, &snakeBody);
		//////////
		// Draw //`
		//////////
			SDL_RenderCopy(renderer, textures[S_BACKGROUND], NULL, NULL);

			//Snake
			if( !dead || frameDead > SHOW_HELP_FRAME )
				drawBody(renderer, textures[S_SNAKE], snakeBody, snakeSize, frameUnkillable);

			//Dead bodies
			for( int i = 0; i < nbDeadBodies; i++ )
				drawDeadBody(renderer, textures[S_ANIM], deadBodies[i]);

			//fruits
			for( int i = 0; i < nbFruits; i++ )
				drawFruit(renderer, textures[S_FRUITS], textures[S_ANIM], fruit[i], hardcore);

			//score
			for( int i = 0; i< nbScoreAffichage; i++ )
				drawScore(renderer, textures[S_CHIFFRE], scoreAffichage[i]);

			//bonus
			if(bonus.frame < FRUIT_TTL * (hardcore ? RATIO_TTL_HARDCORE : 1) + NB_FRAME_DEATH_FRUIT)
				drawFruit(renderer, textures[S_FRUITS], textures[S_ANIM], bonus, hardcore);


			if(frameDead >= SHOW_HELP_FRAME){
				drawReplay(renderer, fonts[S_BASE_FONT], 0.7, PLAYGROUND_SIZE_W,PLAYGROUND_SIZE_H);
				drawHelpText(renderer, textures[S_ARROW]);
			}


			//hud
			SDL_RenderSetScale(renderer, 1, 1);
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderSetViewport(renderer, NULL);
			SDL_RenderCopy(renderer, textures[S_HUD], NULL, NULL);

			drawPotions(renderer, nbPotion, textures[S_FRUITS], ratioWindowSize);
			drawQuit(renderer, fonts[S_BASE_FONT], ratioWindowSize, TOTAL_SCORE_COLOR);
			drawJauge(renderer, textures[S_BASKET], ratioWindowSize, jaugeValue);
			drawTotalScore(renderer, fonts[S_FLAPPY], score.scoreShow, ratioWindowSize);


			//handle thread
			if(thread && updateEnded){
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
				afficherLoading(renderer, textures[S_LOADING], COLOR_JAUGE, 0, 15, frame_anim_loading++, w , h, BASE_WINDOW_W*ratioWindowSize);
			}

			if(frameRetour){
				afficherRetour(renderer, textures[S_LOADING],fonts[S_BASE_FONT], COLOR_JAUGE, 0, 15, frameRetour, w, h, BASE_WINDOW_W*ratioWindowSize);

				if(frameRetour >0)
					frameRetour--;
				else
					frameRetour++;
			}

           //afficher
           SDL_RenderPresent(renderer);
           SDL_RenderSetViewport(renderer, &playgroundView);


           SDL_RenderSetScale(renderer, ratioWindowSize, ratioWindowSize);
		////////////////
		// Next frame //`
		////////////////
			if(dead)
				frameDead++;

			//deadBodies timeout
			for( int i = 0; i<nbDeadBodies; i++ ){
				deadBodies[i].frame++;

				if( deadBodies[i].frame > FRUIT_TTL + NB_FRAME_DEATH_BODY ){

					for( int j = i; j < nbDeadBodies - 1; j++ )
						deadBodies[j] = deadBodies[j + 1];

					nbDeadBodies --;
					if( nbDeadBodies > 0 ){
						deadBodies = realloc(deadBodies, sizeof(SnakePart) * (nbDeadBodies));
						i--;
					}
				}
			}

			//fruitUneat
			for( int i = 0; i<nbFruits; i++ ){
				if(!done && fruit[i].frame == FRUIT_TTL * (hardcore ? RATIO_TTL_HARDCORE : 1)+1 ){

					if( fruit[i].id != CAFE ){//change jauge
						nbFruitEaten += (hardcore ? FRUIT_TIMEOUT_EATEN_HARDCORE : -2);
						if( nbFruitEaten<0 )
							nbFruitEaten = 0;
						else if(nbFruitEaten > APPEAR_MAX)
							nbFruitEaten = APPEAR_MAX;
						frameJaugeAnim = FRAME_JAUGE_ANIM;
					}

					if(!done && hardcore){
						int scoreFruit = FRUIT_TIMEOUT_SCORE_HARDCORE * (fruit[i].giant == 0 ? 1 : GIANT_SCORE) * FRUIT_PROPRIETES[fruit[i].id][SCORE];
						if(scoreFruit){
							score.frameToDest = FRAME_SCORE_ANIM;
							if(!changeProtectedVar(&score_hash, &(score.score), score.score + scoreFruit, keys))
							{
								myFrees(&fruit, &snakeBody,&deadBodies, &scoreAffichage, sounds, textures, fonts, &thread );
								return HACKED;
							}

							//afficher score
							nbScoreAffichage++;
							scoreAffichage = realloc(scoreAffichage, nbScoreAffichage * sizeof(Score) );
							int sizeAffichage = scoreSize(fruit[i]);
							scoreAffichage[nbScoreAffichage - 1] = (Score){
								fruit[i].x,
								fruit[i].y,
								scoreFruit,
								0,
								sizeAffichage,
								SDL_TRUE
							};
						}
					}
				}
			}

			//fruitTimeout
			for( int i = 0; i<nbFruits; i++ ){
				fruit[i].frame++;

				if(fruit[i].frame == FRUIT_TTL * (hardcore ? RATIO_TTL_HARDCORE : 1)){
					Mix_PlayChannel(6, sounds[SOUND_DISAPEAR], 0);
				}

				if( fruit[i].frame > FRUIT_TTL * (hardcore ? RATIO_TTL_HARDCORE : 1) + NB_FRAME_DEATH_FRUIT ){
					for( int j = i; j<nbFruits - 1; j++ )
						fruit[j] = fruit[j + 1];

					if( nbFruits == 1 && !done)
						spawnFruit(snakeBody[SIZE_PRE_RADIUS], fruit, i, nbFruits, nbFruitEaten, &bonus, UNDEF, FROM_NATURAL, 0, sounds[SOUND_APPEAR], hardcore);
					else if( nbFruits != 1 ){

						fruit = realloc(fruit, sizeof(Fruit) * (--nbFruits));
					}

				}
			}

			//score timeout
			for( int i = 0; i<nbScoreAffichage; i++ ){
				scoreAffichage[i].frame++;

				if( scoreAffichage[i].frame >= SCORE_TTL ){

					for( int j = i; j < nbScoreAffichage - 1; j++ )
						scoreAffichage[j] = scoreAffichage[j + 1];

					nbScoreAffichage --;
					if( nbScoreAffichage > 0 ){
						scoreAffichage = realloc(scoreAffichage, sizeof(Score) * (nbScoreAffichage));
						i--;
					}
				}
			}

			//Invicibility timeout
			if( frameUnkillable )
				frameUnkillable--;

			//bonusTimeout
			bonus.frame++;
			if( bonus.frame == FRUIT_TTL * (hardcore ? RATIO_TTL_HARDCORE : 1) + NB_FRAME_DEATH_FRUIT +1 )
				initBonus(&bonus);

			//malus timeout
			if( frameAcce )
				frameAcce--;

			//Frame incrementations
			speedSnake += SCALE_SPEED;
			poisoned *= POISON_SCALE;
			chance_spawn_hardcore -= CHANCE_SPAWN_FRUIT_HARDCORE_RATE;
			if(chance_spawn_hardcore < CHANCE_SPAWN_FRUIT_HARDCORE_MIN)
				chance_spawn_hardcore = CHANCE_SPAWN_FRUIT_HARDCORE_MIN;


			//anim jauge
			if(frameJaugeAnim)
				jaugeValue += (float)(nbFruitEaten - jaugeValue ) / ( frameJaugeAnim-- );

			//anim score
			if(score.frameToDest)
				score.scoreShow += (float)(score.score - score.scoreShow ) / ( score.frameToDest-- );

			//regulateFPS
			currentTime = SDL_GetTicks();
			while( currentTime - lastTime < FRAME_TIME )
				currentTime = SDL_GetTicks();

			lastTime = currentTime;

			// On efface
			SDL_SetRenderDrawColor(renderer, HUD_COLOR.r, HUD_COLOR.g, HUD_COLOR.b, 255);
			SDL_RenderClear(renderer);
		}
	}
	return 0;
}

/*int main(){
	//Window and renderer
	snakeInit();
	float ratioWindowSize = 1;
	SDL_Rect displayBounds;
	if (SDL_GetDisplayBounds(0, &displayBounds) != 0) {
		SDL_Log("SDL_GetDisplayBounds failed: %s", SDL_GetError());
		return 1;
	}
	SDL_Point maxWindowSize = maximizeWindow(displayBounds, &ratioWindowSize);

	SDL_Window *myWindow = SDL_CreateWindow("Snake", SDL_WINDOWPOS_UNDEF, SDL_WINDOWPOS_UNDEF, maxWindowSize.x ,maxWindowSize.y, WINDOW_FLAG);
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

	snake(renderer, ratioWindowSize);
}*/
