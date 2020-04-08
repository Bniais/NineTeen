#include "../../define/define.h"
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

#include <math.h>

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
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

	//rand
	srand(time(NULL));
}

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

double tooClose(Vector2f v1, Vector2f v2){
	return (INIT_DECAL.y / sqrt((v2.x - v1.x) * (v2.x - v1.x) + (v2.y - v1.y) * (v2.y - v1.y)));
}

void turnLeft(double *angle){
	*angle -= TURN_AMMOUNT;

	if(*angle < 0){
		*angle += 2 * PI;
	}
}

void turnRight(double *angle){
	*angle += TURN_AMMOUNT;
}

Vector2f angleToVector2f(double angle){
	Vector2f v;
	v.x = cos(angle);
	v.y = sin(angle);
	return v;
}

double norme(Vector2f v){
	return sqrt(v.x * v.x + v.y * v.y);
}

SDL_Point v2fToPoint(Vector2f v){
	return (SDL_Point){roundf(v.x), roundf(v.y)};
}

void init_body(size_t *size, SnakePart** snake){
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

void initToV(Vector2f *vTab, Vector2f v, int iStart, int iEnd){
	for( int i = iStart; i < iEnd; i++ )
		vTab[i] = v;
}

void decale_pastBody_oldBecomeNew(Vector2f pastBody[REMIND_BODY], int nbDecale){
	for( int i = nbDecale; i < REMIND_BODY; i++ )
		pastBody[i - nbDecale] = pastBody[i];

	initToV(pastBody, UNDEF, REMIND_BODY - nbDecale, REMIND_BODY);
}

int add_body(SnakePart **snake, size_t *size, Vector2f pastBody[REMIND_BODY], int speed, float *radiusRestant){
	int nbAdd = *radiusRestant / RATIO_RADIUS_BODYADD;
	(*snake)[*size - 1].radius = 0;

	*snake = realloc(*snake, (*size + nbAdd) * sizeof(SnakePart));

	for( int i = 0; i < nbAdd; i++ ){
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

	decale_pastBody_oldBecomeNew(pastBody, nbAdd);

	*size += nbAdd;

	*radiusRestant -= nbAdd * RATIO_RADIUS_BODYADD;

	return nbAdd;
}

void decale_pastBody_freeSpace(Vector2f* pastBody){
	for( int i = REMIND_BODY - 2; i >= 0; i-- )
		pastBody[i + 1] = pastBody[i];
	pastBody[0] = UNDEF;
}

void decale_pastBody_Bomb(Vector2f* pastBody){

	for( int i = 0; i < REMIND_BODY - 1; i++ )
		pastBody[i] = pastBody[i + 1];
	pastBody[REMIND_BODY - 1] = UNDEF;

}

void rm_body(SnakePart **snake, size_t *size, int nbRm, Vector2f* pastBody, SnakePart **deadBodies, size_t *nbDeadBodies, int deadly, int ratioTexture, int initFrame){
	//check nbRm doesnt kill snake

	if( !deadly && nbRm >= *size - SIZE_PRE_RADIUS - MIN_BODY_PARTS )
		nbRm = *size - SIZE_PRE_RADIUS - MIN_BODY_PARTS - 1;

	if(nbRm < *size - SIZE_PRE_RADIUS){
		//update pastBody
		for( int i = 0; i < nbRm; i++ )
			decale_pastBody_Bomb(pastBody);

		for( int i = 0; i < nbRm; i++ )
			decale_pastBody_freeSpace(pastBody);

		for( int i = 0; i < nbRm; i++ )
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



void afficherBody(SDL_Renderer *renderer, SDL_Texture *snakeTexture, SnakePart *snake, size_t size, int frameUnkillable){
	SDL_Rect currentBody;

	//do src depending on blinking and if unkillable
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

int hitboxQueue(SnakePart *snake, size_t size){
	for( int i = BODY_DEATH_HITBOX + SIZE_PRE_RADIUS; i < size; i++ )
		if( norme((Vector2f){snake[SIZE_PRE_RADIUS].x - snake[i].x, snake[SIZE_PRE_RADIUS].y - snake[i].y}) < snake[SIZE_PRE_RADIUS].radius + snake[i].radius + 2 * BODY_RADIUS )
			return 1;

	return 0;
}

int tooCloseFromHead(SnakePart head, Fruit fruit){
	return (norme((Vector2f){fruit.dest.x - head.x, fruit.dest.y - head.y}) < DIST_HEAD_FRUIT + FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE) + BODY_RADIUS + head.radius);
}

static int tooCloseFromWall(Vector2f fruit, int dist){
	return (fruit.x < dist  ||  fruit.x > PLAYGROUND_SIZE_W - dist  ||  fruit.y < dist  ||  fruit.y > PLAYGROUND_SIZE_H - dist);
}

int tooCloseFromCorner(Fruit fruit){
	return (norme((Vector2f){fruit.dest.x - 0, fruit.dest.y - 0}) < DIST_CORNER_FRUIT + FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE)
		|| norme((Vector2f){fruit.dest.x - PLAYGROUND_SIZE_W, fruit.dest.y - 0}) < DIST_CORNER_FRUIT + FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE)
		|| norme((Vector2f){fruit.dest.x - PLAYGROUND_SIZE_W, fruit.dest.y - PLAYGROUND_SIZE_H}) < DIST_CORNER_FRUIT + FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE)
		|| norme((Vector2f){fruit.dest.x - 0, fruit.dest.y - PLAYGROUND_SIZE_H}) < DIST_CORNER_FRUIT + FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE));
}

