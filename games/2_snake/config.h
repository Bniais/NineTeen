/////////////////
//SNAKE CONFIGS//
/////////////////
#define PI 3.1415
#define NO_TURN 0
#define LEFT 1
#define RIGHT 2
#define ACCELERATE_DISABLED 1
#define ACCELERATE_ENABLED 2
#define HIT 1
#define NO_HIT 0
const SDL_Point ESPACE_DISPLAY_WINDOW = {67, 57};
const Vector2f UNDEF = {-500, -500};
//textures

//fonts
#define NB_SNAKE_FONTS 2
typedef enum{S_BASE_FONT, S_FLAPPY}T_FONTS;

char* DIR_FONTS_SNAKE[NB_SNAKE_FONTS] = {
	"../games/2_snake/Fonts/zorque.ttf",
	"../games/2_snake/Fonts/flappy.ttf"
};

//hud


	///replay

	#define RELAY_SNAKE_SIZE 15
	#define SIZE_REPLAY 43.

	//jauge
	SDL_Rect BASKET_DIM = {0, 0, 62, 950};
	SDL_Color COLOR_JAUGE_BACK = {0x56,0x46,0x3b};
	SDL_Color COLOR_JAUGE = {0x77,0xcd,0x44};
	#define BASE_JAUGE 1.6
	#define FRAME_JAUGE_ANIM 20

//Window
#define FRAMES_PER_SECOND 30
static const int FRAME_TIME = 1000 / FRAMES_PER_SECOND;
SDL_Color HUD_COLOR = {138,82,47};



//Snake
typedef struct {float x; float y; int frame; float radius;} SnakePart;
const Vector2f INIT_HEAD = {PLAYGROUND_SIZE_W / 2, PLAYGROUND_SIZE_H / 2};
#define BODY_RADIUS 20
#define INIT_BODY 30
const Vector2f INIT_DECAL = {0, 5};
#define BASE_ANGLE 3 * PI / 2
#define TURN_AMMOUNT 0.13
#define REMIND_BODY 100
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
#define MIN_SPEED 5.01
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
#define MAX_TRIES_RAND 100
#define CHANCE_SPAWN_FRUIT (4 * FRAMES_PER_SECOND)
#define CHANCE_SPAWN_FRUIT_HARDCORE_INIT 6
#define CHANCE_SPAWN_FRUIT_HARDCORE_MIN 1.8
#define CHANCE_SPAWN_FRUIT_HARDCORE_RATE ((CHANCE_SPAWN_FRUIT_HARDCORE_INIT - CHANCE_SPAWN_FRUIT_HARDCORE_MIN) / (3.*60 * FRAMES_PER_SECOND)) //3mins
#define FRUIT_TTL (FRAMES_PER_SECOND * 14)
#define NB_FRAME_SPAWN_FRUIT 12
#define NB_ANIM_SPAWN 4
#define NB_FRAME_DEATH_FRUIT 12
#define NB_ANIM_DEATH 4
typedef enum {FRAISE, ORANGE, CITROUILLE, PIMENT, CERISE, POMME, PASTEQUE, CAROTTE, ANANAS, TOMATE, FROMAGE, VIANDE, PIZZA, BURGER, HOT_DOG, PANCAKES, SUCETTE, GLACE_BATON, GLACE_CONE, GLACE_POT, DONUT, MUFFIN, GATEAU, MUFFIN_ROSE, CAFE, PLUME, BOMBE, COFFRE, ARC_EN_CIEL, POTION_HITBOX ,POTION_VERTE, POTION_JAUNE} FRUITS;
#define FROM_NATURAL 0
#define FROM_COFFRE 1
#define FROM_RAINBOW 2
#define HITBOX_SECURITY 1
#define HITBOX_GENTILLE 2.33

//hardcore
#define RATIO_RADIUS_HARDCORE 0.8
#define RATIO_SPEED_HARDCORE 5
#define RATIO_GET_FRUIT_HARDCORE -5
#define FRUIT_EATEN_HARDCORE -5
#define FRUIT_TIMEOUT_EATEN_HARDCORE 0.18
#define FRUIT_TIMEOUT_SCORE_HARDCORE 0.25
#define RATIO_TTL_HARDCORE 0.5
#define PRECISION_SPAWN 100

//Bonuses
#define FLAT_REDUCE_SCISOR 10
#define RELATIVE_REDUCE_SCISOR 0.15
#define DURATION_POTION 180
#define RELATIVE_ACCELERATE_COFEE 0.25
#define NB_BONUSES 7
#define CHANCE_SPAWN_BONUS  (10 * FRAMES_PER_SECOND)

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
#define ESPACEMENT_SHOW_POTION 70
#define NB_POTION_MAX 5
SDL_Rect SHOW_POTION_DEST = {0,BASE_WINDOW_H/2 - 1.25* (ESPACEMENT_SHOW_POTION+60),60,60};
#define MAX_BOMB_SIZE 700

//Malus
#define NB_FRAME_CAFE_ACCE  (4 * FRAMES_PER_SECOND)

