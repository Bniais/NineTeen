/////////////////
//ASTEROID CONFIGS//
/////////////////

typedef struct{  float x;  float y; float angle; int frame_recharge; int temps_recharge; int nb_tir; int bouclier; float vitesse_missile; float degat_missile;}Vaiss;
typedef struct{  float x;  float y; }Vector2f;
typedef struct{ float x; float y; float angle; int frame; float vitesse; float degat;}Missile;
typedef struct{float x; float y; float angle; float taille; int bonus; int pv; float vitesse;}Asteroid;


const Vector2f UNDEFINED = {-500, -500};

// vaisseau
#define TRUE 1
#define NO_TURN 0
#define LEFT 1
#define RIGHT 2
#define RAYON_VAISS 10
#define DECELERATION 1.015
#define TURN_AMMOUNT 0.08
#define RECHARGE_TIR (FRAMES_PER_SECOND/2)
#define VITESSE 10
#define ACCEL 0.35
#define BASE_ANGLE 3 * PI / 2

//asteroid
#define NB_TAILLE 3
#define DIST_2ASTEROID 30
#define DIST_VAISSEAU_ASTEROID 300
typedef enum{PETIT, MOYEN, GRAND}taille_e;
int TAILLE_ASTEROID[NB_TAILLE]={15,30,50};
#define VITESSE_ASTEROID 2
#define FRAME_APPARITION_ASTEROID (10* FRAMES_PER_SECOND)
#define VITESSE_SPAWN_INIT (FRAMES_PER_SECOND*5)
#define VITESSE_SPAWN_MIN (FRAMES_PER_SECOND*3)
#define ACCELERATION_SPAWN 0.05
#define FRAME_2ASTEROID (FRAMES_PER_SECOND/2)
int PV[NB_TAILLE]={1,2,4};
SDL_Point coord_spawn[3]={{0,0},{0,(PLAYGROUND_SIZE_H/2)},{(PLAYGROUND_SIZE_W/2),0}};

//missiles

#define DISTANCE_CANON 50
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
#define PROBA_BONUS 2
#define NB_TIR_MAX 5
int BONUS_POINT[NB_TAILLE]={5,10,20};

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

int proba_bonus[NB_BONUS]={3,6,9,12,15,16,22,26,28};

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
