/**
*\file flappy_bird.c
*\brief Jeu flappy bird
*\author S.Mahi
*\version 1.0
*\date 10/01/2020
*/

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include "../../include/communFunctions.h"
#include "../../include/libWeb.h"
#include "../../include/hashage.h"

#include"../../include/define.h"
//native resolution
#define WINDOW_L 1920.
#define WINDOW_H 1080.

#define FPS 60
const int FRAME_TIME = 1000/FPS;


// scale ratio for native resolution
#define SCALE_TO_FIT 4 // WIINDOW_H/BACKGROUND.h *4

// IMAGE TILSET FIND POSITION
const SDL_Rect BACKGROUND = {0, 0, 144, 256};
const SDL_Rect OBSTACLE_VERT = {52,0, 26, 160};
const SDL_Rect OBSTACLE_ORANGE = {0,0, 26, 160};
const SDL_Rect SOL = {0, 0, 168, 55};
const SDL_Rect CHIFFRE = {0, 0, 12, 18};
const SDL_Rect PERSO = {0, 0, 17, 12};
const SDL_Rect HIGH_SCORE = {0,0,512,512};
const SDL_Rect SCOREBOARD = {0,0,113,57};
const SDL_Rect MEDALS = {0,0,22,22};
#define COMPENSATITION_HITBOX_DOWN 20

const SDL_Color COLOR_LOADING = {0x22,0xf4,0x57};

// PARAMETRE BUFFER OBSTACLE
#define PRELOAD_POS_OBSTACLE 8

// PARAMETRES POUR GESTION DES OBSTACLES
int DISTANCE_BETWEEN_OBSTACLE = 49;
int DISTANCE_UNDER_OBSTACLE = 100; // difficulte max en distance d'obstacle
#define NB_POS_OBSTACLE 5
#define TRANCHE_POS_OBSTACLE 550 // largeur sur Y des positions possible des portes de passages des pilonnes


