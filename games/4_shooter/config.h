//system
#define FRAMES_PER_SECOND 30
#define DRAW_DEBUG 1
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
	static const SDL_Rect LEFT_BACK = {0,0,BASE_WINDOW_W/RATIO_WIDTH_PLAYGROUND, BASE_WINDOW_H};
	static const SDL_Rect RIGHT_BACK = {2*BASE_WINDOW_W/RATIO_WIDTH_PLAYGROUND,0,BASE_WINDOW_W/RATIO_WIDTH_PLAYGROUND, BASE_WINDOW_H};
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
		{1,2,4,5,0},
		{3,2,4,1,5}
	};

	const int DECALLAGE_TURN[3][NB_MAX_WEAPON] = {
		{0,0,0},
		{-1, -2, -3, -3, -4},
		{4, 3, 3, 2, 1}
	};



//vaisseau
	#define SHIP_SPEED 10
	#define RATIO_SPEED_DOWN 0.9
	typedef struct Weapon_s{int id; int frame_reload; int combo;}Weapon;
	typedef struct Ship_s{float x; float y; int dir; int form; int nbWeapon; Weapon weapons[NB_MAX_WEAPON];}Ship;
	#define RATIO_SIZE_SHIP 1.65
	#define NB_SHIP_FORMS 3
	const SDL_Rect  SHIP_SRC = {0, 0, 132, 170};
	const SDL_Point SHIP_CENTER = {66, 66};

	#define FAIR_HITBOX_SHIP 2
	const Ellips  SHIP_HITBOX[NB_SHIP_FORMS] = {
		{66/RATIO_SIZE_SHIP, 70.5/RATIO_SIZE_SHIP, (38.5 -FAIR_HITBOX_SHIP)/RATIO_SIZE_SHIP, (49.5-FAIR_HITBOX_SHIP)/RATIO_SIZE_SHIP},
		{65.5/RATIO_SIZE_SHIP, 71/RATIO_SIZE_SHIP, (44.5-FAIR_HITBOX_SHIP)/RATIO_SIZE_SHIP, (49-FAIR_HITBOX_SHIP)/RATIO_SIZE_SHIP},
		{66/RATIO_SIZE_SHIP, 66/RATIO_SIZE_SHIP, (44-FAIR_HITBOX_SHIP)/RATIO_SIZE_SHIP, (51-FAIR_HITBOX_SHIP)/RATIO_SIZE_SHIP},
	};

	SDL_Rect DIST_BORD_VAISS = {27/RATIO_SIZE_SHIP, 20/RATIO_SIZE_SHIP, 105/RATIO_SIZE_SHIP, 113/RATIO_SIZE_SHIP};


