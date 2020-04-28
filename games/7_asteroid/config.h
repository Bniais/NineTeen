/////////////////
//ASTEROID CONFIGS//
/////////////////
#include "../../define/define.h"
#include "../../include/hashage.h"
#include "../../include/libWeb.h"
#include "../../include/communFunctions.h"
/**
*\struct ScoreTotal
*\brief Contient des informations sur le score total
*/
typedef struct {
	int score; /*!< \brief Le score total stocké */
	float scoreShow; /*!< \brief Le score total affiché (tend vers score)*/
	int frameToDest; /*!< \brief Le nombre de frame avant que scoreShow arrive à score*/
}ScoreTotal;
const SDL_Color SCORE_COLOR = {0x44,0xdd,0xFF};
#define FRAME_SCORE_ANIM 20


static const SDL_Color HUD_COLOR = {0x2f,0x30,0x4f};

#define NB_ASTEROID_FONTS 2
typedef enum{FONT_BONUS, FONT_SCORE}FONTS;

char* DIR_FONTS_ASTEROID[NB_ASTEROID_FONTS] = {
	"../games/2_snake/Fonts/zorque.ttf",
	"../games/2_snake/Fonts/Demonized.ttf"
};


#define NB_ASTEROID_SOUNDS 5
typedef enum{SOUND_SHOOT, SOUND_SHOOT_ZIGZAG , SOUND_SHOOT_TELEGUIDE, SOUND_SHOOT_GLACE, SOUND_LASER_BEAM}SOUNDS;

char* DIR_SOUNDS_ASTEROID[NB_ASTEROID_SOUNDS] = {
	"../games/7_asteroid/Sound/shoot1.wav",
	"../games/7_asteroid/Sound/shoot2.wav",
	"../games/7_asteroid/Sound/shoot3.wav",
	"../games/7_asteroid/Sound/shoot4.wav",
	"../games/7_asteroid/Sound/laser_beam.wav"
};

int SOUND_VOLUMES[NB_ASTEROID_SOUNDS] = {30,110,17,25,50};


typedef struct{  float x;  float y; float angle; int frame_recharge; int temps_recharge; int nb_tir; int bouclier;int missile_id; float vitesse_missile; float degat_missile; int frame_turn_left; int frame_turn_right; int frame_thrust; long frame_explo;}Vaiss;

typedef struct{float x; float y; float angle; float taille; int bonus; float pv; float pv_max; float vitesse; float difficulte; float difficulte_pere; float angle_rota; float vitesse_rota; int frame_hit; int frozen;}Asteroid;

typedef struct{float x; float y; int frame; int taille; int id;}Explosion;

#define NB_EXPLOSIONS 3
typedef enum{EXPLO_MISSILE, EXPLO_ASTE, EXPLO_GLACE}EXPLOSIONS;

const int  FRAME_EXPLOSIONS[NB_EXPLOSIONS] = {12, 20, 20};
const int  NB_ANIM_EXPLOSIONS[NB_EXPLOSIONS] = {6, 10, 6};

const SDL_Rect EXPLO_SRCS[NB_EXPLOSIONS] = {{0,0, 374, 374} , {0,0, 312, 312}, {0,0,216,216}};
const SDL_Rect SRC_BOMB = {0,0,960,540};


#define RATIO_ASTEROID_EXPLO_SIZE 1.2

#define PRECISION_RAND_FLOAT 100.

// vaisseau
#define TRUE 1

typedef enum{
  NO_TURN,
  LEFT,
  RIGHT,
  BOTH
}dir_turn;
#define RAYON_VAISS 25
#define DECELERATION 1.015
#define BONUS_DEAD_DECELERATION 0.05
#define TURN_AMMOUNT 0.13
#define VITESSE 10
#define ACCEL 0.45
#define BASE_ANGLE 3 * PI / 2
#define RATIO_ACCEL_TURN 0.16

