/////////////////
//ASTEROID CONFIGS//
/////////////////

typedef struct{  float x;  float y; float angle; int frame_recharge; int temps_recharge; int nb_tir; int bouclier; float vitesse_missile; float degat_missile; int frame_turn_left; int frame_turn_right; int frame_thrust;}Vaiss;
typedef struct{  float x;  float y; }Vector2f;
typedef struct{ float x; float y; float angle; int frame; float vitesse; float degat;}Missile;
typedef struct{float x; float y; float angle; float taille; int bonus; float pv; float vitesse; float difficulte;}Asteroid;


const Vector2f UNDEFINED = {-500, -500};
#define PRECISION_RAND_FLOAT 100

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
#define TURN_AMMOUNT 0.08
#define RECHARGE_TIR (FRAMES_PER_SECOND/2)
#define VITESSE 10
#define ACCEL 0.35
#define BASE_ANGLE 3 * PI / 2
#define RATIO_ACCEL_TURN 0.06

//asteroid

#define DIST_2ASTEROID 30
#define DIST_VAISSEAU_ASTEROID 300
#define FRAME_APPARITION_ASTEROID (10* FRAMES_PER_SECOND)
#define VITESSE_SPAWN_INIT (FRAMES_PER_SECOND*5)
#define VITESSE_SPAWN_MIN (FRAMES_PER_SECOND*3)
#define ACCELERATION_SPAWN 0.05
#define FRAME_2ASTEROID (FRAMES_PER_SECOND/2)
#define PV_BASE 1
#define VITESSE_BASE 2
SDL_Point coord_spawn[3]={{0,0},{0,(PLAYGROUND_SIZE_H/2)},{(PLAYGROUND_SIZE_W/2),0}};
#define MAX_ASTEROID_SIZE 70
#define TAILLE_MIN_SPLIT 15
#define TAILLE_MIN_ASTEROID 7
#define VITESSE_MAX_ASTEROID 20
#define START_DIFFICULTE 2
#define RATIO_DIFFICULTE_AUGMENT 0.01
//missiles

#define DISTANCE_CANON 20
#define VITESSE_MISSILE 10
#define RAYON_MISSILE 5
#define DUREE_MISSILE 90
#define DEGAT_MISSILE 1

//Window
#define FRAMES_PER_SECOND 30
const int FRAME_TIME = 1000 / FRAMES_PER_SECOND;



//Bonus

#define NB_BONUS 9
#define NO_BONUS -1
#define PROBA_BONUS 7
#define NB_TIR_MAX 5
#define NB_BONUS_POINT 3
int BONUS_POINT[NB_BONUS_POINT]={5,10,20};
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

int proba_bonus[NB_BONUS]={3,6,9,12,15,18,23,26,27};

float angle_tir_multiple[NB_TIR_MAX][NB_TIR_MAX]={
  {0,0,0,0,0},
  {-PI/25,PI/25,0,0,0},
  {-PI/10,0,PI/10,0,0},
  {-PI/6,-PI/18,PI/6,PI/18,0},
  {-PI/4,-PI/8,PI/4,PI/8,0}
};

#define BONUS_ACCELERATION_MISSILE 1.7
#define VITESSE_MISSILE_MAX (2*VITESSE_MISSILE)
#define DEGAT_MISSILE_MAX 5
#define FRAME_BOMBE_NUCLEAIRE (FRAMES_PER_SECOND*1.5)


//Score


//Textures

SDL_Rect THRUST_SRC ={0,0,72,70};
SDL_Rect VAISSEAU_SRC = {0,0,72,70};
SDL_Rect VAISSEAU_DEST = {200,200,72,70};
SDL_Rect GEM_SRC = {0,0,72,70};
SDL_Point CENTRE_VAISS  = {36,27};
