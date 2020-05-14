//system
#define FRAMES_PER_SECOND 30

/**
*\struct ScoreTotal
*\brief Contient des informations sur le score total
*/
typedef struct {
	int score; /*!< \brief Le score total stocké */
	float scoreShow; /*!< \brief Le score total affiché (tend vers score)*/
	int frameToDest; /*!< \brief Le nombre de frame avant que scoreShow arrive à score*/
}ScoreTotal;

typedef struct { float x; float y; float rx; float ry;}Ellips;
#define PRECISION_RAND_FLOAT 100.
//Background
	#define NB_BACKGROUND_LAYER 3
	#define RATIO_WIDTH_PLAYGROUND 3
	static const SDL_Rect BACKGROUND_DEST = {(BASE_WINDOW_W - BASE_WINDOW_W/RATIO_WIDTH_PLAYGROUND)/2, 0, BASE_WINDOW_W/RATIO_WIDTH_PLAYGROUND, BASE_WINDOW_H};
	static const SDL_Rect BACKGROUND_SRC = {0,0,BASE_WINDOW_W/RATIO_WIDTH_PLAYGROUND, BASE_WINDOW_H};
	static const SDL_Rect BACKGROUND_GLOBAL_SRC = {0,0,BASE_WINDOW_W/RATIO_WIDTH_PLAYGROUND, 4096};

	const int SCROLL_SPEED_BACKGROUND[3] = {6,10,12};

//weapon
	#define NB_MAX_WEAPON 5
	#define NB_MAX_WEAPON_ENEMY 7
	const SDL_Rect WEAPON_SRC = {0,0,3,19};
	const SDL_Rect WEAPON_DEST[NB_MAX_WEAPON] = {
		{36,14,3,17},
		{49,10,3,18},
		{64,6,4,13},
		{80,10,3,18},
		{93,14,3,17}
	};

	const int WEAPON_DISPOSITION[NB_MAX_WEAPON][NB_MAX_WEAPON] = {
		{3,0,0,0,0},
		{2,4,0,0,0},
		{3,2,4,0,0},
		{3,2,4,1,0},
		{3,2,4,1,5}
	};

	const int DECALLAGE_TURN[3][NB_MAX_WEAPON] = {
		{0,0,0},
		{-1, -2, -3, -3, -4},
		{4, 3, 3, 2, 1}
	};



//vaisseau
	#define SHIP_SPEED 10
	#define RATIO_SPEED_DOWN 0.85
	typedef struct Weapon_s{int id; int frame_reload;}Weapon;
	typedef struct Ship_s{float x; float y; int dir; int form; int nbWeapon; Weapon weapons[NB_MAX_WEAPON];}Ship;
	#define RATIO_SIZE_SHIP 1.45
	#define NB_SHIP_FORMS 3
	const SDL_Rect  SHIP_SRC = {0, 0, 132, 170};
	const SDL_Point SHIP_CENTER = {66, 66};
	const Ellips  SHIP_HITBOX[NB_SHIP_FORMS] = {
		{66/RATIO_SIZE_SHIP, 70.5/RATIO_SIZE_SHIP, 38.5/RATIO_SIZE_SHIP, 49.5/RATIO_SIZE_SHIP},
		{65.5/RATIO_SIZE_SHIP, 71/RATIO_SIZE_SHIP, 44.5/RATIO_SIZE_SHIP, 49/RATIO_SIZE_SHIP},
		{66/RATIO_SIZE_SHIP, 66/RATIO_SIZE_SHIP, 44/RATIO_SIZE_SHIP, 51/RATIO_SIZE_SHIP},
	};

	SDL_Rect DIST_BORD_VAISS = {27/RATIO_SIZE_SHIP, 20/RATIO_SIZE_SHIP, 105/RATIO_SIZE_SHIP, 113/RATIO_SIZE_SHIP};