//death
#define FINAL_DEATH_RATE_INIT  (9. / FRAMES_PER_SECOND)
#define FINAL_DEATH_RATE_GROW 1.03
#define FINAL_DEATH_MAX  (12. / FRAMES_PER_SECOND)
#define INIT_FRAME_DEATH 4

//Score
#define NB_CHAR_AFFICHAGE_SCORE 21
typedef struct {float x; float y; int score; int frame; int size;int fromTimeOut;} Score;
/**
*\struct ScoreTotal
*\brief Contient des informations sur le score total
*/
typedef struct {
	int score; /*!< \brief Le score total stocké */
	float scoreShow; /*!< \brief Le score total affiché (tend vers score)*/
	int frameToDest; /*!< \brief Le nombre de frame avant que scoreShow arrive à score*/
}ScoreTotal;
#define FRAME_SCORE_ANIM 20
SDL_Color TOTAL_SCORE_COLOR = {0x02,0x31,0x02};

#define RATIO_SCORE_SIZE
#define MIN_SIZE_SCORE 22
#define MAX_SIZE_SCORE 45
#define SCORE_TTL 20
static const int ALPHA_SCORE[SCORE_TTL] = { 40, 80, 160, 255, 255,   255, 255, 255, 255, 255,   255, 255, 255, 255, 255,   255, 195, 150, 80, 40 };
static const int ALPHA_SCORE_TIMEOUT[SCORE_TTL] = { 20, 40, 80, 125, 145,    155, 155, 155, 155, 145,   125, 95, 75, 45, 25,       0,0,0,0,0 };

static SDL_Rect SCORE_SRC = {0,0, 12,18};
#define FONT_HEIGHT_RATIO 1.5

#define NB_BLINK 15
//Sur une base 8
const int BLINK_FRAMES[NB_BLINK] = {(int)(FRAMES_PER_SECOND * 4.5  + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 5.3  + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 5.75 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 6.15 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 6.55 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 6.85 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.05 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.22 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.37 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.45 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.55 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.65 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.75 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.85 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND), (int)(FRAMES_PER_SECOND * 7.93 + ((FRUIT_TTL / FRAMES_PER_SECOND) - 8) * FRAMES_PER_SECOND)};

typedef enum{ACCELERATION, RADIUS, PROBA, SCORE, DIGESTION_SIZE, MIN_FRUIT_TO_APPEAR} PROPRIETES;
#define NB_PROPRIETES 6
const float FRUIT_PROPRIETES[NB_FRUITS + NB_BONUSES][NB_PROPRIETES]=
{// ACCE    RADIUS  PROBA   SCORE    DIGES    APPEAR
{   0.02,    20,      2,      20,       5,       0}, // fraise
{   0.05,    24,      2,      50,       8,       0}, // orange
{   0.09,    36,      2,     150,      13,       0}, // citrouille
{   0.18,    18,      3,     200,       6,       5}, // piment
{   0.02,    18,      3,      41,       4,       8}, // cerise
{   0.05,    22,      4,     107,       7,       10}, // pomme
{   0.06,    30,      4,     350,      11,       11}, // pastèque
{   0.03,    22,      4,     128,       7,       13}, // carotte
{   0.09,    26,      4,     460,      11,       16}, // ananas
{   0.07,    22,      5,     132,       8,       18}, // tomate
{    0.1,    22,      4,     500,      13,       21}, // fromage
{    0.1,    26,      4,    1000,      11,       24}, // viande
{  0.005,    28,      5,    1234,      18,       28}, // pizza
{      0,    24,      6,    2400,      19,       30}, // burger
{      0,    24,      5,    3500,      18,       32}, // hot-dog
{      0,    24,      6,    2222,      17,       34}, // pancakes
{   0.05,    22,      6,    1700,      12,       36}, // sucette
{  -0.44,    24,      5,     500,      14,       42}, // glace baton
{  -0.66,    24,      5,     600,      14,       43}, // glace cone
{  -0.88,    24,      4,     700,      14,       44}, // glace pot
{   0.08,    24,      5,    2180,      15,       45}, // donut
{   0.09,    26,      4,    3700,      16,       46}, // muffin
{   0.08,    24,      3,    5000,      18,       47}, // gateau
{   0.06,    26,      2,   10000,      16,       50}, // muffin op
{      0,    24,      1,    5000,       3,       15}, // café
{     -1,    28,     11,       0,       0,       0 }, // plume
{      0,    28,     11,       0,       0,       0 }, // BOMBE
{      0,    28,      7,       0,       0,       7 }, // coffre
{      0,    28,      5,       0,       0,       20}, // ARC_EN_CIEL
{      0,    28,      4,       0,       0,       25}, // potion hitbox
{      0,    28,      2,       0,       0,       35}, // potion verte
{      0,    28,      2,   30000,       0,       45} // potion jaune
};
#define APPEAR_MAX 50.


#define SIZE_PRE_RADIUS 31
int FRUIT_ADD_RADIUS[NB_FRUITS + NB_BONUSES][SIZE_PRE_RADIUS];
int FRUIT_ADD_RADIUS_GIANT[NB_FRUITS + NB_BONUSES][SIZE_PRE_RADIUS];
