/////////////////
//SNAKE CONFIGS//
/////////////////
#define NO_TURN 0
#define LEFT 1
#define RIGHT 2
#define ACCELERATE_DISABLED 1
#define ACCELERATE_ENABLED 2
#define HIT 1
#define NO_HIT 0
typedef struct {float x; float y;} Vector2f;
const Vector2f UNDEFINED = {-500, -500};


//Window
#define FRAMES_PER_SECOND 30
const int FRAME_TIME = 1000 / FRAMES_PER_SECOND;
//const SDL_Point SNAKE_PLAYGROUND_SIZE = {1500, 700};


//Snake
typedef struct {float x; float y; int frame; float radius;} SnakePart;
const Vector2f INIT_HEAD = {PLAYGROUND_SIZE_W / 2, PLAYGROUND_SIZE_H / 2};
#define BODY_RADIUS 20
#define INIT_BODY 20
const Vector2f INIT_DECAL = {0, 5};
#define BASE_ANGLE 3 * PI / 2
#define TURN_AMMOUNT 0.13
#define REMIND_BODY 200
#define BODY_DEATH_HITBOX 35
#define MIN_BODY_PARTS 5
#define NB_FRAME_DEATH_BODY 12
#define NB_ANIM_DEATH_BODY 4
const SDL_Point BODY_DIM = {64, 64};
#define NB_FRAME_INVINCIBILITY 8 * FRAMES_PER_SECOND
#define NB_BLINK_BODY 15
const int BLINK_FRAMES_BODY[NB_BLINK_BODY] = {(int)(FRAMES_PER_SECOND * 4.5  + ((NB_FRAME_INVINCIBILITY / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 5.3  + ((NB_FRAME_INVINCIBILITY / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 5.75 + ((NB_FRAME_INVINCIBILITY / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 6.15 + ((NB_FRAME_INVINCIBILITY / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 6.55 + ((NB_FRAME_INVINCIBILITY / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 6.85 + ((NB_FRAME_INVINCIBILITY / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.05 + ((NB_FRAME_INVINCIBILITY / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.22 + ((NB_FRAME_INVINCIBILITY / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.37 + ((NB_FRAME_INVINCIBILITY / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.45 + ((NB_FRAME_INVINCIBILITY / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.55 + ((NB_FRAME_INVINCIBILITY / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.65 + ((NB_FRAME_INVINCIBILITY / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.75 + ((NB_FRAME_INVINCIBILITY / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.85 + ((NB_FRAME_INVINCIBILITY / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.93 + ((NB_FRAME_INVINCIBILITY / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND)};
#define RANGEE_INVICIBLE 2
#define RANGEE_BLINK_BODY 3


//Speed
#define BASE_SPEED 7
#define MIN_SPEED 5
#define SPEED_DECOMPOSITION 5
#define SCALE_SPEED 0.0035

//Fruit
typedef struct {float x; float y; int giant; int id; int hitbox; int frame; Vector2f dest; float coefRadius;} Fruit;
#define NO_ID -1
#define DIST_HEAD_FRUIT 30
#define DIST_WALL_FRUIT 30
#define DIST_CORNER_FRUIT 60
#define DIST_FRUITS 10
const SDL_Point FRUIT_DIM = {64, 64};
#define RANGEE_BLINK 2
#define NB_FRUITS 25
#define RATIO_RADIUS_BODYADD 5
#define GIANT_SIZE 40
#define GIANT_DIGESTION 15
#define GIANT_SCORE 10
#define GIANT_SPEED 2
#define UNLOCK_GIANT 0
#define GIANT_CHANCE 200
#define MAX_TRIES_RAND 1000
#define CHANCE_SPAWN_FRUIT (4 * FRAMES_PER_SECOND)
#define FRUIT_TTL (FRAMES_PER_SECOND * 10)
#define NB_FRAME_SPAWN_FRUIT 12
#define NB_ANIM_SPAWN 4
#define NB_FRAME_DEATH_FRUIT 12
#define NB_ANIM_DEATH 4
typedef enum {FRAISE, ORANGE, CITROUILLE, PIMENT, CERISE, POMME, PASTEQUE, CAROTTE, ANANAS, TOMATE, FROMAGE, VIANDE, PIZZA, BURGER, HOT_DOG, PANCAKES, SUCETTE, GLACE_BATON, GLACE_CONE, GLACE_POT, DONUT, MUFFIN, GATEAU, MUFFIN_ROSE, CAFE, PLUME, BOMBE, COFFRE, ARC_EN_CIEL, POTION_HITBOX ,POTION_VERTE, POTION_JAUNE} FRUITS;
#define FROM_NATURAL 0
#define FROM_COFFRE 1
#define FROM_RAINBOW 2
#define HITBOX_SECURITY 1

//Bonuses
#define FLAT_REDUCE_SCISOR 10
#define RELATIVE_REDUCE_SCISOR 0.15
#define DURATION_POTION 180
#define RELATIVE_ACCELERATE_COFEE 0.25
#define NB_BONUSES 7
#define CHANCE_SPAWN_BONUS  (1 * FRAMES_PER_SECOND)
#define FLAT_RM_BOMB 10
#define RELATIVE_RM_BOMB 0.2
#define RELATIVE_SLOW_FEATHER 0.3
#define COFFRE_SPAWN_MIN 50
#define COFFRE_SPAWN_RANGE 100
#define NB_SPAWN_COFFRE 5
#define CHEST_BOOST_FRUIT_EATEN 5
#define FRAME_RETARD_COFFRE 2
#define NB_SPAWN_ARC_EN_CIEL 12
#define RAINBOW_BOOST_FRUIT_EATEN 5
#define DIST_WALL_FRUIT_RAINBOW 80
#define FRAME_RETARD_RAINBOW 3
#define POTION_POISON_ADD (1.5 / FRAMES_PER_SECOND)
#define POISON_SCALE 1.001
#define BOMB_TEXTURE_RATE 4

//Malus
#define NB_FRAME_CAFE_ACCE  (4 * FRAMES_PER_SECOND)

//death
#define FINAL_DEATH_RATE_INIT  (6. / FRAMES_PER_SECOND)
#define FINAL_DEATH_RATE_GROW 1.02
#define FINAL_DEATH_MAX  (10. / FRAMES_PER_SECOND)
#define INIT_FRAME_DEATH 4

//Score
#define NB_CHAR_AFFICHAGE_SCORE 21
typedef struct {float x; float y; char* msg; int frame; int size;} Score;
#define RATIO_SCORE_SIZE
#define MIN_SIZE_SCORE 22
#define MAX_SIZE_SCORE 45
#define SCORE_TTL 20
const int ALPHA_SCORE[SCORE_TTL] = { 40, 80, 160, 255, 255,   255, 255, 255, 255, 255,   255, 255, 255, 255, 255,   255, 195, 150, 80, 40 };

#define NB_BLINK 15
//Sur une base 8
const int BLINK_FRAMES[NB_BLINK] = {(int)(FRAMES_PER_SECOND * 4.5  + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 5.3  + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 5.75 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 6.15 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 6.55 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 6.85 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.05 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.22 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.37 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.45 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.55 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.65 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.75 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.85 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.93 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND)};

typedef enum{ACCELERATION, RADIUS, PROBA, SCORE, DIGESTION_SIZE, MIN_FRUIT_TO_APPEAR} PROPRIETES;
#define NB_PROPRIETES 6
const float FRUIT_PROPRIETES[NB_FRUITS + NB_BONUSES][NB_PROPRIETES]=
{// ACCE    RADIUS  PROBA   SCORE    DIGES    APPEAR
{   0.02,    20,      7,      200,       5,       0 }, // fraise
{   0.08,    24,      7,      500,       8,       0 }, // orange
{   0.12,    36,      7,     1500,      13,       0 }, // citrouille
{   0.24,    18,      6,     2000,       6,       5 }, // piment
{   0.02,    18,      6,      410,       4,       5 }, // cerise
{   0.05,    22,      6,     1070,       7,       5 }, // pomme
{   0.06,    30,      6,     3500,      11,       10 }, // pastèque
{   0.03,    22,      6,     1280,       7,       10}, // carotte
{   0.09,    26,      6,     4600,      11,       10}, // ananas
{   0.07,    22,      6,     1320,       8,       10}, // tomate
{   0.01,    22,      5,     5000,      13,       15}, // fromage
{   0.02,    26,      5,    10000,      11,       15}, // viande
{  0.005,    28,      4,    12345,      18,       20}, // pizza
{      0,    24,      4,    24000,      19,       20}, // burger
{      0,    24,      3,    35001,      18,       20}, // hot-dog
{      0,    24,      4,    22222,      17,       25}, // pancakes
{   0.03,    22,      5,    17000,      12,       25}, // sucette
{   -0.5,    24,      5,     5000,      14,       30}, // glace baton
{     -1,    24,      4,     6000,      14,       30}, // glace cone
{   -1.5,    24,      3,     7000,      14,       30}, // glace pot
{   0.03,    24,      6,    21808,      15,       35}, // donut
{   0.03,    26,      5,    37000,      16,       35}, // muffin
{   0.03,    24,      4,    40000,      18,       35}, // gateau
{   0.06,    26,      2,    91000,      16,       40}, // muffin op
{      0,    24,      2,    50000,       3,       10}, // café
{     -1,    28,     10,        0,       0,       0 }, // plume
{      0,    28,     10,        0,       0,       0 }, // BOMBE
{      0,    28,     10,        0,       0,       0 }, // coffre
{      0,    28,      8,        0,       0,       10}, // ARC_EN_CIEL
{      0,    28,      6,        0,       0,       15}, // potion hitbox
{      0,    28,      2,        0,       0,       35}, // potion verte
{      0,    28,      3,   100000,       0,       25} // potion jaune
};

#define SIZE_PRE_RADIUS 31
int FRUIT_ADD_RADIUS[NB_FRUITS + NB_BONUSES][SIZE_PRE_RADIUS];
int FRUIT_ADD_RADIUS_GIANT[NB_FRUITS + NB_BONUSES][SIZE_PRE_RADIUS];
