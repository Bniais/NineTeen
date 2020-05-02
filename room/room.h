#include "import.h"

typedef struct MeilleureScore_s
{
	char nomJeux[30];
	char nomJoueur[30];
	int score;
	int scoreJoueurActuel;
	float multiplicator;
}MeilleureScore_t;

struct Scene_s
{
	const C_STRUCT aiScene* scene;
	GLuint *textures, *counts,nbTextures,nbMeshes;
};

int room(char *token,struct MeilleureScore_s meilleureScore[], SDL_Window *Window,const C_STRUCT aiScene* scene, int optFullScreen, SDL_Rect borderSize );
void InitMeilleureScore(struct MeilleureScore_s str[]);
int updateMeilleureScore(struct MeilleureScore_s str[] ,char *token);
