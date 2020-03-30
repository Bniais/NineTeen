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

	// SDL Init
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	// Settings
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	if( Mix_OpenAudio( MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 1024 ) == -1 )
		printf("Erreur init SDL_Mixer\n" );


	//rand
	srand(time(NULL));
}

void initFruit(Fruit** fruit){

	*fruit = malloc(sizeof(Fruit));
	(*fruit)->x = UNDEFINED.x;
	(*fruit)->y = UNDEFINED.x;
	(*fruit)->giant = 0;
	(*fruit)->id = NO_ID;
	(*fruit)->hitbox = 0;
	(*fruit)->frame = 0;
	(*fruit)->dest = (Vector2f){UNDEFINED.x, UNDEFINED.y};
	(*fruit)->coefRadius = 1;
}

void initBonus(Fruit* bonus){
	bonus->x = UNDEFINED.x;
	bonus->y = UNDEFINED.x;
	bonus->giant = 0;
	bonus->id = 0;
	bonus->hitbox = 0;
	bonus->frame = FRUIT_TTL + NB_FRAME_DEATH_FRUIT + 1;
	bonus->dest = (Vector2f){UNDEFINED.x, UNDEFINED.y};
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

double Vector2fToAngle(Vector2f v){
	Vector2f v2 = {1, 0};
	return acos((v.x * v2.x + v.y * v2.y) / (norme(v) * norme(v2)));
}

double dirBetween(Vector2f v1, Vector2f v2){
	Vector2f v3;
	v3.x = v2.x - v1.x;
	v3.y = v2.y - v1.y;
	return Vector2fToAngle(v3);
}

SDL_Point v2fToPoint(Vector2f v){
	return (SDL_Point){roundf(v.x), roundf(v.y)};
}

void init_body(size_t *size, SnakePart** snake){
	*snake = malloc(sizeof(SnakePart) * (INIT_BODY + SIZE_PRE_RADIUS));

	for( int i = 0; i < INIT_BODY + SIZE_PRE_RADIUS; i++ )
		(*snake)[i] = (SnakePart){UNDEFINED.x, UNDEFINED.y, 0, 0};

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

	initToV(pastBody, UNDEFINED, REMIND_BODY - nbDecale, REMIND_BODY);
}

int add_body(SnakePart **snake, size_t *size, Vector2f pastBody[REMIND_BODY], int speed, float *radiusRestant){
	int nbAdd = *radiusRestant / RATIO_RADIUS_BODYADD;
	(*snake)[*size - 1].radius = 0;

	*snake = realloc(*snake, (*size + nbAdd) * sizeof(SnakePart));

	for( int i = 0; i < nbAdd; i++ ){
		if( pastBody[i].x == UNDEFINED.x ){
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
	pastBody[0] = UNDEFINED;
}

void decale_pastBody_Bomb(Vector2f* pastBody){

	for( int i = 0; i < REMIND_BODY - 1; i++ )
		pastBody[i] = pastBody[i + 1];
	pastBody[REMIND_BODY - 1] = UNDEFINED;

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

int tooCloseFromWall(Vector2f fruit, int dist){
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

void spawnFruit(SnakePart head, Fruit *fruit, int rang, int nbFruits, int nbFruitEaten, Fruit *bonus, Vector2f spawn, int spawnCondition, int retardFrame){
	fruit[rang].x = UNDEFINED.x;
	fruit[rang].y = UNDEFINED.y;
	fruit[rang].giant = 0;
	fruit[rang].id = NO_ID;
	fruit[rang].hitbox = 0;
	fruit[rang].frame = -NB_FRAME_SPAWN_FRUIT - retardFrame;
	fruit[rang].coefRadius = 1;
	fruit[rang].dest.x = UNDEFINED.x;
	fruit[rang].dest.y = UNDEFINED.y;

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

void spawnBonus(SnakePart head, Fruit* bonus, int nbFruitEaten, Fruit* fruit, int nbFruits){
	bonus->x = UNDEFINED.x;
	bonus->y = UNDEFINED.x;
	bonus->giant = 0;
	bonus->id = NO_ID;
	bonus->hitbox = 0;
	bonus->frame = -NB_FRAME_SPAWN_FRUIT;
	bonus->dest = (Vector2f){UNDEFINED.x, UNDEFINED.y};
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

void afficherFruit(SDL_Renderer *renderer, SDL_Texture *fruitTexture, SDL_Texture *spawnTexture, Fruit fruit){
	SDL_Rect currentBody = (SDL_Rect){
		roundf(fruit.x) - (FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE)) * fruit.coefRadius,
		roundf(fruit.y) - (FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE)) * fruit.coefRadius,
		(FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE)) * 2 * fruit.coefRadius,
		(FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE)) * 2 * fruit.coefRadius
	};

	int blinking = 0;
	if( fruit.frame >= BLINK_FRAMES[0] )
		for( int i = 0; i < NB_BLINK; i++ )
			if( fruit.frame == BLINK_FRAMES[i] )
				blinking = 1;


	if( blinking ){
		SDL_Rect src = (SDL_Rect){fruit.id * FRUIT_DIM.x, FRUIT_DIM.y * (RANGEE_BLINK - 1), FRUIT_DIM.x, FRUIT_DIM.y};
		SDL_RenderCopy(renderer, fruitTexture, &src, &currentBody);
	}
	else if( fruit.frame >= FRUIT_TTL ){
		SDL_Rect src = (SDL_Rect){(fruit.frame - FRUIT_TTL) / (NB_FRAME_DEATH_FRUIT / NB_ANIM_DEATH) * FRUIT_DIM.x, FRUIT_DIM.y, FRUIT_DIM.x, FRUIT_DIM.y};
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

float move_snake(SnakePart** snake, Vector2f pastBody[REMIND_BODY], size_t *size, float speed, double dir, Fruit *fruit, size_t nbFruits, Fruit *bonus, float *radiusRestant)
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
			if( fruit[i].frame > 0  &&  fruit[i].frame < FRUIT_TTL  &&  hitboxFruit(fruit[i], (*snake)[SIZE_PRE_RADIUS]) )
				fruit[i].hitbox = HIT;


		if( bonus->frame > 0  &&  bonus->frame < FRUIT_TTL  &&  hitboxFruit(*bonus, (*snake)[SIZE_PRE_RADIUS]) )
			bonus->hitbox = HIT;

		return speed;
	}
	else{
		// On décompose le mouvement en mouvements de speed inférieurs
		int i;
		while( speed > SPEED_DECOMPOSITION ){
			i++;

			move_snake(snake, pastBody, size, SPEED_DECOMPOSITION, dir, fruit, nbFruits, bonus, radiusRestant);
			speed -= SPEED_DECOMPOSITION;

			decale_radius(snake, size, pastBody, *fruit, speed, radiusRestant);
			if( i % 2 )
				decale_radius(snake, size, pastBody, *fruit, speed, radiusRestant);
		}
	}

	return speed;
}

int scoreSize(Fruit fruit){
	int size = 0.15 + 3.65 * log(FRUIT_PROPRIETES[fruit.id][SCORE]);

	if( size < MIN_SIZE_SCORE )
		size = MIN_SIZE_SCORE;
	else if( size > MAX_SIZE_SCORE )
		size = MAX_SIZE_SCORE;

	return size;
}

void eat_fruit(SnakePart **snake, size_t *size, Fruit** fruitTab, size_t* nbFruits, Fruit fruit, float *speed, int *score, int *nbFruitEaten, Vector2f* pastBody, Fruit* bonus, SnakePart **deadBodies, size_t *nbDeadBodies, int* nbPotion, float* poisoned, Score** scoreAffichage, size_t* nbScoreAffichage, int *frameAcce, Mix_Chunk *flap_wav){
	Mix_PlayChannel(-1, flap_wav, 0);
	//manger
	if( fruit.giant ){
		for( int i = 0; i < SIZE_PRE_RADIUS; i++ )
			(*snake)[i].radius += FRUIT_ADD_RADIUS_GIANT[fruit.id][i];
	}
	else{
		for( int i = 0; i < SIZE_PRE_RADIUS; i++ )
			(*snake)[i].radius += FRUIT_ADD_RADIUS[fruit.id][i];
	}

	//accelerate
	*speed += (fruit.giant == 0 ? 1 : GIANT_SPEED) * FRUIT_PROPRIETES[fruit.id][ACCELERATION];

	if( *speed < MIN_SPEED )
		*speed = MIN_SPEED;

	//score
	*score += (fruit.giant == 0 ? 1 : GIANT_SCORE) * FRUIT_PROPRIETES[fruit.id][SCORE];

	if( FRUIT_PROPRIETES[fruit.id][SCORE] ){
		(*nbScoreAffichage)++;
		*scoreAffichage = realloc(*scoreAffichage, *nbScoreAffichage * sizeof(Score) );
		int sizeAffichage = scoreSize(fruit);
		(*scoreAffichage)[*nbScoreAffichage - 1] = (Score){
			fruit.x,
			fruit.y,
			(int)FRUIT_PROPRIETES[fruit.id][SCORE],
			0,
			sizeAffichage
		};
	}

	//update nbEaten
	if( fruit.id < NB_FRUITS )
		(*nbFruitEaten)++;

	//check malus
	if( fruit.id == CAFE )
		*frameAcce += NB_FRAME_CAFE_ACCE;
	//check bonus
	else if( fruit.id >= NB_FRUITS ){
		switch ( fruit.id ) {
			case BOMBE:
				rm_body(snake, size, FLAT_RM_BOMB + RELATIVE_RM_BOMB * (*size - SIZE_PRE_RADIUS), pastBody, deadBodies, nbDeadBodies, 1, BOMB_TEXTURE_RATE, 0);
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
}

/*void afficherScore(SDL_Renderer *renderer, SDL_Texture *scoreTexture, Score scoreAffichage, TTF_Font *font ){
	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, scoreAffichage.msg, (SDL_Color){255, 255, 255});
	SDL_SetSurfaceAlphaMod(surfaceMessage, ALPHA_SCORE[scoreAffichage.frame]);
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	SDL_Rect src = SCORE_SRC;
	SDL_Rect dest = {SCORE_DEST.w * OPEN_FONT_SIZE/scoreAffichage.size, SCORE_DEST.h * OPEN_FONT_SIZE/scoreAffichage.size};

	SDL_QueryTexture(Message,NULL,SDL_TEXTUREACCESS_STATIC,&(rect.w), &(rect.h) );
	rect.w /= (OPEN_FONT_SIZE / scoreAffichage.size);
	rect.h /= (OPEN_FONT_SIZE / scoreAffichage.size);

	rect.x = scoreAffichage.x - rect.w / 2;
	rect.y = scoreAffichage.y - rect.h / 2;

	SDL_RenderCopy(renderer, scoreTexture, NULL, &rect);

	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);

}*/

int strlen_num(int score)
{
	int count=1;

	while(score >= 10){
		score /=10;
		count++;
	}

	return count;
}

void afficherScore(SDL_Renderer *renderer , SDL_Texture *scoreTexture, Score scoreAffichage)
{
	int taille = strlen_num(scoreAffichage.score);
printf(" f %d\n",scoreAffichage.frame );
printf("s %d\n",scoreAffichage.score );

	SDL_Rect src = SCORE_SRC;
	SDL_Rect dest = {scoreAffichage.x + (scoreAffichage.size*taille)/2 , scoreAffichage.y - (FONT_HEIGHT_RATIO*scoreAffichage.size) / 2 , scoreAffichage.size, FONT_HEIGHT_RATIO*scoreAffichage.size };

	dest.x -= dest.w;
	//dest.y -= dest.h/2;
	for(int i=0 ; i < taille; i++)
	{
		src.x = SCORE_SRC.w * (scoreAffichage.score%10);
		SDL_SetTextureAlphaMod(scoreTexture, ALPHA_SCORE[scoreAffichage.frame]);
		printf("%d\n",scoreAffichage.frame );
		SDL_RenderCopy(renderer,scoreTexture,&src,&dest);
		scoreAffichage.score /=10;
		dest.x -= scoreAffichage.size;
	}
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

// int launchSnake(SDL_Window *myWindow, SDL_Renderer* renderer, char *identifiant, char *token){
int main(){
/////////////////////
/// MISE EN PLACE ///``
/////////////////////
	myInit();

	////////////
	/// Vars ///`
	////////////
	//Keyboard
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	int accelerate = ACCELERATE_DISABLED;

	//Time
	unsigned int lastTime = 0, currentTime;

	//Quantities of things gotten
	int nbPotion = 0;
	int frameUnkillable = 0;
	int score = 0;
	int nbFruitEaten = 0;

	//Snake
	size_t snakeSize;
	int turn;
	double dirAngle = BASE_ANGLE;
	float speedSnake = BASE_SPEED;
	double speedRestant = 0;
	SnakePart *snakeBody = NULL;
	Vector2f pastBody[REMIND_BODY];
	float radiusRestant = 0;
	SnakePart *deadBodies = malloc(sizeof(SnakePart));
	size_t nbDeadBodies = 0;
	float poisoned = 0;
	float poison = 0;
	float finalDeath = 0;
	float finalDeathRate = FINAL_DEATH_RATE_INIT;
	int frameAcce = 0;

	//Fruits
	size_t nbFruits = 1;
	Fruit *fruit;

	//Bonus
	Fruit bonus;

	//Score
	Score *scoreAffichage = malloc(sizeof(Score));
	size_t nbScoreAffichage = 0;

	//Fonts
	TTF_Font* scoreFont = TTF_OpenFont("./fonts/flappy.ttf", OPEN_FONT_SIZE);
	if( scoreFont == NULL ){
		printf("TTF_OpenFont() Failed: %s\n", TTF_GetError());
		return EXIT_FAILURE;
	}

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

	//audioMix_Chunk *s=Mix_LoadWAV(“sword.wav”);

	Mix_Chunk *flap_wav = Mix_LoadWAV( "../3_flappy_bird/Sounds/flap.wav" );

	if( !flap_wav)
		printf("Erreur chargement des sons: %s\n",Mix_GetError());

	SDL_Renderer *renderer = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED);
	if( renderer == NULL ){
		printf("Erreur lors de la creation d'un renderer : %s", SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_Rect playgroundView = {HUD_W/ratioWindowSize, HUD_H/ratioWindowSize, PLAYGROUND_SIZE_W, PLAYGROUND_SIZE_H};
	SDL_Rect hudView = {0, 0, (PLAYGROUND_SIZE_W + 2 * HUD_W), (PLAYGROUND_SIZE_H + 2 * HUD_H)};
	SDL_Rect hudDraw = {0, 0, (PLAYGROUND_SIZE_W + 2 * HUD_W)/ratioWindowSize, (PLAYGROUND_SIZE_H + 2 * HUD_H)/ratioWindowSize};

	//hud and menus
	int paused = 0;
	int rdyToPause = 1;

	//mouse
	SDL_Point mouseCoor;

	//Textures
	SDL_Texture *voileTexture = IMG_LoadTexture(renderer, "Textures/voile.png");
	SDL_Texture *pauseMenuTexture = IMG_LoadTexture(renderer, "Textures/pause.png");
	SDL_Texture *snakeTexture = IMG_LoadTexture(renderer, "Textures/snake.png");
	SDL_Texture *fruitTexture = IMG_LoadTexture(renderer, "Textures/fruits.png");
	SDL_Texture *spawnTexture = IMG_LoadTexture(renderer, "Textures/anim.png");
	SDL_Texture *hudTexture = IMG_LoadTexture(renderer, DIR_HUD);
	SDL_Texture *scoreTexture = IMG_LoadTexture(renderer, "Textures/chiffre.png");

	if( snakeTexture == NULL  ||  fruitTexture == NULL || spawnTexture == NULL || hudTexture == NULL ){// || voileTexture == NULL || pauseMenuTexture == NULL ){
		printf("Erreur lors de la creation de texture");
		return EXIT_FAILURE;
	}


	///////////////////////
	/// Initialize vars ///`
	///////////////////////

	//SNAKE
	init_body(&snakeSize, &snakeBody);
	initToV(pastBody, UNDEFINED, 0, REMIND_BODY);
	//Bonus
	bonus = (Fruit){UNDEFINED.x, UNDEFINED.y, 0, 0, NO_HIT, FRUIT_TTL + NB_FRAME_DEATH_FRUIT + 1};
	//Fruit
	initFruit(&fruit);
	spawnFruit(snakeBody[SIZE_PRE_RADIUS], &fruit[0], 0, nbFruits, nbFruitEaten, &bonus, UNDEFINED, FROM_NATURAL, 0);


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
					// fermer
					return 0;
					break;

				case SDL_MOUSEBUTTONDOWN:
					// gestion souris
					if( event.button.button == SDL_BUTTON_LEFT ){ // test
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
						rdyToPause = 1;
					break;

				case SDL_KEYDOWN:
					if ( event.key.keysym.sym == SDLK_ESCAPE && rdyToPause ){
						paused = !paused;
						rdyToPause = 0;
					}
					break;
			}
		}


	////////////////////////////
	// Handle Keyboard inputs //`
	////////////////////////////
		SDL_PumpEvents();

		if( keystate[SDL_SCANCODE_LEFT] )
			turn = LEFT;
		else if( keystate[SDL_SCANCODE_RIGHT] )
			turn = RIGHT;

		if( frameAcce  ||  keystate[SDL_SCANCODE_UP]  ||  keystate[SDL_SCANCODE_LSHIFT]  ||  keystate[SDL_SCANCODE_SPACE]  )
			accelerate = ACCELERATE_ENABLED;



		if( !paused && !done && !dead ){
	//////////////
	// Gameplay //`
	//////////////
			//poison
			poison += poisoned;
			if( poison >= 1){
				//printf("%f (%f)\n", poisoned, poisoned*FRAMES_PER_SECOND );
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
			speedRestant = move_snake(&snakeBody, pastBody, &snakeSize, accelerate * speedSnake + speedRestant, dirAngle, fruit, nbFruits, &bonus, &radiusRestant);

			//spawn more fruits
			if( rand() % (CHANCE_SPAWN_FRUIT * nbFruits) == 0 ){
				fruit = realloc(fruit, sizeof(Fruit) * (++nbFruits));
				spawnFruit(snakeBody[SIZE_PRE_RADIUS], fruit, nbFruits - 1, nbFruits, nbFruitEaten, &bonus, UNDEFINED, FROM_NATURAL, 0);
			}

			//spawn more bonus
			if( bonus.frame > FRUIT_TTL + NB_FRAME_DEATH_FRUIT && rand() % (CHANCE_SPAWN_BONUS) == 0 )
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
					eat_fruit(&snakeBody, &snakeSize, &fruit, &nbFruits, fruit[i], &speedSnake, &score, &nbFruitEaten, pastBody, &bonus, &deadBodies, &nbDeadBodies, &nbPotion, &poisoned, &scoreAffichage, &nbScoreAffichage, &frameAcce, flap_wav);

					for( int j = i; j<nbFruits - 1; j++ )
						fruit[j] = fruit[j + 1];

					if( nbFruits == 1 )
						spawnFruit(snakeBody[SIZE_PRE_RADIUS], fruit, i, nbFruits, nbFruitEaten, &bonus, UNDEFINED, FROM_NATURAL, 0);
					else
						fruit = realloc(fruit, sizeof(Fruit) * (--nbFruits));
				}
			}

			//Head-bonus
			if( bonus.hitbox == HIT ){

				bonus.hitbox = NO_HIT;

				eat_fruit(&snakeBody, &snakeSize, &fruit, &nbFruits, bonus, &speedSnake, &score, &nbFruitEaten, pastBody, &bonus, &deadBodies, &nbDeadBodies, &nbPotion, &poisoned, &scoreAffichage, &nbScoreAffichage, &frameAcce, flap_wav);
				bonus.frame = FRUIT_TTL + NB_FRAME_DEATH_FRUIT ;
				if(snakeSize == SIZE_PRE_RADIUS){
					done = 1;
					dead = 1;
				}
			}
		}
		else if( !paused && !dead ){ //death anim
			finalDeath += finalDeathRate;
			if( finalDeath >= 1){
				//printf("%f (%f)\n", poisoned, poisoned*FRAMES_PER_SECOND );
				rm_body(&snakeBody, &snakeSize, (int)finalDeath, pastBody, &deadBodies, &nbDeadBodies, 1, 1, INIT_FRAME_DEATH);
				finalDeath -= (int)finalDeath;
				if(snakeSize == SIZE_PRE_RADIUS){
					done = 1;
					dead = 1;
				}
			}
			finalDeathRate *= FINAL_DEATH_RATE_GROW;
		}


	//////////
	// Draw //`
	//////////

		//Snake
		if( !dead )
			afficherBody(renderer, snakeTexture, snakeBody, snakeSize, frameUnkillable);

		//Dead bodies
		for( int i = 0; i < nbDeadBodies; i++ )
			afficherDeadBody(renderer, spawnTexture, deadBodies[i]);

		//fruits
		for( int i = 0; i < nbFruits; i++ )
			afficherFruit(renderer, fruitTexture, spawnTexture, fruit[i]);

		//score
		for( int i = 0; i< nbScoreAffichage; i++ )
			afficherScore(renderer, scoreTexture, scoreAffichage[i]);

		//bonus
		if(bonus.frame < FRUIT_TTL + NB_FRAME_DEATH_FRUIT)
			afficherFruit(renderer, fruitTexture, spawnTexture, bonus);



		//pauseMenu
		if(paused)
			afficherPauseMenu(renderer, mouseCoor, voileTexture, pauseMenuTexture);


		if(dead){ //afficher menu mort

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
		if( !paused ){

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

			//fruitTimeout
			for( int i = 0; i<nbFruits; i++ ){
				fruit[i].frame++;

				if( fruit[i].frame > FRUIT_TTL + NB_FRAME_DEATH_FRUIT ){

					if( fruit[i].id != CAFE ){
						nbFruitEaten -= 2;
						if( nbFruitEaten<0 )
							nbFruitEaten = 0;
					}

					for( int j = i; j<nbFruits - 1; j++ )
						fruit[j] = fruit[j + 1];

					if( nbFruits == 1 && !done)
						spawnFruit(snakeBody[SIZE_PRE_RADIUS], fruit, i, nbFruits, nbFruitEaten, &bonus, UNDEFINED, FROM_NATURAL, 0);
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
			if( bonus.frame == FRUIT_TTL + NB_FRAME_DEATH_FRUIT +1 )
				initBonus(&bonus);

			//malus timeout
			if( frameAcce )
				frameAcce--;

			//Frame incrementations
			speedSnake += SCALE_SPEED;
			poisoned *= POISON_SCALE;
		}

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
	printf("Waw t'es nul, %d\n", score);
	return 0;
}
