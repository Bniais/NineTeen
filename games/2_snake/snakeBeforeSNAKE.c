#include "snake.h"
#include "config.h"
#include <stdlib.h>
#include <time.h>

void myInit() {

  // FRUIT_ADD_RADIUS
  int nbZero;
  for (int i = 0; i < NB_FRUITS; i++) {
    for (int j = 0; j < SIZE_PRE_RADIUS / 2; j++) {
      float t = (j - (1.2 / j)) * FRUIT_PROPRIETES[i][DIGESTION_SIZE] / (SIZE_PRE_RADIUS / 2);
      if (t >= 0) {
        FRUIT_ADD_RADIUS[i][SIZE_PRE_RADIUS - 1 - j] = t;
        FRUIT_ADD_RADIUS[i][j] = t;

      } else {
        FRUIT_ADD_RADIUS[i][j] = 0;
        FRUIT_ADD_RADIUS[i][SIZE_PRE_RADIUS - 1 - j] = 0;
      }
    }
    FRUIT_ADD_RADIUS[i][(SIZE_PRE_RADIUS / 2)] = FRUIT_PROPRIETES[i][DIGESTION_SIZE];
  }
  for (int i = 0; i < NB_FRUITS; i++) {
    nbZero=0;
    for (int j = SIZE_PRE_RADIUS-1; j >= 0 && !FRUIT_ADD_RADIUS[i][j]; j--)
      nbZero++;
    if(nbZero)
      for(int k=SIZE_PRE_RADIUS-1-nbZero; k>=0; k--){
        FRUIT_ADD_RADIUS[i][k+nbZero]=FRUIT_ADD_RADIUS[i][k];
        FRUIT_ADD_RADIUS[i][k]=0;
      }
  }

  for(int i=0; i<SIZE_PRE_RADIUS;i++){
    for(int j=0; j<SIZE_PRE_RADIUS;j++)
      printf("%3d ",FRUIT_ADD_RADIUS[i][j] );
    printf("\n");
  }

  // FRUIT_ADD_RADIUS_GIANT
  for (int i = 0; i < NB_FRUITS; i++) {
    for (int j = 0; j < SIZE_PRE_RADIUS / 2; j++) {
      float t = ((j - (1.2 / j)) * (FRUIT_PROPRIETES[i][DIGESTION_SIZE]  + GIANT_DIGESTION) / (SIZE_PRE_RADIUS / 2));
      if (t >= 0) {
        FRUIT_ADD_RADIUS_GIANT[i][SIZE_PRE_RADIUS - 1 - j] = t;
        FRUIT_ADD_RADIUS_GIANT[i][j] = t;

      } else {
        FRUIT_ADD_RADIUS_GIANT[i][j] = 0;
        FRUIT_ADD_RADIUS_GIANT[i][SIZE_PRE_RADIUS - 1 - j] = 0;
      }
    }
    FRUIT_ADD_RADIUS_GIANT[i][(SIZE_PRE_RADIUS / 2)] = FRUIT_PROPRIETES[i][DIGESTION_SIZE] + GIANT_DIGESTION;
  }
  for (int i = 0; i < NB_FRUITS; i++) {
    nbZero=0;
    for (int j = SIZE_PRE_RADIUS-1; j >= 0 && !FRUIT_ADD_RADIUS_GIANT[i][j]; j--)
      nbZero++;
    if(nbZero)
      for(int k=SIZE_PRE_RADIUS-1-nbZero; k>=0; k--){
        FRUIT_ADD_RADIUS_GIANT[i][k+nbZero]=FRUIT_ADD_RADIUS_GIANT[i][k];
        FRUIT_ADD_RADIUS_GIANT[i][k]=0;
      }
  }
  for(int i=0; i<SIZE_PRE_RADIUS;i++){
    for(int j=0; j<SIZE_PRE_RADIUS;j++)
      printf("%3d ",FRUIT_ADD_RADIUS_GIANT[i][j] );
    printf("\n");
  }

  // SDL Init
  SDL_Init(SDL_INIT_EVERYTHING);

  // Settings
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

  //rand
  srand(time(NULL));

}

