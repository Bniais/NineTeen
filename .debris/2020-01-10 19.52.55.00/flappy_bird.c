#include <stdio.h>
#include "define.h"
#include <time.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

//native resolution
#define WINDOW_L 1820.
#define WINDOW_H 1024.

#define FPS 30
const int FRAME_TIME = 1000/FPS;

// FOR DEBUG ONLY
// definition envoyer depuis la room pour le rescale
#define WINDOW_SEND_L 1920.
#define WINDOW_SEND_H 1080.

// scale ratio for native resolution
#define SCALE_TO_FIT 4 // WIINDOW_H/BACKGROUND.h *4


//CONST Dimensions du poto dans l'image
/*const SDL_Point DIM_BACKGROUND = {144,256};
const SDL_Point POS_BACKGROUND = {0,0};*/
const SDL_Rect BACKGROUND = {0, 0, 144, 256};

/*const SDL_Point DIM_OBSTACLE = {26,160};
const SDL_Point POS_OBSTACLE_H = {78,0};
const SDL_Point POS_OBSTACLE_B = {52,0};*/
const SDL_Rect OBSTACLE_VERT = {52,0, 26, 160};

/*const SDL_Point DIM_SOL = {168,55};
const SDL_Point POS_SOL = {0,0};*/
const SDL_Rect SOL = {0, 0, 168, 55};

/*const SDL_Point DIM_PERSO = {17,12};
const SDL_Point POS_PERSO_1 = {0,0};
const SDL_Point POS_PERSO_2 = {17,0};
const SDL_Point POS_PERSO_3 = {34,0};*/
const SDL_Rect PERSO = {0, 0, 17, 12};
#define COMPENSATITION_HITBOX_DOWN 20

/*const SDL_Point DIM_CHIFFRE = {12,18};
const SDL_Point POS_CHIFFRE = {12,0};*/
const SDL_Rect CHIFFRE = {0, 0, 12, 18};



// COLOR //
const SDL_Color WHITE = {255,255,255};
const SDL_Color BLACK = {0,0,0};

// delay frame


//?


#define DISTANCE_BETWEEN_OBSTACLE 50
#define DISTANCE_UNDER_OBSTACLE 100 
#define NB_POS_OBSTACLE 6
#define TRANCHE_POS_OBSTACLE 550



#define PRELOAD_POS_OBSTACLE 5
// FOR 30 FPS SETTINGS

const int UPPER_STEP = 4 * (FPS/30);
#define UPPER_SPEED 1
#define UPPER_BY_STEP 30
#define GRAVITY_SPEED 1
const int RATIO_ANIM_SOL = 20 * (FPS/30); 


#define BAN_CODE -666

//?
typedef struct s_Environement {
	int position;
	int hauteur;
}Environement;


#define NB_ANIM_CHAR 3.
#define TIME_TOTAL_ANIM 1000



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

