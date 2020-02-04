#include <stdio.h>
#include "define.h"
#include <time.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

//define in main normalement
#define WINDOW_L 1920
#define WINDOW_H 1080

//CONST Dimensions du poto dans l'image
const SDL_Point DIM_BACKGROUND = {144,256};
const SDL_Point POS_BACKGROUND = {0,0};

const SDL_Point DIM_OBSTACLE = {26,160};
const SDL_Point POS_OBSTACLE_H = {78,0};
const SDL_Point POS_OBSTACLE_B = {52,0};

const SDL_Point DIM_SOL = {168,55};
const SDL_Point POS_SOL = {0,0};

const SDL_Point DIM_PERSO = {17,12};
const SDL_Point POS_PERSO_1 = {0,0};
const SDL_Point POS_PERSO_2 = {17,0};
const SDL_Point POS_PERSO_3 = {34,0};

const SDL_Point DIM_CHIFFRE = {12,18};
const SDL_Point POS_CHIFFRE = {12,0};



// COLOR //
const SDL_Color WHITE = {255,255,255};
const SDL_Color BLACK = {0,0,0};

// delay frame
const int FRAME_DELAY = 32;
const int RATIO_ANIM_SOL = 20; // plus c'est petit plus c'est rapide


#define DISTANCE_BETWEEN_OBSTACLE 50
#define DISTANCE_UNDER_OBSTACLE 100 //100
#define NB_POS_OBSTACLE 10
const int TRANCHE_POS_OBSTACLE = WINDOW_H/2;

#define PRELOAD_POS_OBSTACLE 5
#define UPPER_STEP 4
#define UPPER_SPEED 1
#define UPPER_BY_STEP 30
#define GRAVITY_SPEED 2

#define BAN_CODE -333

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
#define DIR_NUM "Textures/chiffre.png"
#define DIR_SOL "Textures/sol.png"
#define DIR_FLAP_WAV "Sounds/flap.wav"
#define DIR_HURT_WAV "Sounds/hurt.wav"
#define DIR_SCORE_WAV "Sounds/score.wav"

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

	SDL_Rect obstacle_h = {POS_OBSTACLE_H.x,POS_OBSTACLE_H.y,DIM_OBSTACLE.x,DIM_OBSTACLE.y};
	SDL_Rect obstacle_b = {POS_OBSTACLE_B.x,POS_OBSTACLE_B.y,DIM_OBSTACLE.x,DIM_OBSTACLE.y};

	SDL_Rect target = {position_axe_x,-TRANCHE_POS_OBSTACLE + (TRANCHE_POS_OBSTACLE/NB_POS_OBSTACLE)*position,DIM_OBSTACLE.x*upscale,DIM_OBSTACLE.y*upscale};

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

void personnage(SDL_Renderer *renderer,SDL_Texture *texture_birds, SDL_Point coordonner_perso,int upscale, int animate_perso){

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

	SDL_Rect target = {coordonner_perso.x - ( DIM_PERSO.x*upscale ) / 2, coordonner_perso.y - ( DIM_PERSO.y *upscale ) / 2,
		 								DIM_PERSO.x*upscale , DIM_PERSO.y*upscale};


	SDL_RenderCopy(renderer,texture_birds,&personnage,&target);
}


int strlen_num(int score)
{
	int count=0;

	if (score)
	{
		while(score > 0)
		{
			score /=10;
			count++;
		}
	}
	else
		count++;

	return count;
}



void print_score(SDL_Renderer *renderer , SDL_Texture *texture_chiffre,int score, int upscale)
{
	int taille = strlen_num(score);

	SDL_Rect chiffre = {POS_CHIFFRE.x,POS_CHIFFRE.y,DIM_CHIFFRE.x,DIM_CHIFFRE.y};
	SDL_Rect target = {WINDOW_L/2 - DIM_CHIFFRE.x*upscale ,WINDOW_H/6  , DIM_CHIFFRE.x * upscale, DIM_CHIFFRE.y * upscale};

	for(int i=0 ; i < taille; i++)
	{
		chiffre.x = POS_CHIFFRE.x* ( score%10);
		target.x -= DIM_CHIFFRE.x*upscale;
		SDL_RenderCopy(renderer,texture_chiffre,&chiffre,&target);

		score /=10;
	}





}