void initFruit(Fruit** fruit){
  *fruit=malloc(sizeof(Fruit));
  (*fruit)->x=UNDEFINED.x;
  (*fruit)->y=UNDEFINED.x;
  (*fruit)->giant=0;
  (*fruit)->id=NO_ID;
  (*fruit)->hitbox=0;
  (*fruit)->frame=-NB_FRAME_SPAWN_FRUIT;
}

void test(char *s, float f, int delay) {
  printf("%s : %f \n", s, f);
  SDL_Delay(delay);
}

double tooClose(Vector2f v1, Vector2f v2) {
  return INIT_DECAL.y /
         sqrt((v2.x - v1.x) * (v2.x - v1.x) + (v2.y - v1.y) * (v2.y - v1.y));
}

void turnLeft(double *angle) {
  *angle -= TURN_AMMOUNT;
  if (*angle < 0)
    *angle += 2 * PI;
}

void turnRight(double *angle) { *angle += TURN_AMMOUNT; }

Vector2f angleToVector2f(double angle) {
  Vector2f v;
  v.x = cos(angle);
  v.y = sin(angle);
  return v;
}

double norme(Vector2f v) { return sqrt(v.x * v.x + v.y * v.y); }

double Vector2fToAngle(Vector2f v) {
  Vector2f v2 = {1, 0};
  return acos((v.x * v2.x + v.y * v2.y) / (norme(v) * norme(v2)));
}

double dirBetween(Vector2f v1, Vector2f v2) {
  Vector2f v3;
  v3.x = v2.x - v1.x;
  v3.y = v2.y - v1.y;
  return Vector2fToAngle(v3);
}

SDL_Point v2fToPoint(Vector2f v) {
  return (SDL_Point){roundf(v.x), roundf(v.y)};
}

void init_body(size_t *size, float **bodyRadius, Vector2f **body) {
  *bodyRadius = malloc(sizeof(float) * (INIT_BODY + SIZE_PRE_RADIUS));
  *body = malloc(sizeof(Vector2f) * INIT_BODY);
  (*body)[0] = INIT_HEAD;

  for (int i = 1; i < INIT_BODY; i++)
    (*body)[i] = (Vector2f){(*body + i - 1)->x + INIT_DECAL.x,
                            (*body + i - 1)->y + INIT_DECAL.y};

  for (int i = 0; i < INIT_BODY + SIZE_PRE_RADIUS; i++)
    (*bodyRadius)[i] = 0;

  *size = INIT_BODY;
}

void initToV(Vector2f *vTab, Vector2f v, int iStart, int iEnd) {
  for (int i = iStart; i < iEnd; i++)
    vTab[i] = v;
}

void decale_pastBody_oldBecomeNew(Vector2f pastBody[REMIND_BODY],
                                  int nbDecale) {
  for (int i = nbDecale; i < REMIND_BODY; i++)
    pastBody[i - nbDecale] = pastBody[i];

  initToV(pastBody, UNDEFINED, REMIND_BODY - nbDecale, REMIND_BODY);
}

int add_body(float **bodyRadius, Vector2f **body, size_t *size, Vector2f pastBody[REMIND_BODY], int speed) {

  int nbAdd = (*bodyRadius)[*size + SIZE_PRE_RADIUS - 1] / RATIO_RADIUS_BODYADD;
  (*bodyRadius)[*size + SIZE_PRE_RADIUS - 1] = 0;

  *bodyRadius = realloc(*bodyRadius, (*size + SIZE_PRE_RADIUS + nbAdd) * sizeof(float));
  *body = realloc(*body, ((*size + nbAdd) * sizeof(Vector2f)));

  for (int i = 0; i < nbAdd; i++){
    if(pastBody[i].x == UNDEFINED.x){
      (*body)[*size + i].x=(*body)[*size + i-1].x + (*body)[*size + i-1].x - (*body)[*size + i-2].x;
      (*body)[*size + i].y=(*body)[*size + i-1].y + (*body)[*size + i-1].y - (*body)[*size + i-2].y;
    }
    else
      (*body)[*size + i] = pastBody[i];
  }

  for (int i = 0; i < nbAdd; i++)
    (*bodyRadius)[*size + SIZE_PRE_RADIUS + i] = 0;

  decale_pastBody_oldBecomeNew(pastBody, nbAdd);

  *size += nbAdd;

  return nbAdd;
}

