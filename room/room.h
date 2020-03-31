typedef struct MeilleureScore_s
{
	char nomJeux[30];
	char nomJoueur[30];
	int score;
}MeilleureScore_t;

void room(char *token,struct MeilleureScore_s meilleureScore[],SDL_Window *Window);
void InitMeilleureScore(struct MeilleureScore_s str[]);
void updateMeilleureScore(struct MeilleureScore_s str[] ,char *token);
