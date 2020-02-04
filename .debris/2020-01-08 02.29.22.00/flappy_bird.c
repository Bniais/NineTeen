#include <stdio.h>
#include "define.h"
#include <time.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

//define in main normalement
#define WINDOW_L 1366
#define WINDOW_H 768

//CONST Dimensions du poto dans l'image
const SDL_Point DIM_BACKGROUND = {144,256};
const SDL_Point POS_BACKGROUND = {0,0};

const SDL_Point DIM_OBSTACLE = {26,160};
const SDL_Point POS_OBSTACLE_H = {26,0};
const SDL_Point POS_OBSTACLE_B = {0,0};

const SDL_Point DIM_SOL = {168,55};
const SDL_Point POS_SOL = {0,0};

const SDL_Point DIM_PERSO = {17,12};
const SDL_Point POS_PERSO_1 = {0,0};
const SDL_Point POS_PERSO_2 = {17,0};
const SDL_Point POS_PERSO_3 = {34,0};

const SDL_Point DIM_CHIFFRE = {14,18};
const SDL_Point POS_CHIFFRE_0 = {490,60};
const SDL_Point POS_CHIFFRE_1 = {134,455};
const SDL_Point POS_CHIFFRE_2 = {292,160};
const SDL_Point POS_CHIFFRE_3 = {306,160};
const SDL_Point POS_CHIFFRE_4 = {320,160};
const SDL_Point POS_CHIFFRE_5 = {334,160};
const SDL_Point POS_CHIFFRE_6 = {292,184};
const SDL_Point POS_CHIFFRE_7 = {306,184};
const SDL_Point POS_CHIFFRE_8 = {320,184};
const SDL_Point POS_CHIFFRE_9 = {334,184};


// delay frame
const int FRAME_DELAY = 50;
const int RATIO_ANIM_SOL = 15; // plus c'est petit plus c'est rapide


#define DISTANCE_BETWEEN_OBSTACLE 50
#define DISTANCE_UNDER_OBSTACLE 100
#define NB_POS_OBSTACLE 10 // equivaut a 6 possition

#define PRELOAD_POS_OBSTACLE 10



struct Environement {
	int position;
	int hauteur;
};


#define NB_ANIM_CHAR 3.0
#define TIME_TOTAL_ANIM 1000
#define FPS 60


#define DIR_TILSET "Textures/tilset.png"

#define DIR_BIRDS "Textures/birds.png"
#define DIR_MEDALS "Textures/medals.png"
#define DIR_PIPES "Textures/pipes.png"
#define DIR_SCOREBOARD "Textures/scoreBoard.png"
#define DIR_BACKGROUND "Textures/backgrounds.png"
#define DIR_SOL "Textures/sol.png"



// HASH FONCTION

long long hashage (int value, long const_1, long const_2, long const_3, long const_4 ){

	long long hash;

	if(value){
		hash = value * const_1;
		hash = hash - const_2;
		hash = hash * const_3;
		hash = hash / const_4;
	} else {
		hash = const_1;
		hash = hash - const_2;
		hash = hash * const_3;
		hash = hash / const_4;
	}

	return hash;
}


void init_const_hash(long *const_1, long *const_2, long *const_3, long *const_4){
	srand (time (NULL));
	*const_1 = rand();
	*const_2 = rand();
	*const_3 = rand();
	*const_4 = rand();

}



SDL_Texture * LoadTextureWithErrorCode(SDL_Renderer *renderer ,char directory[]){

	SDL_Texture* texture = IMG_LoadTexture(renderer,directory);

	if(!texture)
		printf("TEXTURE FAILURE : Erreur lors de la creation de %s SDL_ERROR : %s",directory,SDL_GetError());

	return texture;
}