void eat_fruit(float *bodyRadius, Fruit fruit, float *speed, int *score, int *nbFruitEaten) {
  if(fruit.giant)
    for (int i = 0; i < SIZE_PRE_RADIUS; i++)
      bodyRadius[i] += FRUIT_ADD_RADIUS_GIANT[fruit.id][i];
  else
    for (int i = 0; i < SIZE_PRE_RADIUS; i++)
      bodyRadius[i] += FRUIT_ADD_RADIUS[fruit.id][i];

  *speed += (fruit.giant==0?1:GIANT_SPEED) * FRUIT_PROPRIETES[fruit.id][ACCELERATION];
  if(*speed<MIN_SPEED)
    *speed=MIN_SPEED;

  *score += (fruit.giant==0?1:GIANT_SPEED) * FRUIT_PROPRIETES[fruit.id][SCORE];

  (*nbFruitEaten)++;
}

void afficher_body(SDL_Renderer *renderer, SDL_Texture *bodyTexture, SDL_Texture *headTexture, Vector2f *body, float *bodyRadius, size_t size) {
  SDL_Rect currentBody;
  // Body
  for (int i = size - 1; i > 0; i--) {
    currentBody = (SDL_Rect){ roundf(body[i].x) - (BODY_RADIUS + bodyRadius[i + SIZE_PRE_RADIUS]),
                              roundf(body[i].y) - (BODY_RADIUS + bodyRadius[i + SIZE_PRE_RADIUS]),
                              (BODY_RADIUS + bodyRadius[i + SIZE_PRE_RADIUS]) * 2,
                              (BODY_RADIUS + bodyRadius[i + SIZE_PRE_RADIUS]) * 2};

    SDL_RenderCopy(renderer, bodyTexture, NULL, &currentBody);
  }
  // Head
  currentBody = (SDL_Rect){ roundf(body[0].x) - (BODY_RADIUS + bodyRadius[SIZE_PRE_RADIUS]),
                            roundf(body[0].y) - (BODY_RADIUS + bodyRadius[SIZE_PRE_RADIUS]),
                            (BODY_RADIUS + bodyRadius[SIZE_PRE_RADIUS]) * 2,
                            (BODY_RADIUS + bodyRadius[SIZE_PRE_RADIUS]) * 2};

  SDL_RenderCopy(renderer, headTexture, NULL, &currentBody);
}

int hitboxQueue(Vector2f *body, float *bodyRadius, size_t size) {
  for (int i = BODY_DEATH_HITBOX; i < size; i++)
    if (norme((Vector2f){body[0].x - body[i].x, body[0].y - body[i].y}) <
        bodyRadius[0] + bodyRadius[i] + 2 * BODY_RADIUS)
      return 1;
  return 0;
}

int tooCloseFromHead(Vector2f head, Fruit fruit) {
  return (norme((Vector2f){fruit.x - head.x, fruit.y - head.y}) < DIST_HEAD_FRUIT + FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE) + BODY_RADIUS);
}

int tooCloseFromWall(Vector2f fruit, int dist) {
  return (fruit.x < dist || fruit.x > SNAKE_PLAYGROUND_SIZE_W - dist || fruit.y < dist || fruit.y > SNAKE_PLAYGROUND_SIZE_H - dist);
}