int tooCloseFromFruit(Fruit* fruitRang, int rang, Fruit* fruitSearch, int nbFruitSearch){
	for( int i = 0; i < nbFruitSearch; i++ )
		if( i != rang  &&  norme((Vector2f){fruitRang[rang].dest.x - fruitSearch[i].x, fruitRang[rang].dest.y - fruitSearch[i].y}) < FRUIT_PROPRIETES[fruitRang[rang].id][RADIUS] + FRUIT_PROPRIETES[fruitSearch[i].id][RADIUS] + DIST_FRUITS )
			return 1;

	return 0;
}

void spawnFruit(SnakePart head, Fruit *fruit, int rang, int nbFruits, float nbFruitEaten, Fruit *bonus, Vector2f spawn, int spawnCondition, int retardFrame){
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

void reachDest(Fruit* fruit, size_t nbFruits){
	for( int i=0; i < nbFruits; i++ )
		if( fruit[i].frame < 0  &&  fruit[i].frame >= -NB_FRAME_SPAWN_FRUIT  &&  fruit[i].dest.x != fruit[i].x ){
			fruit[i].coefRadius = 1 + (abs(abs(-fruit[i].frame - NB_FRAME_SPAWN_FRUIT/2.) - NB_FRAME_SPAWN_FRUIT/2.) / (3 * NB_FRAME_SPAWN_FRUIT/2.));
			fruit[i].x += ((NB_FRAME_SPAWN_FRUIT + (float)fruit[i].frame) / NB_FRAME_SPAWN_FRUIT) * (fruit[i].dest.x - fruit[i].x);
			fruit[i].y += ((NB_FRAME_SPAWN_FRUIT + (float)fruit[i].frame) / NB_FRAME_SPAWN_FRUIT) * (fruit[i].dest.y - fruit[i].y);
		}
}

int hitboxFruit(Fruit fruit, SnakePart head){
	return (norme((Vector2f){fruit.x - head.x, fruit.y - head.y}) <= (FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE) + BODY_RADIUS + head.radius) * HITBOX_SECURITY);
}