//anim vaisseau
#define NB_FRAME_TURN 9
#define RESET_TURN 6
const float RATIO_TURN[NB_FRAME_TURN] = {0, 0.1, 0.2, 0.3, 0.6, 0.8, 1, 1, 1};
#define NB_FRAME_THRUST 5
#define RESET_THRUST 2
const float RATIO_ACCEL[NB_FRAME_THRUST+1] = {0, 0.3, 0.6, 1, 1, 1};


//asteroid
#define FRAME_INIT_SPAWN 20
#define DIST_2ASTEROID 30
#define DIST_VAISSEAU_ASTEROID 300
#define FRAME_APPARITION_ASTEROID (16* FRAMES_PER_SECOND)
#define VITESSE_SPAWN_INIT (FRAMES_PER_SECOND*12)
#define VITESSE_SPAWN_MIN (FRAMES_PER_SECOND*6.5)
#define ACCELERATION_SPAWN 0.03
#define FRAME_2ASTEROID (FRAMES_PER_SECOND/2)
#define PV_BASE 1.2
#define VITESSE_BASE 3
SDL_Point coord_spawn[3]={{0,0},{0,(PLAYGROUND_SIZE_H/2)},{(PLAYGROUND_SIZE_W/2),0}};
#define MAX_ASTEROID_SIZE 90
#define TAILLE_MIN_SPLIT 36
#define TAILLE_MIN_ASTEROID 18
#define VITESSE_MAX_ASTEROID 22
#define START_DIFFICULTE 1.01
#define DIFFICULTE_MIN_SPLIT 5
#define RATIO_DIFFICULTE_AUGMENT 0.0027
#define RATIO_DIFFICULTE_AUGMENT_MULTI 0.00033
#define MAX_VITESSE_ROTA 15

#define NB_ASTE_TEXTURES 6
const int SCORE_ASTEROID[NB_ASTE_TEXTURES] = {50, 100, 200, 300, 400, 500};
#define NB_TRANCHE_TAILLE 4
const float MULTI_TAILLE_SCORE[NB_TRANCHE_TAILLE] = {0.2, 0.4, 0.8 ,1};
#define NB_TAILLE_ASTE 2
#define NB_FISSURES 2
#define MAX_DIFF 40
const int DIAMETRE_ASTE[NB_TAILLE_ASTE] = {32,48};
SDL_Rect ASTE_SRC = {0,0,48,48};
#define INTERVALE_RAND_DIFFICULTE 0.3

#define FRAME_HIT_ANIM 2

//ammo
#define NB_ROUE_EMPLACEMENTS 5
SDL_Rect SRC_ROUE = {0,0,622,622};
SDL_Rect ROUE_DIM = {0,0,80,80};
SDL_Rect BOMB_ICON_DIM = {0,0, 71, 70};
#define NB_ARROUND_JAUGES 4
SDL_Color ROUE_COLOR = {40,40,40};
#define MIN_RATIO_COLOR 0.3
#define RATIO_COLOR_JAUGE 0.72
#define FRAME_ROTA_ROUE 13
#define FRAME_AMMO 7
#define BLOCKING_ANIM 1

const int BLOCKING_ANIM_ANGLE[FRAME_ROTA_ROUE] = {0,0,0,4,4,4,4,4,4,1,-1,-12,-12};
SDL_Rect JAUGE_DIM = {0,0,80,500};

typedef struct{int frame; SDL_Color color; float ammo; int frameAmmo;}Jauge;
typedef struct{int frame; float rota; int rota_dest;}Roue;
//missiles