int tooCloseFromCorner(Fruit fruit) {
  return (norme((Vector2f){fruit.x - 0, fruit.y - 0}) < DIST_CORNER_FRUIT + FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE) ||
          norme((Vector2f){fruit.x - SNAKE_PLAYGROUND_SIZE_W, fruit.y - 0}) < DIST_CORNER_FRUIT + FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE) ||
          norme((Vector2f){fruit.x - SNAKE_PLAYGROUND_SIZE_W, fruit.y - SNAKE_PLAYGROUND_SIZE_H}) < DIST_CORNER_FRUIT + FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE) ||
          norme((Vector2f){fruit.x - 0, fruit.y - SNAKE_PLAYGROUND_SIZE_H}) < DIST_CORNER_FRUIT + FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE));
}

int tooCloseFromFruit(Fruit* fruit, int rang, int nbFruits){
  for(int i=0; i<nbFruits; i++)
    if(i != rang && norme((Vector2f){fruit[rang].x - fruit[i].x, fruit[rang].y - fruit[i].y}) < FRUIT_PROPRIETES[fruit[rang].id][RADIUS] + FRUIT_PROPRIETES[fruit[i].id][RADIUS] + DIST_FRUITS  )
      return 1;
  return 0;
}

void spawnFruit(Vector2f head, Fruit *fruit, int rang, int nbFruitEaten, int nbFruits) {

  fruit[rang].x=UNDEFINED.x;
  fruit[rang].y=UNDEFINED.x;
  fruit[rang].giant=0;
  fruit[rang].id=NO_ID;
  fruit[rang].hitbox=0;
  fruit[rang].frame=-NB_FRAME_SPAWN_FRUIT;

  printf("nb f :: %d\n",nbFruitEaten );
  //fruit.id
  int nbRand = 0;

  for(int i=0; i<NB_FRUITS-NB_BONUSES; i++)
    if(FRUIT_PROPRIETES[i][MIN_FRUIT_TO_APPEAR] <= nbFruitEaten)
      nbRand += FRUIT_PROPRIETES[i][PROBA];

  printf("nb r :: %d\n",nbRand );
  nbRand = rand() % nbRand;
  printf("nb r :: %d\n",nbRand );
  int i;

  for(i=0; i<NB_FRUITS-NB_BONUSES && nbRand>=FRUIT_PROPRIETES[i][PROBA]; i++)
    if(FRUIT_PROPRIETES[i][MIN_FRUIT_TO_APPEAR] <= nbFruitEaten)
        nbRand -= FRUIT_PROPRIETES[i][PROBA];

  fruit[rang].id=i;
  printf("id :: %d\n",fruit[rang].id );

  //fruit.giant
  if(nbFruitEaten >= UNLOCK_GIANT){
    fruit[rang].giant = rand() % GIANT_CHANCE;
    if(fruit[rang].giant != 1)
      fruit[rang].giant = 0;
  }
  //fruit coordinates
  i=-1;
  do {
    fruit[rang].x = rand() % (int)(SNAKE_PLAYGROUND_SIZE_W);
    fruit[rang].y = rand() % (int)(SNAKE_PLAYGROUND_SIZE_H);
    i++;
  } while ( i < MAX_TRIES_RAND && (tooCloseFromFruit(fruit, rang, nbFruits) || tooCloseFromHead(head, fruit[rang]) || tooCloseFromWall((Vector2f){fruit[rang].x, fruit[rang].y}, DIST_WALL_FRUIT + FRUIT_PROPRIETES[fruit[rang].id][RADIUS] + (fruit[rang].giant * GIANT_SIZE)) || tooCloseFromCorner(fruit[rang])) );

  //Moins de conditions de spawn
  if(i == MAX_TRIES_RAND){
    i=0;
    do {
      fruit[rang].x = rand() % (int)(SNAKE_PLAYGROUND_SIZE_W);
      fruit[rang].y = rand() % (int)(SNAKE_PLAYGROUND_SIZE_H);
      i++;
    } while ( i < MAX_TRIES_RAND && ( tooCloseFromHead(head, fruit[rang]) || tooCloseFromWall((Vector2f){fruit[rang].x, fruit[rang].y}, DIST_WALL_FRUIT + FRUIT_PROPRIETES[fruit[rang].id][RADIUS] + (fruit[rang].giant * GIANT_SIZE)) || tooCloseFromCorner(fruit[rang])) );
  }

}

