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

// IMAGE TILSET FIND POSITION
const SDL_Rect BACKGROUND = {0, 0, 144, 256};
const SDL_Rect OBSTACLE_VERT = {52,0, 26, 160};
const SDL_Rect SOL = {0, 0, 168, 55};
const SDL_Rect CHIFFRE = {0, 0, 12, 18};
const SDL_Rect PERSO = {0, 0, 17, 12};
const SDL_Rect HIGH_SCORE = {0,0,512,512};
#define COMPENSATITION_HITBOX_DOWN 20

// PARAMETRE BUFFER OBSTACLE
#define PRELOAD_POS_OBSTACLE 8

// PARAMETRES POUR GESTION DES OBSTACLES
#define DISTANCE_BETWEEN_OBSTACLE 50 
#define DISTANCE_UNDER_OBSTACLE 80 // difficulte max en distance d'obstacle
#define NB_POS_OBSTACLE 5
#define TRANCHE_POS_OBSTACLE 550

// PERMET DE STOCKER LE BUFFER DES PILONNES DANS UNE STRUCTURE
typedef struct {
	int position; // POSITION DU PILONE SUR L'AXE DES X
	int hauteur; // POSITION DE LA PORTE SUR L'AXE DES Y
}pilonne;

// NOMBRE DE BOUCLE POUR UNE ANIMATION DE MONTER
// PLUS UPPER_STEP est elever plus le delay d'un animation sera long
// il est deconseiller de toucher au autre stat
const int UPPER_STEP = 4 * (FPS/30);
#define UPPER_BY_STEP 30 // VALEUR A AJOUTER A LAXE DES X DU PERSONNAGE
const int GRAVITY_SPEED = 2 * (FPS/30);// VALEUR DE LA GRAVITER 
const int VITESSE_DEPLACEMENT_DECOR = 8 / (FPS/30); // vitesse de deplacement des decors
#define ANGLE_UP -30
#define ANGLE_DOWN 90
#define ANGLE_VITESSE 8

#define BAN_CODE -666

// ADRESS TEXTURE
#define DIR_TILSET "Textures/tilset.png"
#define DIR_BIRDS "Textures/birds.png"
#define DIR_MEDALS "Textures/medals.png"
#define DIR_PIPES "Textures/pipes.png"
#define DIR_SCOREBOARD "Textures/scoreBoard.png"
#define DIR_BACKGROUND "Textures/backgrounds.png"
#define DIR_NUM "Textures/chiffre.png"
#define DIR_SOL "Textures/sol.png"
#define DIR_HIGHSCORE "Textures/high_score.png"

// ADRESS SOUND
#define DIR_FLAP_WAV "Sounds/flap.wav"
#define DIR_HURT_WAV "Sounds/hurt.wav"
#define DIR_SCORE_WAV "Sounds/score.wav"

//LOCAL ERROR CODE
#define IMAGE_ERROR_LOAD -101
#define SOUND_ERROR_LOAD -102
#define SDL_RENDER_ERROR_BACKGROUND -103
#define SDL_RENDER_ERROR_OBSTACLE -104
#define SDL_RENDER_ERROR_SOL -105
#define SDL_RENDER_ERROR_PERSONNAGE -106
#define SDL_RENDER_ERROR_PRINT_SCORE -107
#define SDL_RENDER_ERROR_PRINT_HS -108