#define NB_MISSILES 5
	//color
	const SDL_Color GEM_COLORS[NB_MISSILES]={
		{250,35,35},
		{0xfa,0xa6,0x32},
		{0x71,0xb5,0x45},
		{0x68,0xcf,0xf1},
		{0xb0,0x6c,0xd0}
	};

	const SDL_Color BOUCLIER_COLOR = {0xfd,0xff,0x37};

	//attributs
	typedef struct{ float x; float y; float angle; float target_angle; int frame; float vitesse; float degat; int id;}Missile;

	typedef enum{SHOT_NORMAL, SHOT_ZIGZAG, SHOT_TELEGUIDE, SHOT_GLACE, SHOT_LASER}shots;

	// attributs zigzag
	#define BASE_ZIGZAG_ANGLE 0.1
	#define INTERVALLE_ZIGZAG_ANGLE 0.05
	#define FRAME_TO_REACH_ANGLE 4
	// attributs teleguide
	#define ANGLE_TELEGUIDE 0.1
	#define CHAMP_VISION_TELEGUIDE (PI/2)

	// attributs laser
	#define NB_LASER_BEAM 8
	#define LASER_ACCEL 0.05
	#define BASE_TAILLE_EXPLOSION 40
	#define RATIO_DMG_UP_LASER 0.9

	//attributs glace
	#define DEAD_FROZEN -999
	const SDL_Color FISSURE_GLACE_COLOR = {0x58,0x95,0xaa};

	//attributs communs
	#define DISTANCE_CANON 23

	#define FREQUENCE_BASE (FRAMES_PER_SECOND/2)
	const float FREQUENCE_MISSILES[NB_MISSILES] = {1, 0.66, 1.6, 1.33, 0};

	#define BASE_VITESSE_MISSILE 15
	const float VITESSE_MISSILES[NB_MISSILES] = {1.2, 1.4, 1, 1.3, 0};

	#define BASE_DEGAT_MISSILE 1.5
	const float DEGAT_MISSILES[NB_MISSILES] = {1.3, 1.6, 3.6, 0, 12./30};

	const int RAYON_MISSILES[NB_MISSILES] = {6, 10, 14, 10, 0};

	const float TAILLE_EXPLOSIONS[NB_MISSILES] = {0.9, 1.1, 1.3, 1, 0};

	#define DUREE_MISSILE_BASE (2*FRAMES_PER_SECOND)
	const float DUREE_MISSILES[NB_MISSILES] = {0.9, 0.7, 1.6, 1, 0};


	const float MUNITIONS_USAGE[NB_MISSILES] = {0, 0.01, 0.0334, 0.02, 1./(14*30)};
											  //INF / 100 / 33 /  50/ 16s


    #define MAX_RATIO_AMMO_OBTAINABLE 0.5
	#define AMMO_GRANT 0.33

	//texures missiles
	#define MISSILE_CUT 56
	SDL_Point MISSILE_SRC[NB_MISSILES] ={
		{42,82},
		{56,74},
		{56,82},
		{56,82},
		{2385,124}
	};

	SDL_Point MISSILE_CENTRES[NB_MISSILES] ={
		{21,18},
		{28,26},
		{28,19},
		{28,27},
		{22,62}
	};

	const int MISSILES_SRC_RAYON[NB_MISSILES]={6, 15, 18, 13, 0};

	//death missile
	#define FRAME_MISSILE_DEATH 8
	const int ALPHA_MISSILE[FRAME_MISSILE_DEATH]={10, 30, 60, 90, 140, 180, 215, 250};


//Window
#define FRAMES_PER_SECOND 30
static const int FRAME_TIME = 1000 / FRAMES_PER_SECOND;

//dead
#define FRAME_SHOW_HELP -50
#define FRAME_DESTROY_ASTE -30

//help
static SDL_Rect FLECHE_SRC = {0,0, 500, 435};
static SDL_Rect FLECHE_DEST[6] = {
	{PLAYGROUND_SIZE_W/2 - 50/2,  PLAYGROUND_SIZE_H/2 - RAYON_VAISS -50 -4,50,30},
	{PLAYGROUND_SIZE_W/2 - 50 -10 - RAYON_VAISS,  PLAYGROUND_SIZE_H/2 -30/2,50,30},
	{PLAYGROUND_SIZE_W/2 + 10 + RAYON_VAISS,  PLAYGROUND_SIZE_H/2- 30/2,50,30},
	{10,720,40,20},
	{26,736,40,20},
	{10,834,56,38}
};