void afficherFruit(SDL_Renderer *renderer, SDL_Texture *fruitTexture, SDL_Texture *spawnTexture, Fruit fruit, int hardcore){
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

void afficherPotions(SDL_Renderer* renderer, int nbPotion, SDL_Texture * fruitTexture, float ratioWindowSize){

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
*\fn void drawQuit(SDL_Renderer* renderer, TTF_Font* font)
*\brief Affiche la commande pour quitter le jeu
*\param renderer Le renderer où afficher
*\param font La police d'écriture
*/
static void drawQuit(SDL_Renderer* renderer, TTF_Font* font, float ratioWindowSize){
	char * text = QUIT;

	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, TOTAL_SCORE_COLOR);
	SDL_Rect dest = QUIT_DEST;
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	SDL_QueryTexture(Message,NULL,(int*)SDL_TEXTUREACCESS_STATIC,&(dest.w), &(dest.h) );
	dest.w /= (OPEN_FONT_SIZE / SIZE_QUIT);
	dest.h /= (OPEN_FONT_SIZE / SIZE_QUIT);
	dest.w *= ratioWindowSize;
	dest.h *= ratioWindowSize;
	dest.x *= ratioWindowSize;
	dest.y *= ratioWindowSize;

	SDL_RenderCopy(renderer, Message, NULL, &dest);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
}

/**
*\fn void drawReplay(SDL_Renderer* renderer, TTF_Font* font)
*\brief Affiche un texte invitant le joueur à rejouer
*\param renderer Le renderer où afficher
*\param font La police d'écriture
*/
static void drawReplay(SDL_Renderer* renderer, TTF_Font* font){
	char * text = REPLAY;

	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, WHITE);
	SDL_Rect dest;
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	SDL_QueryTexture(Message,NULL,(int*)SDL_TEXTUREACCESS_STATIC,&(dest.w), &(dest.h) );
	dest.w /= (OPEN_FONT_SIZE / SIZE_REPLAY);
	dest.h /= (OPEN_FONT_SIZE / SIZE_REPLAY);

	//centrer
	dest.x = PLAYGROUND_SIZE_W/2 - dest.w/2;
	dest.y = PLAYGROUND_SIZE_H/2 - dest.h/2;

	SDL_RenderCopy(renderer, Message, NULL, &dest);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
}


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

	jauge.y += (jauge.h*(1-ratioFruti));
	jauge.h *= ratioFruti;
	SDL_SetRenderDrawColor(renderer, COLOR_JAUGE.r, COLOR_JAUGE.g, COLOR_JAUGE.b, 255);
	SDL_RenderFillRect(renderer, &jauge);

	SDL_RenderCopy(renderer, basketTexture, NULL, &dest);
}


void afficherDeadBody(SDL_Renderer *renderer, SDL_Texture *deadBodyTexture, SnakePart deadBody){
	SDL_Rect currentBody = (SDL_Rect){
		roundf(deadBody.x) - deadBody.radius,
		roundf(deadBody.y) - deadBody.radius,
		deadBody.radius * 2,
		deadBody.radius * 2
	};

	SDL_Rect src = (SDL_Rect){(deadBody.frame - FRUIT_TTL) / (NB_FRAME_DEATH_BODY / NB_ANIM_DEATH_BODY) * FRUIT_DIM.x, FRUIT_DIM.y, FRUIT_DIM.x, FRUIT_DIM.y};
	SDL_RenderCopy(renderer, deadBodyTexture, &src, &currentBody);

}