//Missile
	#define NB_ALLY_MISSILES 3
	#define NB_ENEMY_MISSILES 2
	#define NO_WEAPON -1
	typedef enum{W_BASE, W_ZIGZAG, W_AUTOAIM, W_BASE_ENEMY, W_LASER_ENEMY}WEAPONS;

	#define RATIO_SIZE_MISSILE_0 1
	#define RATIO_SIZE_MISSILE_1 1
	#define RATIO_SIZE_MISSILE_2 1
	#define RATIO_SIZE_MISSILE_3 0.4
	#define RATIO_SIZE_MISSILE_4 1
	const float RATIO_SIZE_MISSILE[NB_ALLY_MISSILES + NB_ENEMY_MISSILES] = {RATIO_SIZE_MISSILE_0, RATIO_SIZE_MISSILE_1, RATIO_SIZE_MISSILE_2, RATIO_SIZE_MISSILE_3, RATIO_SIZE_MISSILE_4};

	typedef struct Missile_s{float x; float y; int id; float rota; float damage; float speed; int frame;}Missile;
	 Ellips MISSILE_HITBOX[NB_ALLY_MISSILES + NB_ENEMY_MISSILES] = {
		{0,0,6*RATIO_SIZE_MISSILE_0, 16*RATIO_SIZE_MISSILE_0},
		{0,0,6*RATIO_SIZE_MISSILE_1 ,16*RATIO_SIZE_MISSILE_1},
		{0,0,6*RATIO_SIZE_MISSILE_2, 16*RATIO_SIZE_MISSILE_2},
		{0,0,12.5*RATIO_SIZE_MISSILE_3, 12.5*RATIO_SIZE_MISSILE_3},
		{0,0,26*RATIO_SIZE_MISSILE_4, 178*RATIO_SIZE_MISSILE_4}
	};

	static const SDL_Rect MISSILE_SRC[NB_ALLY_MISSILES + NB_ENEMY_MISSILES] = {
		{0,0,38,68},
		{0,0,38,68},
		{0,0,38,68},
		{38,0,60,68},
		{98,0,64,210}
	};

	const SDL_Point MISSILE_CENTER[NB_ALLY_MISSILES + NB_ENEMY_MISSILES] = {
		{19*RATIO_SIZE_MISSILE_0,26*RATIO_SIZE_MISSILE_0},
		{19*RATIO_SIZE_MISSILE_1,26*RATIO_SIZE_MISSILE_1},
		{19*RATIO_SIZE_MISSILE_2,26*RATIO_SIZE_MISSILE_2},
		{29*RATIO_SIZE_MISSILE_3, 30*RATIO_SIZE_MISSILE_3},
		{32*RATIO_SIZE_MISSILE_4, 120*RATIO_SIZE_MISSILE_4}
	};

	const float MISSILE_SPEED[NB_ALLY_MISSILES] ={24, 18, 18};
	const int RELOAD_FRAME[NB_ALLY_MISSILES] = {10};
	static const float TAILLE_EXPLOSIONS[NB_ALLY_MISSILES + NB_ENEMY_MISSILES] = {1, 1, 1, 1, 1};