int hitboxFruit(Fruit fruit, Vector2f head, double headRadius) {
  return (norme((Vector2f){fruit.x - head.x, fruit.y - head.y}) <= FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE) + BODY_RADIUS);
}

void afficher_fruit(SDL_Renderer *renderer, SDL_Texture *fruitTexture, SDL_Texture *spawnTexture, Fruit fruit) {
  SDL_Rect currentBody = (SDL_Rect){ roundf(fruit.x) - (FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE)),
                                     roundf(fruit.y) - (FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE)),
                                     (FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE)) * 2 ,
                                     (FRUIT_PROPRIETES[fruit.id][RADIUS] + (fruit.giant * GIANT_SIZE)) * 2 };
  int blinking=0;
  if(fruit.frame >= BLINK_FRAMES[0])
    for(int i = 0; i < NB_BLINK; i++)
      if(fruit.frame == BLINK_FRAMES[i])
        blinking=1;

  if(blinking){
    SDL_Rect src = (SDL_Rect){fruit.id * FRUIT_DIM.x, FRUIT_DIM.y * (RANGEE_BLINK-1), FRUIT_DIM.x, FRUIT_DIM.y};
    SDL_RenderCopy(renderer, fruitTexture, &src, &currentBody);
    printf("wow it blinks\n");
  }
  else if(fruit.frame>=FRUIT_TTL){
    SDL_Rect src = (SDL_Rect){ (fruit.frame - FRUIT_TTL) / (NB_FRAME_DEATH_FRUIT/NB_ANIM_DEATH) * FRUIT_DIM.x, FRUIT_DIM.y, FRUIT_DIM.x, FRUIT_DIM.y};
    SDL_RenderCopy(renderer, spawnTexture, &src, &currentBody);
  }
  else if(fruit.frame >= 0){
    SDL_Rect src = (SDL_Rect){fruit.id * FRUIT_DIM.x, 0, FRUIT_DIM.x, FRUIT_DIM.y};
    SDL_RenderCopy(renderer, fruitTexture, &src, &currentBody);
  }
  else{
    SDL_Rect src = (SDL_Rect){ (fruit.frame + NB_FRAME_SPAWN_FRUIT) / (NB_FRAME_SPAWN_FRUIT/NB_ANIM_SPAWN) * FRUIT_DIM.x, 0, FRUIT_DIM.x, FRUIT_DIM.y};
    SDL_RenderCopy(renderer, spawnTexture, &src, &currentBody);
  }

}

void decale_pastBody_freeSpace(Vector2f pastBody[REMIND_BODY]) {
  for (int i = REMIND_BODY - 2; i >= 0; i--)
    pastBody[i + 1] = pastBody[i];
}

void decale_radius(float **bodyRadius, size_t *size, Vector2f **body, Vector2f pastBody[REMIND_BODY], Fruit fruit, float speed) {
  int nbAdd = 0;

  // Si la queue est grosse, on ralonge le serpent
  if ((*bodyRadius)[*size + SIZE_PRE_RADIUS - 1] >= RATIO_RADIUS_BODYADD)
    nbAdd = add_body(bodyRadius, body, size, pastBody, speed);

  // On décale tout
  for (int i = *size + SIZE_PRE_RADIUS - 2; i >= 0; i--)
    (*bodyRadius)[i + 1] = (*bodyRadius)[i];
  (*bodyRadius)[0] = 0;

  // On décale le corps digéré jusqu'au bout de la queue
  if (nbAdd) {
    int i;
    for (i = *size + SIZE_PRE_RADIUS - 2 - nbAdd/3; !(*bodyRadius)[i] && i >= 0; i--);
    for (; (*bodyRadius)[i] && i >= 0; i--)
      (*bodyRadius)[i + nbAdd - 1] = (*bodyRadius)[i];
    for (int j = 0; j < nbAdd; j++)
      (*bodyRadius)[i - 1 + j] = 0;
  }
}