//Bonus

#define NB_BONUS 9
#define NO_BONUS -1
#define PROBA_BONUS 3
#define NB_TIR_MAX 3
const float MUNITIONS_USAGE_MULTIPLE[NB_TIR_MAX]={1,1.3,1.50};
#define NB_BONUS_POINT 3
#define MAX_BOMBE_NUCLEAIRE 1
int BONUS_POINT[NB_BONUS_POINT]={500,1500,5000};
typedef enum{
  PETIT,
  MOYEN,
  GRAND
}taille_bonus;
typedef enum{
  TIR_MULTIPLE,
  BOUCLIER,
  VITESSE_DE_TIR,
  BONUS_VITESSE_MISSILE,
  DEGAT,
  BOMBE_NUCLEAIRE,
  POINT_PETIT,
  POINT_MOYEN,
  POINT_GRAND
}bonus_e;

char * TEXT_BONUS[NB_BONUS + NB_MISSILES - 1]= {
	"Tir multiple !",
	"Bouclier !",
	"Frequence de tir !",
	"Vitesse des missiles !",
	"Degat !",
	"Bombe nucleaire !",
	"+500 points !",
	"+1500 points !",
	"+5000 points !",
	"Munitions zigzag !",
	"Munitions a tete chercheuse !",
	"Munitons de glace !",
	"Munitions laser !"
};

typedef struct {int id; int frame;}TextBonus;
#define MAX_TEXT_BONUS 10
#define FRAME_SHOW_BONUS_TEXT 30
#define ESPACEMENT_SHOW_TEXT 70
#define SIZE_BONUS_TEXT 44.

const SDL_Color BONUS_TEXT_COLOR = {0xa7,0x96,0xff};
static const int ALPHA_BONUS[FRAME_SHOW_BONUS_TEXT] = { 40, 80, 160, 200, 235,   255, 255, 255, 255, 255,	 255, 255, 255, 255, 255,	 255, 255, 255, 255, 255,   245, 235, 225, 215, 205,   180, 150, 110, 80, 40 };

int CHANCE_BONUS[NB_BONUS + NB_MISSILES - 1]={1,3,1,3,4,1,10,6,2,4,4,4,4};

float angle_tir_multiple[NB_TIR_MAX][NB_TIR_MAX]={
  {0,0,0},//,0,0},
  {-PI/25,PI/25,0},//,0,0},
  {-PI/15,0,PI/15}//,0,0},
 // {-PI/10,-PI/25,PI/10,PI/25,0},
  //{-PI/8,-PI/15,PI/15,PI/8,0}
};

#define BONUS_ACCELERATION_MISSILE 1.7
#define VITESSE_MISSILE_MAX (2*BASE_VITESSE_MISSILE)
#define BONUS_FREQUENCE_MISSILE 1.4
#define FREQUENCE_MISSILE_MIN (FRAMES_PER_SECOND/6.)
#define DEGAT_MISSILE_MAX 6
#define DEGAT_ADD 0.75
#define FRAME_BOMBE_NUCLEAIRE (FRAMES_PER_SECOND*1.5)
#define FRAME_ANIM_BOMB 24
#define NB_ANIM_BOMB 16
#define FRAME_KILL_ASTERO_BOMB 15
#define NB_ROW_BOMB 4
#define NB_COL_BOMB 4

//Score


//Textures
SDL_Rect THRUST_SRC ={0,0,72,70};
SDL_Rect VAISSEAU_SRC = {0,0,72,70};
SDL_Rect VAISSEAU_DEST = {200,200,72,70};
SDL_Rect GEM_SRC = {0,0,72,70};
SDL_Point CENTRE_VAISS  = {36,26};