//Enemies
	#define NMAX 4 //nombre de points dans leurs trajectoires pour l'interpolation polynomiale
	#define NB_ENEMY 5
	#define FRAME_HIT_ANIM 2
	#define ALPHA_HIT 210
	enum{BASE_ENEMY, LASER_ENEMY, BOSS_ENEMY, ARM_ENEMY, ARM_BOSS_ENEMY};
	typedef struct {int id; float x; float y; float rota; float hp; int frameHit; float dist; int frameWait; int dir; int nbMove; int nbWeapon; Weapon weapons[NB_MAX_WEAPON_ENEMY]; double abscisses[NMAX]; double coefs[NMAX]; double infos[6];}Enemy;
	typedef struct {int idSrc; int id; float x; float y; float rota[2]; float dirX; float dirY;}EnemyPart;

	const SDL_Rect ENEMY_SRC[NB_ENEMY] = {
		{0,0,79,79},
		{0,0,150,109},
		{0,0,213,143},
		{0,0,104,283},
		{0,0,480,271}
	};
	#define RATIO_SIZE_ENEMY_0 1.5
	#define RATIO_SIZE_ENEMY_1 1.25
	#define RATIO_SIZE_ENEMY_2 0.7
	#define RATIO_SIZE_ENEMY_3 0.72
	#define RATIO_SIZE_ENEMY_4 0.75
	const float RATIO_SIZE_ENEMY[NB_ENEMY] = {RATIO_SIZE_ENEMY_0, RATIO_SIZE_ENEMY_1, RATIO_SIZE_ENEMY_2, RATIO_SIZE_ENEMY_3, RATIO_SIZE_ENEMY_4};


	const float SPEED_ENEMY[NB_ENEMY] = {5.3, 6.3, 5, 1,0};
	#define SPEED_DECOMPOSITION 5
	const int ENEMY_HP[NB_ENEMY] = {1, 7, 25, 40, 90};
	const float FRAME_MULTI_SPAWN[NB_ENEMY] = {9, 15, 0, 0, 0};
	const int NB_WEAPON_ENEMY[NB_ENEMY] = {0,3,6,0,1};

	const int SPAWN_ENEMY_Y[NB_ENEMY] = {0, 0, 100*0.7/RATIO_SIZE_ENEMY_2, -1350*0.72/RATIO_SIZE_ENEMY_3, -1500*0.75/RATIO_SIZE_ENEMY_4};
	const int TARGET_ENEMY_Y[NB_ENEMY] = {0, 0, 100*0.7/RATIO_SIZE_ENEMY_2, -50*0.72/RATIO_SIZE_ENEMY_3, -200*0.75/RATIO_SIZE_ENEMY_4};
	#define EXTRA_TIME_WAIT_BOSS 40

	const Weapon WEAPONS_ENEMY[NB_ENEMY][NB_MAX_WEAPON_ENEMY] = {
		{ {W_BASE_ENEMY, 30 ,0}, {NO_WEAPON, -1 ,0},    {NO_WEAPON, -1 ,0},    {NO_WEAPON, -1 ,0},    {NO_WEAPON, -1 ,0},   {NO_WEAPON, -1 ,0},   {NO_WEAPON, -1 ,0}   },
		{ {W_BASE_ENEMY, 40 ,0}, {W_LASER_ENEMY, 70 ,0}, {W_LASER_ENEMY, 70 ,0}, {W_BASE_ENEMY, 40 ,0},  {NO_WEAPON, -1 ,0},   {NO_WEAPON, -1 ,0},   {NO_WEAPON, -1 ,0}   },
		{ {W_BASE_ENEMY, 40 ,0}, {W_BASE_ENEMY, 55 ,0},  {W_BASE_ENEMY, 80 ,0},  {W_LASER_ENEMY, 100 ,0}, {W_BASE_ENEMY, 80 ,0}, {W_BASE_ENEMY, 55 ,0}, {W_BASE_ENEMY, 40 ,0} },
		{ {W_LASER_ENEMY, 30 ,0}, {NO_WEAPON, -1 ,0},    {NO_WEAPON, -1 ,0},    {NO_WEAPON, -1 ,0},    {NO_WEAPON, -1 ,0},   {NO_WEAPON, -1 ,0},   {NO_WEAPON, -1 ,0}   },
		{ {W_BASE_ENEMY, 30 ,0}, {W_BASE_ENEMY, 30 ,0},    {NO_WEAPON, -1 ,0},    {NO_WEAPON, -1 ,0},    {NO_WEAPON, -1 ,0},   {NO_WEAPON, -1 ,0},   {NO_WEAPON, -1 ,0}   }
	};

	const Vector2f WEAPON_DEST_ENEMY[NB_ENEMY][NB_MAX_WEAPON_ENEMY] = {
		{{39.5/RATIO_SIZE_ENEMY_0, 77/RATIO_SIZE_ENEMY_0}},
		{{43.5/RATIO_SIZE_ENEMY_1, 104/RATIO_SIZE_ENEMY_1}, {52.5/RATIO_SIZE_ENEMY_1, 108/RATIO_SIZE_ENEMY_1}, {97.5/RATIO_SIZE_ENEMY_1, 108/RATIO_SIZE_ENEMY_1}, {107/RATIO_SIZE_ENEMY_1, 103/RATIO_SIZE_ENEMY_1}},
		{{44.5/RATIO_SIZE_ENEMY_2, 137/RATIO_SIZE_ENEMY_2}, {50.5/RATIO_SIZE_ENEMY_2, 140/RATIO_SIZE_ENEMY_2}, {57/RATIO_SIZE_ENEMY_2, 142/RATIO_SIZE_ENEMY_2}, {107/RATIO_SIZE_ENEMY_2, 139/RATIO_SIZE_ENEMY_2}, {156/RATIO_SIZE_ENEMY_2, 142/RATIO_SIZE_ENEMY_2}, {162.5/RATIO_SIZE_ENEMY_2, 140/RATIO_SIZE_ENEMY_2}, {169/RATIO_SIZE_ENEMY_2,137/RATIO_SIZE_ENEMY_2}},
		{{52/RATIO_SIZE_ENEMY_3, 252/RATIO_SIZE_ENEMY_3}},
		{{2*114.5/RATIO_SIZE_ENEMY_4, 2*135/RATIO_SIZE_ENEMY_4}, {2*125.5/RATIO_SIZE_ENEMY_4, 2*135/RATIO_SIZE_ENEMY_4} }
	};

	const int RELOAD_FRAME_ENEMY[NB_ENEMY][NB_MAX_WEAPON_ENEMY] ={
		{50},
		{40, 80, 80, 40},
		{66, 66, 66, 100, 66, 66, 66},
		{500},
		{15, 15}
	};

	const int RELOAD_FRAME_RAND_ENEMY[NB_ENEMY][NB_MAX_WEAPON_ENEMY] ={
		{20},
		{15, 20, -1, 0},
		{20, 0, 0, 30, 0, 0, 0},
		{1},
		{1, 0}
	};

	const int COMBO_ENEMY[NB_ENEMY][NB_MAX_WEAPON_ENEMY] ={
		{0},
		{4, 1, 1, 4},
		{3, 3, 4, 2, 4, 3, 3},
		{2},
		{12,12}
	};

	const int COMBO_RAND_ENEMY[NB_ENEMY][NB_MAX_WEAPON_ENEMY] ={
		{1},
		{3, 1, -1, 0},
		{2, 3, 2, 1, -2, -1, 0},
		{1},
		{1,1}
	};

	const int RELOAD_FRAME_COMBO_ENEMY[NB_ENEMY][NB_MAX_WEAPON_ENEMY] ={
		{0},
		{2, 8, 8, 2},
		{3, 3, 2, 15, 2, 3, 3},
		{6},
		{0,0}
	};

	enum{AIMED, STRAIGHT, VERTICAL};
	const int TYPE_ENEMY_FIRE[NB_ENEMY][NB_MAX_WEAPON_ENEMY] ={
		{AIMED},
		{AIMED, VERTICAL, VERTICAL, AIMED},
		{AIMED, AIMED, AIMED, VERTICAL, AIMED, AIMED, AIMED},
		{STRAIGHT},
		{STRAIGHT, STRAIGHT}
	};

	const float ANGLE_ENEMY_FIRE[NB_ENEMY][NB_MAX_WEAPON_ENEMY] ={
		{PI/5},
		{PI/8, 0, 0, PI/8},
		{PI/5, PI/5, PI/5, 0, PI/5, PI/5, PI/5},
		{0},
		{0,0}
	};

	const float MISSILE_SPEED_ENEMY[NB_ENEMY][NB_MAX_WEAPON_ENEMY] ={
		{6},
		{9, 45, 45, 9},
		{8, 7, 6, 60, 6, 7, 8},
		{50},
		{7,7}
	};

	const SDL_Point ENEMY_CENTER[NB_ENEMY] = {
		{39,39},
		{75,54},
		{106,91},
		{52/RATIO_SIZE_ENEMY_3,89/RATIO_SIZE_ENEMY_3},
		{0,0}
	};


	const SDL_Point ROTATION_POINT_ARM={52/RATIO_SIZE_ENEMY_3,151/RATIO_SIZE_ENEMY_3};
	#define DIST_ROTA_HITBOX_BASIC_ARM 71.5
	#define DIST_ROTA_CANON_ARM 106
	#define DIST_ROTA_CENTER_HITBOX_ARM 56.5
	#define DIST_ROTA_CENTER_ADDITIONAL_HITBOX_ARM 30
	#define DIST_ROTA_REAL_HITBOX_ARM 68
	#define FRAME_RESET_ARM_MOVE -15 //here
	#define FRAME_SHOOT_ARM -1
	#define FRAME_ARM_MOVE 24
	#define FRAME_ARM_ROTATE 36
	#define NB_FRAME_WARMUP 8
	#define RAND_FRAME_ARM_MOVE 1
	#define MIN_ANGLE_ARM 10
	#define MAX_ANGLE_ARM 50
	#define MIN_ANGLE_AVANT_ARM 15
	#define MAX_ANGLE_AVANT_ARM 60
	#define SHIFT_X_ARM 25
	#define NB_SMALL_ATK_ARM 4
	#define RAND_SMALL_ATK_ARM 3//entre 3 et 5 atks
	#define ANGLE_TURN_BEAM_ARM 21
	#define FRAME_ARM_BEAM_MOVE 42
	#define RATIO_ADAPT_ANGLE_ARM 0.66
	#define RATIO_ALLONGEMENT_LASER 1.35
	#define FRAME_LASER_BEAM_DISAPEAR 7
	#define ANGLE_ENEMY_ARM_BOSS_FIRE 30
	#define BASE_ANGLE_ARM_BOSS_FIRE PI/40
	#define NERF_FREQUENCY_PER_ARM 6

	//explo arm
	#define NB_ARM_EXPLOSIONS 12
	const int PART_ARM_EXPLO[NB_ARM_EXPLOSIONS] = {0,0,0,0,0,0,0,0,1,1,1,1};
	const Vector2f EXPLO_COOR[NB_ARM_EXPLOSIONS] = {
		{20/RATIO_SIZE_ENEMY_3, 30/RATIO_SIZE_ENEMY_3},
		{-35/RATIO_SIZE_ENEMY_3, -40/RATIO_SIZE_ENEMY_3},
		{40/RATIO_SIZE_ENEMY_3, -12/RATIO_SIZE_ENEMY_3},
		{18/RATIO_SIZE_ENEMY_3, 38/RATIO_SIZE_ENEMY_3},
		{7/RATIO_SIZE_ENEMY_3, -27/RATIO_SIZE_ENEMY_3},
		{77/RATIO_SIZE_ENEMY_3, 57/RATIO_SIZE_ENEMY_3},
		{58/RATIO_SIZE_ENEMY_3, -87/RATIO_SIZE_ENEMY_3},
		{-80/RATIO_SIZE_ENEMY_3, -27/RATIO_SIZE_ENEMY_3},
		{-30/RATIO_SIZE_ENEMY_3, -27/RATIO_SIZE_ENEMY_3},
		{-45/RATIO_SIZE_ENEMY_3, 32/RATIO_SIZE_ENEMY_3},
		{12/RATIO_SIZE_ENEMY_3, -21/RATIO_SIZE_ENEMY_3},
		{38/RATIO_SIZE_ENEMY_3, -27/RATIO_SIZE_ENEMY_3}
	};
	const int SIZE_ARM_EXPLO[NB_ARM_EXPLOSIONS] = {80/RATIO_SIZE_ENEMY_3, 110/RATIO_SIZE_ENEMY_3, 70/RATIO_SIZE_ENEMY_3, 90/RATIO_SIZE_ENEMY_3, 97/RATIO_SIZE_ENEMY_3, 127/RATIO_SIZE_ENEMY_3, 99/RATIO_SIZE_ENEMY_3, 103/RATIO_SIZE_ENEMY_3, 83/RATIO_SIZE_ENEMY_3, 95/RATIO_SIZE_ENEMY_3, 103/RATIO_SIZE_ENEMY_3, 113/RATIO_SIZE_ENEMY_3};

	//exploArmpart
	#define NB_ARM_COMPONENT 8
	const float RATIO_HP_PIECES[NB_ARM_COMPONENT]={0,0, 1/7., 3/7., 5/7., 2/7., 4/7., 6/7. };
	const int DIST_ARM_PART[NB_ARM_COMPONENT] = {0,0, 202-151, 187-151, 176-151, 76-151, 67-151, 48-151};
	const int PART_ARM_SIZE[NB_ARM_COMPONENT] = {0,0, 100, 125, 107, 137, 106, 111};
	const int PART_ARM_SIDE[NB_ARM_COMPONENT] = {0,1,1,1,1,0,0,0};

	#define NB_ARM_BOSS_EXPLO 18
	#define MAX_Y_EXPLO_BOSS 100
	#define MIN_SIZE_EXPLO_BOSS (80/RATIO_SIZE_ENEMY_3)
	#define RAND_SIZE_EXPLO_BOSS (80/RATIO_SIZE_ENEMY_3)

	//enemyparts
	#define GRAVITY_ENEMY_PARTS 0.32
	#define RAND_X_PARTS 1.5
	#define RAND_Y_PARTS 0.6
	#define X_PARTS 2
	#define Y_PARTS 1




	const SDL_Rect LASER_BEAM_SRC = {0,0,854, 480};
	const SDL_Point LASER_BEAM_CENTER = {87*RATIO_ALLONGEMENT_LASER,244};
	#define NB_ROW_LASER_BEAM 7
	#define NB_COL_LASER_BEAM 9
	#define FIRST_FRAME_LASER 18
	#define NB_FRAME_START_LASER_BEAM 6
	const int LASER_BEAM_DIST_START[NB_FRAME_START_LASER_BEAM]={95, 364, 540, 630, 710, 767};
	#define FIRST_END_FRAME_LASER 57
	#define NB_FRAME_END_LASER_BEAM 3
	const int LASER_BEAM_DIST_END[NB_FRAME_END_LASER_BEAM] = {630, 364, 95};
	#define LASER_BEAM_WIDTH_HITBOX 25
	#define LASER_BEAM_ADDITIONAL_HITBOX 180

	const Ellips ENEMY_HITBOX[NB_ENEMY] = {
		{ 39.5/RATIO_SIZE_ENEMY_0,   39.5/RATIO_SIZE_ENEMY_0,  39.5/RATIO_SIZE_ENEMY_0,  39.5/RATIO_SIZE_ENEMY_0},
		{  75./RATIO_SIZE_ENEMY_1,   54.5/RATIO_SIZE_ENEMY_1,   56./RATIO_SIZE_ENEMY_1,  36.5/RATIO_SIZE_ENEMY_1},
		{106.5/RATIO_SIZE_ENEMY_2,   91.5/RATIO_SIZE_ENEMY_2,  88.5/RATIO_SIZE_ENEMY_2,  44.5/RATIO_SIZE_ENEMY_2},
		{ 52/RATIO_SIZE_ENEMY_3,   89.5/RATIO_SIZE_ENEMY_3,    49/RATIO_SIZE_ENEMY_3,  79.5/RATIO_SIZE_ENEMY_3},
		{ 2*120/RATIO_SIZE_ENEMY_4,   2*56/RATIO_SIZE_ENEMY_4,    2*140.5/RATIO_SIZE_ENEMY_4,  2*58/RATIO_SIZE_ENEMY_4}
	};

	const Ellips ENEMY_HITBOX_ARM = {0,0, 45/RATIO_SIZE_ENEMY_3, 76.5/RATIO_SIZE_ENEMY_3};
	const Ellips ENEMY_ADDITIONAL_HITBOX_ARM = {0,0 , 49/RATIO_SIZE_ENEMY_3, 42/RATIO_SIZE_ENEMY_3};

	const Ellips ENEMY_ADDITIONAL_HITBOX[NB_ENEMY] = {
		{39.5/RATIO_SIZE_ENEMY_0,39.5/RATIO_SIZE_ENEMY_0,1,1},
		{75/RATIO_SIZE_ENEMY_1,54.5/RATIO_SIZE_ENEMY_1,75/RATIO_SIZE_ENEMY_1,9.5/RATIO_SIZE_ENEMY_1},
		{106.5/RATIO_SIZE_ENEMY_2,81/RATIO_SIZE_ENEMY_2,106.5/RATIO_SIZE_ENEMY_2,10/RATIO_SIZE_ENEMY_2},
		{ 52/RATIO_SIZE_ENEMY_3,   90/RATIO_SIZE_ENEMY_3, 45/RATIO_SIZE_ENEMY_3, 72/RATIO_SIZE_ENEMY_3},
		{ 2*120/RATIO_SIZE_ENEMY_4,   2*113.5/RATIO_SIZE_ENEMY_4,    2*9/RATIO_SIZE_ENEMY_4,  2*13.5/RATIO_SIZE_ENEMY_4}
	};


	//laser enemy
	enum{INTERPOLATION, ROUND_TRIP, BESACE, ARM, NONE};
	const float TYPE_MOVE_ENEMY[NB_ENEMY] = {INTERPOLATION, ROUND_TRIP, BESACE, ARM, NONE};
	const int NB_MOVE_ENEMY[NB_ENEMY] = {0, 7, 5, 100, 0};
	const float RESET_MOVE_ENEMY[NB_ENEMY] = {0, 0.05, 0.01, 0, 0};
	#define MAX_Y_BOSS 60
	#define MOVE_Y_BOSS 2.3