void afficherScoreTotal(SDL_Renderer *renderer, TTF_Font *font, int scoreShow, float ratioWindowSize){
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

void decale_radius(SnakePart **snake, size_t *size, Vector2f pastBody[REMIND_BODY], Fruit fruit, float speed, float *radiusRestant){
	int nbAdd = 0;

	*radiusRestant += (*snake)[*size - 1].radius;
	// Si la queue est grosse, on ralonge le serpent
	if( *radiusRestant >= RATIO_RADIUS_BODYADD )
		nbAdd = add_body(snake, size, pastBody, speed, radiusRestant);

	// On décale tout
	for( int i = *size - 2; i >= 0; i-- )
		(*snake)[i + 1].radius = (*snake)[i].radius;

	(*snake)[0].radius = 0;

	// On décale le corps digéré jusqu'au bout de la queue
	if( nbAdd ){
		int i;

		for( i = *size - 2 - nbAdd / 3; !(*snake)[i].radius  &&  i >= 0; i-- );

		for( ; (*snake)[i].radius  &&  i >= 0; i-- )
			(*snake)[i + nbAdd - 1].radius = (*snake)[i].radius;

		for( int j = 0; j < nbAdd; j++ )
			(*snake)[i - 1 + j].radius = 0;
	}
}

float move_snake(SnakePart** snake, Vector2f pastBody[REMIND_BODY], size_t *size, float speed, double dir, Fruit *fruit, size_t nbFruits, Fruit *bonus, float *radiusRestant, int hardcore)
{
	// Verif speed not too high
	if( speed == SPEED_DECOMPOSITION ){
		// Se souvenir de l'emplacement qui va partir
		decale_pastBody_freeSpace(pastBody);
		pastBody[0].x = (*snake)[*size - 1].x;
		pastBody[0].y = (*snake)[*size - 1].y;

		// Bouger
		for( int i = *size - 1; i > SIZE_PRE_RADIUS; i-- ){
			(*snake)[i].x = (*snake)[i - 1].x;
			(*snake)[i].y = (*snake)[i - 1].y;
		}

		(*snake)[SIZE_PRE_RADIUS].x += angleToVector2f(dir).x * speed;
		(*snake)[SIZE_PRE_RADIUS].y += angleToVector2f(dir).y * speed;

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

			move_snake(snake, pastBody, size, SPEED_DECOMPOSITION, dir, fruit, nbFruits, bonus, radiusRestant, hardcore);
			speed -= SPEED_DECOMPOSITION;

			decale_radius(snake, size, pastBody, *fruit, speed, radiusRestant);
			if( i % 2 )
				decale_radius(snake, size, pastBody, *fruit, speed, radiusRestant);
		}
	}

	return speed;
}

int scoreSize(Fruit fruit){
	int size = 0.15 + 3.65 * log((int)(fruit.giant == 0 ? 1 : GIANT_SCORE) * FRUIT_PROPRIETES[fruit.id][SCORE]);

	if( size < MIN_SIZE_SCORE )
		size = MIN_SIZE_SCORE;
	else if( size > MAX_SIZE_SCORE )
		size = MAX_SIZE_SCORE;

	return size;
}