//Missile
	#define NB_ALLY_MISSILES 3
	#define NB_ENEMY_MISSILES 2
	#define NO_WEAPON -1
	typedef enum{W_BASE, W_ZIGZAG, W_AUTOAIM, W_BASE_ENEMY, W_LASER_ENEMY}WEAPONS;
	const float RATIO_SIZE_MISSILE[NB_ALLY_MISSILES + NB_ENEMY_MISSILES] = {1, 1, 1, 0.5, 1};
	#define RATIO_SIZE_MISSILE_0 1
	#define RATIO_SIZE_MISSILE_1 1
	#define RATIO_SIZE_MISSILE_2 1
	#define RATIO_SIZE_MISSILE_3 0.5
	#define RATIO_SIZE_MISSILE_4 1

	typedef struct Missile_s{float x; float y; int id; float rota; float damage; float speed; int frame;}Missile;
	 Ellips MISSILE_HITBOX[NB_ALLY_MISSILES + NB_ENEMY_MISSILES] = {
		{0,0,6*RATIO_SIZE_MISSILE_0, 16*RATIO_SIZE_MISSILE_0},
		{0,0,6*RATIO_SIZE_MISSILE_1 ,16*RATIO_SIZE_MISSILE_1},
		{0,0,6*RATIO_SIZE_MISSILE_2, 16*RATIO_SIZE_MISSILE_2},
		{0,0,12.5*RATIO_SIZE_MISSILE_3, 12.5*RATIO_SIZE_MISSILE_3},
		{0,0,6*RATIO_SIZE_MISSILE_4, 16*RATIO_SIZE_MISSILE_4}
	};

	static const SDL_Rect MISSILE_SRC[NB_ALLY_MISSILES + NB_ENEMY_MISSILES] = {
		{0,0,38,68},
		{0,0,38,68},
		{0,0,38,68},
		{38,0,60,62},
		{0,0,38,68}
	};

	const SDL_Point MISSILE_CENTER[NB_ALLY_MISSILES + NB_ENEMY_MISSILES] = {
		{19*RATIO_SIZE_MISSILE_0,26*RATIO_SIZE_MISSILE_0},
		{19*RATIO_SIZE_MISSILE_1,26*RATIO_SIZE_MISSILE_1},
		{19*RATIO_SIZE_MISSILE_2,26*RATIO_SIZE_MISSILE_2},
		{29*RATIO_SIZE_MISSILE_3, 30*RATIO_SIZE_MISSILE_3},
		{19*RATIO_SIZE_MISSILE_4, 26*RATIO_SIZE_MISSILE_4}
	};

	const float MISSILE_SPEED[NB_ALLY_MISSILES] ={18, 18, 18};
	const int RELOAD_FRAME[NB_ALLY_MISSILES] = {10};
	static const float TAILLE_EXPLOSIONS[NB_ALLY_MISSILES + NB_ENEMY_MISSILES] = {1, 1, 1, 1, 1};