// regroupe toute les fonctions d'affichage
void view(SDL_Renderer * renderer, SDL_Point coordonner_perso,int upscale, struct Environement environement[] ,int score,int animate_perso,int animate_sol,SDL_Texture *texture_background, SDL_Texture *texture_pipes, SDL_Texture *texture_birds, SDL_Texture *texture_medals, SDL_Texture *texture_scoreBoard, SDL_Texture *texture_sol, SDL_Texture *texture_chiffre){

	SDL_RenderClear(renderer);


	background(renderer,texture_background,upscale);

	// cas particuler charger X obstacle
	for (int i = 0; i < PRELOAD_POS_OBSTACLE; i++) {
		obstacle(renderer,texture_pipes,upscale,environement[i].hauteur,environement[i].position);
	}


	sol(renderer,texture_sol,upscale,animate_sol);
	personnage(renderer , texture_birds, coordonner_perso , upscale , animate_perso );
	print_score(renderer, texture_chiffre, score, upscale);

	//On affiche
	SDL_RenderPresent(renderer);

}

void init_environement(struct Environement *environement, int upscale, int *animate_perso, int *animate_sol, int *end, int *traitement){


	for(int i=0; i<PRELOAD_POS_OBSTACLE;i++){
		environement[i].hauteur = rand() % NB_POS_OBSTACLE ;;
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

int traitement_environement(struct Environement *environement, int upscale, int traitement, int *score, long long *score_hash, long *const_1, long *const_2, long *const_3, long *const_4, Mix_Chunk *score_wav){

	if(traitement == -1){
		if(environement[0].position + DIM_OBSTACLE.x*upscale < WINDOW_L/2 ){
			traitement = 0;


		if( *score_hash == hashage(*score, *const_1, *const_2, *const_3, *const_4))
			{
				Mix_PlayChannel( -1, score_wav,0);
				*const_1 = rand();
				*const_2 = rand();
				*const_3 = rand();
				*const_4 = rand();

				*score += 1;

				*score_hash = *score * *const_1;
				*score_hash = *score_hash - *const_2;
				*score_hash = *score_hash * *const_3;
				*score_hash = *score_hash / *const_4;

			}
		else {
			// BAN
			printf("Baned\n");
			traitement = BAN_CODE;
		}



		}

	} else if(traitement == 0){
		if(environement[1].position + DIM_OBSTACLE.x*upscale < WINDOW_L/2 ){
			traitement = 1;


			if( *score_hash == hashage(*score, *const_1, *const_2, *const_3, *const_4))
				{
					Mix_PlayChannel( -1, score_wav,0);
					*const_1 = rand();
					*const_2 = rand();
					*const_3 = rand();
					*const_4 = rand();

					*score += 1;

					*score_hash = *score * *const_1;
					*score_hash = *score_hash - *const_2;
					*score_hash = *score_hash * *const_3;
					*score_hash = *score_hash / *const_4;


				}
			else {
				// BAN
				printf("Baned\n");
				traitement = BAN_CODE;
			}

		}
	} else if (traitement == 2 || traitement == 1){
		if(environement[2].position + DIM_OBSTACLE.x*upscale < WINDOW_L/2 ){
				traitement = 3;
				Mix_PlayChannel( -1, score_wav,0);

			if( *score_hash == hashage(*score, *const_1, *const_2, *const_3, *const_4))
				{

					*const_1 = rand();
					*const_2 = rand();
					*const_3 = rand();
					*const_4 = rand();

					*score += 1;

					*score_hash = *score * *const_1;
					*score_hash = *score_hash - *const_2;
					*score_hash = *score_hash * *const_3;
					*score_hash = *score_hash / *const_4;


				}
			else {
				// BAN
				printf("Baned\n");
				traitement = BAN_CODE;
			}

			}

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

		environement[PRELOAD_POS_OBSTACLE-1].hauteur = rand() % NB_POS_OBSTACLE ;
		environement[PRELOAD_POS_OBSTACLE-1].position = environement[PRELOAD_POS_OBSTACLE-2].position + DISTANCE_UNDER_OBSTACLE*upscale;

	}

	return traitement;
}

int collision(struct Environement *environement, SDL_Point coordonner_perso, int upscale, Mix_Chunk *hurt_wav){
	int collision_detecter = 0;

	// permet de dire quand on sur l'axe d'un obstacle et de qu'elle obstacle
	// + detection collision
	for (int i = 0; i < PRELOAD_POS_OBSTACLE; i++) {
		if( environement[i].position + DIM_PERSO.x*upscale > (  WINDOW_L/2 - ( (DIM_OBSTACLE.x*upscale)/2 ) ) && environement[i].position + DIM_PERSO.x*upscale< (  WINDOW_L/2 + ( (DIM_OBSTACLE.x*upscale)/2 ) )){

			int bas =( (  -TRANCHE_POS_OBSTACLE + (TRANCHE_POS_OBSTACLE/NB_POS_OBSTACLE)*environement[i].hauteur ) + DIM_OBSTACLE.y*upscale ) + DIM_PERSO.y*upscale/2;
			int haut = (  -TRANCHE_POS_OBSTACLE + (TRANCHE_POS_OBSTACLE/NB_POS_OBSTACLE)*environement[i].hauteur ) + DIM_OBSTACLE.y*upscale + DISTANCE_BETWEEN_OBSTACLE*upscale - DIM_PERSO.y*upscale/2;
			if(! (coordonner_perso.y > bas && coordonner_perso.y < haut)  )
				collision_detecter = 1;

		}

	}

// detection du sol
	if(coordonner_perso.y + DIM_PERSO.y*upscale > WINDOW_H - DIM_SOL.y*upscale){
		collision_detecter = 1;
	}

	// jouer un son
	if (collision_detecter)
		Mix_PlayChannel( -1, hurt_wav,0);

	return collision_detecter;
}

void traitement_variable(int *animate_perso,int *animate_sol){
	if(*animate_perso > 3)
		*animate_perso = 1;

	if(*animate_sol > 100)
			*animate_sol = 0;
}

// primary func
int flappy_bird(SDL_Window *window, SDL_Renderer *renderer, int upscale){

	srand(time(NULL));

	// init texture
	SDL_Texture *texture_background = LoadTextureWithErrorCode(renderer,DIR_BACKGROUND);
	SDL_Texture *texture_pipes = LoadTextureWithErrorCode(renderer,DIR_PIPES);
	SDL_Texture *texture_birds = LoadTextureWithErrorCode(renderer,DIR_BIRDS);
	SDL_Texture *texture_medals = LoadTextureWithErrorCode(renderer,DIR_MEDALS);
	SDL_Texture *texture_scoreBoard = LoadTextureWithErrorCode(renderer,DIR_SCOREBOARD);
	SDL_Texture *texture_sol = LoadTextureWithErrorCode(renderer,DIR_SOL);
	SDL_Texture *texture_chiffre = LoadTextureWithErrorCode(renderer,DIR_NUM);

	// init sound effect //
	Mix_Chunk *flap_wav = Mix_LoadWAV( DIR_FLAP_WAV);
	Mix_Chunk *hurt_wav = Mix_LoadWAV( DIR_HURT_WAV);
	Mix_Chunk *score_wav = Mix_LoadWAV( DIR_SCORE_WAV);

	if( !flap_wav || !hurt_wav || !score_wav)
		printf("Erreur chargement des sons\n");
	// end sound effect //


	// init struct environement //
	struct Environement environement[PRELOAD_POS_OBSTACLE];


	int animate_perso,animate_sol,end,traitement;
	long long score_hash;
	int score=0;



	init_environement(environement,upscale,&animate_perso,&animate_sol,&end,&traitement);
	// end init struct environement //

	// init personnage y //

	SDL_Point coordonner_perso = {WINDOW_L/2 - ( (DIM_PERSO.x*upscale)/2 ) , WINDOW_H/2};

	//									//


	long const_1, const_2, const_3, const_4;
	init_const_hash(&const_1, &const_2, &const_3, &const_4);
	score_hash = hashage(score, const_1, const_2, const_3, const_4);
	// end init

	int upper = 0;
	int gravity_speed = 0;
	int acceleration = 0;

	while(!end){

		SDL_Event ev;
		while ( SDL_PollEvent(&ev) )
		{
			if (ev.type == SDL_KEYDOWN){

					if(ev.key.keysym.sym == SDLK_SPACE)
					{

						Mix_PlayChannel( -1, flap_wav,0);
						printf("SOUND\n" );
						upper = UPPER_STEP;
						gravity_speed = 0;
					}
					else if (ev.key.keysym.sym == SDLK_ESCAPE)
					{
						SDL_Delay(5000);
					}

			}

		}

		if (upper>0){
			coordonner_perso.y -= UPPER_BY_STEP - upper;
			upper -= UPPER_SPEED;
		} else {
			acceleration = 0;
			coordonner_perso.y += gravity_speed += GRAVITY_SPEED;
		}


		view(renderer, coordonner_perso , upscale, environement, score ,animate_perso++ , animate_sol++  ,texture_background,
																																									 texture_pipes,
																																									 texture_birds,
																																									 texture_medals,
																																									 texture_scoreBoard,
																																									 texture_sol,
																																								 	 texture_chiffre);

		// traitement de mon environement
		traitement = traitement_environement(environement,upscale,traitement,&score,
																			 &score_hash,
																			 &const_1,
																			 &const_2,
																			 &const_3,
																			 &const_4,
																		 score_wav);

		traitement_variable(&animate_perso,&animate_sol);

		end = collision(environement,coordonner_perso,upscale,hurt_wav);

		SDL_Delay(FRAME_DELAY);

	}

	// animate end
	printf("coordonner_perso.y = %d ||| sol = %d ",coordonner_perso.y,WINDOW_H - DIM_SOL.y*upscale);
	while ( coordonner_perso.y < WINDOW_H - DIM_SOL.y*upscale )
	{
		coordonner_perso.y += gravity_speed++;
		view(renderer, coordonner_perso , upscale, environement, score ,0 , 0  ,texture_background,
																																									 texture_pipes,
																																									 texture_birds,
																																									 texture_medals,
																																									 texture_scoreBoard,
																																									 texture_sol,
																																								 	 texture_chiffre );

		SDL_Delay(FRAME_DELAY);
	}





	// liberer la memoire
	Mix_FreeChunk(flap_wav);
	Mix_FreeChunk(hurt_wav);
	Mix_FreeChunk(score_wav);

	SDL_DestroyTexture(texture_background);
	SDL_DestroyTexture(texture_pipes);
	SDL_DestroyTexture(texture_birds);
	SDL_DestroyTexture(texture_medals);
	SDL_DestroyTexture(texture_scoreBoard);
	SDL_DestroyTexture(texture_sol);
	SDL_DestroyTexture(texture_chiffre);

	return 0;
}




// launch debug
int main() {
	SDL_Init(SDL_INIT_EVERYTHING );
	if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 1, 1024 ) == -1 )
	{
		printf("Erreur init SDL_Mixer\n" );
	}




	// main du jeu
	SDL_Window* window = SDL_CreateWindow("Nineteen | Flappy Bird", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_L, WINDOW_H, SDL_WINDOW_SHOWN );
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	int upscale = WINDOW_H/DIM_BACKGROUND.y;
	flappy_bird(window, renderer,upscale);


	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

}
