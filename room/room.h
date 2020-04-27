#include "import.h"
#include "../define/define.h"
typedef struct MeilleureScore_s
{
	char nomJeux[30];
	char nomJoueur[30];
	int score;
	int scoreJoueurActuel;
	float multiplicator;
}MeilleureScore_t;

int room(char *token,struct MeilleureScore_s meilleureScore[], SDL_Window *Window,const C_STRUCT aiScene* scene, int optFullScreen , SDL_Renderer * pRenderer, SDL_Texture * textures[NB_GAMES][NB_MAX_TEXTURES]);
void InitMeilleureScore(struct MeilleureScore_s str[]);
void updateMeilleureScore(struct MeilleureScore_s str[] ,char *token);
void aiLoadTexture(const char* filename, const C_STRUCT aiScene *_scene);
