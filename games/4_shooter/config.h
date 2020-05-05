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

const SDL_Rect PLAYGROUND = {BASE_WINDOW_W*0.3,0,BASE_WINDOW_W/2.5,BASE_WINDOW_H};

//vaisseau
#define RATIO_TAILLE 1.45
#define NB_SHIP_FORMS 3
const SDL_Rect  SHIP_SRC = {0, 0, 132, 170};
const SDL_Point SHIP_CENTER = {66, 66};
const SDL_Rect  SHIP_HITBOX[NB_SHIP_FORMS] = {
	{27, 22, 78, 96},
	{21, 18, 90, 100},
	{22, 15, 88, 102},
};

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


//colors
	//background
	static const SDL_Color BACKGROUND_COLOR = {0x00, 0x19, 0x3a};
	static const SDL_Color JAUGE_COLOR = {0x00,0xcc,0xFF};
