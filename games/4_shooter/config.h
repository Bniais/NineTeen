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

//Background
#define RATIO_WIDTH_PLAYGROUND 3
static const SDL_Rect BACKGROUND_DEST = {(BASE_WINDOW_W - BASE_WINDOW_W/RATIO_WIDTH_PLAYGROUND)/2, 0, BASE_WINDOW_W/RATIO_WIDTH_PLAYGROUND, BASE_WINDOW_H};
static const SDL_Rect BACKGROUND_SRC = {0,0,BASE_WINDOW_W/RATIO_WIDTH_PLAYGROUND, BASE_WINDOW_H};
static const SDL_Rect BACKGROUND_GLOBAL_SRC = {0,0,BASE_WINDOW_W/RATIO_WIDTH_PLAYGROUND, 4096};

const int SCROLL_SPEED_BACKGROUND[3] = {6,10,12};

//weapon
#define NB_MAX_WEAPON 5
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
#define NB_MISSILES 1
typedef struct Missile_s{float x; float y; int id; float rota;}Missile;
#define RELOAD_FRAME 15
const Ellips MISSILE_HITBOX[NB_MISSILES] = {
	{0,0,6,16}
};

static const SDL_Rect MISSILE_SRC[NB_MISSILES] = {
	{0,0,38,68}
};

const SDL_Point MISSILE_CENTER[NB_MISSILES] = {
	{19,26}
};

//Enemies
#define NB_ENEMY 3
typedef struct {int id; float x; float y; float r; float rota; float hp;}Enemy;
const SDL_Rect ENEMY_SRC[NB_ENEMY] = {
	{0,0,79,79},
	{0,0,150,109},
	{0,0,213,143}
};



const float RATIO_SIZE_ENEMY[NB_ENEMY] = {1.5, 1.25, 1};

const Ellips ENEMY_HITBOX[NB_ENEMY] = {
	{39.5/RATIO_SIZE_ENEMY[0],  39.5/RATIO_SIZE_ENEMY[0],  39.5/RATIO_SIZE_ENEMY[0],  39.5/RATIO_SIZE_ENEMY[0]},
	{75./RATIO_SIZE_ENEMY[1],   54.5/RATIO_SIZE_ENEMY[1],   56./RATIO_SIZE_ENEMY[1],  36.5/RATIO_SIZE_ENEMY[1]},
	{106.5/RATIO_SIZE_ENEMY[2], 91.5/RATIO_SIZE_ENEMY[2],  88.5/RATIO_SIZE_ENEMY[2],  44.5/RATIO_SIZE_ENEMY[2]}
};

const Ellips ENEMY_ADDITIONAL_HITBOX[NB_ENEMY] = {
	{39.5/RATIO_SIZE_ENEMY[0],39.5/RATIO_SIZE_ENEMY[0],1,1},
	{75/RATIO_SIZE_ENEMY[1],54.5/RATIO_SIZE_ENEMY[1],75/RATIO_SIZE_ENEMY[1],9.5/RATIO_SIZE_ENEMY[1]},
	{106.5/RATIO_SIZE_ENEMY[2],81/RATIO_SIZE_ENEMY[2],106.5/RATIO_SIZE_ENEMY[2],10/RATIO_SIZE_ENEMY[2]}
};

//colors
	//background
	static const SDL_Color BACKGROUND_COLOR = {0x00, 0x19, 0x3a};
	static const SDL_Color JAUGE_COLOR = {0x00,0xcc,0xFF};