// PERMET DE STOCKER LE BUFFER DES PILONNES DANS UNE STRUCTURE
/**
*\struct pilonne
*\brief Stock la position d'un obstacle
*/
typedef struct {
	int position; /*!< \POSITION DU PILONE SUR L'AXE DES X *///
	int hauteur; /*!< \POSITION DU PILONE SUR L'AXE DES Y *///
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

#define BAN_CODE 123

// ADRESS SOUND
#define DIR_FLAP_WAV "../games/3_flappy_bird/Sounds/flap.wav"
#define DIR_HURT_WAV "../games/3_flappy_bird/Sounds/hurt.wav"
#define DIR_SCORE_WAV "../games/3_flappy_bird/Sounds/score.wav"

//LOCAL ERROR CODE
#define IMAGE_ERROR_LOAD -101
#define SOUND_ERROR_LOAD -102
#define SDL_RENDER_ERROR_BACKGROUND -103
#define SDL_RENDER_ERROR_OBSTACLE -104
#define SDL_RENDER_ERROR_SOL -105
#define SDL_RENDER_ERROR_PERSONNAGE -106
#define SDL_RENDER_ERROR_PRINT_SCORE -107
#define SDL_RENDER_ERROR_PRINT_HS -108






/////////////////////////////////////////////////////
/// \fn int afficherBackground(SDL_Renderer *renderer, SDL_Texture *texture_background, int theme)
/// \brief affiche l'image de fond
///
/// \param  renderer
/// \param  texture_background
/// \param  theme
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
static
int afficherBackground(SDL_Renderer *renderer, SDL_Texture *texture_background, int theme);


/////////////////////////////////////////////////////
/// \fn int afficherPilonne(SDL_Renderer *renderer, SDL_Texture *texture_pipes, int position, int position_axe_x, int theme)
/// \brief affiche le OBSTACLE
///
/// \param renderer
/// \param texture_pipes
/// \param position
/// \param position_axe_x
/// \param theme
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
static
int afficherPilonne(SDL_Renderer *renderer, SDL_Texture *texture_pipes, int position, int position_axe_x, int theme);


/////////////////////////////////////////////////////
/// \fn int afficherSol(SDL_Renderer *renderer, SDL_Texture *texture_sol,int target_x)
/// \brief affiche le SOL DECORE
///
/// \param  renderer
/// \param  texture_sol
/// \param  target_x
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
static
int afficherSol(SDL_Renderer *renderer, SDL_Texture *texture_sol,int target_x);



/////////////////////////////////////////////////////
/// \fn int AfficherPersonnage(SDL_Renderer *renderer,SDL_Texture *texture_birds, SDL_Point emplacementPersonnage, int varAnimationPersonnage, double angle, int theme)
/// \brief affiche le personnage a l ecran
///
/// \param  renderer
/// \param  texture_birds
/// \param  emplacementPersonnage
/// \param  varAnimationPersonnage
/// \param  angle
/// \param  theme
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
static
int AfficherPersonnage(SDL_Renderer *renderer,SDL_Texture *texture_birds, SDL_Point emplacementPersonnage, int varAnimationPersonnage, double angle,int theme);



/////////////////////////////////////////////////////
/// \fn int intLen(int score)
/// \brief retourne la longueur d'un nombre
///
/// \param  score
///
/// \return int
/////////////////////////////////////////////////////
static
int intLen(int score);




/////////////////////////////////////////////////////
/// \fn int afficherScore(SDL_Renderer *renderer , SDL_Texture *texture_chiffre,int score,int mode)
/// \brief affiche le score en cours a l ecran
///
/// \param  renderer
/// \param  texture_chiffre
/// \param  score
/// \param  mode
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
static
int afficherScore(SDL_Renderer *renderer , SDL_Texture *texture_chiffre,int score,int mode);


/////////////////////////////////////////////////////
/// \fn int afficherMeilleurScore(SDL_Renderer* renderer,SDL_Texture *texture_highscore, int score, int high_score, int cible)
/// \brief affiche le meilleur score a l'ecran
///
/// \param  renderer
/// \param  texture_highscore
/// \param  score
/// \param  high_score
/// \param  cible
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
static
int afficherMeilleurScore(SDL_Renderer* renderer,SDL_Texture *texture_highscore, int score, int high_score, int cible);



/////////////////////////////////////////////////////
/// \fn int int afficherTout(SDL_Renderer *renderer,SDL_Thread **thread, TTF_Font *myFont, int *retour, int *frame_anim_loading, int *frameRetour, SDL_Texture *texture_loading,SDL_Point emplacementPersonnage, pilonne *pilonne ,int score,int varAnimationPersonnage,int varAnimationSol, int cible, double angle ,SDL_Texture *texture_background, SDL_Texture *texture_pipes, SDL_Texture *texture_birds, SDL_Texture *texture_medals, SDL_Texture *texture_scoreBoard, SDL_Texture *texture_sol, SDL_Texture *texture_chiffre, SDL_Texture *texture_highscore, int theme)
/// \brief regroupement de toute les fonctions d'affichage pour afficher tous
///
/// \param  renderer
/// \param  emplacementPersonnage
/// \param  pilonne
/// \param  score
/// \param  varAnimationPersonnage
/// \param  cible
/// \param  angle
/// \param  texture_background
/// \param  texture_pipes
/// \param  texture_birds
/// \param  texture_medals
/// \param  texture_scoreBoard
/// \param  texture_sol
/// \param  texture_chiffre
/// \param  texture_highscore)
/// \param  theme
///
/// \return void
/////////////////////////////////////////////////////
static
int afficherTout(SDL_Renderer *renderer,SDL_Thread **thread, TTF_Font* myFont, int *retour, int *frame_anim_loading, int *frameRetour, SDL_Texture* texture_loading,SDL_Point emplacementPersonnage, pilonne *pilonne ,int score,int varAnimationPersonnage,int varAnimationSol, int cible, double angle ,SDL_Texture *texture_background, SDL_Texture *texture_pipes, SDL_Texture *texture_birds, SDL_Texture *texture_medals, SDL_Texture *texture_scoreBoard, SDL_Texture *texture_sol, SDL_Texture *texture_chiffre, SDL_Texture* texture_highscore, int theme);


/////////////////////////////////////////////////////
/// \fn void init_pilonne(pilonne *pilonne, int *varAnimationPersonnage, int *varAnimationSol, int *end, int *dead, int *traitement)
/// \brief initialisation des pilonnes et des animation et de certaine variables liee au boucle du jeu
///
/// \param  pilonne struct pilonne
/// \param  varAnimationPersonnage
/// \param  varAnimationSol
/// \param  end
/// \param  dead
/// \param  traitement
///
/// \return void
/////////////////////////////////////////////////////
static
void init_pilonne(pilonne *pilonne, int *varAnimationPersonnage, int *varAnimationSol, int *end, int *dead, int *traitement);



/////////////////////////////////////////////////////
/// \fn int traitement_pilonne(pilonne *pilonne, int traitement, int *score, long long *score_hash, long keys[4], Mix_Chunk *score_wav)
/// \brief detecte le passage d'un pilonne et joue un son
///
/// \param  pilonne struct pilonne
/// \param  traitement pilonne a traiter
/// \param  score score a incrementer
/// \param  score_hash liee a la securite
/// \param  keys liee a la securite
/// \param  score_wav son a jouer
///
/// \return int TRUE/FALSE
/////////////////////////////////////////////////////
static
int traitement_pilonne(pilonne *pilonne, int traitement, int *score, long long *score_hash, long keys[4], Mix_Chunk *score_wav);



/////////////////////////////////////////////////////
/// \fn int collision(pilonne *pilonne, SDL_Point emplacementPersonnage, Mix_Chunk *hurt_wav)
/// \brief permet de detecter une collision et de jouer un son
///
/// \param  pilonne
/// \param  emplacementPersonnage
/// \param  hurt_wav
///
/// \return int TRUE/FALSE
/////////////////////////////////////////////////////
static
int collision(pilonne *pilonne, SDL_Point emplacementPersonnage, Mix_Chunk *hurt_wav);


/////////////////////////////////////////////////////
/// \fn void traitementVariableAnimation(int *varAnimationPersonnage,int *varAnimationSol)
/// \brief mise a jours des variables liee a l'environement/decor
///
/// \param  varAnimationPersonnage
/// \param  varAnimationSol
///
/// \return void
/////////////////////////////////////////////////////
static
void traitementVariableAnimation(int *varAnimationPersonnage,int *varAnimationSol);


/////////////////////////////////////////////////////
/// \fn int attendreAvantDepart(Mix_Chunk *flap_wav, int * rdyToSpace)
/// \brief attend l'appui sur espace pour demarrer joue un son
///
/// \param  flap_wav jouer le son passer en parametre
/// \param  Determine si la touche espace a été relachée
///
/// \return faux si le joueur quitte le jeu, sinon vrai
/////////////////////////////////////////////////////
static
int attendreAvantDepart(Mix_Chunk *flap_wav, int * rdyToSpace);



/////////////////////////////////////////////////////
/// \fn int evenement(int *upper,int *vitesseGraviter, int *nb_boucle,Mix_Chunk *flap_wav, int *rdyToSpace)
/// \brief remet les variables au DEFINE si on appui sur espace
///
/// \param  upper
/// \param  vitesseGraviter
/// \param  nb_boucle
/// \param  flap_wav
/// \param  Determine si la touche espace a été relachée
///
/// \return void
/////////////////////////////////////////////////////
static
int evenement(int *upper,int *vitesseGraviter, int *nb_boucle,Mix_Chunk *flap_wav, int *rdyToSpace);



/////////////////////////////////////////////////////
/// \fn void updateVariableEnvironement(SDL_Point *emplacementPersonnage, int *upper, double *angle, int *nb_boucle, int *vitesseGraviter)
/// \brief permet de mettre a jours les variables utilisez
///
/// \param  emplacementPersonnage point X,Y sur la position de l'oiseau
/// \param  upper l'oiseau monte
/// \param  angle angle de l'oiseau
/// \param  nb_boucle nombre de boucle
/// \param  vitesseGraviter vitesse de graviter
///
/// \return void
/////////////////////////////////////////////////////
static
void updateVariableEnvironement(SDL_Point *emplacementPersonnage, int *upper, double *angle, int *nb_boucle, int *vitesseGraviter);



extern int updateEnded;
/////////////////////////////////////////////////////
/// \fn int flappy_bird( SDL_Renderer *renderer , int highscore, int send_l, int send_h, char *token , int hardcore, SDL_Texture **textures)

/// \brief Fonction principale qui joue le jeu flappy bird
///
/// \param renderer Le renderer où afficher le jeu
/// \param highscore le record du jeu
/// \param send_l La largeur d'écran
/// \param send_h La hauteur d'écran
/// \param token La clé de connexion du joueur
/// \param hardcore Le mode de jeu
/// \param textures Les textures du jeu
/// \return L'erreur associée ou EXIT_SUCCESS si pas d'erreur
/////////////////////////////////////////////////////
int flappy_bird( SDL_Renderer *renderer , int highscore, int send_l, int send_h, char *token , int hardcore, SDL_Texture **textures)
{
	//SDL_Init(SDL_INIT_EVERYTHING);

	SDL_Thread *thread = NULL;
	char buffer[10];
	EnvoiScore envoiScore;
	updateEnded = SDL_FALSE;
	int retour = EXIT_FAILURE;
	int frameRetour = 0;
	int frame_anim_loading = 0;
	int rdyToSpace = SDL_TRUE;
	///////////////////////////////////////////////////////
	// INITALISATION DU RAND
	srand(time(NULL));
	///////////////////////////////////////////////////////



	///////////////////////////////////////////////////////
	// mise a jour variable pour difficulte hardcore
	if( hardcore == 1 )
	{
		DISTANCE_UNDER_OBSTACLE = 82;
		DISTANCE_BETWEEN_OBSTACLE = 48;
	}
	else
	{
		DISTANCE_UNDER_OBSTACLE = 100;
		DISTANCE_BETWEEN_OBSTACLE = 51;
	}
	///////////////////////////////////////////////////////


	///////////////////////////////////////////////////////
	// Verifier l'existance d'une fenetre SDL
	if (renderer == NULL){
		printf("\nCREATION RENDU ECHEC %s",SDL_GetError());
		return EXIT_FAILURE;
	}
	///////////////////////////////////////////////////////


	///////////////////////////////////////////////////////
	// RESCALE A LA TAILLE DE LA VUE
	SDL_RenderSetScale(renderer,
                       send_l/WINDOW_L,
                       send_h/WINDOW_H);
	///////////////////////////////////////////////////////



	//police
	TTF_Init();
	TTF_Font * myFont = TTF_OpenFont("../games/2_snake/Fonts/zorque.ttf", OPEN_FONT_SIZE);
	if(!myFont)
		printf("Impossible d'ouvrir la police %s\n", TTF_GetError() );


	//////////////////////////////////////////////////////////////////
	// INITIALISER SON + GESTION ERREUR
	Mix_Chunk *flap_wav = Mix_LoadWAV( DIR_FLAP_WAV);
	if(!flap_wav)
	{
		printf("\nError load %s CODE : %s", DIR_FLAP_WAV,SDL_GetError() );
		return SOUND_ERROR_LOAD;
	}
	Mix_Chunk *hurt_wav = Mix_LoadWAV( DIR_HURT_WAV);
	if(!hurt_wav)
	{
		printf("\nError load %s CODE : %s", DIR_HURT_WAV,SDL_GetError() );
		return SOUND_ERROR_LOAD;
	}

	Mix_Chunk *score_wav = Mix_LoadWAV( DIR_SCORE_WAV);
	if(!score_wav)
	{
		printf("\nError load %s CODE : %s", DIR_SCORE_WAV,SDL_GetError() );
		return SOUND_ERROR_LOAD;
	}

	Mix_Volume(5, 12); //flap

	Mix_Volume(6, 20); // score

	Mix_Volume(7, 20); //hurt
	//////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////
	// INITALISATION PILONNE AVEC PRECHARGEMENT
	pilonne pilonne[PRELOAD_POS_OBSTACLE];
	//////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////
	// INITALISATION VARIABLE NECESSAIRE A L'ENVIRONEMENT
	// VAR ANIMATION EVENEMENT JEU
	int varAnimationPersonnage,varAnimationSol,end,dead,traitement;
	//////////////////////////////////////////////////////////////////
	// VAR SCORE
	long long score_hash;
	int score=0;
	long keys[4];
	// preset var hash
	initialisationConstantHashage(keys);
	// preset score hash
	score_hash = hashage(score, keys);
	//////////////////////////////////////////////////////////////////
	// INITALISATION DES PILONNES ET VALEURS VARIABLES
	init_pilonne(pilonne,&varAnimationPersonnage,&varAnimationSol,&end,&dead,&traitement);
	int cible = (highscore-1)*DISTANCE_UNDER_OBSTACLE*SCALE_TO_FIT + pilonne[0].position;
	// ANGLE DU PERSONNAGES AU DEBUT
	double angle  = 0;
	//////////////////////////////////////////////////////////////////
	// INIT POSTITION PERSONNAGES
	SDL_Point emplacementPersonnage = {WINDOW_L/2 - ( (PERSO.w*SCALE_TO_FIT)/2 ) , WINDOW_H/2};
	//////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////
	// CREATTION DES VARIABLES DE MOUVEMENT PEROSNNAGES
	int upper = 0;
	int nb_boucle = 0;
	int vitesseGraviter = 0;
	//////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// AFFICHAGE DU JEU
	int exitCode = afficherTout(renderer, &thread, myFont, &retour, &frame_anim_loading, &frameRetour, textures[F_LOADING], emplacementPersonnage , pilonne, score ,1 , 0, cible, angle ,
																										textures[F_BACKGROUND],  textures[F_PIPES], textures[F_BIRDS], textures[F_MEDALS],  textures[F_SCOREBOARD],  textures[F_SOL], textures[F_CHIFFRE], textures[F_HIGHSCORE],hardcore);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	drawReplay(renderer, myFont,0.57, BASE_WINDOW_W,BASE_WINDOW_H);
	SDL_RenderPresent(renderer);
	//////////////////////////////////////////////////////////////////
	// ATTENDRE APPUI TOUCHE ESPACE + JOUER PREMIER SON + PREMIER SAUT
	if(!attendreAvantDepart(flap_wav, &rdyToSpace))
		end = SDL_TRUE;
	upper = UPPER_STEP;
	//////////////////////////////////////////////////////////////////

	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	//////////////////////////////////////////////////////////////////
	// DEBUT DU JEU
	while(!end && !exitCode){

		if( keystate[SDL_SCANCODE_ESCAPE])
			end = SDL_TRUE;
		//////////////////////////////////////////////////////////////////
		// PERMET DE LIMITER LES FRAMES
		int tempsDebutFrame = SDL_GetTicks();

		//////////////////////////////////////////////////////////////////
		// SI JOUEUR EN VIE
		if(!dead){

				//////////////////////////////////////////////////////////////////
				// VERIFIER EVENEMENT
				if(!evenement(&upper,&vitesseGraviter,&nb_boucle,flap_wav, &rdyToSpace))
					end = SDL_TRUE;

				// UPDATE VARIABLE ENVIRONEMENT
				updateVariableEnvironement(&emplacementPersonnage, &upper, &angle, &nb_boucle,&vitesseGraviter);


				// UPDATE AFFICHAGE
				exitCode = afficherTout(renderer, &thread, myFont, &retour, &frame_anim_loading, &frameRetour, textures[F_LOADING], emplacementPersonnage , pilonne, score ,varAnimationPersonnage , varAnimationSol, cible, angle ,
																																						textures[F_BACKGROUND],  textures[F_PIPES], textures[F_BIRDS], textures[F_MEDALS],  textures[F_SCOREBOARD],  textures[F_SOL], textures[F_CHIFFRE], textures[F_HIGHSCORE],hardcore);

				SDL_RenderPresent(renderer);
				// UPDATE DECORE
				traitement = traitement_pilonne(pilonne,traitement,&score,  &score_hash, keys, score_wav);

				// UPDATE VARIABLE ANIMATION
				traitementVariableAnimation(&varAnimationPersonnage,&varAnimationSol);
				cible -= VITESSE_DEPLACEMENT_DECOR;


				// DETECTION COLLISION
				dead = collision(pilonne,emplacementPersonnage,hurt_wav);

				//////////////////////////////////////////////////////////////////
				//////////////////////////////////////////////////////////////////
				// REPRENDRE LA BOUCLE
				//////////////////////////////////////////////////////////////////
				//////////////////////////////////////////////////////////////////

		}
		else
		{
			//////////////////////////////////////////////////////////////////
			// LE JOUEUR EST MORT
			if (angle < ANGLE_DOWN)
						angle += ANGLE_VITESSE*2 / (FPS/30);

			// MISE A JOURS DE l'EMPLACEMENT PERSONNAGES
			emplacementPersonnage.y += (vitesseGraviter++) / (FPS/30);

			// MISE A JOUR DE l"AFFICHAGE
			exitCode = afficherTout(renderer, &thread, myFont, &retour, &frame_anim_loading, &frameRetour, textures[F_LOADING], emplacementPersonnage , pilonne, score ,1 , 0 , cible , angle,
																												textures[F_BACKGROUND],  textures[F_PIPES], textures[F_BIRDS], textures[F_MEDALS],  textures[F_SCOREBOARD],  textures[F_SOL], textures[F_CHIFFRE], textures[F_HIGHSCORE], hardcore);
			SDL_RenderPresent(renderer);
			// SI LE PERSONNAGE A ATTENDRE LE SOL
			if(emplacementPersonnage.y >= WINDOW_H - SOL.h*SCALE_TO_FIT)
			{

				// VERIFIER QUE LE SCORE N 'AS PAS ETAIT CHANGER
				if (  score_hash == hashage(score, keys) )
				{
					frame_anim_loading = 0;
					//On atttend si jamais un envoi est en cours
					if(thread){
						SDL_WaitThread(thread, &retour);
						if( retour == EXIT_FAILURE){
							frameRetour = -2*FRAME_ANIM_RETOUR;
						}
						else{
							 frameRetour = FRAME_ANIM_RETOUR;
						}
						thread = NULL;
					}


					// CONVERTIR SCORE EN TEXT

					sprintf(buffer,"%d",score);

					if(hardcore)
						envoiScore = (EnvoiScore){"1", buffer, token};
					//updateScore("5",buffer,token);
					else
						envoiScore = (EnvoiScore){"12", buffer, token};
					//updateScore("8",buffer,token);

					printf("CONNEXION...\n" );

					thread = SDL_CreateThread(  (int(*)(void*))updateScore, NULL, &envoiScore );
				}



				// ON AUGMENTE LA TAILLE DE LA MEDALS PAR 2
				SDL_Rect positionMedals = {WINDOW_L/4 + 13 * SCALE_TO_FIT * 2 ,WINDOW_H/4 + 21 * SCALE_TO_FIT * 2,22*SCALE_TO_FIT * 2,22*SCALE_TO_FIT * 2};


				//////////////////////////////////////////////////////////////////
				// CHOISIR MEDAIL A AFFICHER EN FONCTION DU SCORE
				//
				int choixMedalsAvecScore = 3;
				if ( score < 5)
					choixMedalsAvecScore = 4; // pas de médaille
				else if(score < 10)
						choixMedalsAvecScore = 0;
				else if(score < 20 )
						choixMedalsAvecScore = 1;
				else if(score < 30 )
						choixMedalsAvecScore = 2;


				//////////////////////////////////////////////////////////////////

				//////////////////////////////////////////////////////////////////
				// ATTENDRE UNE DECISION DU JOUEUR
				// REJOUER OU QUITTER
				int wait = 1;
				//////////////////////////////////////////////////////////////////
				while( wait )
				{
					SDL_RenderClear(renderer);

					afficherTout(renderer, &thread, myFont, &retour, &frame_anim_loading, &frameRetour, textures[F_LOADING], emplacementPersonnage , pilonne, score ,1 , 0, cible, angle ,

																												textures[F_BACKGROUND],  textures[F_PIPES], textures[F_BIRDS], textures[F_MEDALS],  textures[F_SCOREBOARD],  textures[F_SOL], textures[F_CHIFFRE], textures[F_HIGHSCORE] , hardcore);
					//////////////////////////////////////////////////////////////////
					//////////////////////////////////////////////////////////////////
					// AFFICHER SCORE ET MEDAIL ICI
					// ON AUGMENTE LA TAILLE DU SCOREBOARD PAR 2
					SDL_Rect positionScoreBoard= {WINDOW_L/4,WINDOW_H/4,SCOREBOARD.w * SCALE_TO_FIT * 2, SCOREBOARD.h*SCALE_TO_FIT * 2};
					drawReplay(renderer, myFont,0.57, WINDOW_L,WINDOW_H);
					SDL_RenderCopy(renderer, textures[F_SCOREBOARD], NULL, &positionScoreBoard);


					SDL_Rect choisirMedals = {MEDALS.w * choixMedalsAvecScore , MEDALS.y, MEDALS.w, MEDALS.h};
					SDL_RenderCopy(renderer, textures[F_MEDALS], &choisirMedals, &positionMedals);
					//////////////////////////////////////////////////////////////////


					//////////////////////////////////////////////////////////////////
					// AFFICHAGE DU BEST SCORE ET DE SON SCORE DANS L'AFFICHEUR
					afficherScore(renderer, textures[F_CHIFFRE], score , 2);
					if( score > highscore)
						highscore = score;
					afficherScore(renderer, textures[F_CHIFFRE], highscore , 3);
					if(thread && updateEnded){
						SDL_WaitThread(thread, &retour);
						thread = NULL;
						if(retour == EXIT_FAILURE){
							frameRetour = -2*FRAME_ANIM_RETOUR;
						}
						else{
							 frameRetour = FRAME_ANIM_RETOUR;
						}
					}
					else if(thread){
						printf("afficher loading\n");
						afficherLoading(renderer, textures[F_LOADING], COLOR_LOADING, 0, 0, frame_anim_loading++, WINDOW_L  , WINDOW_H, WINDOW_L );
					}

					if(frameRetour){

						afficherRetour(renderer, textures[F_LOADING] , myFont  , COLOR_LOADING, 0, 0, frameRetour, WINDOW_L  , WINDOW_H, WINDOW_L );
						if(frameRetour >0)
							frameRetour--;
						else
							frameRetour++;
					}

					int frame_delay = SDL_GetTicks() - tempsDebutFrame;
					if(frame_delay < FRAME_TIME)
						SDL_Delay(FRAME_TIME - frame_delay );
					SDL_RenderPresent(renderer);
					SDL_Event ev;

					while ( SDL_PollEvent(&ev) )
					{

						if(ev.type == SDL_QUIT){
							//////////////////////////////////////////////////////////////////
							// QUIITER WHILE ET JEU
							wait = 0;
							end = SDL_TRUE;
							//////////////////////////////////////////////////////////////////
						}
						if (ev.type == SDL_KEYDOWN){
							if(ev.key.keysym.sym == SDLK_SPACE)
							{
								//////////////////////////////////////////////////////////////////
								// LE JOUEUR REJOUE

								//////////////////////////////////////////////////////////////////
								// REMISE A ZERO DES VARIABLES DE JEU
								score=0;
								angle =0;
								upper = 0;
								nb_boucle = 0;
								vitesseGraviter = 0;
								//////////////////////////////////////////////////////////////////
								// GENERER NOUVEAU DECORS
								init_pilonne(pilonne,&varAnimationPersonnage,&varAnimationSol,&end,&dead,&traitement);
								cible = (highscore-1)*DISTANCE_UNDER_OBSTACLE*SCALE_TO_FIT + pilonne[0].position;
								//////////////////////////////////////////////////////////////////
								// REPLACER PERSONNAGE
								emplacementPersonnage.x = WINDOW_L/2 - ( (PERSO.w*SCALE_TO_FIT)/2 );
								emplacementPersonnage.y =  WINDOW_H/2;
								//////////////////////////////////////////////////////////////////

								//////////////////////////////////////////////////////////////////
								// REINIALISER HASHAGE DU SCORE
								initialisationConstantHashage(keys);
								score_hash = hashage(score, keys);
								//////////////////////////////////////////////////////////////////

								//////////////////////////////////////////////////////////////////
								// TOUS AFFICHER

								exitCode = afficherTout(renderer, &thread, myFont, &retour, &frame_anim_loading, &frameRetour, textures[F_LOADING], emplacementPersonnage , pilonne, score ,1 , 0, cible, angle ,

																															textures[F_BACKGROUND],  textures[F_PIPES], textures[F_BIRDS], textures[F_MEDALS],  textures[F_SCOREBOARD],  textures[F_SOL], textures[F_CHIFFRE], textures[F_HIGHSCORE] , hardcore);
								//////////////////////////////////////////////////////////////////
								// ARRETER d'ATTENDRE
								wait = 0;
								// QUIITER LE while
								//////////////////////////////////////////////////////////////////
							}
							else if (ev.key.keysym.sym == SDLK_ESCAPE)
							{
								//////////////////////////////////////////////////////////////////
								// QUIITER WHILE ET JEU
								wait = 0;
								end = SDL_TRUE;
								//////////////////////////////////////////////////////////////////
							}
						}

					}
				}
				//////////////////////////////////////////////////////////////////
				// VIDER LA PILE D'EVENEMENT
				if(end == SDL_FALSE)
				{
					//////////////////////////////////////////////////////////////////
					// ATTENDRE UNE TOUCHE + SON + UP
					if(!attendreAvantDepart(flap_wav, &rdyToSpace))
						end = SDL_TRUE;
					upper = UPPER_STEP;
					//////////////////////////////////////////////////////////////////
				}
				//////////////////////////////////////////////////////////////////

			}

		}
		//////////////////////////////////////////////////////////////////
		// DELAI FRAME
		int frame_delay = SDL_GetTicks() - tempsDebutFrame;
		if(frame_delay < FRAME_TIME)
			SDL_Delay(FRAME_TIME - frame_delay );
		//////////////////////////////////////////////////////////////////


	}


	if(thread){
		printf("wait final\n" );
		SDL_WaitThread(thread, NULL);
		thread = NULL;
	}
	//////////////////////////////////////////////////////////////////
	// LIBERER MEMOIRE ALLOUER NON NECESSAIRE
	Mix_FreeChunk(flap_wav);
	Mix_FreeChunk(hurt_wav);
	Mix_FreeChunk(score_wav);

	//////////////////////////////////////////////////////////////////


	return exitCode;
}



































int afficherBackground(SDL_Renderer *renderer, SDL_Texture* texture_background, int theme)
{
	SDL_Rect target = {0, 0, BACKGROUND.w*SCALE_TO_FIT, WINDOW_H};

	SDL_Rect choiceBackground = BACKGROUND;

	///////////////////////////////////////////////
	// PERMET DE CHOISIR UNE COULEUR DE FOND
	if(theme)
		choiceBackground.x = BACKGROUND.w;

	//On le dessine tant qu'on a pas rempli l'ecran
	while (target.x + BACKGROUND.w*SCALE_TO_FIT < WINDOW_L) {
		SDL_RenderCopy(renderer, texture_background, &choiceBackground, &target);
		target.x += BACKGROUND.w*SCALE_TO_FIT;
	}
	return SDL_RenderCopy(renderer, texture_background, &choiceBackground, &target);

}







int afficherPilonne(SDL_Renderer *renderer, SDL_Texture *texture_pipes, int position, int position_axe_x, int theme)
{


	SDL_Rect obstacle = OBSTACLE_VERT;
	if( theme )
		obstacle.x = 0;

	obstacle.x += obstacle.w;

	SDL_Rect target = {position_axe_x, -TRANCHE_POS_OBSTACLE + (TRANCHE_POS_OBSTACLE/NB_POS_OBSTACLE)*position,OBSTACLE_VERT.w*SCALE_TO_FIT,OBSTACLE_VERT.h*SCALE_TO_FIT};

	SDL_RenderCopy(renderer,texture_pipes,&obstacle,&target);

	target.y += OBSTACLE_VERT.h*SCALE_TO_FIT + DISTANCE_BETWEEN_OBSTACLE*SCALE_TO_FIT;
	obstacle.x -= obstacle.w;


	/*SDL_Rect test = {position_axe_x , (-TRANCHE_POS_OBSTACLE + (TRANCHE_POS_OBSTACLE/NB_POS_OBSTACLE)*position) + OBSTACLE_VERT.h*SCALE_TO_FIT, OBSTACLE_VERT.w*SCALE_TO_FIT ,DISTANCE_BETWEEN_OBSTACLE*SCALE_TO_FIT};*/
	//SDL_RenderFillRect(renderer, &test);

	return SDL_RenderCopy(renderer,texture_pipes,&obstacle,&target);

}






int afficherSol(SDL_Renderer *renderer, SDL_Texture *texture_sol,int target_x)
{
	int error_code = 0;
	SDL_Rect target = {target_x,WINDOW_H-(SOL.h*SCALE_TO_FIT),SOL.w*SCALE_TO_FIT,SOL.h*SCALE_TO_FIT};

	// AFFICHE LE PREMIERE ELLEMENT DE SOL
	SDL_RenderCopy(renderer,texture_sol,&SOL,&target);

	// REMPLI L'AFFICHAGE DE TEXTURE SOL
	while (target.x + SOL.w*SCALE_TO_FIT < WINDOW_L) {
		target.x +=  SOL.w*SCALE_TO_FIT;
	 	error_code = SDL_RenderCopy(renderer, texture_sol, &SOL, &target);
		if(error_code)
			return error_code;
	}
	return error_code;
}



int AfficherPersonnage(SDL_Renderer *renderer,SDL_Texture *texture_birds, SDL_Point emplacementPersonnage, int varAnimationPersonnage, double angle,int theme)
{

	SDL_Rect pos_print = PERSO;

	pos_print.x += (varAnimationPersonnage-1) * pos_print.w;

	///////////////////////////////////////////////
	// PERMET DE CHOISIR UNE COULEUR DE PERSONNAGES
	if(theme)
		pos_print.y = PERSO.h;


	// ON AFFICHE A L EMPLACEMENT PASSER EN PARAMETRE
	// LE PERSONNAGES
	SDL_Rect target = {emplacementPersonnage.x - ( PERSO.w*SCALE_TO_FIT ) / 2, emplacementPersonnage.y - ( PERSO.h *SCALE_TO_FIT ) / 2,
		 								PERSO.w*SCALE_TO_FIT , PERSO.h*SCALE_TO_FIT};

	SDL_Point centre = { (PERSO.w*SCALE_TO_FIT) /2, (PERSO.h*SCALE_TO_FIT)/2 };


	//SDL_RenderFillRect(renderer,&target);

	return SDL_RenderCopyEx(renderer,texture_birds,&pos_print,&target, angle , &centre,0);
}


int intLen(int score)
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




int afficherScore(SDL_Renderer *renderer , SDL_Texture *texture_chiffre,int score,int mode)
{
	/////////////////////////////////////////////////////
	// retourne taille d'un nombre
	int taille = intLen(score);
	int error_code = 0;


	SDL_Rect chiffre = CHIFFRE;
	SDL_Rect target  = {100+(WINDOW_L)/2- (CHIFFRE.w * SCALE_TO_FIT*taille)/2, (CHIFFRE.h * SCALE_TO_FIT)/2 , CHIFFRE.w * SCALE_TO_FIT, CHIFFRE.h * SCALE_TO_FIT};

	/////////////////////////////////////////////////////
	// score mode 2 sur le tableau final score
	if (mode == 2)
	{
		target.x = WINDOW_L*0.72 - CHIFFRE.w*SCALE_TO_FIT;
		target.y = WINDOW_H*0.39;
	}
	/////////////////////////////////////////////////////
	// score mode 3 sur le tableau final score en bas
	else if ( mode == 3)
	{
		target.x = WINDOW_L*0.72 - CHIFFRE.w*SCALE_TO_FIT;
		target.y = WINDOW_H*0.56;
	}



	/////////////////////////////////////////////////////
	// fait un rendu des chiffres
	for(int i=0 ; i < taille; i++)
	{
		chiffre.x = CHIFFRE.w * (score%10);
		target.x -= CHIFFRE.w * SCALE_TO_FIT;
		error_code = SDL_RenderCopy(renderer,texture_chiffre,&chiffre,&target);
		if (error_code)
			return error_code;

		score /= 10;
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





int afficherTout(SDL_Renderer * renderer,SDL_Thread ** thread, TTF_Font* myFont, int *retour, int *frame_anim_loading, int *frameRetour, SDL_Texture* texture_loading,SDL_Point emplacementPersonnage, pilonne *pilonne ,int score,int varAnimationPersonnage,int varAnimationSol, int cible, double angle ,SDL_Texture *texture_background, SDL_Texture *texture_pipes, SDL_Texture *texture_birds, SDL_Texture *texture_medals, SDL_Texture *texture_scoreBoard, SDL_Texture *texture_sol, SDL_Texture *texture_chiffre, SDL_Texture* texture_highscore, int theme)
{

	/////////////////////////////////////////////////////
	// NETTOYAGE DE LA FENETRE PRECEDANTE
	SDL_RenderClear(renderer);




	/////////////////////////////////////////////////////
	// AFFICHAGE DU FOND
	if ( afficherBackground(renderer,texture_background,theme) )
		return SDL_RENDER_ERROR_BACKGROUND;

	/////////////////////////////////////////////////////
	// cas particuler charger X obstacle
	for (int i = 0; i < PRELOAD_POS_OBSTACLE; i++) {
		// AFFICHER LES PILONNES
		if ( afficherPilonne(renderer,texture_pipes,pilonne[i].hauteur,pilonne[i].position,theme) )
			return SDL_RENDER_ERROR_OBSTACLE;

	}

	////////////////////////////////////////////////////
	// AFFICHER LE SOL
	if ( afficherSol(renderer,texture_sol,varAnimationSol) )
		return SDL_RENDER_ERROR_SOL;
	/////////////////////////////////////////////////////
	// AFFICHER PERSONNAGES
	if ( AfficherPersonnage(renderer , texture_birds, emplacementPersonnage  , varAnimationPersonnage , angle,theme) )
		return SDL_RENDER_ERROR_PERSONNAGE;
	/////////////////////////////////////////////////////
	// AFFICHER SCORE
	if ( afficherScore(renderer, texture_chiffre, score , theme) )
		return SDL_RENDER_ERROR_PRINT_SCORE;
	/////////////////////////////////////////////////////
	// AFFICHER MEILLEURE SCORE
	if( afficherMeilleurScore(renderer,texture_highscore,score,6,cible ) )
		return SDL_RENDER_ERROR_PRINT_HS;


	if(*thread && updateEnded){
		SDL_WaitThread(*thread, retour);
		*thread = NULL;
		if(*retour == EXIT_FAILURE){
			*frameRetour = -2*FRAME_ANIM_RETOUR;
		}
		else{
			 *frameRetour = FRAME_ANIM_RETOUR;
		}
	}
	else if(*thread){
		printf("afficher loading\n");
		if(frame_anim_loading)
			afficherLoading(renderer, texture_loading, COLOR_LOADING, 0, 0, (*frame_anim_loading)++, WINDOW_L  , WINDOW_H, WINDOW_L);
	}

	if(*frameRetour){
		afficherRetour(renderer, texture_loading , myFont  , COLOR_LOADING, 0, 0, *frameRetour, WINDOW_L  , WINDOW_H, WINDOW_L);
		if(*frameRetour >0)
			(*frameRetour)--;
		else
			(*frameRetour)++;
	}


	/////////////////////////////////////////////////////
	// AFFICHER TOUS
//	SDL_RenderPresent(renderer);

	return 0;

}


void init_pilonne(pilonne *pilonne, int *varAnimationPersonnage, int *varAnimationSol, int *end, int* dead, int *traitement)
{

	/////////////////////////////////////////////////////
	// ATTRIBUTION D UNE VARIABLE ALEATOIRE
	for(int i=0; i<PRELOAD_POS_OBSTACLE;i++){
		pilonne[i].hauteur = rand() % NB_POS_OBSTACLE ;
		if (i)
			pilonne[i].position = DISTANCE_UNDER_OBSTACLE*SCALE_TO_FIT + pilonne[i-1].position;
		else
			pilonne[i].position = WINDOW_L*2/3.;
	}

	/////////////////////////////////////////////////////
	// SET TO 0
	*varAnimationPersonnage = 0;
	*varAnimationSol = 0;
	*end = 0;
	*dead = 0;
	*traitement = -1;
}




int traitement_pilonne(pilonne *pilonne, int traitement, int *score, long long *score_hash, long keys[4], Mix_Chunk *score_wav)
{

	/////////////////////////////////////////////////////
	// INITIALISATION SI AUCUN TRAITEMENT REALISER
	if(traitement == -1){
		/////////////////////////////////////////////////////
		// ON VERIFIE SI ON A PASSER
		if(pilonne[0].position + OBSTACLE_VERT.w*SCALE_TO_FIT + PERSO.w*SCALE_TO_FIT < WINDOW_L/2  ){
			/////////////////////////////////////////////////////
			// ON TRAIT LE PILONNE SUIVANT
			traitement = 0;
			// UPDATE SCORE AVEC SECURITER
			if(changeProtectedVar(score_hash, score, (*score)+1, keys))
				Mix_PlayChannel( 6, score_wav,0);
				// LE JOUEUR EST REPORTER
		/*else
				ban commands*/
		}

	}
	/////////////////////////////////////////////////////
	// TRAITEMENT DU PILONNE 1
	else if(traitement == 0){
		/////////////////////////////////////////////////////
		// ON VERIFIE SI ON A PASSER
		if(pilonne[1].position + OBSTACLE_VERT.w*SCALE_TO_FIT + PERSO.w*SCALE_TO_FIT < WINDOW_L/2 ){
			/////////////////////////////////////////////////////
			// ON TRAIT LE PILONNE SUIVANT
			traitement = 1;
			// UPDATE SCORE AVEC SECURITER
			if(changeProtectedVar(score_hash, score, (*score)+1, keys))
				Mix_PlayChannel( 6, score_wav,0);
				// LE JOUEUR EST REPORTER
			/*else
				ban commands*/
		}

	}
	/////////////////////////////////////////////////////
	// TRAITEMENT DU PILONNE 2
	else if ( traitement == 1){
		/////////////////////////////////////////////////////
		// ON VERIFIE SI ON A PASSER
		if(pilonne[2].position + OBSTACLE_VERT.w*SCALE_TO_FIT + PERSO.w*SCALE_TO_FIT < WINDOW_L/2 ){
			/////////////////////////////////////////////////////
			// ON TRAIT LE PILONNE SUIVANT
			traitement = 2;
			// UPDATE SCORE AVEC SECURITER
			if(changeProtectedVar(score_hash, score, (*score)+1, keys))
				Mix_PlayChannel( 6, score_wav,0);
				// LE JOUEUR EST REPORTER
			/*else
				ban commands*/
		}

	}
	/////////////////////////////////////////////////////
	// ICI ON BOUCLERA ENTRE PILONNE 3 ET 4
	/////////////////////////////////////////////////////
	// TRAITEMENT DU PILONNE 3
	else if ( traitement == 2){
		/////////////////////////////////////////////////////
		// ON VERIFIE SI ON A PASSER
		if(pilonne[3].position + OBSTACLE_VERT.w*SCALE_TO_FIT + PERSO.w*SCALE_TO_FIT < WINDOW_L/2 ){
			/////////////////////////////////////////////////////
			// ON TRAIT LE PILONNE SUIVANT
			traitement = 3;
			// UPDATE SCORE AVEC SECURITER
			if( changeProtectedVar(score_hash, score, (*score)+1, keys) )
				Mix_PlayChannel( 6, score_wav,0);
				// LE JOUEUR EST REPORTER
			/*else
				ban commands*/
		}

	}
	/////////////////////////////////////////////////////
	// TRAITEMENT DU PILONNE 4
	else if (traitement == 3 ){
		/////////////////////////////////////////////////////
		// TRAITEMENT DU PILONNE 3
		if(  !(pilonne[3].position + OBSTACLE_VERT.w*SCALE_TO_FIT + PERSO.w*SCALE_TO_FIT < WINDOW_L/2)  )
		{
			/////////////////////////////////////////////////////
			// ON TRAIT LE PILONNE PRECEDANT
			traitement = 2;
		}
	}


	/////////////////////////////////////////////////////
	for(int i=0; i<PRELOAD_POS_OBSTACLE;i++)

		pilonne[i].position -= VITESSE_DEPLACEMENT_DECOR;

	/////////////////////////////////////////////////////
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
	/////////////////////////////////////////////////////
	// permet de dire quand on sur l'axe d'un obstacle et de qu'elle obstacle
	// + detection collision
	for (int i = 0; i < PRELOAD_POS_OBSTACLE; i++) {
		/////////////////////////////////////////////////////
		// verifier qu'il ce trouve actuellement sur l'axe de deux bloque
		float gaucheh =  pilonne[i].position;
		float droiteh = pilonne[i].position + OBSTACLE_VERT.w*SCALE_TO_FIT;
		float hauth = (-TRANCHE_POS_OBSTACLE + (TRANCHE_POS_OBSTACLE/NB_POS_OBSTACLE)*pilonne[i].hauteur) + OBSTACLE_VERT.h*SCALE_TO_FIT;
		float bash = (-TRANCHE_POS_OBSTACLE + (TRANCHE_POS_OBSTACLE/NB_POS_OBSTACLE)*pilonne[i].hauteur) + OBSTACLE_VERT.h*SCALE_TO_FIT + DISTANCE_BETWEEN_OBSTACLE*SCALE_TO_FIT;

		if(emplacementPersonnage.x > gaucheh  && emplacementPersonnage.x < droiteh){
			int bas =( (  -TRANCHE_POS_OBSTACLE + (TRANCHE_POS_OBSTACLE/NB_POS_OBSTACLE)*pilonne[i].hauteur ) + OBSTACLE_VERT.h*SCALE_TO_FIT ) + PERSO.h*SCALE_TO_FIT/2;
			int haut = (  -TRANCHE_POS_OBSTACLE + (TRANCHE_POS_OBSTACLE/NB_POS_OBSTACLE)*pilonne[i].hauteur ) + OBSTACLE_VERT.h*SCALE_TO_FIT + DISTANCE_BETWEEN_OBSTACLE*SCALE_TO_FIT - PERSO.h*SCALE_TO_FIT + COMPENSATITION_HITBOX_DOWN;
			if(! (emplacementPersonnage.y > bas && emplacementPersonnage.y < haut)  )
				collision_detecter = 1;
		}
		else if( emplacementPersonnage.x  + (PERSO.w*SCALE_TO_FIT)/2 > gaucheh &&  emplacementPersonnage.x  - (PERSO.w*SCALE_TO_FIT)/2 < droiteh){
			if( sqrt( pow(emplacementPersonnage.x - gaucheh , 2) + pow(emplacementPersonnage.y- hauth  , 2)) <= (PERSO.h*SCALE_TO_FIT)/2.
			|| sqrt( pow( emplacementPersonnage.x - gaucheh, 2) + pow(emplacementPersonnage.y - bash , 2)) <= (PERSO.h*SCALE_TO_FIT)/2.
			|| sqrt( pow( emplacementPersonnage.x - droiteh, 2) + pow( emplacementPersonnage.y - hauth, 2)) <= (PERSO.h*SCALE_TO_FIT)/2.
			|| sqrt( pow( emplacementPersonnage.x - droiteh, 2) + pow( emplacementPersonnage.y - bash, 2)) <= (PERSO.h*SCALE_TO_FIT)/2.){
                collision_detecter = 1;
			}
		}

	}
	/////////////////////////////////////////////////////
	// DETECTION DU SOL
	if(emplacementPersonnage.y + PERSO.h*SCALE_TO_FIT > WINDOW_H - SOL.h*SCALE_TO_FIT){
		collision_detecter = 1;
	}
	/////////////////////////////////////////////////////
	// JOUER UN SON SI COLLISION
	if (collision_detecter)
		Mix_PlayChannel( 7, hurt_wav,0);

	return collision_detecter;
}



void traitementVariableAnimation(int *varAnimationPersonnage,int *varAnimationSol)
{
	// ON IMCREMTNTE L IMAGE CHOISI DE L OISEAU
	*varAnimationPersonnage += 1;
	if(*varAnimationPersonnage > 3 )
		*varAnimationPersonnage = 1;

	// ON FAIT AVANCER L IMAGE DU SOL
	if( ( *varAnimationSol * -1) >= ( SOL.w*SCALE_TO_FIT)*3  )
		*varAnimationSol = 0;
	else
		*varAnimationSol -= VITESSE_DEPLACEMENT_DECOR;
}


int attendreAvantDepart(Mix_Chunk *flap_wav, int* rdyToSpace)
{
	// attendre le toucher de la barre espace pour demarrer

	while ( 1 )
	{
		SDL_Event spacebar;

		while( SDL_PollEvent(&spacebar) )
		{


			if(spacebar.type == SDL_QUIT)
				return SDL_FALSE;


			if(*rdyToSpace && spacebar.type == SDL_KEYDOWN && spacebar.key.keysym.sym == SDLK_SPACE ){
				// jouer le son de saut //
				Mix_PlayChannel( 5, flap_wav,0);
				*rdyToSpace = SDL_FALSE;
				return SDL_TRUE;
			}
			else if (spacebar.type == SDL_KEYUP && spacebar.key.keysym.sym == SDLK_SPACE ){
				*rdyToSpace = SDL_TRUE;
			}

			if(spacebar.key.keysym.sym == SDLK_ESCAPE ){
				return SDL_FALSE;
			}

		}
	}


}


int evenement(int *upper,int *vitesseGraviter, int *nb_boucle,Mix_Chunk *flap_wav, int *rdyToSpace)
{
	SDL_Event ev;
	while ( SDL_PollEvent(&ev) )
	{
		if(ev.type == SDL_QUIT)
			return SDL_FALSE;

		if (ev.type == SDL_KEYDOWN){

			if(ev.key.keysym.sym == SDLK_SPACE)
			{
				if(*rdyToSpace){
					// JOUER SON DU SAUT
					Mix_PlayChannel( 5, flap_wav,0);
					// AJOUTER UN SAUT
					*upper = UPPER_STEP;
					// REMETTRE A 0 LA BOUCLE ET LA VITESSE DE GRAVITER
					*vitesseGraviter = 0;
					*nb_boucle = 0;
					*rdyToSpace = SDL_FALSE;
				}


			}
			else if(ev.key.keysym.sym == SDLK_ESCAPE)
			{
					return SDL_FALSE;
			}

		}
		else if(ev.type == SDL_KEYUP && ev.key.keysym.sym == SDLK_SPACE)
		{
			*rdyToSpace = SDL_TRUE;
		}

	}
	return SDL_TRUE;
}



void updateVariableEnvironement(SDL_Point *emplacementPersonnage, int *upper, double *angle, int *nb_boucle, int* vitesseGraviter)
{
	/////////////////////////////////////////////////////
	// SI ON MONTE
	if ( *upper > 0 ){
		/////////////////////////////////////////////////////
		// ON REDUIT LA VITESSE DE MONTER
		emplacementPersonnage->y -= ( UPPER_BY_STEP - ( *upper ) )/ (FPS/30);
		// ON REDUIT LA BOUCLE DE MONTER
		*upper -= 1;
		/////////////////////////////////////////////////////
		// ON REDUIT L'ANGLE DE MONTER
		if( *angle > ANGLE_UP)
		{
			/////////////////////////////////////////////////////
			// ON REDUIT L ANGLE 2 FOIS PLUS VITE SI > 0
			if (*angle > 0)
				*angle -= ANGLE_VITESSE*2 ;
			else
				*angle -= ANGLE_VITESSE ;
		}
	}
	/////////////////////////////////////////////////////
	// SI IL MONTE PAS ON FAIT JOUER JUSTE LA GRAVITER
	else if ( *nb_boucle% (FPS/30) == 0 )
	{
			/////////////////////////////////////////////////////
			// LE FAIRE DESCENDRE
			emplacementPersonnage->y += ( *vitesseGraviter += GRAVITY_SPEED ) / (FPS/30);
			// REDUIRE L'ANGLE
			if ( *angle < ANGLE_DOWN)
				*angle += ANGLE_VITESSE / 2;
		}
		/////////////////////////////////////////////////////
		// AJOUTER UNE BOUCLE
		*nb_boucle += 1;
}