// permet de genrer l'affichage du bakcground en plusieur fois selon
// la dimension de l'ecran de sorti
void background(SDL_Renderer *renderer, SDL_Texture* texture_background, int upscale){


	SDL_Rect background = {POS_BACKGROUND.x,POS_BACKGROUND.y,DIM_BACKGROUND.x,DIM_BACKGROUND.y};

	//On le dessine tant qu'on a pas rempli l'ecran
	SDL_Rect target = {0,0,DIM_BACKGROUND.x*upscale,WINDOW_H};
  SDL_RenderCopy(renderer, texture_background,&background, &target);
	while (target.x + DIM_BACKGROUND.x*upscale < WINDOW_L) {
		target.x += DIM_BACKGROUND.x*upscale;
	  SDL_RenderCopy(renderer, texture_background,&background, &target);
	}


}

// affichage d'un obstacle
void obstacle(SDL_Renderer *renderer, SDL_Texture *texture_pipes, int upscale, int position, int position_axe_x){

//	float ratio = DIM_SOL.x/RATIO_ANIM_SOL;

//	ratio_pos_x *= ratio;

	int pos = -DIM_OBSTACLE.y*upscale - DISTANCE_BETWEEN_OBSTACLE * upscale + (  ( WINDOW_H - DIM_SOL.y *upscale )/NB_POS_OBSTACLE ) * position;

	SDL_Rect obstacle_h = {POS_OBSTACLE_H.x,POS_OBSTACLE_H.y,DIM_OBSTACLE.x,DIM_OBSTACLE.y};
	SDL_Rect obstacle_b = {POS_OBSTACLE_B.x,POS_OBSTACLE_B.y,DIM_OBSTACLE.x,DIM_OBSTACLE.y};

	SDL_Rect target = {position_axe_x,pos,DIM_OBSTACLE.x*upscale,DIM_OBSTACLE.y*upscale};
	SDL_RenderCopy(renderer,texture_pipes,&obstacle_h,&target);
	target.y += DIM_OBSTACLE.y*upscale + DISTANCE_BETWEEN_OBSTACLE*upscale;
	SDL_RenderCopy(renderer,texture_pipes,&obstacle_b,&target);


}

// affichage du sol
void sol(SDL_Renderer *renderer, SDL_Texture *texture_sol, int upscale,int animate_sol){
	SDL_Rect sol = {POS_SOL.x,POS_SOL.y,DIM_SOL.x,DIM_SOL.y};

	// ratio sol pour avoir 100 position de sol//
	float ratio = DIM_SOL.x/RATIO_ANIM_SOL;

	animate_sol *= ratio;

	SDL_Rect target = {-animate_sol,WINDOW_H-(DIM_SOL.y*upscale),DIM_SOL.x*upscale,DIM_SOL.y*upscale};
	SDL_RenderCopy(renderer,texture_sol,&sol,&target);

	while (target.x + DIM_SOL.x*upscale < WINDOW_L) {
		target.x +=  DIM_SOL.x*upscale;
	  SDL_RenderCopy(renderer, texture_sol,&sol, &target);
	}
}

void personnage(SDL_Renderer *renderer,SDL_Texture *texture_birds, int upscale, int animate_perso){

	SDL_Point pos_print;

	switch(animate_perso){
		case 1 :{
				pos_print = POS_PERSO_1;
		};break;
		case 2 :{
				pos_print = POS_PERSO_2;
		} ;break;
		case 3 :{
				pos_print = POS_PERSO_3;
		} ;break;
	}


	SDL_Rect personnage = {pos_print.x,pos_print.y,DIM_PERSO.x,DIM_PERSO.y};

	SDL_Rect target = {WINDOW_L/2 ,WINDOW_H/2,DIM_PERSO.x*upscale,DIM_PERSO.y*upscale};
	SDL_RenderCopy(renderer,texture_birds,&personnage,&target);
}

void score(SDL_Renderer *renderer, int score){
	TTF_Init();
	char str[21];
	TTF_Font *myFont = TTF_OpenFont("/Textures/flappy.ttf",100);

	SDL_Color White = {255,255,255};
	SDL_Surface* surfaceMessage = NULL;
	SDL_Texture* Message = NULL;

	sprintf(str, "%d", score);

	SDL_FreeSurface(surfaceMessage);
	surfaceMessage = TTF_RenderText_Blended(myFont, str, White);
  SDL_DestroyTexture(Message); //Pas sûr que ce soit obligatoire
  Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

//A faire à chaque frame où on veut afficher le nombre
  SDL_RenderCopy(renderer, Message, NULL, &Message_rect);

}