int eat_fruit(SnakePart **snake, size_t *size, Fruit** fruitTab, size_t* nbFruits, Fruit fruit, float *speed, ScoreTotal *score, float *nbFruitEaten, int *frameJaugeAnim, Vector2f* pastBody, Fruit* bonus, SnakePart **deadBodies, size_t *nbDeadBodies, int* nbPotion, float* poisoned, Score** scoreAffichage, size_t* nbScoreAffichage, int *frameAcce, Mix_Chunk *flap_wav, long long *score_hash, long keys[4], int hardcore){
	//Mix_PlayChannel(-1, flap_wav, 0);
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
	int scoreFruit = (int)((hardcore ? RATIO_GET_FRUIT_HARDCORE : 1) * (fruit.giant == 0 ? 1 : GIANT_SCORE) * FRUIT_PROPRIETES[fruit.id][SCORE]);
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
				if(*size<MAX_BOMB_SIZE)
					rm_body(snake, size, FLAT_RM_BOMB + RELATIVE_RM_BOMB * (*size - SIZE_PRE_RADIUS), pastBody, deadBodies, nbDeadBodies, 1, BOMB_TEXTURE_RATE, 0);
				else
					rm_body(snake, size, FLAT_RM_BOMB + RELATIVE_RM_BOMB * (MAX_BOMB_SIZE - SIZE_PRE_RADIUS), pastBody, deadBodies, nbDeadBodies, 1, BOMB_TEXTURE_RATE, 0);

				break;

			case PLUME:
				*speed = *speed - RELATIVE_SLOW_FEATHER * (*speed - MIN_SPEED);
				break;

			case COFFRE:
				*fruitTab = realloc(*fruitTab, (*nbFruits + NB_SPAWN_COFFRE) * sizeof(Fruit));
				*nbFruits += NB_SPAWN_COFFRE;
				for( int i=0; i < NB_SPAWN_COFFRE; i++ )
					spawnFruit((*snake)[SIZE_PRE_RADIUS], *fruitTab, *nbFruits - NB_SPAWN_COFFRE + i, *nbFruits - NB_SPAWN_COFFRE + 1 + i, *nbFruitEaten + CHEST_BOOST_FRUIT_EATEN, bonus, (Vector2f){fruit.x, fruit.y}, FROM_COFFRE, FRAME_RETARD_COFFRE);
				break;

			case ARC_EN_CIEL:

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
					spawnFruit((*snake)[SIZE_PRE_RADIUS], *fruitTab, *nbFruits - NB_SPAWN_ARC_EN_CIEL + i, *nbFruits - NB_SPAWN_ARC_EN_CIEL + 1 + i, *nbFruitEaten + RAINBOW_BOOST_FRUIT_EATEN, bonus, (Vector2f){corner.x + cos(minAngle) * dist, corner.y + sin(minAngle) * dist}, FROM_RAINBOW, i * FRAME_RETARD_RAINBOW);
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

static int strlen_num(int score)
{
	int count=1;

	while(score >= 10){
		score /=10;
		count++;
	}

	return count;
}

static void afficherScore(SDL_Renderer *renderer , SDL_Texture *scoreTexture, Score scoreAffichage)
{
	int negative = SDL_FALSE;
	if (scoreAffichage.score < 0 ){
		scoreAffichage.score*=-1;
		negative = 1;
	}
	SDL_SetTextureAlphaMod(scoreTexture, scoreAffichage.fromTimeOut ? ALPHA_SCORE_TIMEOUT[scoreAffichage.frame] : ALPHA_SCORE[scoreAffichage.frame]);
	int taille = strlen_num(scoreAffichage.score) + negative;

	SDL_Rect src = SCORE_SRC;
	SDL_Rect dest = {scoreAffichage.x + (scoreAffichage.size*taille)/2 , scoreAffichage.y - (FONT_HEIGHT_RATIO*scoreAffichage.size) / 2 , scoreAffichage.size, FONT_HEIGHT_RATIO*scoreAffichage.size };

	dest.x -= dest.w;
	//dest.y -= dest.h/2;
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

static void myFrees(Fruit ** fruits, SnakePart ** deadBodies, SnakePart **snakeBody, Score ** scoreAffichage, Mix_Chunk ** sound, SDL_Texture * textures[NB_SNAKE_TEXTURES], TTF_Font * fonts[NB_SNAKE_FONTS]){
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

	if(*sound){
		Mix_FreeChunk(*sound);
	}


	for(int i=0; i<NB_SNAKE_TEXTURES; i++)
		if(textures[i]){
			SDL_DestroyTexture(textures[i]);
			textures[i] = NULL;
		}


	for(int i=0; i<NB_SNAKE_FONTS; i++)
		if(fonts[i]){
			TTF_CloseFont(fonts[i]);
			fonts[i] = NULL;
		}

}


int snake(SDL_Renderer * renderer,int highscore, float ratioWindowSize, char *token, int hardcore){
/////////////////////
/// MISE EN PLACE ///``
/////////////////////
	snakeInit();
	//Textures
	SDL_Texture* textures[NB_SNAKE_TEXTURES];
	for(int i=0; i< NB_SNAKE_TEXTURES; i++){
		 textures[i] = IMG_LoadTexture(renderer, DIR_TEXTURES_SNAKE[i]);
		 if( textures[i] == NULL ){
			printf("Erreur lors de la creation de texture %s", SDL_GetError());
			return EXIT_FAILURE;
		}
	}
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
	Mix_Chunk *flap_wav = NULL;
	/*Mix_Chunk *flap_wav = Mix_LoadWAV( "WRONG/PATH" );

	if( !flap_wav)
		printf("Erreur chargement des sons: %s\n",Mix_GetError());*/

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


	while(1){
		////////////
		/// Vars ///`
		////////////
		//Keyboard

		int accelerate = ACCELERATE_DISABLED;

		//Time
		unsigned int lastTime = 0, currentTime;
		int rdyToSpace = SDL_TRUE;

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
		init_body(&snakeSize, &snakeBody);

		nbDeadBodies = 0;
		deadBodies = realloc(deadBodies, sizeof(SnakePart));

		initToV(pastBody, UNDEF, 0, REMIND_BODY);

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

		SDL_Rect playgroundView = {HUD_W*ratioWindowSize, HUD_H*ratioWindowSize, PLAYGROUND_SIZE_W*ratioWindowSize, PLAYGROUND_SIZE_H*ratioWindowSize};

		//hud and menus
		int paused = 0;


		//mouse
		SDL_Point mouseCoor;


		///////////////////////
		/// Initialize vars ///`
		///////////////////////

		//Bonus
		bonus = (Fruit){UNDEF.x, UNDEF.y, 0, 0, NO_HIT, FRUIT_TTL * (hardcore ? RATIO_TTL_HARDCORE : 1) + NB_FRAME_DEATH_FRUIT + 1};
		//Fruit

		spawnFruit(snakeBody[SIZE_PRE_RADIUS], &fruit[0], 0, nbFruits, nbFruitEaten, &bonus, UNDEF, FROM_NATURAL, 0);


		/////////////////////
		/// BOUCLE DU JEU ///``
		/////////////////////

		int done = 0, dead = 0;
		while( 1 ){
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
						myFrees(&fruit, &snakeBody,&deadBodies, &scoreAffichage, &flap_wav, textures, fonts );
						return 0;// fermer
					case SDL_KEYUP:
						if( event.key.keysym.sym == SDLK_SPACE)
							rdyToSpace = SDL_TRUE;
						break;

				}
			}

		////////////////////////////
		// Handle Keyboard inputs //`
		////////////////////////////
			SDL_PumpEvents();
			if( keystate[SDL_SCANCODE_SPACE] && done && rdyToSpace)
				break; //replay

			if(keystate[SDL_SCANCODE_ESCAPE]){
				myFrees(&fruit, &snakeBody,&deadBodies, &scoreAffichage, &flap_wav, textures, fonts );
				return 0;
			}

			if( keystate[SDL_SCANCODE_LEFT] )
				turn = LEFT;
			else if( keystate[SDL_SCANCODE_RIGHT] )
				turn = RIGHT;

			if( frameAcce  ||  keystate[SDL_SCANCODE_UP]  ||  keystate[SDL_SCANCODE_LSHIFT]  ||  keystate[SDL_SCANCODE_SPACE]  ){
				accelerate = ACCELERATE_ENABLED;
				if( keystate[SDL_SCANCODE_SPACE] )
					rdyToSpace = SDL_FALSE;
			}


			if( !paused && !done && !dead ){
		//////////////
		// Gameplay //`
		//////////////
				//poison
				poison += poisoned;
				if( poison >= 1){
					rm_body(&snakeBody, &snakeSize, (int)poison, pastBody, &deadBodies, &nbDeadBodies, 1, 1, 0);
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

				speedRestant = move_snake(&snakeBody, pastBody, &snakeSize, accelerate * speedSnake + speedRestant, dirAngle, fruit, nbFruits, &bonus, &radiusRestant, hardcore);

				//spawn more fruits
				if( (hardcore && rand()%(int)(PRECISION_SPAWN*chance_spawn_hardcore) < PRECISION_SPAWN)  || rand() % (CHANCE_SPAWN_FRUIT * nbFruits) == 0 ){
					fruit = realloc(fruit, sizeof(Fruit) * (++nbFruits));
					spawnFruit(snakeBody[SIZE_PRE_RADIUS], fruit, nbFruits - 1, nbFruits, nbFruitEaten, &bonus, UNDEF, FROM_NATURAL, 0);
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
				if( tooCloseFromWall((Vector2f){snakeBody[SIZE_PRE_RADIUS].x, snakeBody[SIZE_PRE_RADIUS].y}, BODY_RADIUS + snakeBody[SIZE_PRE_RADIUS].radius) )
					done = 1;

				if( frameUnkillable == 0  &&  hitboxQueue(snakeBody, snakeSize)){
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
						if(!eat_fruit(&snakeBody, &snakeSize, &fruit, &nbFruits, fruit[i], &speedSnake, &score, &nbFruitEaten, &frameJaugeAnim, pastBody, &bonus, &deadBodies, &nbDeadBodies, &nbPotion, &poisoned, &scoreAffichage, &nbScoreAffichage, &frameAcce, flap_wav, &score_hash, keys, hardcore)){
							myFrees(&fruit, &snakeBody,&deadBodies, &scoreAffichage, &flap_wav, textures, fonts );
							return HACKED;
						}


						for( int j = i; j<nbFruits - 1; j++ )
							fruit[j] = fruit[j + 1];

						if( nbFruits == 1 )
							spawnFruit(snakeBody[SIZE_PRE_RADIUS], fruit, i, nbFruits, nbFruitEaten, &bonus, UNDEF, FROM_NATURAL, 0);
						else
							fruit = realloc(fruit, sizeof(Fruit) * (--nbFruits));
					}
				}


				//Head-bonus
				if( bonus.hitbox == HIT ){

					bonus.hitbox = NO_HIT;

					if(!eat_fruit(&snakeBody, &snakeSize, &fruit, &nbFruits, bonus, &speedSnake, &score, &nbFruitEaten, &frameJaugeAnim, pastBody, &bonus, &deadBodies, &nbDeadBodies, &nbPotion, &poisoned, &scoreAffichage, &nbScoreAffichage, &frameAcce, flap_wav, &score_hash, keys, hardcore))
					{
						myFrees(&fruit, &snakeBody,&deadBodies, &scoreAffichage, &flap_wav, textures, fonts );
						return HACKED;
					}

					bonus.frame = FRUIT_TTL * (hardcore ? RATIO_TTL_HARDCORE : 1) + NB_FRAME_DEATH_FRUIT ;
					if(snakeSize == SIZE_PRE_RADIUS){
						done = 1;
						dead = 1;
					}
				}
			}
			else if( !paused && !dead ){ //death anim
				finalDeath += finalDeathRate;
				if( finalDeath >= 1){
					rm_body(&snakeBody, &snakeSize, (int)finalDeath, pastBody, &deadBodies, &nbDeadBodies, 1, 1, INIT_FRAME_DEATH);
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
					// CONVERTIR SCORE EN TEXT
					char buffer[10];
					sprintf(buffer,"%d",score.score);
					printf("ATTENDRE ENVI DU SCORE\n" );
					if(hardcore)
						updateScore("5",buffer,token);
					else
						updateScore("8",buffer,token);
					printf("SCORE ENVOYER\n" );
					//////////////////////////////////////////////////////////////////
				}
				else{
					myFrees(&fruit, &snakeBody,&deadBodies, &scoreAffichage, &flap_wav, textures, fonts );
					return HACKED;
				}

			}

		//////////
		// Draw //`
		//////////
			SDL_RenderCopy(renderer, textures[S_BACKGROUND], NULL, NULL);

			//Snake
			if( !dead )
				afficherBody(renderer, textures[S_SNAKE], snakeBody, snakeSize, frameUnkillable);

			//Dead bodies
			for( int i = 0; i < nbDeadBodies; i++ )
				afficherDeadBody(renderer, textures[S_ANIM], deadBodies[i]);

			//fruits
			for( int i = 0; i < nbFruits; i++ )
				afficherFruit(renderer, textures[S_FRUITS], textures[S_ANIM], fruit[i], hardcore);

			//score
			for( int i = 0; i< nbScoreAffichage; i++ )
				afficherScore(renderer, textures[S_CHIFFRE], scoreAffichage[i]);

			//bonus
			if(bonus.frame < FRUIT_TTL * (hardcore ? RATIO_TTL_HARDCORE : 1) + NB_FRAME_DEATH_FRUIT)
				afficherFruit(renderer, textures[S_FRUITS], textures[S_ANIM], bonus, hardcore);


			if(done && snakeSize < (SIZE_PRE_RADIUS + RELAY_SNAKE_SIZE))
				drawReplay(renderer, fonts[S_BASE_FONT]);



			//hud
           SDL_RenderSetScale(renderer, 1, 1);
           SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
           SDL_RenderSetViewport(renderer, NULL);
           SDL_RenderCopy(renderer, textures[S_HUD], NULL, NULL);

           afficherPotions(renderer, nbPotion, textures[S_FRUITS], ratioWindowSize);
           drawQuit(renderer, fonts[S_BASE_FONT], ratioWindowSize);
           drawJauge(renderer, textures[S_BASKET], ratioWindowSize, jaugeValue);
           afficherScoreTotal(renderer, fonts[S_FLAPPY], score.scoreShow, ratioWindowSize);


           //afficher
           SDL_RenderPresent(renderer);
           SDL_RenderSetViewport(renderer, &playgroundView);


           SDL_RenderSetScale(renderer, ratioWindowSize, ratioWindowSize);
		////////////////
		// Next frame //`
		////////////////
			if( !paused ){

				//deadBodies timeout
				for( int i = 0; i<nbDeadBodies; i++ ){
					deadBodies[i].frame++;

					if( deadBodies[i].frame > FRUIT_TTL * (hardcore ? RATIO_TTL_HARDCORE : 1) + NB_FRAME_DEATH_BODY ){

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
							frameJaugeAnim = FRAME_JAUGE_ANIM;
						}

						if(!done && hardcore){
							int scoreFruit = FRUIT_TIMEOUT_SCORE_HARDCORE * (fruit[i].giant == 0 ? 1 : GIANT_SCORE) * FRUIT_PROPRIETES[fruit[i].id][SCORE];
							if(scoreFruit){
								score.frameToDest = FRAME_SCORE_ANIM;
								if(!changeProtectedVar(&score_hash, &(score.score), score.score + scoreFruit, keys))
								{
									myFrees(&fruit, &snakeBody,&deadBodies, &scoreAffichage, &flap_wav, textures, fonts );
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

					if( fruit[i].frame > FRUIT_TTL * (hardcore ? RATIO_TTL_HARDCORE : 1) + NB_FRAME_DEATH_FRUIT ){
						for( int j = i; j<nbFruits - 1; j++ )
							fruit[j] = fruit[j + 1];

						if( nbFruits == 1 && !done)
							spawnFruit(snakeBody[SIZE_PRE_RADIUS], fruit, i, nbFruits, nbFruitEaten, &bonus, UNDEF, FROM_NATURAL, 0);
						else if( !done )
							fruit = realloc(fruit, sizeof(Fruit) * (--nbFruits));
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
			}

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
