#include <stdio.h>
#include "define.h"
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

//native resolution
#define WINDOW_L 1820.
#define WINDOW_H 1024.

#define FPS 60
const int FRAME_TIME = 1000/FPS;

// FOR DEBUG ONLY
// definition envoyer depuis la room pour le rescale
#define WINDOW_SEND_L 720.
#define WINDOW_SEND_H 480.

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
#define TRANCHE_POS_OBSTACLE 550 // largeur sur Y des positions possible des portes de passages des pilonnes

// PERMET DE STOCKER LE BUFFER DES PILONNES DANS UNE STRUCTURE
typedef struct {
	int position; // POSITION DU PILONE SUR L'AXE DES X
	int hauteur; // POSITION DE LA PORTE SUR L'AXE DES Y
}pilonne;

// NOMBRE DE BOUCLE POUR UNE ANIMATION DE MONTER
// PLUS UPPER_STEP est elever plus le delay d'un animation sera long
// il est deconseiller de toucher au autre stat
const int UPPER_STEP = 4 * (FPS/30);
#define UPPER_BY_STEP 30 // VALEUR A AJOUTER A LAXE DES X DU AfficherPersonnage
const int GRAVITY_SPEED = 2 * (FPS/30);// VALEUR DE LA GRAVITER
const int VITESSE_DEPLACEMENT_DECOR = 8 / (FPS/30); // vitesse de deplacement des decors
#define ANGLE_UP -30
#define ANGLE_DOWN 90
#define ANGLE_VITESSE 8

#define BAN_CODE -666

// ADRESS TEXTURE
#define DIR_TILSET "../../games/3_flappy_bird/Textures/tilset.png"
#define DIR_BIRDS "../../games/3_flappy_bird/Textures/birds.png"
#define DIR_MEDALS "../../games/3_flappy_bird/Textures/medals.png"
#define DIR_PIPES "../../games/3_flappy_bird/Textures/pipes.png"
#define DIR_SCOREBOARD "../../games/3_flappy_bird/Textures/scoreBoard.png"
#define DIR_BACKGROUND "../../games/3_flappy_bird/Textures/backgrounds.png"
#define DIR_NUM "../../games/3_flappy_bird/Textures/chiffre.png"
#define DIR_SOL "../../games/3_flappy_bird/Textures/sol.png"
#define DIR_HIGHSCORE "../../games/3_flappy_bird/Textures/high_score.png"

// ADRESS SOUND
#define DIR_FLAP_WAV "../../games/3_flappy_bird/Sounds/flap.wav"
#define DIR_HURT_WAV "../../games/3_flappy_bird/Sounds/hurt.wav"
#define DIR_SCORE_WAV "../../games/3_flappy_bird/Sounds/score.wav"

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
void initialisationConstantHashage(long *const_1, long *const_2, long *const_3, long *const_4)
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
		printf("\nTEXTURE FAILURE : Erreur lors de la creation de %s SDL_ERROR : %s",directory,SDL_GetError());

	return texture;
}


// AFFICHAGE DU BACKGROUND
int afficherBackground(SDL_Renderer *renderer, SDL_Texture* texture_background)
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
int afficherPilonne(SDL_Renderer *renderer, SDL_Texture *texture_pipes, int position, int position_axe_x)
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
int afficherSol(SDL_Renderer *renderer, SDL_Texture *texture_sol,int target_x)
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