// regroupe toute les fonctions d'affichage
void view(SDL_Renderer * renderer,int upscale, struct Environement environement[] ,int score,int animate_perso,int animate_sol,SDL_Texture *texture_background, SDL_Texture *texture_pipes, SDL_Texture *texture_birds, SDL_Texture *texture_medals, SDL_Texture *texture_scoreBoard, SDL_Texture *texture_sol){

	SDL_RenderClear(renderer);


	background(renderer,texture_background,upscale);

	// cas particuler
	for (int i = 0; i < PRELOAD_POS_OBSTACLE; i++) {
		obstacle(renderer,texture_pipes,upscale,environement[i].hauteur,environement[i].position); // 3 - 9 //
	}


	sol(renderer,texture_sol,upscale,animate_sol);
	personnage(renderer,texture_birds,upscale,animate_perso);
	score(renderer,score);

	//On affiche
	SDL_RenderPresent(renderer);

}

void init_environement(struct Environement *environement, int upscale, int *animate_perso, int *animate_sol, int *end, int *traitement){


	for(int i=0; i<PRELOAD_POS_OBSTACLE;i++){
		environement[i].hauteur = (rand() % (9 + 1 - 3)) + 3 ;
		if (i)
			environement[i].position = DISTANCE_UNDER_OBSTACLE*upscale + environement[i-1].position;
		else
			environement[i].position = WINDOW_L;
	}

	*animate_perso = 0;
	*animate_sol = 100;
	*end = 0;
	*traitement = -1;
}

int traitement_environement(struct Environement *environement, int upscale, int traitement, int *score, long long *score_hash, long *const_1, long *const_2, long *const_3, long *const_4){


	if(traitement == -1){
		if(environement[0].position + DIM_OBSTACLE.x*upscale < WINDOW_L/2 ){
			traitement = 0;


			if( *score_hash == hashage(*score, *const_1, *const_2, *const_3, *const_4))
				{
					srand (time (NULL));
					*const_1 = rand();
					*const_2 = rand();
					*const_3 = rand();
					*const_4 = rand();

					*score += 1;

					*score_hash = *score * *const_1;
					*score_hash = *score_hash - *const_2;
					*score_hash = *score_hash * *const_3;
					*score_hash = *score_hash / *const_4;


					printf("\nscore = %d\n",*score);
				}
			else {
				// BAN
				printf("Baned\n");
				traitement = -30;
			}



		}

	} else if(traitement == 0){
		if(environement[1].position + DIM_OBSTACLE.x*upscale < WINDOW_L/2 ){
			traitement = 1;


			if( *score_hash == hashage(*score, *const_1, *const_2, *const_3, *const_4))
				{
					srand (time (NULL));
					*const_1 = rand();
					*const_2 = rand();
					*const_3 = rand();
					*const_4 = rand();

					*score += 1;

					*score_hash = *score * *const_1;
					*score_hash = *score_hash - *const_2;
					*score_hash = *score_hash * *const_3;
					*score_hash = *score_hash / *const_4;


					printf("\nscore = %d\n",*score);
				}
			else {
				// BAN
				printf("Baned\n");
				traitement = -30;
			}

		}
	} else if (traitement == 2 || traitement == 1){
		if(environement[2].position + DIM_OBSTACLE.x*upscale < WINDOW_L/2 ){
				traitement = 3;

				if( *score_hash == hashage(*score, *const_1, *const_2, *const_3, *const_4))
					{
						srand (time (NULL));
						*const_1 = rand();
						*const_2 = rand();
						*const_3 = rand();
						*const_4 = rand();

						*score += 1;

						*score_hash = *score * *const_1;
						*score_hash = *score_hash - *const_2;
						*score_hash = *score_hash * *const_3;
						*score_hash = *score_hash / *const_4;


						printf("\nscore = %d\n",*score);
					}
				else {
					// BAN
					printf("Baned\n");
					traitement = -30;
				}			}

	} else if (traitement == 3){
		if(  !(environement[2].position + DIM_OBSTACLE.x*upscale < WINDOW_L/2)  )
			traitement = 2;

	}


	for(int i=0; i<PRELOAD_POS_OBSTACLE;i++)

		environement[i].position -= DIM_SOL.x/RATIO_ANIM_SOL;

	if( environement[0].position + DIM_OBSTACLE.x*upscale < 0){
		//printf("Supprimer env 0\n" );
		for(int i = 1 ; i < PRELOAD_POS_OBSTACLE ; i++){
			environement[i-1] = environement[i];
		}

		environement[PRELOAD_POS_OBSTACLE-1].hauteur = (rand() % (9 + 1 - 3)) + 3 ;
		environement[PRELOAD_POS_OBSTACLE-1].position = environement[PRELOAD_POS_OBSTACLE-2].position + DISTANCE_UNDER_OBSTACLE*upscale;

	}

	return traitement;
}