// HASH FONCTION -- A METTRE DANS UN FICHIER GLOBALE
long long int hashage (int value, long const_1, long const_2, long const_3, long const_4 )
{

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

// INIT VARIABLE ALEATOIRE
void init_const_hash(long *const_1, long *const_2, long *const_3, long *const_4)
{
	*const_1 = rand();
	*const_2 = rand();
	*const_3 = rand();
	*const_4 = rand();

}

// VERIFIE SI LE SCORE NA PAS CHANGER
// RETOURNE VRAI SI RIEN N'A CHANGER
// GENERER DE NOUVELLE VALEUR ALEATOIRE
// HASH LE NOUVEAU SCORE AVEC C'EST VALEUR ALEATOIRE
int changeProtectedVar(long long int *score_hash,int *var, int updatedVar, long *const_1, long *const_2, long *const_3, long *const_4)
{
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

// CHARGER LES TEXTURES
SDL_Texture * LoadTextureWithErrorCode(SDL_Renderer *renderer ,char directory[])
{

	SDL_Texture* texture = IMG_LoadTexture(renderer,directory);

	if(!texture)
		printf("TEXTURE FAILURE : Erreur lors de la creation de %s SDL_ERROR : %s",directory,SDL_GetError());

	return texture;
}


// AFFICHAGE DU BACKGROUND
int background(SDL_Renderer *renderer, SDL_Texture* texture_background)
{

	SDL_Rect target = {0, 0, BACKGROUND.w*SCALE_TO_FIT, WINDOW_H};

	//On le dessine tant qu'on a pas rempli l'ecran
	while (target.x + BACKGROUND.w*SCALE_TO_FIT < WINDOW_L) {
		SDL_RenderCopy(renderer, texture_background, &BACKGROUND, &target);
		target.x += BACKGROUND.w*SCALE_TO_FIT;
	}
	return SDL_RenderCopy(renderer, texture_background, &BACKGROUND, &target);

}

// AFFICHAGE DES OBSTACLE/PILONNE
int obstacle(SDL_Renderer *renderer, SDL_Texture *texture_pipes, int position, int position_axe_x)
{

	/*SDL_Rect obstacle_h = {POS_OBSTACLE_H.x,POS_OBSTACLE_H.y,DIM_OBSTACLE.x,DIM_OBSTACLE.y};
	SDL_Rect obstacle_b = {POS_OBSTACLE_B.x,POS_OBSTACLE_B.y,DIM_OBSTACLE.x,DIM_OBSTACLE.y};*/
	SDL_Rect obstacle = OBSTACLE_VERT;
	obstacle.x += obstacle.w;

	SDL_Rect target = {position_axe_x,-TRANCHE_POS_OBSTACLE + (TRANCHE_POS_OBSTACLE/NB_POS_OBSTACLE)*position,OBSTACLE_VERT.w*SCALE_TO_FIT,OBSTACLE_VERT.h*SCALE_TO_FIT};

	SDL_RenderCopy(renderer,texture_pipes,&obstacle,&target);

	target.y += OBSTACLE_VERT.h*SCALE_TO_FIT + DISTANCE_BETWEEN_OBSTACLE*SCALE_TO_FIT;
	obstacle.x -= obstacle.w;
	return SDL_RenderCopy(renderer,texture_pipes,&obstacle,&target);


}

// AFFICHAGE DU SOL
int sol(SDL_Renderer *renderer, SDL_Texture *texture_sol,int target_x)
{
	int error_code = 0;
	SDL_Rect target = {target_x,WINDOW_H-(SOL.h*SCALE_TO_FIT),SOL.w*SCALE_TO_FIT,SOL.h*SCALE_TO_FIT};

	SDL_RenderCopy(renderer,texture_sol,&SOL,&target);

	while (target.x + SOL.w*SCALE_TO_FIT < WINDOW_L) {
		target.x +=  SOL.w*SCALE_TO_FIT;
	 	error_code = SDL_RenderCopy(renderer, texture_sol, &SOL, &target);
		if(error_code)
			return error_code;
	}
	return error_code;
}

int personnage(SDL_Renderer *renderer,SDL_Texture *texture_birds, SDL_Point coordonner_perso, int animate_perso, double angle)
{

	SDL_Rect pos_print = PERSO;

	pos_print.x += (animate_perso-1) * pos_print.w;

	


	//SDL_Rect personnage = {pos_print.x,pos_print.y,DIM_PERSO.x,DIM_PERSO.y};

	SDL_Rect target = {coordonner_perso.x - ( PERSO.w*SCALE_TO_FIT ) / 2, coordonner_perso.y - ( PERSO.h *SCALE_TO_FIT ) / 2,
		 								PERSO.w*SCALE_TO_FIT , PERSO.h*SCALE_TO_FIT};

	SDL_Point centre = { (PERSO.w*SCALE_TO_FIT) /2, (PERSO.h*SCALE_TO_FIT)/2 };

	return SDL_RenderCopyEx(renderer,texture_birds,&pos_print,&target, angle , &centre,0);
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



int print_score(SDL_Renderer *renderer , SDL_Texture *texture_chiffre,int score)
{
	int taille = strlen_num(score);
	int error_code = 0;

	SDL_Rect chiffre = CHIFFRE;
	SDL_Rect target = {WINDOW_L/2 - CHIFFRE.w*SCALE_TO_FIT ,WINDOW_H/6  , CHIFFRE.w * SCALE_TO_FIT, CHIFFRE.h * SCALE_TO_FIT};

	for(int i=0 ; i < taille; i++)
	{
		chiffre.x = CHIFFRE.w * (score%10);
		target.x -= CHIFFRE.w * SCALE_TO_FIT;
		error_code = SDL_RenderCopy(renderer,texture_chiffre,&chiffre,&target);
		if (error_code)
			return error_code;
		
		score /=10;
	}
	return error_code;

}

int print_high_score(SDL_Renderer* renderer,SDL_Texture *texture_highscore, int score, int high_score, int cible)
{
	if(score + PRELOAD_POS_OBSTACLE >= high_score )
	{
		SDL_Rect target = {cible,WINDOW_H*0.81,50*SCALE_TO_FIT,50*SCALE_TO_FIT};
		target.x -= target.h/SCALE_TO_FIT;
		return SDL_RenderCopy(renderer,texture_highscore,&HIGH_SCORE,&target);
	}
	return 0;
}

// regroupe toute les fonctions d'affichage
int view(SDL_Renderer * renderer, SDL_Point coordonner_perso, pilonne *pilonne ,int score,int animate_perso,int animate_sol, int cible, double angle ,SDL_Texture *texture_background, SDL_Texture *texture_pipes, SDL_Texture *texture_birds, SDL_Texture *texture_medals, SDL_Texture *texture_scoreBoard, SDL_Texture *texture_sol, SDL_Texture *texture_chiffre, SDL_Texture* texture_highscore)
{
	

	SDL_RenderClear(renderer);


	if ( background(renderer,texture_background) )
		return SDL_RENDER_ERROR_BACKGROUND;

	// cas particuler charger X obstacle
	for (int i = 0; i < PRELOAD_POS_OBSTACLE; i++) {
		if ( obstacle(renderer,texture_pipes,pilonne[i].hauteur,pilonne[i].position) )
			return SDL_RENDER_ERROR_OBSTACLE;
		
	}


	if ( sol(renderer,texture_sol,animate_sol) )
		return SDL_RENDER_ERROR_SOL;
	if ( personnage(renderer , texture_birds, coordonner_perso  , animate_perso , angle) )
		return SDL_RENDER_ERROR_PERSONNAGE;
	if ( print_score(renderer, texture_chiffre, score) )
		return SDL_RENDER_ERROR_PRINT_SCORE;
	if( print_high_score(renderer,texture_highscore,score,6,cible ) )
		return SDL_RENDER_ERROR_PRINT_HS;

	//On affiche

	SDL_RenderPresent(renderer);
	return 0;

}

void init_pilonne(pilonne *pilonne, int *animate_perso, int *animate_sol, int *end, int* dead, int *traitement)
{


	for(int i=0; i<PRELOAD_POS_OBSTACLE;i++){
		pilonne[i].hauteur = rand() % NB_POS_OBSTACLE ;
		if (i)
			pilonne[i].position = DISTANCE_UNDER_OBSTACLE*SCALE_TO_FIT + pilonne[i-1].position;
		else
			pilonne[i].position = WINDOW_L;
	}

	*animate_perso = 0;
	*animate_sol = 0;
	*end = 0;
	*dead = 0;
	*traitement = -1;
}

int traitement_pilonne(pilonne *pilonne, int traitement, int *score, long long *score_hash, long *const_1, long *const_2, long *const_3, long *const_4, Mix_Chunk *score_wav)
{

	if(traitement == -1){
		if(pilonne[0].position + OBSTACLE_VERT.w*SCALE_TO_FIT + PERSO.w*SCALE_TO_FIT < WINDOW_L/2  ){
			traitement = 0;

			if(changeProtectedVar(score_hash, score, (*score)+1, const_1, const_2, const_3, const_4))
				Mix_PlayChannel( -1, score_wav,0);
			/*else
				ban commands*/
		}

	} else if(traitement == 0){
		if(pilonne[1].position + OBSTACLE_VERT.w*SCALE_TO_FIT + PERSO.w*SCALE_TO_FIT < WINDOW_L/2 ){
			traitement = 1;


			if(changeProtectedVar(score_hash, score, (*score)+1, const_1, const_2, const_3, const_4))
				Mix_PlayChannel( -1, score_wav,0);
			/*else
				ban commands*/

		}
	} else if ( traitement == 1){
		if(pilonne[2].position + OBSTACLE_VERT.w*SCALE_TO_FIT + PERSO.w*SCALE_TO_FIT < WINDOW_L/2 ){
			traitement = 2;
			if(changeProtectedVar(score_hash, score, (*score)+1, const_1, const_2, const_3, const_4))
				Mix_PlayChannel( -1, score_wav,0);
			/*else
				ban commands*/
		}

	} else if ( traitement == 2){
		if(pilonne[3].position + OBSTACLE_VERT.w*SCALE_TO_FIT + PERSO.w*SCALE_TO_FIT < WINDOW_L/2 ){
			traitement = 3;
			if(changeProtectedVar(score_hash, score, (*score)+1, const_1, const_2, const_3, const_4))
				Mix_PlayChannel( -1, score_wav,0);
			/*else
				ban commands*/
		}

	} 
	else if (traitement == 3 ){
		
		if(  !(pilonne[3].position + OBSTACLE_VERT.w*SCALE_TO_FIT + PERSO.w*SCALE_TO_FIT < WINDOW_L/2)  )
		{
			traitement = 2;
		}
			


	}


	for(int i=0; i<PRELOAD_POS_OBSTACLE;i++)

		pilonne[i].position -= VITESSE_DEPLACEMENT_DECOR;

	//condition pour supprimer le dernier pilonne
	// il faut qu'on est franchis le troisieme pillonne et qu'il est disparu de la vue
	if( (pilonne[0].position + OBSTACLE_VERT.w*SCALE_TO_FIT < -( DISTANCE_UNDER_OBSTACLE*3 ) - OBSTACLE_VERT.w*SCALE_TO_FIT - PERSO.w*SCALE_TO_FIT) && pilonne[0].position + OBSTACLE_VERT.w*SCALE_TO_FIT < 0 ){
		for(int i = 1 ; i < PRELOAD_POS_OBSTACLE ; i++){
			pilonne[i-1] = pilonne[i];
		}

		pilonne[PRELOAD_POS_OBSTACLE-1].hauteur = rand() % NB_POS_OBSTACLE ;
		pilonne[PRELOAD_POS_OBSTACLE-1].position = pilonne[PRELOAD_POS_OBSTACLE-2].position + DISTANCE_UNDER_OBSTACLE*SCALE_TO_FIT;

	}

	return traitement;
}

int collision(pilonne *pilonne, SDL_Point coordonner_perso, Mix_Chunk *hurt_wav)
{
	int collision_detecter = 0;

	// permet de dire quand on sur l'axe d'un obstacle et de qu'elle obstacle
	// + detection collision
	for (int i = 0; i < PRELOAD_POS_OBSTACLE; i++) {
		// verifier qu'il ce trouve actuellement sur l'axe de deux bloque
		if( pilonne[i].position + PERSO.w*SCALE_TO_FIT > (  WINDOW_L/2 - ( (OBSTACLE_VERT.w*SCALE_TO_FIT) ) ) && pilonne[i].position + PERSO.w*SCALE_TO_FIT < (  WINDOW_L/2 + ( (OBSTACLE_VERT.w*SCALE_TO_FIT)/2 ) )){
			int bas =( (  -TRANCHE_POS_OBSTACLE + (TRANCHE_POS_OBSTACLE/NB_POS_OBSTACLE)*pilonne[i].hauteur ) + OBSTACLE_VERT.h*SCALE_TO_FIT ) + PERSO.h*SCALE_TO_FIT/2;
			int haut = (  -TRANCHE_POS_OBSTACLE + (TRANCHE_POS_OBSTACLE/NB_POS_OBSTACLE)*pilonne[i].hauteur ) + OBSTACLE_VERT.h*SCALE_TO_FIT + DISTANCE_BETWEEN_OBSTACLE*SCALE_TO_FIT - PERSO.h*SCALE_TO_FIT + COMPENSATITION_HITBOX_DOWN;

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
	if(*animate_perso > 3 )
		*animate_perso = 1;

	if( ( *animate_sol * -1) >= ( SOL.w*SCALE_TO_FIT)*3  )
		*animate_sol = 0;
	else 
		*animate_sol -= VITESSE_DEPLACEMENT_DECOR;
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

void evenement(int *upper,int *gravity_speed, int *nb_boucle,Mix_Chunk *flap_wav)
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
				*nb_boucle = 0;
			}
			else if (ev.key.keysym.sym == SDLK_ESCAPE)
			{
				SDL_Delay(5000);
			}

		}

	}
}

void maj_var_environement(SDL_Point *coordonner_perso, int *upper, double *angle, int *nb_boucle, int* gravity_speed)
{
	if ( *upper > 0 ){
		coordonner_perso->y -= ( UPPER_BY_STEP - ( *upper ) )/ (FPS/30);
		*upper -= 1;
		if( *angle > ANGLE_UP)
		{
			
			*angle -= ANGLE_VITESSE*2;
		}
		} else if( *nb_boucle% (FPS/30) == 0 ){
			coordonner_perso->y += ( *gravity_speed += GRAVITY_SPEED ) / (FPS/30);
			if ( *angle < ANGLE_DOWN)
				*angle += ANGLE_VITESSE/2;
		}
		*nb_boucle += 1;
}
				


// primary func
int flappy_bird(SDL_Window *window, SDL_Renderer *renderer, int highscore)
{
	int exit_code = 0;
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
	SDL_Texture *texture_highscore = LoadTextureWithErrorCode(renderer,DIR_HIGHSCORE);

	if( !texture_background || !texture_pipes || !texture_birds || !texture_medals || !texture_scoreBoard || !texture_sol || !texture_chiffre || !texture_highscore)
		exit_code = IMAGE_ERROR_LOAD;
	

	// init sound effect //
	Mix_Chunk *flap_wav = Mix_LoadWAV( DIR_FLAP_WAV);
	Mix_Chunk *hurt_wav = Mix_LoadWAV( DIR_HURT_WAV);
	Mix_Chunk *score_wav = Mix_LoadWAV( DIR_SCORE_WAV);


	if( !flap_wav || !hurt_wav || !score_wav)
		exit_code = SOUND_ERROR_LOAD;
	// end sound effect //

	if (!exit_code)
	{
		//init pilone
		pilonne pilonne[PRELOAD_POS_OBSTACLE];

		// init variable
		int animate_perso,animate_sol,end,dead,traitement;
		long long score_hash;
		int score=0;
		init_pilonne(pilonne,&animate_perso,&animate_sol,&end,&dead,&traitement);
		int cible = (highscore-1)*DISTANCE_UNDER_OBSTACLE*SCALE_TO_FIT + pilonne[0].position;
		double angle  = 0;
		// end init pilonne //

		// init personnage y //

		SDL_Point coordonner_perso = {WINDOW_L/2 - ( (PERSO.w*SCALE_TO_FIT)/2 ) , WINDOW_H/2};

		//init var to hash score
		long const_1, const_2, const_3, const_4;
		init_const_hash(&const_1, &const_2, &const_3, &const_4);
		score_hash = hashage(score, const_1, const_2, const_3, const_4);

		// initialisation des variables de suivi est d'animation
		int upper = 0;
		int nb_boucle = 0;
		int gravity_speed = 0;

		// preaffichage avant lancement
		// afficher la vue //
		exit_code = view(renderer, coordonner_perso , pilonne, score ,1 , 0, cible, angle ,texture_background, 
																		   				   texture_pipes, 
																		   				   texture_birds, 
																		  				   texture_medals,  
																		  				   texture_scoreBoard, 
																		  				   texture_sol, 
																		  				   texture_chiffre,
																		  				   texture_highscore);

		// attendre la touch espace //
		wait_before_start(flap_wav);
		// faire un premier saut //
		upper = UPPER_STEP;

		while(!end && !exit_code){

				int times_at_start_frame = SDL_GetTicks();
				//si pas dans l'anim de mort
				if(!dead){

					evenement(&upper,&gravity_speed,&nb_boucle,flap_wav);

					maj_var_environement(&coordonner_perso, &upper, &angle, &nb_boucle,&gravity_speed);
		


				exit_code = view(renderer, coordonner_perso , pilonne, score ,animate_perso++ , animate_sol, cible, angle ,texture_background,
																														   texture_pipes, 
																														   texture_birds, 
																														   texture_medals,  
																														   texture_scoreBoard, 
																													       texture_sol, 
																														   texture_chiffre,
																														   texture_highscore);

				// traitement de mon pilonne
				traitement = traitement_pilonne(pilonne,traitement,&score,  &score_hash,  &const_1,  
																											&const_2, 
																											&const_3,  
																											&const_4, score_wav);

				traitement_variable(&animate_perso,&animate_sol);
				cible -= VITESSE_DEPLACEMENT_DECOR;

				dead = collision(pilonne,coordonner_perso,hurt_wav);

			}
			else
			{ //animate death
				if (angle < ANGLE_DOWN)
							angle += ANGLE_VITESSE*2 / (FPS/30);
				coordonner_perso.y += (gravity_speed++) / (FPS/30);
				exit_code = view(renderer, coordonner_perso , pilonne, score ,1 , 0 , cible , angle, texture_background, 
																								     texture_pipes, 
																									 texture_birds,  
																									 texture_medals, 
																									 texture_scoreBoard, 
																									 texture_sol,  
																									 texture_chiffre,
																									 texture_highscore );

				if(coordonner_perso.y >= WINDOW_H - SOL.h*SCALE_TO_FIT)
					end = SDL_TRUE;
			}

			//next frame
			int frame_delay = SDL_GetTicks() - times_at_start_frame;
			if(frame_delay < FRAME_TIME)
				SDL_Delay(FRAME_TIME - frame_delay );


		}

	}
	SDL_Delay(1000);

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
	SDL_DestroyTexture(texture_highscore);


	return exit_code;
}




// launch debug
int main() {
	SDL_Init(SDL_INIT_EVERYTHING );
	if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 1, 1024 ) == -1 )
	{
		printf("Erreur init SDL_Mixer\n" );
	}

	// main du jeu
	SDL_Window* window = SDL_CreateWindow("Nineteen | Flappy Bird", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_SEND_L, WINDOW_SEND_H, SDL_WINDOW_SHOWN  );
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
	SDL_ShowCursor(SDL_DISABLE);

	printf("Code d'erreur de sorti : %d", flappy_bird(window, renderer, 5) );

	SDL_ShowCursor(SDL_ENABLE);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	

	return EXIT_SUCCESS;
}