//Enemies
	#define NMAX 4 //nombre de points dans leurs trajectoires pour l'interpolation polynomiale
	#define NB_ENEMY 3
	#define FRAME_HIT_ANIM 2
	#define ALPHA_HIT 210
	enum{BASE_ENEMY, LASER_ENEMY, BOSS_ENEMY};
	typedef struct {int id; float x; float y; float rota; float hp; int frameHit; float dist; int frameWait; int dir;int nbWeapon; Weapon weapons[NB_MAX_WEAPON_ENEMY]; double abscisses[NMAX]; double coefs[NMAX]; }Enemy;
	const SDL_Rect ENEMY_SRC[NB_ENEMY] = {
		{0,0,79,79},
		{0,0,150,109},
		{0,0,213,143}
	};
	const float RATIO_SIZE_ENEMY[NB_ENEMY] = {1.5, 1.25, 1};
	#define RATIO_SIZE_ENEMY_0 1.5
	#define RATIO_SIZE_ENEMY_1 1.25
	#define RATIO_SIZE_ENEMY_2 1

	const float SPEED_ENEMY[NB_ENEMY] = {6, 3, 3};
	#define SPEED_DECOMPOSITION 5
	const int ENEMY_HP[NB_ENEMY] = {1, 3, 7};
	const float FRAME_MULTI_SPAWN[NB_ENEMY] = {8, 0, 0};
	const int NB_WEAPON_ENEMY[NB_ENEMY] = {0,3,6};

	const Weapon WEAPONS_ENEMY[NB_ENEMY][NB_MAX_WEAPON_ENEMY] = {
		{ {W_BASE_ENEMY, 0}, {NO_WEAPON, -1},    {NO_WEAPON, -1},    {NO_WEAPON, -1},    {NO_WEAPON, -1},   {NO_WEAPON, -1},   {NO_WEAPON, -1}   },
		{ {W_BASE_ENEMY, 0}, {W_LASER_ENEMY, 0}, {W_LASER_ENEMY, 0}, {W_BASE_ENEMY, 0},  {NO_WEAPON, -1},   {NO_WEAPON, -1},   {NO_WEAPON, -1}   },
		{ {W_BASE_ENEMY, 0}, {W_BASE_ENEMY, 0},  {W_BASE_ENEMY, 0},  {W_LASER_ENEMY, 0}, {W_BASE_ENEMY, 0}, {W_BASE_ENEMY, 0}, {W_BASE_ENEMY, 0} }
	};




	const Vector2f WEAPON_DEST_ENEMY[NB_ENEMY][NB_MAX_WEAPON_ENEMY] = {
		{{39.5/RATIO_SIZE_ENEMY_0, 77/RATIO_SIZE_ENEMY_0}},
		{{43.5/RATIO_SIZE_ENEMY_1, 104/RATIO_SIZE_ENEMY_1}, {52.5/RATIO_SIZE_ENEMY_1, 108/RATIO_SIZE_ENEMY_1}, {97.5/RATIO_SIZE_ENEMY_1, 108/RATIO_SIZE_ENEMY_1}, {107/RATIO_SIZE_ENEMY_1, 103/RATIO_SIZE_ENEMY_1}},
		{{44.5/RATIO_SIZE_ENEMY_2, 137/RATIO_SIZE_ENEMY_2}, {50.5/RATIO_SIZE_ENEMY_2, 140/RATIO_SIZE_ENEMY_2}, {57/RATIO_SIZE_ENEMY_2, 142/RATIO_SIZE_ENEMY_2}, {107/RATIO_SIZE_ENEMY_2, 139/RATIO_SIZE_ENEMY_2}, {156/RATIO_SIZE_ENEMY_2, 142/RATIO_SIZE_ENEMY_2}, {162.5/RATIO_SIZE_ENEMY_2, 140/RATIO_SIZE_ENEMY_2}, {169/RATIO_SIZE_ENEMY_2,137/RATIO_SIZE_ENEMY_2}}
	};

	const int RELOAD_FRAME_ENEMY[NB_ENEMY][NB_MAX_WEAPON_ENEMY] ={
		{50},
		{40, 50, 50, 40},
		{20, 30, 40, 50, 40, 30, 20}
	};

	const float ANGLE_ENEMY_FIRE[NB_ENEMY][NB_MAX_WEAPON_ENEMY] ={
		{PI/6},
		{PI/6, 0, 0, PI/6},
		{PI/6, PI/6, PI/6, 0, PI/6, PI/6, PI/6}
	};

	const float MISSILE_SPEED_ENEMY[NB_ENEMY][NB_MAX_WEAPON_ENEMY] ={
		{12},
		{20, 30, 30, 20},
		{20, 30, 40, 50, 40, 30, 20}
	};

	const Ellips ENEMY_HITBOX[NB_ENEMY] = {
		{39.5/RATIO_SIZE_ENEMY_0,  39.5/RATIO_SIZE_ENEMY_0,  39.5/RATIO_SIZE_ENEMY_0,  39.5/RATIO_SIZE_ENEMY_0},
		{75./RATIO_SIZE_ENEMY_1,   54.5/RATIO_SIZE_ENEMY_1,   56./RATIO_SIZE_ENEMY_1,  36.5/RATIO_SIZE_ENEMY_1},
		{106.5/RATIO_SIZE_ENEMY_2, 91.5/RATIO_SIZE_ENEMY_2,  88.5/RATIO_SIZE_ENEMY_2,  44.5/RATIO_SIZE_ENEMY_2}
	};

	const Ellips ENEMY_ADDITIONAL_HITBOX[NB_ENEMY] = {
		{39.5/RATIO_SIZE_ENEMY_0,39.5/RATIO_SIZE_ENEMY_0,1,1},
		{75/RATIO_SIZE_ENEMY_1,54.5/RATIO_SIZE_ENEMY_1,75/RATIO_SIZE_ENEMY_1,9.5/RATIO_SIZE_ENEMY_1},
		{106.5/RATIO_SIZE_ENEMY_2,81/RATIO_SIZE_ENEMY_2,106.5/RATIO_SIZE_ENEMY_2,10/RATIO_SIZE_ENEMY_2}
	};

//Explosion
	typedef struct{float x; float y; int frame; int taille; int id;}Explosion;
	#define NB_EXPLOSIONS 2
	enum{EXPLO_MISSILE, EXPLO_SHIP};

	static const int  FRAME_EXPLOSIONS[NB_EXPLOSIONS] = {12, 20};
	static const int  NB_ANIM_EXPLOSIONS[NB_EXPLOSIONS] = {6, 10};

	static const SDL_Rect EXPLO_SRCS[NB_EXPLOSIONS] = {{0,0, 374, 374} , {0,0, 312, 312}};

	#define BASE_TAILLE_EXPLOSION 40
	#define RATIO_SHIP_EXPLO_SIZE 1.2



//colors
	//background
	static const SDL_Color BACKGROUND_COLOR = {0x00, 0x19, 0x3a};
	static const SDL_Color JAUGE_COLOR = {0x00,0xcc,0xFF};