void traitement_variable(int *animate_perso,int *animate_sol){
	if(*animate_perso > 3)
		*animate_perso = 1;

	if(*animate_sol > 100)
			*animate_sol = 0;
}


// primary func
int flappy_bird(SDL_Window *window, SDL_Renderer *renderer, int upscale){

	// init texture
	SDL_Texture *texture_background = LoadTextureWithErrorCode(renderer,DIR_BACKGROUND);
	SDL_Texture *texture_pipes = LoadTextureWithErrorCode(renderer,DIR_PIPES);
	SDL_Texture *texture_birds = LoadTextureWithErrorCode(renderer,DIR_BIRDS);
	SDL_Texture *texture_medals = LoadTextureWithErrorCode(renderer,DIR_MEDALS);
	SDL_Texture *texture_scoreBoard = LoadTextureWithErrorCode(renderer,DIR_SCOREBOARD);
	SDL_Texture *texture_sol = LoadTextureWithErrorCode(renderer,DIR_SOL);

	// generation des futur position aleatoire des obstacles
	// a mettre en fonction init du jeux

	struct Environement environement[PRELOAD_POS_OBSTACLE];
	int animate_perso,animate_sol,end,traitement;
	long long score_hash;
	int score=0;
	srand(time(NULL));
	init_environement(environement,upscale,&animate_perso,&animate_sol,&end,&traitement);

	long const_1, const_2, const_3, const_4;
	init_const_hash(&const_1, &const_2, &const_3, &const_4);
	score_hash = hashage(score, const_1, const_2, const_3, const_4);
	// end init


	while(!end){

		SDL_Event ev;
		while ( SDL_PollEvent(&ev) )
		{
			if (ev.type == SDL_KEYUP)
				end = 1;
		}

		view(renderer,upscale,environement, score ,animate_perso++ , animate_sol++  ,texture_background,
																																									 texture_pipes,
																																									 texture_birds,
																																									 texture_medals,
																																									 texture_scoreBoard,
																																									 texture_sol);

		// traitement de mon environement
		traitement = traitement_environement(environement,upscale,traitement,&score,
																			 &score_hash,
																			 &const_1,
																			 &const_2,
																			 &const_3,
																			 &const_4);


		traitement_variable(&animate_perso,&animate_sol);

		SDL_Delay(FRAME_DELAY);

	}

	return 0;
}




// launch debug
int main() {
	SDL_Init(SDL_INIT_EVERYTHING );
	// main du jeu
	SDL_Window* window = SDL_CreateWindow("Nineteen | Launcher", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_L, WINDOW_H, SDL_WINDOW_SHOWN );
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

		int upscale = WINDOW_H/DIM_BACKGROUND.y;
	flappy_bird(window, renderer,upscale);


	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

}