float move_snake(Vector2f **body, float **bodyRadius, Vector2f pastBody[REMIND_BODY], size_t *size, float speed, double dir, Fruit *fruit, size_t nbFruits) {
  // Verif speed not too high

  if (speed == SPEED_DECOMPOSITION) {

    // Se souvenir de l'emplacement qui va partir
    decale_pastBody_freeSpace(pastBody);
    pastBody[0] = (*body)[*size - 1];

    // Bouger
    for (int i = *size - 1; i > 0; i--) {
      (*body)[i].x = (*body)[i - 1].x;
      (*body)[i].y = (*body)[i - 1].y;
    }
    (*body)[0].x += angleToVector2f(dir).x * speed;
    (*body)[0].y += angleToVector2f(dir).y * speed;

    for(int i=0; i<nbFruits; i++)
      if (fruit[i].frame > 0 && fruit[i].frame < FRUIT_TTL && hitboxFruit(fruit[i], (*body)[0], (*bodyRadius)[SIZE_PRE_RADIUS]))
        fruit[i].hitbox = HIT;

    return speed;
  } else {
    int i;
    // On décompose le mouvement en mouvements de speed inférieurs
    while (speed > SPEED_DECOMPOSITION) {
      move_snake(body, bodyRadius, pastBody, size, SPEED_DECOMPOSITION, dir, fruit, nbFruits);
      speed -= SPEED_DECOMPOSITION;
      i++;
      decale_radius(bodyRadius, size, body, pastBody, *fruit, speed);
      if (i % 2)
        decale_radius(bodyRadius, size, body, pastBody, *fruit, speed);
    }
  }
  return speed;
}



// int launchSnake(SDL_Window *myWindow, SDL_Renderer* renderer, char
// *identifiant, char *token){


