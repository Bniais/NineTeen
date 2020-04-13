/////////////////
//ASTEROID CONFIGS//
/////////////////
#include "../../define/define.h"
//textures
#define NB_ASTEROID_TEXTURES 9
typedef enum{T_VAISS, T_GEM, T_THRUST, T_BACKGROUND, T_HUD, T_ASTEROID, T_FISSURE, T_BULLET, T_LASER}TEXTURES;


char* DIR_TEXTURES_ASTEROID[NB_ASTEROID_TEXTURES] = {
	"games/7_asteroid/Textures/vaisseau.png",
	"games/7_asteroid/Textures/gem.png",
	"games/7_asteroid/Textures/thrust.png",
	"games/7_asteroid/Textures/background.png",
	"games/7_asteroid/Textures/hud.png",
	"games/7_asteroid/Textures/asteroid.png",
	"games/7_asteroid/Textures/fissure.png",
	"games/7_asteroid/Textures/bullet.png",
	"games/7_asteroid/Textures/laser_beam.png"
};



typedef struct{  float x;  float y; float angle; int frame_recharge; int temps_recharge; int nb_tir; int bouclier;int missile_id; float vitesse_missile; float degat_missile; int frame_turn_left; int frame_turn_right; int frame_thrust;}Vaiss;


typedef struct{float x; float y; float angle; float taille; int bonus; float pv; float pv_max; float vitesse; float difficulte; float difficulte_pere; float angle_rota; float vitesse_rota;}Asteroid;

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

#define DIST_2ASTEROID 30
#define DIST_VAISSEAU_ASTEROID 300
#define FRAME_APPARITION_ASTEROID (10* FRAMES_PER_SECOND)
#define VITESSE_SPAWN_INIT (FRAMES_PER_SECOND*10)
#define VITESSE_SPAWN_MIN (FRAMES_PER_SECOND*5)
#define ACCELERATION_SPAWN 0.03
#define FRAME_2ASTEROID (FRAMES_PER_SECOND/2)
#define PV_BASE 1
#define VITESSE_BASE 3
SDL_Point coord_spawn[3]={{0,0},{0,(PLAYGROUND_SIZE_H/2)},{(PLAYGROUND_SIZE_W/2),0}};
#define MAX_ASTEROID_SIZE 90
#define TAILLE_MIN_SPLIT 32
#define TAILLE_MIN_ASTEROID 14
#define VITESSE_MAX_ASTEROID 18
#define START_DIFFICULTE 1.5
#define RATIO_DIFFICULTE_AUGMENT 0.004
#define MAX_VITESSE_ROTA 14

#define NB_ASTE_TEXTURES 6
#define NB_TAILLE_ASTE 2
#define NB_FISSURES 2
#define MAX_DIFF 40
const int DIAMETRE_ASTE[NB_TAILLE_ASTE] = {32,48};
SDL_Rect ASTE_SRC = {0,0,48,48};
#define INTERVALE_RAND_DIFFICULTE 0.3

//missiles
#define CHAMP_VISION_TELEGUIDE (PI/6)
#define NB_MISSILES 4
	//color
	const SDL_Color GEM_COLORS[NB_MISSILES]={
		{255,0,0},
		{0xfb,0xb3,0x28},
		{0,255,0},
		{255,0,255}
	};

	const SDL_Color BOUCLIER_COLOR = {0xfd,0xff,0x37};

	//attributs
	typedef struct{ float x; float y; float angle; float target_angle; int frame; float vitesse; float degat; int id;}Missile;

	typedef enum{SHOT_NORMAL, SHOT_ZIGZAG, SHOT_TELEGUIDE, SHOT_LASER}shots;

	// attributs zigzag
	#define BASE_ZIGZAG_ANGLE 0.1
	#define INTERVALLE_ZIGZAG_ANGLE 0.05
	#define FRAME_TO_REACH_ANGLE 4
	// attributs teleguide
	#define ANGLE_TELEGUIDE 0.2

	// attributs laser


	#define DISTANCE_CANON 23

	#define FREQUENCE_BASE (FRAMES_PER_SECOND/2)
	const float FREQUENCE_MISSILES[NB_MISSILES] = {1, 0.66, 1, 0};

	#define BASE_VITESSE_MISSILE 15
	const float VITESSE_MISSILES[NB_MISSILES] = {1, 1.25, 1, 0};

	#define BASE_DEGAT_MISSILE 1
	const float DEGAT_MISSILES[NB_MISSILES] = {1, 1.5, 2, 0};

	const int RAYON_MISSILES[NB_MISSILES] = {6, 10, 14, 0};


	//texures missiles
	#define MISSILE_CUT 56
	SDL_Point MISSILE_SRC[NB_MISSILES] ={
		{42,82},
		{56,74},
		{56,82},
		{954,124}
	};

	SDL_Point MISSILE_CENTRES[NB_MISSILES] ={
		{21,18},
		{28,26},
		{28,19},
		{22,62}
	};

	const int MISSILES_SRC_RAYON[NB_MISSILES]={6, 15, 18,0};

	//death missile
	#define DUREE_MISSILE (2*FRAMES_PER_SECOND)
	#define FRAME_MISSILE_DEATH 8
	const int ALPHA_MISSILE[FRAME_MISSILE_DEATH]={10, 30, 60, 90, 140, 180, 215, 250};


//Window
#define FRAMES_PER_SECOND 30
static const int FRAME_TIME = 1000 / FRAMES_PER_SECOND;



//Bonus

#define NB_BONUS 9
#define NO_BONUS -1
#define PROBA_BONUS 3
#define NB_TIR_MAX 5
#define NB_BONUS_POINT 3
int BONUS_POINT[NB_BONUS_POINT]={15,50,100};
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

char* NOM_BONUS[NB_BONUS] ={
	"TIR_MULTIPLE",
    "BOUCLIER",
    "VITESSE_DE_TIR",
    "BONUS_VITESSE_MISSILE",
    "DEGAT",
    "BOMBE_NUCLEAIRE",
    "POINT_PETIT",
    "POINT_MOYEN",
    "POINT_GRAND"
};

int CHANCE_BONUS[NB_BONUS]={1,3,1,2,1,1,8,5,2};

float angle_tir_multiple[NB_TIR_MAX][NB_TIR_MAX]={
  {0,0,0,0,0},
  {-PI/25,PI/25,0,0,0},
  {-PI/15,0,PI/15,0,0},
  {-PI/10,-PI/25,PI/10,PI/25,0},
  {-PI/8,-PI/15,PI/15,PI/8,0}
};

#define BONUS_ACCELERATION_MISSILE 1.7
#define VITESSE_MISSILE_MAX (2*BASE_VITESSE_MISSILE)
#define BONUS_FREQUENCE_MISSILE 1.5
#define FREQUENCE_MISSILE_MIN (FRAMES_PER_SECOND/6)
#define DEGAT_MISSILE_MAX 5
#define FRAME_BOMBE_NUCLEAIRE (FRAMES_PER_SECOND*1.5)


//Score


//Textures

SDL_Rect THRUST_SRC ={0,0,72,70};
SDL_Rect VAISSEAU_SRC = {0,0,72,70};
SDL_Rect VAISSEAU_DEST = {200,200,72,70};
SDL_Rect GEM_SRC = {0,0,72,70};
SDL_Point CENTRE_VAISS  = {36,27};