//BossBar
	typedef struct{float maxHp; int frameSpawn;}BossBar;
	#define FRAME_BAR_SPAWN 225
	#define PERIODE_CLIGNOTE (2*45)
	#define NB_PERIODE 2
	#define FRAME_APPARITION_BAR 45
	#define TIME_APPARITION_BAR 45

	const SDL_Rect BOSS_BAR_SRC = {0,0,1920,1080};
	const SDL_Rect BOSS_BAR_DEST = {0,0,1920,1080};

	const SDL_Color BOSS_BAR_COLOR = {50,80,90,255};
	const SDL_Rect BOSS_BAR_DEST1 = {565,45,51,990};
	const SDL_Rect BOSS_BAR_DEST2 = {1304,45,51,990};


//Explosion
	typedef struct{float x; float y; int frame; int taille; int id; int delay;}Explosion;
	#define NB_EXPLOSIONS 2
	enum{EXPLO_MISSILE, EXPLO_SHIP};

	static const int  FRAME_EXPLOSIONS[NB_EXPLOSIONS] = {12, 20};
	static const int  NB_ANIM_EXPLOSIONS[NB_EXPLOSIONS] = {6, 10};

	static const SDL_Rect EXPLO_SRCS[NB_EXPLOSIONS] = {{0,0, 374, 374} , {0,0, 312, 312}};

	#define BASE_TAILLE_EXPLOSION 40
	#define RATIO_SHIP_EXPLO_SIZE 1.2

// Laser sparks
	#define RAND_NEW_SPARK 1150
	#define PRECISION_DIV_SPARK 1000
	#define LASER_WIDTH 5
	#define DIVIDE_ALPHA_PER_DIST 1.5
	#define RAND_POS_SPARKS 15
	#define BASE_SIZE_SPARKS 2
	#define RAND_SIZE_SPARKS 5
	#define MAX_SPAWN_DIST_SPARK 0.4
	#define BASE_SPARK_SPEED 7
	#define RAND_SPARK_SPEED 15
	#define ADDITIONAL_I_ALPHA_LASER 1.4
	#define DELTA_CENTER 10
	#define LASER_HALO_WIDTH 15
	const SDL_Color SPARK_BASE_COLOR = {200,0,0,100};
	const SDL_Color SPARK_RAND_COLOR = {56,150,0,100};
	const SDL_Point CENTER_MIRROR = {35,25};
	const SDL_Rect MIRROR_SRC = {59,0,59,59};

//colors
	//background
	static const SDL_Color BACKGROUND_COLOR = {0x00, 0x19, 0x3a,255};
	static const SDL_Color JAUGE_COLOR = {0x00,0xcc,0xFF,255};