int main() {

  // initialize SDL
  myInit();
  printf("(hu)\n");
  // Creation fenêtre
  SDL_Window *myWindow = SDL_CreateWindow("Snake", WINDOW_X, WINDOW_Y, SNAKE_PLAYGROUND_SIZE_W, SNAKE_PLAYGROUND_SIZE_H, WINDOW_FLAG);

  // Creation renderer
  SDL_Renderer *renderer = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED);

  if (renderer == NULL){// Gestion des erreurs
    printf("Erreur lors de la creation d'un renderer : %s", SDL_GetError());
    return EXIT_FAILURE;
  }

  // Load textures
  SDL_Texture *bodyTexture = IMG_LoadTexture(renderer, "Textures/ecaille.png");
  SDL_Texture *headTexture = IMG_LoadTexture(renderer, "Textures/snakeHead.png");
  SDL_Texture *fruitTexture = IMG_LoadTexture(renderer, "Textures/fruits.png");
  SDL_Texture *spawnTexture = IMG_LoadTexture(renderer, "Textures/anim.png");
  if (bodyTexture == NULL || headTexture == NULL || fruitTexture == NULL) {
    printf("Erreur lors de la creation de texture : %s", SDL_GetError());
    return EXIT_FAILURE;
  }

  // Vars
  const Uint8 *keystate = SDL_GetKeyboardState(NULL);
  unsigned int lastTime = 0, currentTime;
  int turn;
  int nbPotion = 0;
  int score = 0;
  int nbFruitEaten = 0;

  double dirAngle = BASE_ANGLE;

  size_t snakeSize;
  float speedSnake = BASE_SPEED;
  double speedRestant = 0;
  float accelerate = ACCELERATE_DISABLED;
  float *bodyRadius = NULL;
  Vector2f *snakeBody = NULL;
  Vector2f pastBody[REMIND_BODY];   // On donne une taille supérieure au cas où on mange plusieurs fruits de suite
  init_body(&snakeSize, &bodyRadius, &snakeBody);
  initToV(pastBody, UNDEFINED, 0, REMIND_BODY);

  size_t nbFruits=1;
  Fruit* fruit;
  initFruit(&fruit);

  spawnFruit(snakeBody[0], &fruit[0], 0, nbFruitEaten, nbFruits);

  for(int i=0; i<NB_BLINK; i++)
    printf("%d ", BLINK_FRAMES[i] );
  printf("\n %d \n", FRUIT_TTL );

  //Jeu
  int done = 0;
  while (!done) {
    // Init input
    turn = NO_TURN;
    accelerate = ACCELERATE_DISABLED;

    // Events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        // fermer
        return 0;
        break;

      case SDL_MOUSEBUTTONDOWN:
        // gestion souris
        if (event.button.button == SDL_BUTTON_LEFT) // test
          return 0;
        break;
      }
    }

    // Keyboard inputs
    SDL_PumpEvents();
    if (keystate[SDL_SCANCODE_LEFT])
      turn = LEFT;
    if (keystate[SDL_SCANCODE_RIGHT])
      turn = RIGHT;
    if (keystate[SDL_SCANCODE_UP] || keystate[SDL_SCANCODE_SPACE] || keystate[SDL_SCANCODE_LSHIFT])
      accelerate = ACCELERATE_ENABLED;


    // Gameplay
    if (turn == LEFT)
      turnLeft(&dirAngle);
    else if (turn == RIGHT)
      turnRight(&dirAngle);

    speedRestant = move_snake(&snakeBody, &bodyRadius, pastBody, &snakeSize, accelerate * speedSnake + speedRestant, dirAngle, fruit, nbFruits);

    if( rand()%(CHANCE_SPAWN*nbFruits) == 0 ){
      fruit=realloc(fruit, sizeof(Fruit) * (++nbFruits));
      spawnFruit(snakeBody[0], fruit, nbFruits-1, nbFruitEaten, nbFruits);
    }


    // Check hitboxs
    if (tooCloseFromWall(snakeBody[0], BODY_RADIUS + bodyRadius[SIZE_PRE_RADIUS]) || hitboxQueue(snakeBody, bodyRadius, snakeSize))
      done = 1;

    for(int i=0; i<nbFruits; i++){
      if (fruit[i].hitbox == HIT) {
        fruit[i].hitbox = NO_HIT;
        eat_fruit(bodyRadius, fruit[i], &speedSnake, &score, &nbFruitEaten);
        for(int j=i; j<nbFruits-1; j++)
          fruit[j]=fruit[j+1];

        if(nbFruits == 1)
          spawnFruit(snakeBody[0], fruit, i, nbFruitEaten, nbFruits);
        else
          fruit=realloc(fruit, sizeof(Fruit) * (--nbFruits));

      }
    }

    // Dessiner
    afficher_body(renderer, bodyTexture, headTexture, snakeBody, bodyRadius, snakeSize);
    for(int i=0; i<nbFruits; i++)
      afficher_fruit(renderer, fruitTexture, spawnTexture, fruit[i]);
    // SDL_RenderSetLogicalSize(renderer, 2,2);
    //  SDL_RenderSetScale(renderer, 2, 2);
    SDL_RenderPresent(renderer);

    //fruitTimeout
    for(int i=0; i<nbFruits; i++){
      fruit[i].frame++;
      if (fruit[i].frame > FRUIT_TTL + NB_FRAME_DEATH_FRUIT) {

        nbFruitEaten-=2;
        if(nbFruitEaten<0)
          nbFruitEaten=0;

        for(int j=i; j<nbFruits-1; j++)
          fruit[j]=fruit[j+1];
        if(nbFruits == 1)
          spawnFruit(snakeBody[0], fruit, i, nbFruitEaten, nbFruits);
        else
          fruit=realloc(fruit, sizeof(Fruit) * (--nbFruits));
      }
    }

    // Next frame
    currentTime = SDL_GetTicks();
    while (currentTime - lastTime < 1000 / FRAMES_PER_SECOND) {
      currentTime = SDL_GetTicks();
    }

    if(currentTime - lastTime >34)
      printf(" TIME FRAME : %d\n", currentTime - lastTime);



  //  printf("SIZE : %ld\n", snakeSize);
    lastTime = currentTime;
    speedSnake += SCALE_SPEED;

    // On efface
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
  }
  printf("Waw t'es nul, %d\n", score);
  return 0;
}