int AfficherPersonnage(SDL_Renderer *renderer,SDL_Texture *texture_birds, SDL_Point emplacementPersonnage, int varAnimationPersonnage, double angle)
{

	SDL_Rect pos_print = PERSO;

	pos_print.x += (varAnimationPersonnage-1) * pos_print.w;




	//SDL_Rect personnage = {pos_print.x,pos_print.y,DIM_PERSO.x,DIM_PERSO.y};

	SDL_Rect target = {emplacementPersonnage.x - ( PERSO.w*SCALE_TO_FIT ) / 2, emplacementPersonnage.y - ( PERSO.h *SCALE_TO_FIT ) / 2,
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



int afficherScore(SDL_Renderer *renderer , SDL_Texture *texture_chiffre,int score)
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

int afficherMeilleurScore(SDL_Renderer* renderer,SDL_Texture *texture_highscore, int score, int high_score, int cible)
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
int afficherTout(SDL_Renderer * renderer, SDL_Point emplacementPersonnage, pilonne *pilonne ,int score,int varAnimationPersonnage,int varAnimationSol, int cible, double angle ,SDL_Texture *texture_background, SDL_Texture *texture_pipes, SDL_Texture *texture_birds, SDL_Texture *texture_medals, SDL_Texture *texture_scoreBoard, SDL_Texture *texture_sol, SDL_Texture *texture_chiffre, SDL_Texture* texture_highscore)
{


	SDL_RenderClear(renderer);


	if ( afficherBackground(renderer,texture_background) )
		return SDL_RENDER_ERROR_BACKGROUND;

	// cas particuler charger X obstacle
	for (int i = 0; i < PRELOAD_POS_OBSTACLE; i++) {
		if ( afficherPilonne(renderer,texture_pipes,pilonne[i].hauteur,pilonne[i].position) )
			return SDL_RENDER_ERROR_OBSTACLE;

	}


	if ( afficherSol(renderer,texture_sol,varAnimationSol) )
		return SDL_RENDER_ERROR_SOL;
	if ( AfficherPersonnage(renderer , texture_birds, emplacementPersonnage  , varAnimationPersonnage , angle) )
		return SDL_RENDER_ERROR_PERSONNAGE;
	if ( afficherScore(renderer, texture_chiffre, score) )
		return SDL_RENDER_ERROR_PRINT_SCORE;
	if( afficherMeilleurScore(renderer,texture_highscore,score,6,cible ) )
		return SDL_RENDER_ERROR_PRINT_HS;

	//On affiche

	SDL_RenderPresent(renderer);
	return 0;

}

void init_pilonne(pilonne *pilonne, int *varAnimationPersonnage, int *varAnimationSol, int *end, int* dead, int *traitement)
{


	for(int i=0; i<PRELOAD_POS_OBSTACLE;i++){
		pilonne[i].hauteur = rand() % NB_POS_OBSTACLE ;
		if (i)
			pilonne[i].position = DISTANCE_UNDER_OBSTACLE*SCALE_TO_FIT + pilonne[i-1].position;
		else
			pilonne[i].position = WINDOW_L;
	}

	*varAnimationPersonnage = 0;
	*varAnimationSol = 0;
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
				Mix_PlayChannel( 5, score_wav,0);
			/*else
				ban commands*/
		}

	} else if(traitement == 0){
		if(pilonne[1].position + OBSTACLE_VERT.w*SCALE_TO_FIT + PERSO.w*SCALE_TO_FIT < WINDOW_L/2 ){
			traitement = 1;


			if(changeProtectedVar(score_hash, score, (*score)+1, const_1, const_2, const_3, const_4))
				Mix_PlayChannel( 5, score_wav,0);
			/*else
				ban commands*/

		}
	} else if ( traitement == 1){
		if(pilonne[2].position + OBSTACLE_VERT.w*SCALE_TO_FIT + PERSO.w*SCALE_TO_FIT < WINDOW_L/2 ){
			traitement = 2;
			if(changeProtectedVar(score_hash, score, (*score)+1, const_1, const_2, const_3, const_4))
				Mix_PlayChannel( 5, score_wav,0);
			/*else
				ban commands*/
		}

	} else if ( traitement == 2){
		if(pilonne[3].position + OBSTACLE_VERT.w*SCALE_TO_FIT + PERSO.w*SCALE_TO_FIT < WINDOW_L/2 ){
			traitement = 3;
			if(changeProtectedVar(score_hash, score, (*score)+1, const_1, const_2, const_3, const_4))
				Mix_PlayChannel( 5, score_wav,0);
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

int collision(pilonne *pilonne, SDL_Point emplacementPersonnage, Mix_Chunk *hurt_wav)
{
	int collision_detecter = 0;

	// permet de dire quand on sur l'axe d'un obstacle et de qu'elle obstacle
	// + detection collision
	for (int i = 0; i < PRELOAD_POS_OBSTACLE; i++) {
		// verifier qu'il ce trouve actuellement sur l'axe de deux bloque
		if( pilonne[i].position + PERSO.w*SCALE_TO_FIT > (  WINDOW_L/2 - ( (OBSTACLE_VERT.w*SCALE_TO_FIT) ) ) && pilonne[i].position + PERSO.w*SCALE_TO_FIT < (  WINDOW_L/2 + ( (OBSTACLE_VERT.w*SCALE_TO_FIT)/2 ) )){
			int bas =( (  -TRANCHE_POS_OBSTACLE + (TRANCHE_POS_OBSTACLE/NB_POS_OBSTACLE)*pilonne[i].hauteur ) + OBSTACLE_VERT.h*SCALE_TO_FIT ) + PERSO.h*SCALE_TO_FIT/2;
			int haut = (  -TRANCHE_POS_OBSTACLE + (TRANCHE_POS_OBSTACLE/NB_POS_OBSTACLE)*pilonne[i].hauteur ) + OBSTACLE_VERT.h*SCALE_TO_FIT + DISTANCE_BETWEEN_OBSTACLE*SCALE_TO_FIT - PERSO.h*SCALE_TO_FIT + COMPENSATITION_HITBOX_DOWN;
			if(! (emplacementPersonnage.y > bas && emplacementPersonnage.y < haut)  )
				collision_detecter = 1;

		}

	}

	// detection du sol
	if(emplacementPersonnage.y + PERSO.h*SCALE_TO_FIT > WINDOW_H - SOL.h*SCALE_TO_FIT){
		collision_detecter = 1;
	}

	// jouer un son
	if (collision_detecter)
		Mix_PlayChannel( 6, hurt_wav,0);

	return collision_detecter;
}

void traitementVariableAnimation(int *varAnimationPersonnage,int *varAnimationSol)
{
	*varAnimationPersonnage += 1;
	if(*varAnimationPersonnage > 3 )
		*varAnimationPersonnage = 1;

	if( ( *varAnimationSol * -1) >= ( SOL.w*SCALE_TO_FIT)*3  )
		*varAnimationSol = 0;
	else
		*varAnimationSol -= VITESSE_DEPLACEMENT_DECOR;
}

void attendreAvantDepart(Mix_Chunk *flap_wav)
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
	Mix_PlayChannel( 4, flap_wav,0);

}

void evenement(int *upper,int *vitesseGraviter, int *nb_boucle,Mix_Chunk *flap_wav)
{
	SDL_Event ev;
	while ( SDL_PollEvent(&ev) )
	{
		if (ev.type == SDL_KEYDOWN){

			if(ev.key.keysym.sym == SDLK_SPACE)
			{
				Mix_PlayChannel( 4, flap_wav,0);
				*upper = UPPER_STEP;
				*vitesseGraviter = 0;
				*nb_boucle = 0;
			}
			else if (ev.key.keysym.sym == SDLK_ESCAPE)
			{
				SDL_Delay(5000);
			}

		}

	}
}

void maj_var_environement(SDL_Point *emplacementPersonnage, int *upper, double *angle, int *nb_boucle, int* vitesseGraviter)
{
	if ( *upper > 0 ){
		emplacementPersonnage->y -= ( UPPER_BY_STEP - ( *upper ) )/ (FPS/30);
		*upper -= 1;
		if( *angle > ANGLE_UP)
		{
			if (*angle > 0)
				*angle -= ANGLE_VITESSE*2 ;
			else
				*angle -= ANGLE_VITESSE ;
		}
		} else if( *nb_boucle% (FPS/30) == 0 ){
			emplacementPersonnage->y += ( *vitesseGraviter += GRAVITY_SPEED ) / (FPS/30);
			if ( *angle < ANGLE_DOWN)
				*angle += ANGLE_VITESSE / 2;
		}
		*nb_boucle += 1;
}



// primary func
int flappy_bird( SDL_Renderer *renderer , int highscore, int send_l, int send_h)
{
	//SDL_Renderer *renderer = SDL_GetRenderer(window);
	if (renderer == NULL){
		printf("\nCREATION RENDU ECHEC %s",SDL_GetError());
		return -1;
	}


	printf("\nLe jeu commence");
	int exit_code = 0;
	srand(time(NULL));
	// rescale afficherTout
	SDL_RenderSetScale(renderer,
                       send_l/WINDOW_L,
                       send_h/WINDOW_H);

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
	if(!flap_wav)
		printf("\nError load %s CODE : %s", DIR_FLAP_WAV,SDL_GetError() );

	Mix_Chunk *hurt_wav = Mix_LoadWAV( DIR_HURT_WAV);
	if(!hurt_wav)
		printf("\nError load %s CODE : %s", DIR_HURT_WAV,SDL_GetError() );

	Mix_Chunk *score_wav = Mix_LoadWAV( DIR_SCORE_WAV);
	if(!score_wav)
		printf("\nError load %s CODE : %s", DIR_SCORE_WAV,SDL_GetError() );


	//if( !flap_wav || !hurt_wav || !score_wav)
		//exit_code = SOUND_ERROR_LOAD;
	// end sound effect //

	if (!exit_code)
	{
		//init pilone
		pilonne pilonne[PRELOAD_POS_OBSTACLE];

		// init variable
		int varAnimationPersonnage,varAnimationSol,end,dead,traitement;
		long long score_hash;
		int score=0;
		init_pilonne(pilonne,&varAnimationPersonnage,&varAnimationSol,&end,&dead,&traitement);
		int cible = (highscore-1)*DISTANCE_UNDER_OBSTACLE*SCALE_TO_FIT + pilonne[0].position;
		double angle  = 0;
		// end init pilonne //

		// init AfficherPersonnage y //

		SDL_Point emplacementPersonnage = {WINDOW_L/2 - ( (PERSO.w*SCALE_TO_FIT)/2 ) , WINDOW_H/2};

		//init var to hash score
		long const_1, const_2, const_3, const_4;
		initialisationConstantHashage(&const_1, &const_2, &const_3, &const_4);
		score_hash = hashage(score, const_1, const_2, const_3, const_4);

		// initialisation des variables de suivi est d'animation
		int upper = 0;
		int nb_boucle = 0;
		int vitesseGraviter = 0;

		// preaffichage avant lancement
		// afficher la vue //
		exit_code = afficherTout(renderer, emplacementPersonnage , pilonne, score ,1 , 0, cible, angle ,
																										texture_background, texture_pipes,  texture_birds, texture_medals,   texture_scoreBoard, texture_sol, texture_chiffre,texture_highscore);

		// attendre la touch espace //
		attendreAvantDepart(flap_wav);
		// faire un premier saut //
		upper = UPPER_STEP;

		while(!end && !exit_code){

				int times_at_start_frame = SDL_GetTicks();
				//si pas dans l'anim de mort
				if(!dead){

					evenement(&upper,&vitesseGraviter,&nb_boucle,flap_wav);

					maj_var_environement(&emplacementPersonnage, &upper, &angle, &nb_boucle,&vitesseGraviter);



					exit_code = afficherTout(renderer, emplacementPersonnage , pilonne, score ,varAnimationPersonnage , varAnimationSol, cible, angle ,
																																						texture_background,  texture_pipes, texture_birds, texture_medals,  texture_scoreBoard,  texture_sol, texture_chiffre, texture_highscore);

					// traitement de mon pilonne
					traitement = traitement_pilonne(pilonne,traitement,&score,  &score_hash,
																							 &const_1,   &const_2,   &const_3,    &const_4, score_wav);

					traitementVariableAnimation(&varAnimationPersonnage,&varAnimationSol);
					cible -= VITESSE_DEPLACEMENT_DECOR;

					dead = collision(pilonne,emplacementPersonnage,hurt_wav);

			}
			else
			{ //animate death
				if (angle < ANGLE_DOWN)
							angle += ANGLE_VITESSE*2 / (FPS/30);
				emplacementPersonnage.y += (vitesseGraviter++) / (FPS/30);
				exit_code = afficherTout(renderer, emplacementPersonnage , pilonne, score ,1 , 0 , cible , angle,
																												texture_background,  texture_pipes,  texture_birds,   texture_medals,  texture_scoreBoard,  texture_sol,   texture_chiffre, texture_highscore );

				if(emplacementPersonnage.y >= WINDOW_H - SOL.h*SCALE_TO_FIT)
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