long long int hashage (int value, long const_1, long const_2, long const_3, long const_4 ){

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

int changeProtectedVar(long long int *score_hash,int *var, int updatedVar, long *const_1, long *const_2, long *const_3, long *const_4){
	if( *score_hash == hashage(*var, *const_1, *const_2, *const_3, *const_4)){
		*const_1 = rand();
		*const_2 = rand();
		*const_3 = rand();
		*const_4 = rand();

		*score_hash = updatedVar * *const_1;
		*score_hash = *score_hash - *const_2;
		*score_hash = *score_hash * *const_3;
		*score_hash = *score_hash / *const_4;

		*var = updatedVar;
		return 1;
	}
	else{
		// BAN
		printf("Baned\n");
		return 0;
	}
}


SDL_Texture * LoadTextureWithErrorCode(SDL_Renderer *renderer ,char directory[]){

	SDL_Texture* texture = IMG_LoadTexture(renderer,directory);

	if(!texture)
		printf("TEXTURE FAILURE : Erreur lors de la creation de %s SDL_ERROR : %s",directory,SDL_GetError());

	return texture;
}


// permet de genrer l'affichage du bakcground en plusieur fois selon
// la dimension de l'ecran de sorti
void background(SDL_Renderer *renderer, SDL_Texture* texture_background)
{

	SDL_Rect target = {0, 0, BACKGROUND.w*SCALE_TO_FIT, WINDOW_H};

	//On le dessine tant qu'on a pas rempli l'ecran
	while (target.x + BACKGROUND.w*SCALE_TO_FIT < WINDOW_L) {
		SDL_RenderCopy(renderer, texture_background, &BACKGROUND, &target);
		target.x += BACKGROUND.w*SCALE_TO_FIT;
	}
	SDL_RenderCopy(renderer, texture_background, &BACKGROUND, &target);

}

// affichage d'un obstacle
void obstacle(SDL_Renderer *renderer, SDL_Texture *texture_pipes, int position, int position_axe_x)
{

	/*SDL_Rect obstacle_h = {POS_OBSTACLE_H.x,POS_OBSTACLE_H.y,DIM_OBSTACLE.x,DIM_OBSTACLE.y};
	SDL_Rect obstacle_b = {POS_OBSTACLE_B.x,POS_OBSTACLE_B.y,DIM_OBSTACLE.x,DIM_OBSTACLE.y};*/
	SDL_Rect obstacle = OBSTACLE_VERT;
	obstacle.x += obstacle.w;

	SDL_Rect target = {position_axe_x,-TRANCHE_POS_OBSTACLE + (TRANCHE_POS_OBSTACLE/NB_POS_OBSTACLE)*position,OBSTACLE_VERT.w*SCALE_TO_FIT,OBSTACLE_VERT.h*SCALE_TO_FIT};

	SDL_RenderCopy(renderer,texture_pipes,&obstacle,&target);

	target.y += OBSTACLE_VERT.h*SCALE_TO_FIT + DISTANCE_BETWEEN_OBSTACLE*SCALE_TO_FIT;
	obstacle.x -= obstacle.w;
	SDL_RenderCopy(renderer,texture_pipes,&obstacle,&target);


}

// affichage du sol
void sol(SDL_Renderer *renderer, SDL_Texture *texture_sol,int animate_sol)
{
	// ratio sol pour avoir 100 position de sol//
	float ratio = SOL.w/RATIO_ANIM_SOL;

	animate_sol *= ratio;

	SDL_Rect target = {-animate_sol,WINDOW_H-(SOL.h*SCALE_TO_FIT),SOL.w*SCALE_TO_FIT,SOL.h*SCALE_TO_FIT};

	SDL_RenderCopy(renderer,texture_sol,&SOL,&target);

	while (target.x + SOL.w*SCALE_TO_FIT < WINDOW_L) {
		target.x +=  SOL.w*SCALE_TO_FIT;
	  SDL_RenderCopy(renderer, texture_sol, &SOL, &target);
	}
}

void personnage(SDL_Renderer *renderer,SDL_Texture *texture_birds, SDL_Point coordonner_perso, int animate_perso){

	SDL_Rect pos_print = PERSO;

	/*switch(animate_perso){
		case 1 :{
				pos_print = POS_PERSO_1;
		};break;
		case 2 :{
				pos_print = POS_PERSO_2;
		} ;break;
		case 3 :{
				pos_print = POS_PERSO_3;
		} ;break;
	}*/
	pos_print.x += (animate_perso - 1) * pos_print.w;


	//SDL_Rect personnage = {pos_print.x,pos_print.y,DIM_PERSO.x,DIM_PERSO.y};

	SDL_Rect target = {coordonner_perso.x - ( PERSO.w*SCALE_TO_FIT ) / 2, coordonner_perso.y - ( PERSO.h *SCALE_TO_FIT ) / 2,
		 								PERSO.w*SCALE_TO_FIT , PERSO.h*SCALE_TO_FIT};


	SDL_RenderCopy(renderer,texture_birds,&pos_print,&target);
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



void print_score(SDL_Renderer *renderer , SDL_Texture *texture_chiffre,int score)
{
	int taille = strlen_num(score);

	SDL_Rect chiffre = CHIFFRE;
	SDL_Rect target = {WINDOW_L/2 - CHIFFRE.w*SCALE_TO_FIT ,WINDOW_H/6  , CHIFFRE.w * SCALE_TO_FIT, CHIFFRE.h * SCALE_TO_FIT};

	for(int i=0 ; i < taille; i++)
	{
		chiffre.x = CHIFFRE.w * (score%10);
		target.x -= CHIFFRE.w * SCALE_TO_FIT;
		SDL_RenderCopy(renderer,texture_chiffre,&chiffre,&target);

		score /=10;
	}

}



// regroupe toute les fonctions d'affichage
void view(SDL_Renderer * renderer, SDL_Point coordonner_perso, Environement *environement ,int score,int animate_perso,int animate_sol,SDL_Texture *texture_background, SDL_Texture *texture_pipes, SDL_Texture *texture_birds, SDL_Texture *texture_medals, SDL_Texture *texture_scoreBoard, SDL_Texture *texture_sol, SDL_Texture *texture_chiffre)
{

	SDL_RenderClear(renderer);


	background(renderer,texture_background);

	// cas particuler charger X obstacle
	for (int i = 0; i < PRELOAD_POS_OBSTACLE; i++) {
		obstacle(renderer,texture_pipes,environement[i].hauteur,environement[i].position);
	}


	sol(renderer,texture_sol,animate_sol);
	personnage(renderer , texture_birds, coordonner_perso  , animate_perso );
	print_score(renderer, texture_chiffre, score);

	//On affiche
	SDL_RenderPresent(renderer);

}

void init_environement(Environement *environement, int *animate_perso, int *animate_sol, int *end, int* dead, int *traitement)
{


	for(int i=0; i<PRELOAD_POS_OBSTACLE;i++){
		environement[i].hauteur = rand() % NB_POS_OBSTACLE ;
		if (i)
			environement[i].position = DISTANCE_UNDER_OBSTACLE*SCALE_TO_FIT + environement[i-1].position;
		else
			environement[i].position = WINDOW_L;
	}

	*animate_perso = 0;
	*animate_sol = 100;
	*end = 0;
	*dead = 0;
	*traitement = -1;
}

int traitement_environement(Environement *environement, int traitement, int *score, long long *score_hash, long *const_1, long *const_2, long *const_3, long *const_4, Mix_Chunk *score_wav)
{

	if(traitement == -1){
		if(environement[0].position + OBSTACLE_VERT.w*SCALE_TO_FIT + PERSO.w*SCALE_TO_FIT < WINDOW_L/2  ){
			traitement = 0;
			if(changeProtectedVar(score_hash, score, (*score)+1, const_1, const_2, const_3, const_4))
				Mix_PlayChannel( -1, score_wav,0);
			/*else
				ban commands*/
		}

	} else if(traitement == 0){
		if(environement[1].position + OBSTACLE_VERT.w*SCALE_TO_FIT + PERSO.w*SCALE_TO_FIT < WINDOW_L/2 ){
			traitement = 1;


			if(changeProtectedVar(score_hash, score, (*score)+1, const_1, const_2, const_3, const_4))
				Mix_PlayChannel( -1, score_wav,0);
			/*else
				ban commands*/

		}
	} else if (traitement == 2 || traitement == 1){
		if(environement[2].position + OBSTACLE_VERT.w*SCALE_TO_FIT + PERSO.w*SCALE_TO_FIT < WINDOW_L/2 ){
			traitement = 3;
			if(changeProtectedVar(score_hash, score, (*score)+1, const_1, const_2, const_3, const_4))
				Mix_PlayChannel( -1, score_wav,0);
			/*else
				ban commands*/
		}

	} else if (traitement == 3){
		if(  !(environement[2].position + OBSTACLE_VERT.w*SCALE_TO_FIT + PERSO.w*SCALE_TO_FIT < WINDOW_L/2)  )
			traitement = 2;

	}


	for(int i=0; i<PRELOAD_POS_OBSTACLE;i++)

		environement[i].position -= SOL.w/RATIO_ANIM_SOL;

	if( environement[0].position + OBSTACLE_VERT.w*SCALE_TO_FIT < 0){
		//printf("Supprimer env 0\n" );
		for(int i = 1 ; i < PRELOAD_POS_OBSTACLE ; i++){
			environement[i-1] = environement[i];
		}

		environement[PRELOAD_POS_OBSTACLE-1].hauteur = rand() % NB_POS_OBSTACLE ;
		environement[PRELOAD_POS_OBSTACLE-1].position = environement[PRELOAD_POS_OBSTACLE-2].position + DISTANCE_UNDER_OBSTACLE*SCALE_TO_FIT;

	}

	return traitement;
}

int collision(Environement *environement, SDL_Point coordonner_perso, Mix_Chunk *hurt_wav)
{
	int collision_detecter = 0;

	// permet de dire quand on sur l'axe d'un obstacle et de qu'elle obstacle
	// + detection collision
	for (int i = 0; i < PRELOAD_POS_OBSTACLE; i++) {
		// verifier qu'il ce trouve actuellement sur l'axe de deux bloque
		if( environement[i].position + PERSO.w*SCALE_TO_FIT > (  WINDOW_L/2 - ( (OBSTACLE_VERT.w*SCALE_TO_FIT) ) ) && environement[i].position + PERSO.w*SCALE_TO_FIT < (  WINDOW_L/2 + ( (OBSTACLE_VERT.w*SCALE_TO_FIT)/2 ) )){
			int bas =( (  -TRANCHE_POS_OBSTACLE + (TRANCHE_POS_OBSTACLE/NB_POS_OBSTACLE)*environement[i].hauteur ) + OBSTACLE_VERT.h*SCALE_TO_FIT ) + PERSO.h*SCALE_TO_FIT/2;
			int haut = (  -TRANCHE_POS_OBSTACLE + (TRANCHE_POS_OBSTACLE/NB_POS_OBSTACLE)*environement[i].hauteur ) + OBSTACLE_VERT.h*SCALE_TO_FIT + DISTANCE_BETWEEN_OBSTACLE*SCALE_TO_FIT - PERSO.h*SCALE_TO_FIT + COMPENSATITION_HITBOX_DOWN;

			if(! (coordonner_perso.y > bas && coordonner_perso.y < haut)  )
				collision_detecter = 1;

		}

	}

	// detection du sol
	if(coordonner_perso.y + PERSO.h*SCALE_TO_FIT > WINDOW_H - SOL.h*SCALE_TO_FIT){
		collision_detecter = 1;
	}

	// jouer un son
	if (collision_detecter)
		Mix_PlayChannel( -1, hurt_wav,0);

	return collision_detecter;
}

void traitement_variable(int *animate_perso,int *animate_sol)
{
	if(*animate_perso > 3)
		*animate_perso = 1;

	if(*animate_sol > 100)
			*animate_sol = 0;
}

void wait_before_start(Mix_Chunk *flap_wav)
{
	// attendre le toucher de la barre espace pour demarrer
	int attente = SDL_TRUE;
	
	while ( attente )
	{
		SDL_Event spacebar;

		while( SDL_PollEvent(&spacebar) )
		{
			if(spacebar.key.keysym.sym == SDLK_SPACE)
					attente = SDL_FALSE;
		}
	}
	
	// jouer le son de saut //
	Mix_PlayChannel( -1, flap_wav,0);

}

void evenement(int *upper,int *gravity_speed, int *passage_gravity,Mix_Chunk *flap_wav)
{
	SDL_Event ev;
	while ( SDL_PollEvent(&ev) )
	{
		if (ev.type == SDL_KEYDOWN){

			if(ev.key.keysym.sym == SDLK_SPACE)
			{
				Mix_PlayChannel( -1, flap_wav,0);
				*upper = UPPER_STEP;
				*gravity_speed = 0;
				*passage_gravity = 0;
			}
			else if (ev.key.keysym.sym == SDLK_ESCAPE)
			{
				SDL_Delay(5000);
			}

		}

	}
}

// primary func
int flappy_bird(SDL_Window *window, SDL_Renderer *renderer)
{

	srand(time(NULL));

	// rescale view
	SDL_RenderSetScale(renderer,
                       WINDOW_SEND_L/WINDOW_L,
                       WINDOW_SEND_H/WINDOW_H);

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


	// init Environement //
	Environement environement[PRELOAD_POS_OBSTACLE];


	int animate_perso,animate_sol,end,dead,traitement;
	long long score_hash;
	int score=0;



	init_environement(environement,&animate_perso,&animate_sol,&end,&dead,&traitement);
	// end init Environement //

	// init personnage y //

	SDL_Point coordonner_perso = {WINDOW_L/2 - ( (PERSO.w*SCALE_TO_FIT)/2 ) , WINDOW_H/2};

	//									//


	long const_1, const_2, const_3, const_4;
	init_const_hash(&const_1, &const_2, &const_3, &const_4);
	score_hash = hashage(score, const_1, const_2, const_3, const_4);
	// end init

	int upper = 0;
	int gravity_speed = 0;


	// start game //

	// afficher la vue //
	view(renderer, coordonner_perso , environement, score ,1 , 0 ,texture_background, 
																		   texture_pipes, 
																		   texture_birds, 
																		   texture_medals,  
																		   texture_scoreBoard, 
																		   texture_sol, 
																		   texture_chiffre);

	// attendre la touch espace //
	wait_before_start(flap_wav);

	// faire un premier saut //
	upper = UPPER_STEP;

	int passage_gravity = 0;

	while(!end){

			int times_at_start_frame = SDL_GetTicks();
			//si pas dans l'anim de mort
			if(!dead){

				evenement(&upper,&gravity_speed,&passage_gravity,flap_wav);
			
				if (upper>0){
					coordonner_perso.y -= ( UPPER_BY_STEP - (upper -= UPPER_SPEED) ) / (FPS/30);
				} else if( passage_gravity% (FPS/30) == 0 ){
					printf("Gravity = %d \n",( gravity_speed += GRAVITY_SPEED ) / (FPS/30));
					coordonner_perso.y += ( gravity_speed += GRAVITY_SPEED ) / (FPS/30);
					passage_gravity++;
				}


			view(renderer, coordonner_perso , environement, score ,animate_perso++ , animate_sol++  ,texture_background,
																											  texture_pipes, 
																											  texture_birds, 
																											  texture_medals,  
																											  texture_scoreBoard, 
																											  texture_sol, 
																											  texture_chiffre);

			// traitement de mon environement
			traitement = traitement_environement(environement,traitement,&score,  &score_hash,  &const_1,  
																										&const_2, 
																										&const_3,  
																										&const_4, score_wav);

			traitement_variable(&animate_perso,&animate_sol);

			dead = collision(environement,coordonner_perso,hurt_wav);

		}
		else
		{ //animate death
			coordonner_perso.y += gravity_speed++;
			view(renderer, coordonner_perso , environement, score ,1 , 0  ,texture_background, 
																					texture_pipes, 
																					texture_birds,  
																					texture_medals, 
																					texture_scoreBoard, 
																					texture_sol,  
																					texture_chiffre );

			if(coordonner_perso.y >= WINDOW_H - SOL.h*SCALE_TO_FIT)
				end = SDL_TRUE;
		}

		//next frame
		int times_at_end_frame = SDL_GetTicks();
		//printf("%d\n",times_at_end_frame - times_at_start_frame);
		SDL_Delay(FRAME_TIME - (times_at_end_frame - times_at_start_frame) );

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
	SDL_Window* window = SDL_CreateWindow("Nineteen | Flappy Bird", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_SEND_L, WINDOW_SEND_H, SDL_WINDOW_SHOWN );
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	flappy_bird(window, renderer);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}
