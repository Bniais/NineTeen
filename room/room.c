// CODERUNNER COMPILEFLAG
// -std=c99 -framework OpenGL -framework GLUT -lassimp -lm -F/Library/Frameworks -framework SDL2

// GLOBAL LIBRARY
#include <stdio.h>
#ifdef __APPLE__
	#define GL_SILENCE_DEPRECATION
	#include <OpenGL/gl.h>
	#include <OpenGL/OpenGL.h>
	#include <GLUT/glut.h>
#endif
#ifdef __linux
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glut.h>
  #define M_PI 3.1415
#endif
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <math.h>

// LOCAL LIBRARY
#include "import.h" // YOU NEED ASSIMP LIB FOR import.h (README.dm)
#include "room.h"
#include "../include/libWeb.h"
#include "../games/2_snake/snake.h"
#include "../games/3_flappy_bird/flappy_bird.h"
#include "../games/5_tetris/tetris.h"
// END INCLUDE

//
// SON
#define DIR_SON_ENIRONNEMENT_1 "room/borne1.wav"
#define DIR_SON_ENIRONNEMENT_2 "room/borne2.wav"
#define DIR_SON_ENIRONNEMENT_3 "room/borne3.wav"
#define DIR_SON_ENIRONNEMENT_WALK "room/walk.wav"
// POLICE
#define DIR_FONT_SEGA "room/sega.ttf"
#define DIR_FONT_POLICE "room/police.ttf"
//OBJS
#define DIR_OBJ_LOAD "room/salle.obj"
// EMPLACEMENT FIHCIER NECESSAIRE

#define VITESSE_DEPLACEMENT_DEBOUT 0.09F
#define VITESSE_DEPLACEMENT_ACCROUPI 0.035F
#define SENSIBILITE_CAMERA 0.08F
#define HAUTEUR_CAMERA_DEBOUT 3.5F
#define HAUTEUR_CAMERA_ACCROUPI 2.7F
#define MAX_Y_AXE_CIBLE 2.8F

#define START_PX 0.0F //AXE X
#define START_PY 3.5F // hauteur CAMERA
#define START_PZ 19.0F // AXE Y
#define START_CIBLE_Y 0.0F // REGARDE SUR AXE HAUTEUR
#define START_CIBLE_X M_PI// REGARDE SUR AXE X
#define START_OUVERTURE 70.0F //OUVERTURE CAMERA


const int SON_PAS = 5;
float HAUTEUR_PERSONNAGE = HAUTEUR_CAMERA_DEBOUT;



#define MIN_INTENSITE 50 // INTENSITE SON STEREO

float VITESSE_DEPLACEMENT = VITESSE_DEPLACEMENT_DEBOUT;
float HAUTEUR_CAMERA = HAUTEUR_CAMERA_DEBOUT;


#define RATIO_WINDOW_SCREEN 0.93
static int WinWidth = 1280;
static int WinHeight = 720;


//////////////////////////////////////////////////
// FIXER NOMBRE FPS MAX
#define FPS 60
// EN DEDUIRE LE DELAI MIN D'UNE FRAME
static const float FRAME_TIME = 1000/FPS;


SDL_Color Text_rouge = {255,0,0};


// lier au son
#define NB_INDICE_PORTER 2
#define MAX_VOLUME_ARCADE 80

// STATIC VAR FOR CAMERA
struct Camera_s
{
	float px,py,pz,cible_py,angle,ouverture;
};



enum { SCORE,FLAPPY_HARD,TETRIS_HARD,ASTEROID_HARD,PACMAN_HARD,SNAKE_HARD,DEMINEUR_HARD,DEMINEUR_EASY,SNAKE_EASY,PACMAN_EASY,ASTEROID_EASY,FLAPPY_EASY,TETRIS_EASY};



/////////////////////////////////////////////////////
/// \fn int room(char *token,struct MeilleureScore_s meilleureScore[], SDL_Window *Window,const C_STRUCT aiScene* scene )
/// \brief fonction d'appel principal
///
/// \param char *token cle de connexion
/// \param struct MeilleureScore_s meilleureScore[] tableau charger des scores
/// \param SDL_Window *Window fenetre initaliser
/// \param const C_STRUCT aiScene* scene scene charger
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int room(char *token,struct MeilleureScore_s meilleureScore[], SDL_Window *Window,const C_STRUCT aiScene* scene );



/////////////////////////////////////////////////////
/// \fn int windowMaxSize()
/// \brief fonction qui charge fixe la taille max de la fenetre
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int windowMaxSize();


/////////////////////////////////////////////////////
/// \fn void limiterFrame(const float delayLancementFrame,float *_IPS)
/// \brief permet de mettre un nombre d'image max et reglger la vitesse de deplacement en fonction des ips
///
/// \param const float delayLancementFrame temps au lancement de la frame
/// \param float *_IPS mise a jour de la constante IPS
///
/// \return void
/////////////////////////////////////////////////////
void limiterFrame(const float delayLancementFrame,float *_IPS);

/////////////////////////////////////////////////////
/// \fn int attendreXsecondeMessage(int *compterSeconde, int *afficherMessage,const int MS, const float _IPS)
/// \brief permet d'attendre avant d'afficher message CLIGNOTANT affiche egalement les IPS
///
/// \param int *compterSeconde delai d'attente mesurer
/// \param int *afficherMessage booleen vrai ou faux
/// \param const int MS delai d'attente
/// \param const float _IPS affichage des IPS
///
/// \return VRAI/FAUX
/////////////////////////////////////////////////////
int attendreXsecondeMessage(int *compterSeconde, int *afficherMessage,const int MS, const float _FPS);

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//// FUNCTION LIER A L'ENVIRONNEMENT ////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

/////////////////////////////////////////////////////
/// \fn int detectionEnvironnement(float x,float y)
/// \brief fonction de collision avec l'environement
///
/// \param float x coordonner x
/// \param float y coordonner y
///
/// \return TRUE/FALSE
/////////////////////////////////////////////////////
int detectionEnvironnement(float x,float y);


/////////////////////////////////////////////////////
/// \fn detecterMachine(float x,float y,float angle)
/// \brief fonction qui detecte la proxmiter avec une machine de jeu
///
/// \param float x coordonner x
/// \param float y coordonner y
/// \param float angle angle
///
/// \return TRUE/FALSE
/////////////////////////////////////////////////////
int detecterMachine(float x,float y,float angle);


/////////////////////////////////////////////////////
/// \fn void mixerInit()
/// \brief initalisation du volume
///
/////////////////////////////////////////////////////
void mixerInit();


/////////////////////////////////////////////////////
/// \fn float distancePoint(float xa, float ya, float xb, float yb)
/// \brief renvoi la distance entre deux point
///
/// \param float xa
/// \param float ya
/// \param float xb
/// \param float yb
///
/// \return float distance point a et b
/////////////////////////////////////////////////////
float distancePoint(float xa, float ya, float xb, float yb);



/////////////////////////////////////////////////////
/// \fn void reglageVolume(int channel, float xa, float ya, float xb, float yb, int porter)
/// \brief regle le volume de l'environement sur un chainnel particuliere
///
/// \param int channel canal du son a regler
/// \param float xa
/// \param float ya
/// \param float xb
/// \param float yb
/// \param int porter Porter du son
///
/// \return void
/////////////////////////////////////////////////////
void reglageVolume(int channel, float xa, float ya, float xb, float yb, float porter,float angleJoueur);


/////////////////////////////////////////////////////
/// \fn void bruitagePas(struct Camera_s *dernierePosition, struct Camera_s camera, int channel, Mix_Chunk *music)
/// \brief permet de deplacer la source du son des pas et de le jouer
///
/// \param struct Camera_s *dernierePosition derniere position ou le son a etait jouer
/// \param struct Camera_s camera position actuel
/// \param struct int channel channel sur le quelle jouer le son
/// \param struct Mix_Chumk *music son a jouer
/// \param float angleJoueur angle de la camera du joueur
///
/// \return void
/////////////////////////////////////////////////////
void bruitagePas(struct Camera_s *dernierePosition, struct Camera_s camera, int channel, Mix_Chunk *music);

/////////////////////////////////////////////////////
/// \fn float calculAngle(float xa, float ya,      float xb, float yb,       float xc, float yc)
/// \brief renvoi l'angle entre 3 points
///
/// \param float xa
/// \param float ya
/// \param float xb
/// \param float yb
/// \param float xc
/// \param float yc
///
/// \return void
/////////////////////////////////////////////////////
float calculAngle(float xa, float ya,      float xb, float yb,       float xc, float yc);


/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
////   FUNCTION LIER A L'AFFICHAGE   ////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////




/////////////////////////////////////////////////////
/// \fn void GL_InitialiserParametre(int width, int height, struct Camera_s camera)
/// \brief regle le mode de vue voulu et les lummiere de opengl
///
/// \param int width
/// \param int height
/// \param struct Camera_s camera
///
/// \return void
/////////////////////////////////////////////////////
void GL_InitialiserParametre(int width, int height, struct Camera_s camera);


/////////////////////////////////////////////////////
/// \fn void InitCamera(struct Camera_s *camera, struct Camera_s *cible)
/// \brief initialisation des variables lier au camera joueur et machines
///
/// \param struct Camera_s *camera
/// \param struct Camera_s *cible
///
/// \return void
/////////////////////////////////////////////////////
void InitCamera(struct Camera_s *camera, struct Camera_s *cible);


/////////////////////////////////////////////////////
/// \fn void mouvementCamera(struct Camera_s *camera,const float IPS)
/// \brief Permet de gerer les deplacements de la camera
///
/// \param struct Camera_s *camera prend l'adresse de la camera
/// \param
///
/// \return void
/////////////////////////////////////////////////////
void mouvementCamera(struct Camera_s *camera, const float IPS);


/////////////////////////////////////////////////////
/// \fn void SDL_GL_AppliquerScene(const C_STRUCT aiScene *scene,struct Camera_s *camera,GLuint *scene_list)
/// \brief permet d'appliquer la scene dans la window + mouvement camera
///
/// \param const C_STRUCT aiScene *scene scene ou est stocker notre obj
/// \param struct Camera_s *camera mise a jour de LookAt
/// \param GLuint *scene_list Nombre de scene
///
/// \return void
/////////////////////////////////////////////////////
void SDL_GL_AppliquerScene(const C_STRUCT aiScene *scene,struct Camera_s *camera,GLuint *scene_list, const float IPS);


/////////////////////////////////////////////////////
/// \fn void AfficherText(TTF_Font *font, char *message, SDL_Color color, int x, int y)
/// \brief affichage de text sur un rendu OpenGL
///
/// \param TTF_Font *font police d'ecriture du texte
/// \param char *message message a afficher
/// \param SDL_Color color couleur du texte
/// \param int x position du text sur X -1 = centrer
/// \param int y position du text sur Y -1 = centrer
///
/// \return void
/////////////////////////////////////////////////////
void AfficherText(TTF_Font *font, char *message, SDL_Color color, int x, int y);

/////////////////////////////////////////////////////
/// \fn void messageMachine(struct MeilleureScore_s str[],struct Camera_s camera,TTF_Font *font,int afficherMessage)
/// \brief affichage un message propre a chaque machine
///
/// \param struct MeilleureScore_s str[] structure stockant les nom et score des meilleure joueur par machine
/// \param struct Camera_s camera Camera du joueur pour detection des machines
/// \param TTF_Font *font pointeur sur la police
/// \param int afficherMessage afficher messages clignotant ou nom
///
/// \return void
/////////////////////////////////////////////////////
void messageMachine(struct MeilleureScore_s str[],struct Camera_s camera,TTF_Font *font,int afficherMessage);


/////////////////////////////////////////////////////
/// \fn void MessageQuitterRoom()
/// \brief permet d'afficher le message avant de quitter
///
///
/////////////////////////////////////////////////////
void MessageQuitterRoom();

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
////   FUNCTION LIER A L'ANIMATION   ////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////



/////////////////////////////////////////////////////
/// \fn void lancerMachine(const C_STRUCT aiScene *scene,int *Running, struct Camera_s camera, struct Camera_s cible[],char *token, struct MeilleureScore_s meilleureScore[],GLuint *scene_list,SDL_Window *Window,SDL_GLContext *Context);
/// \brief permet de lancer une machine de jeux comprend plusieur fonction annexe
///
/// \param const C_STRUCT aiScene *scene permet de passer la scene a re afficher au retour de la machine
/// \param int *Running permet de mettre fin au jeux
/// \param struct Camera_s camera camera du joueur
/// \param struct Camera_s cible[] camera cible machine
/// \param char *token cle du joueur pour mode en ligne
/// \param struct MeilleureScore_s meilleureScore[] strucuteur des score et nom meilleure joueur
/// \param GLuint *scene_list liste des scenes
/// \param SDL_Window *Window fenetre sdl/opengl
/// \param SDL_GLContext *Context context sdl/opengl
///
/// \return void
/////////////////////////////////////////////////////
void lancerMachine(const C_STRUCT aiScene *scene,int *Running, struct Camera_s camera, struct Camera_s cible[],char *token, struct MeilleureScore_s meilleureScore[],GLuint *scene_list,SDL_Window *Window,SDL_GLContext *Context);


/////////////////////////////////////////////////////
/// \fn void animationLancerMachine(struct Camera_s camera, struct Camera_s cible,GLuint scene_list,SDL_Window *Window)
/// \brief permet d'animer le lancement et retour des machines
///
/// \param struct Camera_s camera camera de depart
/// \param struct Camera_s cible camera de fin
/// \param GLuint scene_list scene
/// \param SDL_Window *Window vue d'affichae
///
/// \return void
/////////////////////////////////////////////////////
void animationLancerMachine(struct Camera_s camera, struct Camera_s cible,GLuint scene_list,SDL_Window *Window);






/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
////  FUNCTION LIER AU DONNER RESEAU ////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////



/////////////////////////////////////////////////////
/// \fn void InitMeilleureScore(struct MeilleureScore_s str[])
/// \brief initialise la strcuture MeilleureScore_s avec le nom des jeux
///
/// \param struct MeilleureScore_s str[] tableau de donner
///
/////////////////////////////////////////////////////
void InitMeilleureScore(struct MeilleureScore_s str[]);


/////////////////////////////////////////////////////
/// \fn void updateMeilleureScore(struct MeilleureScore_s str[] ,char *token)
/// \brief Mes a jours les donnees liee au score de la structure meilleureScore
///
/// \param struct MeilleureScore_s str[] tableau de donner
///
/////////////////////////////////////////////////////
void updateMeilleureScore(struct MeilleureScore_s str[] ,char *token);





int room(char *token,struct MeilleureScore_s meilleureScore[],SDL_Window *Window, const C_STRUCT aiScene* scene)
{
	//////////////////////////////////////////////////////////
	// RECUPERER C'EST VALEUR DES PARAMS A L'AVENIR
	windowMaxSize();
	//////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////
	// OpenGL
	//////////////////////////////////////////////////////////
	// INITIALISATION LIST SCENE
	GLuint scene_list = 0; // NB SCENE
	//////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////
	// INITIALISATION CAMERA / CIBLE SUR MACHINE /
	static struct Camera_s camera,cible[15],jouerSon;
	InitCamera(&camera,cible);
	jouerSon = camera;
	//////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////
	// INITALISATION SON
	mixerInit();
	// CHARGER LES MUSIQUES D"AMBIANCE
	Mix_Chunk *music_01	= Mix_LoadWAV(DIR_SON_ENIRONNEMENT_1);
	if( !music_01 )
	{
		printf("Erreur de chargement son %s\n",DIR_SON_ENIRONNEMENT_1);
		return EXIT_FAILURE;
	}

	Mix_Chunk *music_02	= Mix_LoadWAV(DIR_SON_ENIRONNEMENT_2);
	if( !music_02 )
	{
		printf("Erreur de chargement son %s\n",DIR_SON_ENIRONNEMENT_2);
		return EXIT_FAILURE;
	}

	Mix_Chunk *music_03	= Mix_LoadWAV(DIR_SON_ENIRONNEMENT_3);
	if( !music_03 )
	{
		printf("Erreur de chargement son %s\n",DIR_SON_ENIRONNEMENT_3);
		return EXIT_FAILURE;
	}

	Mix_Chunk *music_walk	= Mix_LoadWAV(DIR_SON_ENIRONNEMENT_WALK);
	if( !music_walk )
	{
		printf("Erreur de chargement son %s\n",DIR_SON_ENIRONNEMENT_WALK);
		return EXIT_FAILURE;
	}
	//////////////////////////////////////////////////////////
	// LANCER LES MUSIQUE D'AMBIANCE //
	Mix_PlayChannel(0 , music_01, -1);
	Mix_PlayChannel(1 , music_02, -1);
	Mix_PlayChannel(2 , music_03 , -1);
	//////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////
	// INITALISATION DES POLICES
	TTF_Font * font = TTF_OpenFont(DIR_FONT_POLICE, WinWidth/50);
	if(!font)
	{
		printf("Erreur chargement font %s\n",DIR_FONT_POLICE);
		return EXIT_FAILURE;
	}
	TTF_Font * sega = TTF_OpenFont(DIR_FONT_SEGA, WinWidth/50);
	if(!sega)
	{
		printf("Erreur chargement font %s\n",DIR_FONT_SEGA);
		return EXIT_FAILURE;
	}
	//////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////
	//
	/*WinWidth = 2304;
	WinHeight = 1296;*/
	Window = SDL_CreateWindow("Nineteen", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, WinWidth, WinHeight, SDL_WINDOW_OPENGL );
	//////////////////////////////////////////////////////////
	// VERIFIER EXISTANCE DE LA FENETRE ET CREATION CONTEXT
	if( !Window)
	{
		printf("Impossible de cree la fenetre %s\n",SDL_GetError() );
		return EXIT_FAILURE;
	}
	//////////////////////////////////////////////////////////
	// CREATION DE L'ICON D'APPLICATION
	SDL_Surface *favicon;
	if( ( favicon=IMG_Load("favicon.png") ) )
		SDL_SetWindowIcon(Window, favicon);
	else
		printf("Erreur chargement favicon\n");
	SDL_FreeSurface(favicon);

	SDL_GLContext Context = SDL_GL_CreateContext(Window);
	if( !Context)
	{
		printf("Impossible de cree le context %s\n",SDL_GetError() );
		return EXIT_FAILURE;
	}
	//////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////
	// VARIABLE DE DEROULEMENT
	int Running = 1;
	int compterSeconde = SDL_GetTicks();
	int afficherMessage = 0;
	float _IPS = FPS;
	//////////////////////////////////////////////////////////



	while (Running)
	{
		int delayLancementFrame = SDL_GetTicks();

		//////////////////////////////////////////////////////////
		// REGLAGE SON ENVIRONEMENT AVEC LEUR POSITION
		// MUSIQUE LOT MACHINE GAUCHE
		reglageVolume(0,-5.0,11.0,camera.px,camera.pz,10.0,camera.angle);
		// MUSIQUE LOT MACHINE DROITE
		reglageVolume(1,5.0,11.0,camera.px,camera.pz,10.0,camera.angle);
		// MUSIQUE MACHINE SEUL
		reglageVolume(2,0.0,0.0,camera.px,camera.pz,15.0,camera.angle);
		//////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////
		// JOUER SON BRUIT DE PAS QUAND C'EST NECESSAIRE
		bruitagePas(&jouerSon,camera,3,music_walk);
		//////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////
		// OpenGL
		//////////////////////////////////////////////////////////
		// APPLIQUER PARAMETRE D'ORIGINE OPENGL
		GL_InitialiserParametre(WinWidth,WinHeight,camera);
		//////////////////////////////////////////////////////////
		// CHARGER LA SCENE
		SDL_GL_AppliquerScene(scene,&camera,&scene_list,_IPS);
		//////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////
		// OpemGL Text
		//////////////////////////////////////////////////////////
		// AFFICHER CLASSEMENT / SCORE EN HAUT A GAUCHE
		AfficherText(sega,meilleureScore[0].nomJeux,Text_rouge,WinWidth/30,WinHeight - WinWidth/30);
		AfficherText(sega,meilleureScore[0].nomJoueur,Text_rouge,WinWidth/30,WinHeight - WinWidth/15);
		//////////////////////////////////////////////////////////
		// AFFICHAGE MESSAGE A PROXIMITER DES MACHINES
		messageMachine(meilleureScore,camera,sega,afficherMessage);
		//////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////
		// LANCEMENT DES MACHINES
		lancerMachine(scene,&Running,camera,cible,token,meilleureScore,&scene_list,Window,&Context);
		//////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////
		// OpenGL
		//////////////////////////////////////////////////////////
		// APPLIQUER MODIF SUR LA VUE
		SDL_GL_SwapWindow(Window);

		//////////////////////////////////////////////////////////
		// LIMITER LES FRAMES A CONST FPS
		limiterFrame(delayLancementFrame,&_IPS);
		//////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////
		// ATTENDRE 1000 MS POUR MESSAGE CLIGNOTANT
		attendreXsecondeMessage(&compterSeconde, &afficherMessage,1000, _IPS);
		//////////////////////////////////////////////////////////

	}


	//////////////////////////////////////////////////////////
	// LIBERER LA MEMOIRE ALLOUER ET NON NECESSAIRE A LA SUITE
	// LIBERATION DES SONS
	Mix_FreeChunk(music_01 );
	Mix_FreeChunk(music_02 );
	Mix_FreeChunk(music_03 );
	Mix_FreeChunk(music_walk );
	//////////////////////////////////////////////////////////
	// LIBERATION DES POLICES
	TTF_CloseFont(font);
	TTF_CloseFont(sega);
	//////////////////////////////////////////////////////////
	// LIBERATION DU CONTEXT
	SDL_GL_DeleteContext(Context);
	//////////////////////////////////////////////////////////
	return EXIT_SUCCESS;
}


















































void InitMeilleureScore(struct MeilleureScore_s str[])
{
	////////////////////////////////////////////////////////////////
	// PERMET DE REMPLIR LE TABLEAU DES SCORES DE JEUX PAR LE NOM DES JEUX
	// FUNCTION APPELER DANS LAUNCHER UNIQUEMENT
	// PRESENTE ICI CAR ELLE EST UTILISER UNIQUEMENT ICI CETTE STRCUTURE

	strcpy(str[FLAPPY_HARD].nomJeux,"FLAPPY   BIRD");

	strcpy(str[TETRIS_HARD].nomJeux,"TETRIS");

	strcpy(str[ASTEROID_HARD].nomJeux,"ASTEROID");

	strcpy(str[PACMAN_HARD].nomJeux,"PACMAN");

	strcpy(str[SNAKE_HARD].nomJeux,"SNAKE");

	strcpy(str[DEMINEUR_HARD].nomJeux,"DEMINEUR");

	strcpy(str[DEMINEUR_EASY].nomJeux,"DEMINEUR");

	strcpy(str[SNAKE_EASY].nomJeux,"SNAKE");

	strcpy(str[PACMAN_EASY].nomJeux,"PACMAN");

	strcpy(str[ASTEROID_EASY].nomJeux,"ASTEROID");

	strcpy(str[FLAPPY_EASY].nomJeux,"FLAPPY   BIRD");

	strcpy(str[TETRIS_EASY].nomJeux,"TETRIS");


	strcpy(str[13].nomJeux,"COMMING   SOON");
	strcpy(str[14].nomJeux,"COMMING   SOON");
	strcpy(str[15].nomJeux,"COMMING   SOON");
}



void updateMeilleureScore(struct MeilleureScore_s str[] ,char *token)
{
	///////////////////////////////////////////////////////////
	// INITALISATION D'UNE CHAINE POUR STOCKER LA REPONSE
	char reponse[1024];

	///////////////////////////////////////////////////////////
	// RECUPERATION DU SCORE JUSQU'A REUSSITE
	while( getCoinsValues(token,reponse) == EXIT_FAILURE );

	///////////////////////////////////////////////////////////
	// PARSING DANS LA CHAINE DE DONNER RECU
	int temp1,temp2;
	sscanf(reponse,"%d %d / %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d", &str[0].score,&temp1,&temp2,
																																str[1].nomJoueur,&str[1].score,
																																str[2].nomJoueur,&str[2].score,
																																str[3].nomJoueur,&str[3].score,
																																str[4].nomJoueur,&str[4].score,
																																str[5].nomJoueur,&str[5].score,
																																str[6].nomJoueur,&str[6].score,
																																str[7].nomJoueur,&str[7].score,
																																str[8].nomJoueur,&str[8].score,
																																str[9].nomJoueur,&str[9].score,
																																str[10].nomJoueur,&str[10].score,
																																str[11].nomJoueur,&str[11].score,
																																str[12].nomJoueur,&str[12].score,
																																str[13].nomJoueur,&str[13].score,
																																str[14].nomJoueur,&str[14].score,
																																str[15].nomJoueur,&str[15].score
																															);

		///////////////////////////////////////////////////////////
		// REMPLISSAGE DE L'EMPLACEMENT 0 PAR MEILLEURE SCORE ET CLASSEMENT DU JOUEUR
		sprintf(str[0].nomJeux,"SCORE : %d",str[0].score);
		sprintf(str[0].nomJoueur,"CLASSEMENT : %d / %d",temp1,temp2);

}



void mixerInit()
{
		// REGLER VOLUME DES PISTES AUDIO
		Mix_Volume(0,0);
		Mix_Volume(1,0);
		Mix_Volume(2,0);
		Mix_Volume(3,SON_PAS);
		Mix_Volume(4,MIX_MAX_VOLUME/5);
		Mix_Volume(5,MIX_MAX_VOLUME/5);
		Mix_Volume(6,MIX_MAX_VOLUME/5);
}




int windowMaxSize()
{
	/////////////////////////////////////////////////////
	// CREATION VARIABLE
	SDL_Rect dm;

	/////////////////////////////////////////////////////
	// RECUPRATION DE LA TAILLE D'ECRAN SI CA ECHOU ON RECUPERER L'ERREUR
	if (SDL_GetDisplayUsableBounds(0,&dm) != 0)
	{
		SDL_Log("SDL_GetDisplayUsableBounds failed: %s", SDL_GetError());
		return EXIT_FAILURE;
	}

	dm.w *= RATIO_WINDOW_SCREEN;
	dm.h *= RATIO_WINDOW_SCREEN;

	/////////////////////////////////////////////////////
	// ON APPLIQUE A NOTRE VARIABLE GLOBALE
	if((float)dm.w/dm.h > 16/9.){
		dm.w = 16 * dm.h / 9.;
	}
	else if((float)dm.w/dm.h < 16/9.){
		dm.h = 9 * dm.w / 16.;
	}

	WinWidth = dm.w;
	WinHeight = dm.h;


	printf("SIZE : %d %d\n", WinWidth, WinHeight);

	return EXIT_SUCCESS;
}



float distancePoint(float xa, float ya, float xb, float yb)
{
	/////////////////////////////////////////////////////
	// PERMET DE CALCULER LA DISTANCE ENTRE 2 POINTS
	float resultat = sqrt( pow( (xb - xa) , 2 ) + pow( (yb - ya) , 2 ) ) ;
	 	return resultat;
}


void limiterFrame(const float delayLancementFrame,float *_IPS)
{
	float delayFrame = SDL_GetTicks() - delayLancementFrame;
	// MISE A JOURS DU NOMBRE DE FPS
	*_IPS = 1000.0/delayFrame;
	// ATTENDRE SI NECESSAIRE POUR LIMITER A 60 FPS
	if(delayFrame < FRAME_TIME)
	{
		SDL_Delay(FRAME_TIME - delayFrame );
		// MAJ IPS APRES ATTENTE
		*_IPS = 1000.0/ ( delayFrame + (FRAME_TIME - delayFrame) );
	}

}

int attendreXsecondeMessage(int *compterSeconde, int *afficherMessage, const int MS, const float _IPS)
{
	if(SDL_GetTicks() - *compterSeconde > MS){
		*compterSeconde = SDL_GetTicks();
		*afficherMessage = !(*afficherMessage);
		// AFFICHER NOMBRE IPS
		printf("IPS : %f\n",_IPS );
	}
	return *afficherMessage;
}

float calculAngle(float xa, float ya,      float xb, float yb,       float xc, float yc)
{
	// ON A LE TRIANGLE A B C
	// ON VEUT CALCULER L ANGLE BAC
	// AVEC A SOURCE DU SON
	// B EMPLACEMENT PERSONNAGES
	// C POINT SUR LE MEME AXE X QUE A SOURCE DU SON

	float numerateur = yb*(xa-xc) + ya*(xc-xb) + yc*(xb-xa);
	float denominateur = (xb-xa)*(xa-xc) + (yb-ya)*(ya-yc);

	float ratio = numerateur/denominateur;

	float angleRad = atan(ratio);


	if(xa<xb)
	{
		if(ya<yb)
		{
			// COMPENSATION DEVANT A DROIT DE LA SOURCE
			angleRad += 2*M_PI;
		}
		else
		{
			// COMPENSATION DERRIERE A DROIT DE LA SOURCE
			angleRad += M_PI;
		}
	}
	else
	{
		if(ya<yb)
		{
			// PAS DE COMPENSATION NECESSAIRE DEVANT GAUCHE
		}
		else
		{
			// COMPENSATION DERRIERE A GAUCHE DE LA SOURCE
			angleRad += M_PI;
		}
	}

	return (2*M_PI) - angleRad;
}

void reglageVolume(int channel, float xa, float ya, float xb, float yb, float porter, float angleJoueur)
{
	////////////////////////////////////////////////////
	// FIX VOLUME MAX PAR DEFAULT
	float volume = MIX_MAX_VOLUME;
	// CALCUL DISTANCE SOURCE SONOR
	float distance = distancePoint(xa,ya,xb,yb);
	////////////////////////////////////////////////////

	////////////////////////////////////////////////////
	// MET LE SON A 0 SI TROP LOIN
	if(distance > porter)
		volume = 0;
	else
	{
		////////////////////////////////////////////////////
		// REGLE LE SON EN FONCTION DE LA DISTANCE
		float _distance = porter - distance;
		volume = ( volume /porter) * _distance;
	}

	////////////////////////////////////////////////////
	// APPLIQUE CE QUI A ETAIT FAIT
	Mix_Volume(channel, (int)volume);



	////////////////////////////////////////////////////
	// RETOURNE ANGLE SOURCE/PERSONNAGES
	float angleSrc = calculAngle(xa,ya,  xb,yb,  xa,ya - 1.0);

	////////////////////////////////////////////////////
	// VARIABLES AVEC ANGLE POUR OREILLES G/D
	float angleOreilleD = angleJoueur - M_PI/4;
	float angleOreilleG = angleJoueur + M_PI/4;

	////////////////////////////////////////////////////
	// CALCUL DU DELTA ENTRE ANGLE
	float deltaAngleD = fabs(angleSrc + M_PI - angleOreilleD); //
	float deltaAngleG = fabs(angleSrc + M_PI - angleOreilleG);


	while(deltaAngleD >M_PI)
		deltaAngleD-=2*M_PI;
	while(deltaAngleG >M_PI)
	 	deltaAngleG-=2*M_PI;

	////////////////////////////////////////////////////
	// REDUCTION LINEAIRE EN FONCTION DU DELTA
	int intensiteOreilleD = 255 - fabs(deltaAngleD)*(255-MIN_INTENSITE)/(M_PI);
	int intensiteOreilleG = 255 - fabs(deltaAngleG)*(255-MIN_INTENSITE)/(M_PI);

	////////////////////////////////////////////////////
	// APPLICATION DU SON STEREO
	if(!Mix_SetPanning(channel, intensiteOreilleG, intensiteOreilleD))
		printf("Mix_SetPanning: %s\n", Mix_GetError());



}



void bruitagePas(struct Camera_s *dernierePosition, struct Camera_s camera, int channel, Mix_Chunk *music)
{
	////////////////////////////////////////////////////
	// CALCUL LA DISTANCE ENTRE LA DERNIERE POSITION ET CELLE ACTUELLE
	if ( distancePoint(dernierePosition->px,dernierePosition->pz, camera.px, camera.pz) > 1.6F )
	{
		////////////////////////////////////////////////////
		// SI ON A PARCOURU 1.6M ON JOUE UN SONS
		// MISE A JOUR DE LA DERNIERE POSITION OU ON A JOUER LE SON
		*dernierePosition = camera;
		Mix_PlayChannel(channel,music,0);
	}
}



void SDL_GL_AppliquerScene(const C_STRUCT aiScene *scene,struct Camera_s *camera,GLuint *scene_list,const float IPS)
{
	////////////////////////////////////////////////////
	// REGLE COULEUR DE FOND NETTOIE LE DERNIERE AFFICHAGE
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// REGLE EN MODE MODELVIEW
	glMatrixMode(GL_MODELVIEW);
	// CHARGE LES REGLAGES
	glLoadIdentity();


	////////////////////////////////////////////////////
	// GERER LES MOUVEMENT DE CAMERA
	mouvementCamera(camera,60.0);

	////////////////////////////////////////////////////
	// SI LE RENDU DE LA SCENE N'EST PAS FAIT LE FAIRE
	if(*scene_list == 0) {
		// FIXER LA SCENE A 1
		*scene_list = glGenLists(1);
		glNewList(*scene_list, GL_COMPILE);
		aiDessinerScene(scene, scene->mRootNode);
		glEndList();
	}

	// APPELER LA LISTE DE SCENE CREE
	glCallList(*scene_list);
}


void GL_InitialiserParametre(int width, int height, struct Camera_s camera)
{

	///////////////////////////////////////////////////
	//ON CLEAR
	glClearColor (0.0, 0.0, 0.0, 0.0);
	// ACTIVATION DU LISSAGE
	glShadeModel (GL_SMOOTH);

	///////////////////////////////////////////////////
	// NIVEAU DE LUSTRAGE 0 - 128
	GLfloat mat_shininess[] = { 50.0 };
	// APPLICATION DU LUSTRAGE
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	// POSITION DE LA LUMIERE SUR LE PERSONNAGES
	GLfloat light_position0[] = { camera.px, camera.pz, 0.0, 1.0 };
	// POSITIONNEMENT LUMIERE
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);


	///////////////////////////////////////////////////
	// ACTIVATION DES LUMIERES
	glEnable(GL_LIGHTING);
	// ACTIVATION DE LA LUMIERE 0
	glEnable(GL_LIGHT0);
	// ACTIVATION DE LA DETECTION D'OBJET L'UN DEVANT L'AUTRE POUR NE PAS TOUS AFFICHER
	glEnable(GL_DEPTH_TEST);

	///////////////////////////////////////////////////
	// MOD PROJECTION
	glMatrixMode( GL_PROJECTION );
	// CHARGEMENT DE LA MATRICE
	glLoadIdentity();

	///////////////////////////////////////////////////
	// REGALGES DU NIVEAU DE PERSPECTIVE
	// REGLAGES DE LA DISTANCE DE VUE MIN MAX
	gluPerspective(camera.ouverture,(float)(width)/(float)(height),0.2,500);

	////////////////////////////////////////////////////
	// ACTIVATION DU MODE MODEL VIEW
	glMatrixMode( GL_MODELVIEW );
	// CHARGEMENT DE LA MATRICE
	glLoadIdentity();

	// ACTIVATION DES TEXTURES 2D
	glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
	// CHARGEMENT DE LA MATRICE
	glLoadIdentity();
}




void InitCamera(struct Camera_s *camera, struct Camera_s *cible)
{
	///////////////////////////////////////////////////
	// POSITION DU DEPART DU PERSONNAGE
	camera->px = START_PX;
	camera->py = START_PY;
	camera->pz = START_PZ;
	camera->cible_py = START_CIBLE_Y;
	camera->angle = START_CIBLE_X;
	camera->ouverture = START_OUVERTURE;


	///////////////////////////////////////////////////
	// PLACEMENT DES CAMERA FIX SUR LES MACHINES
	cible[FLAPPY_HARD-1].px = -6.56;
	cible[FLAPPY_HARD-1].pz = 12.9;
	cible[FLAPPY_HARD-1].py = 3.45;
	cible[FLAPPY_HARD-1].cible_py = -.34;
	cible[FLAPPY_HARD-1].angle = M_PI;
	cible[FLAPPY_HARD-1].ouverture =70;

	cible[TETRIS_HARD-1].px = -4.62;
	cible[TETRIS_HARD-1].pz = 12.85;
	cible[TETRIS_HARD-1].py = 3.45;
	cible[TETRIS_HARD-1].cible_py = -.34;
	cible[TETRIS_HARD-1].angle = M_PI;
	cible[TETRIS_HARD-1].ouverture =70;

	cible[ASTEROID_HARD-1].px = -2.68;
	cible[ASTEROID_HARD-1].pz = 12.85;
	cible[ASTEROID_HARD-1].py = 3.45;
	cible[ASTEROID_HARD-1].cible_py = -.34;
	cible[ASTEROID_HARD-1].angle = M_PI;
	cible[ASTEROID_HARD-1].ouverture =70;

	cible[PACMAN_HARD-1].px = -6.56;
	cible[PACMAN_HARD-1].pz = 9.25;
	cible[PACMAN_HARD-1].py = 3.45;
	cible[PACMAN_HARD-1].cible_py = -.34;
	cible[PACMAN_HARD-1].angle = 0.0;
	cible[PACMAN_HARD-1].ouverture =70;

	cible[SNAKE_HARD-1].px = -4.62;
	cible[SNAKE_HARD-1].pz = 9.25;
	cible[SNAKE_HARD-1].py = 3.45;
	cible[SNAKE_HARD-1].cible_py = -.34;
	cible[SNAKE_HARD-1].angle = 0;
	cible[SNAKE_HARD-1].ouverture =70;

	cible[DEMINEUR_HARD-1].px = -2.68;
	cible[DEMINEUR_HARD-1].pz = 9.25;
	cible[DEMINEUR_HARD-1].py = 3.45;
	cible[DEMINEUR_HARD-1].cible_py = -.34;
	cible[DEMINEUR_HARD-1].angle = 0;
	cible[DEMINEUR_HARD-1].ouverture =70;

	cible[DEMINEUR_EASY-1].px = 2.68;
	cible[DEMINEUR_EASY-1].pz = 12.85;
	cible[DEMINEUR_EASY-1].py = 3.45;
	cible[DEMINEUR_EASY-1].cible_py = -.34;
	cible[DEMINEUR_EASY-1].angle = M_PI;
	cible[DEMINEUR_EASY-1].ouverture =70;

	cible[SNAKE_EASY-1].px = 4.62;
	cible[SNAKE_EASY-1].pz = 12.85;
	cible[SNAKE_EASY-1].py = 3.45;
	cible[SNAKE_EASY-1].cible_py = -.34;
	cible[SNAKE_EASY-1].angle = M_PI;
	cible[SNAKE_EASY-1].ouverture =70;

	cible[PACMAN_EASY-1].px = 6.56;
	cible[PACMAN_EASY-1].pz = 12.85;
	cible[PACMAN_EASY-1].py = 3.45;
	cible[PACMAN_EASY-1].cible_py = -.34;
	cible[PACMAN_EASY-1].angle = M_PI;
	cible[PACMAN_EASY-1].ouverture =70;

	cible[ASTEROID_EASY-1].px = 2.71;
	cible[ASTEROID_EASY-1].pz = 9.25;
	cible[ASTEROID_EASY-1].py = 3.45;
	cible[ASTEROID_EASY-1].cible_py = -.34;
	cible[ASTEROID_EASY-1].angle = 0.0;
	cible[ASTEROID_EASY-1].ouverture =70;

	cible[FLAPPY_EASY-1].px = 4.65;
	cible[FLAPPY_EASY-1].pz = 9.25;
	cible[FLAPPY_EASY-1].py = 3.45;
	cible[FLAPPY_EASY-1].cible_py = -.34;
	cible[FLAPPY_EASY-1].angle = 0;
	cible[FLAPPY_EASY-1].ouverture =70;

	cible[TETRIS_EASY-1].px = 6.59;
	cible[TETRIS_EASY-1].pz = 9.25;
	cible[TETRIS_EASY-1].py = 3.45;
	cible[TETRIS_EASY-1].cible_py = -.34;
	cible[TETRIS_EASY-1].angle = 0;
	cible[TETRIS_EASY-1].ouverture =70;

	cible[12].px = -13.10;
	cible[12].pz = 10.50;
	cible[12].py = 3.45;
	cible[12].cible_py = -.34;
	cible[12].angle = 3*M_PI/2;
	cible[12].ouverture =70;

	cible[13].px = -13.10;
	cible[13].pz = 12.44;
	cible[13].py = 3.45;
	cible[13].cible_py = -.34;
	cible[13].angle = 3*M_PI/2;
	cible[13].ouverture =70;

	cible[14].px = 0.0;
	cible[14].pz = 0.82;
	cible[14].py = 4.65;
	cible[14].cible_py = -.34;
	cible[14].angle = M_PI;
	cible[14].ouverture =70;
}

/* ---------------------------------------------------------------------------- */


void mouvementCamera(struct Camera_s *camera, const float IPS)
{

	///////////////////////////////////////////////////
	// REGLAGE EN FONCTION DES FPS FIXER
	// VALEUR BASER SUR 60 FPS DE BASE
	float _SENSIBILITE_CAMERA = SENSIBILITE_CAMERA * (IPS/FPS);
	float _VITESSE_DEPLACEMENT = VITESSE_DEPLACEMENT * (IPS/FPS);


	///////////////////////////////////////////////////
	// INITALISATION D'UN EVENEMENT SDL
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);

	///////////////////////////////////////////////////
	// MOUVEMENT CAMERA SUR X ET Z


	///////////////////////////////////////////////////
	// APUI FLECHE GAUCHE
	if( keystate[SDL_SCANCODE_LEFT] )
	{
		camera->angle += _SENSIBILITE_CAMERA;

		///////////////////////////////////////////////////
		// PERMET DE REMETRE A VALEUR COMPRISE ENTRE 0 et 2*M_PI L'ANGLE CAMERA
		if( camera->angle > 2 * M_PI)
			camera->angle -= 2*M_PI;
	}

	///////////////////////////////////////////////////
	// APUI FLECHE DROITE
	if( keystate[SDL_SCANCODE_RIGHT] )
	{
		camera->angle -= _SENSIBILITE_CAMERA;

		///////////////////////////////////////////////////
		// PERMET DE REMETRE A VALEUR COMPRISE ENTRE 0 et 2*M_PI L'ANGLE CAMERA
		if( camera->angle < 0)
			camera->angle += 2*M_PI;
	}

	///////////////////////////////////////////////////
	// APUI FLECHE HAUT
	if( keystate[SDL_SCANCODE_UP] )

				///////////////////////////////////////////////////
				// VERIFIE SI ON A ATTEIND LA VALEUR MAX
				if(camera->cible_py + _SENSIBILITE_CAMERA < MAX_Y_AXE_CIBLE )
					camera->cible_py += _SENSIBILITE_CAMERA;

	///////////////////////////////////////////////////
	// APUI FLECHE BAS
	if( keystate[SDL_SCANCODE_DOWN] )
				///////////////////////////////////////////////////
				// VERIFIE SI ON A ATTEIND LA VALEUR MAX
				if(camera->cible_py - _SENSIBILITE_CAMERA > -MAX_Y_AXE_CIBLE )
					camera->cible_py -= _SENSIBILITE_CAMERA;




	///////////////////////////////////////////////////
	// MOUVEMENT PERSONNAGES SUR X et Z

	///////////////////////////////////////////////////
	// APUI TOUCHE W
	if( keystate[SDL_SCANCODE_W] )
	{

		///////////////////////////////////////////////////
		// VERIFIE SI LE NOUVEAU DEPLACEMENT RENTRE TOUJOURS DANS LES DIMENSIONS DE LA SALLE
		if( detectionEnvironnement( (camera->px + _VITESSE_DEPLACEMENT *sin(camera->angle) ),
															( camera->pz +  _VITESSE_DEPLACEMENT *cos(camera->angle) ) )   )
		{
					///////////////////////////////////////////////////
					// CONDITION VERIFIER ON DEPLACE X ET Y EN FONCTION DE L' ANGLE
					// ON AVANCE
					camera->px += _VITESSE_DEPLACEMENT *sin(camera->angle);
					camera->pz += _VITESSE_DEPLACEMENT *cos(camera->angle);
		}

	}


	if( keystate[SDL_SCANCODE_S] )
	{

		///////////////////////////////////////////////////
		// VERIFIE SI LE NOUVEAU DEPLACEMENT RENTRE TOUJOURS DANS LES DIMENSIONS DE LA SALLE
		if(	detectionEnvironnement(  camera->px - _VITESSE_DEPLACEMENT *sin(camera->angle),
									 							camera->pz - _VITESSE_DEPLACEMENT *cos(camera->angle)   )	)
		{
				///////////////////////////////////////////////////
				// CONDITION VERIFIER ON DEPLACE X ET Y EN FONCTION DE L'ANGLE
				// ON RECULE
				camera->px -= _VITESSE_DEPLACEMENT *sin(camera->angle);
				camera->pz -= _VITESSE_DEPLACEMENT *cos(camera->angle);
		}

	}
	if( keystate[SDL_SCANCODE_A] )
	{
		///////////////////////////////////////////////////
		// VERIFIE SI LE NOUVEAU DEPLACEMENT RENTRE TOUJOURS DANS LES DIMENSIONS DE LA SALLE
		if( detectionEnvironnement(  camera->px + _VITESSE_DEPLACEMENT *sin(camera->angle + M_PI/2),
									 								camera->pz + _VITESSE_DEPLACEMENT *cos(camera->angle + M_PI/2)		  )		)
		{
				///////////////////////////////////////////////////
				// CONDITION VERIFIER ON DEPLACE X ET Y EN FONCTION DE L'ANGLE
				// ON VAS AU GAUCHE
				camera->px += _VITESSE_DEPLACEMENT *sin(camera->angle + M_PI/2);
				camera->pz += _VITESSE_DEPLACEMENT *cos(camera->angle + M_PI/2);
		}


	}
	if( keystate[SDL_SCANCODE_D] )
	{
		///////////////////////////////////////////////////
		// VERIFIE SI LE NOUVEAU DEPLACEMENT RENTRE TOUJOURS DANS LES DIMENSIONS DE LA SALLE
		if(  detectionEnvironnement(  camera->px - _VITESSE_DEPLACEMENT *sin(camera->angle + M_PI/2),
									 								camera->pz - _VITESSE_DEPLACEMENT *cos(camera->angle + M_PI/2)		  )		)
		{
			///////////////////////////////////////////////////
			// CONDITION VERIFIER ON DEPLACE X ET Y EN FONCTION DE L'ANGLE
			// ON VAS A DROITE
			camera->px -= _VITESSE_DEPLACEMENT *sin(camera->angle + M_PI/2);
			camera->pz -= _VITESSE_DEPLACEMENT *cos(camera->angle + M_PI/2);
		}

	}



	///////////////////////////////////////////////////
	// ON VERIFIE SI ON EST SUR LE BLOQUE SUR ELEVER CENTRAL
	// SI C'EST LE CAS ON AUGMENTE LA HAUTEUR DE LA CAMERA
	if (camera->px <= 4 && camera->px >= -4 && camera->pz <= 4 && camera->pz >= -4)
		camera->py = 1.2F + HAUTEUR_CAMERA;
	else
		camera->py = HAUTEUR_CAMERA;




	// MISE A JOURS DE LA POSITION DE LA CAMERA
	gluLookAt(camera->px                   ,camera->py    ,camera->pz                  ,
			  camera->px+sin(camera->angle) ,camera->py + camera->cible_py    , camera->pz+cos(camera->angle),
			  0.0
			               ,1.0         ,0.0)                        ;
}

int detectionEnvironnement(float x,float y)
{

	///////////////////////////////////////////////////
	// HITBOX
	///////////////////////////////////////////////////

	///////////////////////////////////////////////////
	// ACCUEIL NINETEEN
	if( x <= 5.0 && x >= -5.0 && y >= 20.0)
		return 0;

	///////////////////////////////////////////////////
	// LOT 12 MACHINE MILIEU DE SALLE
	if( y >= 8.5 && y <= 13.5 && (  (x <= 8.0 && x >= 1.5) || (x > -8.0 && x <= -1.5)   )   )
		return 0;

	///////////////////////////////////////////////////
	// DOUBLE MACHINE A GAUCHE DE LA SALLE
	if( x <= -12.5 && y <= 13.5 && y >= 9.0)
		return 0;

	///////////////////////////////////////////////////
	// MACHINE SEUL CENTRAL
	if( x >= -1.5 && x <= 1.5 && y >= -1.5 && y <= 1.5)
		return 0;

	///////////////////////////////////////////////////
	// FUNTION POUR MUR AVEC ANGLE A PROXIMITER DE NINETEEN
	if(  y >= 14.0 &&  (    ( x >= -0.4736842105*y + 19.36842105  ) || (  x <= -(-0.4736842105*y + 19.36842105))    )   )
		return 0;

	///////////////////////////////////////////////////
	// OUVERTURE ENTRE POUR ALLER DANS LA SALLE TOILETTE
	if( (y >= 7.0 || y <= 2.0) && x >= 14.5 && x <= 15.5)
		return 0;

	///////////////////////////////////////////////////
	// MUR QUI SEPARE LA SALLE ET LES TOILETTES
	if( x >= 18.0 && x <= 19.0 && y >= -0.5 && y <= 10.0 )
		return 0;

	///////////////////////////////////////////////////
	// MACHINE VOITURE
	if ( x >= 4.0 && x <= 10.0 && y <= 3.6 && y >= - 3.6)
		return 0;

	///////////////////////////////////////////////////
	// BILLARD
	if( x <= -7.5 && x >= -12.0 && y <= 5.0 && y >= -2.3)
		return 0;

	///////////////////////////////////////////////////
	// DIMESION SALLE DES TOILETTE
	if(x > 15.0 && (  y < -3.0 || y > 12.5  )  )
		return 0;

	///////////////////////////////////////////////////
	// SEPARATION ENTRE TOILETTE
	if( x > 21.0 && x < 22.0 && y < 9.0 && y > 0.5)
		return 0;

	///////////////////////////////////////////////////
	// SEPARATION AXE Y TOILETTE
	if( x > 18.0 && y > 4.5 && y < 5.5)
		return 0;


	///////////////////////////////////////////////////
	// DIMESION SALLE DE BASE
	// HAUT DE LA SALLE
	if( y < -5.0 )
		return 0;
	// BAS DE LA SALLE
	if( y > 24.0 )
		return 0;
	// GAUCHE DE LA SALLE
	if( x < -14.5 )
		return 0;
	// DROIT DE LA SALLE
	if ( x > 24.0 )
		return 0;
	return 1;
}



int detecterMachine(float x,float y,float angle)
{
	///////////////////////////////////////////////////
	// DETECTION MACHINE
	///////////////////////////////////////////////////
	// DETECTER RANGER 6 MACHINE A GAUCHE
	if(x > -8.0 && x < -1.0 && y > 7.0 && y < 15.0)
	{
		///////////////////////////////////////////////////
		// DETECTER SI RANGER NORD / SUD
		if( y > 11.0)
		{
			///////////////////////////////////////////////////
			// DETECTER SI ON A UN ANGLE MAX DE 60 DEGRES
			if(angle > M_PI - (M_PI/3) && angle < M_PI + (M_PI/3) )
			{
				///////////////////////////////////////////////////
				// DETECTER PRECICEMENT LA MACHINE PARMIS LES 3
				if( x < -5.7 )
					return 1;
				else if ( x < -3.7)
					return 2;
				else
					return 3;
			}
		}
		///////////////////////////////////////////////////
		// RANGER SUD
		else
		{
			///////////////////////////////////////////////////
			// DETECTER SI ON A UN ANGLE MAX DE 60 DEGRES
			if( ( angle > ( 2*M_PI - (M_PI/3) ) && angle <= 2*M_PI  ) ||    (    angle >= 0 && angle < 0 + (M_PI/3)   )   )
			{
				///////////////////////////////////////////////////
				// DETECTER PRECICEMENT LA MACHINE PARMIS LES 3
				if( x < -5.7 )
					return 4;
				else if ( x < -3.7)
					return 5;
				else
					return 6;
			}
		}
	}


	///////////////////////////////////////////////////
	// DETECTER RANGER 6 MACHINE A DROITE
	if(x < 8.0 && x > 1.0 && y > 7.0 && y < 15.0)
	{
		///////////////////////////////////////////////////
		// DETECTER SI RANGER NORD / SUD
		if( y > 11.0) // vrai = face
		{
			///////////////////////////////////////////////////
			// DETECTER SI ON A UN ANGLE MAX DE 60 DEGRES
			if(angle > M_PI - (M_PI/3) && angle < M_PI + (M_PI/3) )
			{
				if( x < 3.4 )
					return 7;
				else if ( x < 5.8)
					return 8;
				else
					return 9;
			}
		}
		///////////////////////////////////////////////////
		// RANGER SUD
		else
		{
			///////////////////////////////////////////////////
			// DETECTER SI ON A UN ANGLE MAX DE 60 DEGRES
			if( ( angle > ( 2*M_PI - (M_PI/3) ) && angle <= 2*M_PI  ) ||    (    angle >= 0 && angle < 0 + (M_PI/3)   )   )
			{
				if( x < 3.4 )
					return 10;
				else if ( x < 5.8)
					return 11;
				else
					return 12;
			}
		}
	}

	///////////////////////////////////////////////////
	// DETECTER 2 MACHINE A GAUCHE
	if( x < -11 && y > 9.0 && y < 13.5)
	{
		///////////////////////////////////////////////////
		// DETECTER SI ON A UN ANGLE MAX DE 60 DEGRES
		if(angle > 3*M_PI/2 - (M_PI/3) && angle < 3*M_PI/2 + (M_PI/3))
		{
			if(y < 11.25)
				return 13;
			else
				return 14;
		}
	}

	///////////////////////////////////////////////////
	// DETECTER MACHINE CENTRAL
	if( x > -1.0 && x < 1.0 && y > 1.0 && y < 3.0)
		///////////////////////////////////////////////////
		// DETECTER SI ON A UN ANGLE MAX DE 60 DEGRES
		if(angle > M_PI - (M_PI/3) && angle < M_PI + (M_PI/3) )
			return 15;



	return 0;
}

void messageMachine(struct MeilleureScore_s str[],struct Camera_s camera,TTF_Font *font,int afficherMessage)
{
	///////////////////////////////////////////////////////////////////
	// RETOURNE LE CODE DE LA MACHINE DETECTER 0 SI NON DETECTER
	int detection = detecterMachine(camera.px, camera.pz,camera.angle);
	if(detection)
	{
		////////////////////////////////////////////////
		SDL_Color white = {255,255,255};
		// BUFFER POUR LE MESSAGE DES RECORDS
		char message[60];
		// CONCATENATION DES SCORES ET NOM DES JOUEUR DANS UNE MEME CHAINE
		sprintf(message, "RECORD   :  %d  PAR  %s",str[detection].score,str[detection].nomJoueur);
		// AFFICHAGE DU TEXT DU RECORD
		AfficherText(font,message,white,-1,WinHeight/8);
		// AFFICHAGE DU TEXT DU NOM DU JEU
		AfficherText(font,str[detection].nomJeux,white,-1,WinHeight/6);

		////////////////////////////////////////////////
		// AFFICHAGE CLIGNOTANT
		if(afficherMessage)
				AfficherText(font,"APPUYER   SUR   E",white,-1,-1);

	}
}


void animationLancerMachine(struct Camera_s camera, struct Camera_s cible,GLuint scene_list,SDL_Window *Window)
{
	// FIXER DUREE ANIMATION
	float DUREE_ANIM = 60.0F;

	////////////////////////////////////////////////////////////
	// CALCUL DES DIFFERENTE VALEUR A INCREMENTER
	float x = (cible.px - camera.px)/DUREE_ANIM ;
	float z = (cible.pz - camera.pz)/DUREE_ANIM ;
	float cib = (cible.cible_py - camera.cible_py) / DUREE_ANIM;
	float y = (cible.py - camera.py)/DUREE_ANIM;


	float angle = cible.angle - camera.angle;
	////////////////////////////////////////////////////////////
	// CORRECTION DES ANGLES NEGATIF POUR INCREMENTATION VUE QUE A 2 PI RETOUR A 0
	if( angle < -M_PI){
		cible.angle = 2*M_PI;
		angle = cible.angle - camera.angle;
	}
	////////////////////////////////////////////////////////////
	// CORRECTION DES ANGLES POSITIF POUR INCREMENTATION VUE QUE A 2 PI RETOUR A 0
	else if ( angle > M_PI)
		angle -= 2*M_PI;
	////////////////////////////////////////////////////////////
	angle /= DUREE_ANIM ;

	////////////////////////////////////////////////////////////
	// DEBUT ANIM
	int i = 0;
	while( i++ < DUREE_ANIM)
	{
		////////////////////////////////////////////////////////////
		// INCREMENTATION
		camera.px += x;
		camera.pz += z;
		camera.cible_py += cib;
		camera.py += y;
		camera.angle += angle;


		// CHARGEMENT DE LA MATRICE
	  glLoadIdentity();
		// RESOULTION POUR AFFICHAGE 2D
	  gluOrtho2D(0, WinWidth, 0, WinHeight);
		// INIT GL PARAMS
		GL_InitialiserParametre(WinWidth,WinHeight,camera);

		// NETTOYAGE DE LA FENETRE
		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		// MISE A JOURS DE LA CAMERA
		gluLookAt(camera.px                   ,camera.py    ,camera.pz                  ,
				  camera.px+sin(camera.angle) ,camera.py + camera.cible_py    , camera.pz+cos(camera.angle),
				  0.0
				               ,1.0         ,0.0)                        ;

		// APPEL LIST SCENE
		glCallList(scene_list);
		// APPLICATION A LA WINDOW
		SDL_GL_SwapWindow(Window);

	}

}

void lancerMachine(const C_STRUCT aiScene *scene,int *Running, struct Camera_s camera, struct Camera_s cible[], char *token, struct MeilleureScore_s meilleureScore[],GLuint *scene_list,SDL_Window *Window,SDL_GLContext *Context)
{
	///////////////////////////////////////////////////
	// GESTION EVENEMENT
	SDL_Event Event;
	while (SDL_PollEvent(&Event))
	{
		///////////////////////////////////////////////////
		// EVENEMENT APPUI TOUCHE
		if (Event.type == SDL_KEYDOWN)
		{
			switch (Event.key.keysym.sym)
			{
				///////////////////////////////////////////////////
				// TOUCHE ESPACE METTRE FIN AU JEUX
				case SDLK_ESCAPE:
				{
						///////////////////////////////////////////////////
						// INIT AFFICHAGE DU MESSAGE
						GL_InitialiserParametre(WinWidth,WinHeight,camera);
						SDL_GL_AppliquerScene(scene,&camera,scene_list,FPS);
						MessageQuitterRoom();
						SDL_GL_SwapWindow(Window);

						///////////////////////////////////////////////////
						// VIDER LA LISTE DES EVENEMENTS
						do
      			{
         				SDL_WaitEvent(&Event);
      			}
      			while (Event.key.keysym.sym != SDLK_ESCAPE);


						///////////////////////////////////////////////////
						// DECISION PRISE
						int decision = 1;
						while(decision)
						{
							while (SDL_PollEvent(&Event))
							{
								switch (Event.key.keysym.sym)
								{
									case SDLK_ESCAPE:
										decision = 0;
										printf("Commande annuler\n");
										break;
									case SDLK_q:
										decision = 0;
										printf("Vous quittez\n");
										*Running = 0;
										break;
									case SDLK_d:
										decision = 0;
										printf("Vous vous deconnecter\n");
										FILE *fp = fopen("/tmp/.Nineteen","w");
										fclose(fp);
										*Running = 0;
										break;
									default:break;
								}
							}
						}
						break;
				}
				///////////////////////////////////////////////////
				// TOUCHE E ENTRER DANS UN JEUX
				case SDLK_e:
				{
						///////////////////////////////////////////////////
						// VERIFIER SI ON EST PROCHES D UNE MACHINE
						// RENVOIE LE CODE DE LA MACHINE
						int machine = detecterMachine(camera.px, camera.pz, camera.angle);
						if ( machine)
						{
							///////////////////////////////////////////////////
							// ANIMATION CENTRAGE SUR MACHINE
							animationLancerMachine(camera,cible[machine-1],*scene_list,Window);

							///////////////////////////////////////////////////
							// CREATION D'UN RENDU AUTRE QUE OPENGL CAR NON COMPATIBLE
							SDL_Renderer *pRenderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC |SDL_RENDERER_TARGETTEXTURE);

							///////////////////////////////////////////////////
							// CASE POUR CHAQUE MACHINE
							// AVEC UPDATE DU SCORE A L ISSUS
							switch (machine) {
								case 1:
									printf( "\nEXIT CODE = %d\n" , flappy_bird( pRenderer, meilleureScore[FLAPPY_HARD].score,WinWidth,WinHeight,token,1));
									updateMeilleureScore(meilleureScore,token);
								break;
								case 2:
									tetris( pRenderer ,meilleureScore[TETRIS_HARD].score, 1920./WinWidth,token,1);
									updateMeilleureScore(meilleureScore,token);
									break;
								case 3: SDL_Delay(500);break;
								case 4: SDL_Delay(500);break;
								case 5:
									snake( pRenderer ,meilleureScore[SNAKE_HARD].score, WinWidth/1920.,token,1);
									updateMeilleureScore(meilleureScore,token);
								case 6: SDL_Delay(500);break;
								case 7: SDL_Delay(500);break;
								case 8:
									snake( pRenderer ,meilleureScore[SNAKE_EASY].score, WinWidth/1920.,token,0);
									updateMeilleureScore(meilleureScore,token);
								case 9: SDL_Delay(500);break;
								case 10: SDL_Delay(500);break;
								case 11: {
									printf( "\nEXIT CODE = %d\n" , flappy_bird( pRenderer, meilleureScore[FLAPPY_EASY].score,WinWidth,WinHeight,token,0));
									updateMeilleureScore(meilleureScore,token);
								}break;
								case 12:
									tetris( pRenderer ,meilleureScore[TETRIS_EASY].score, 1920./WinWidth,token,0);
									updateMeilleureScore(meilleureScore,token);
									break;
								case 13: SDL_Delay(500);break;
								case 14: SDL_Delay(500);break;
								case 15: SDL_Delay(500);break;
								default:break;
							}

							///////////////////////////////////////////////////
							// DESTRUCTION DU RENDU ET CONTEXT POUR RECREATION CONTEXT OPENGL
							SDL_DestroyRenderer(pRenderer);
							SDL_GL_DeleteContext(*Context);
							///////////////////////////////////////////////////
							*Context = SDL_GL_CreateContext(Window);
							///////////////////////////////////////////////////
							// REMISE A ZERO DE LA SCENE
							*scene_list = 0;
							// ATTENTE POUR MAC OS AFIN DE VOIR L'ANIMATION
							while(SDL_PollEvent(&Event));
							// AFFICHAGE DE LA SCENE
							SDL_GL_AppliquerScene(scene,&camera,scene_list,FPS);
							// ANIMATION DE RETOUR SUR MACHINE
							animationLancerMachine(cible[machine-1],camera,*scene_list,Window);
							// VIDER POLL EVENEMENT
							while(SDL_PollEvent(&Event));
						}
					}
					break;

				///////////////////////////////////////////////////
				// TOUCHE C CE METTRE A CROUPI
				case SDLK_c:
					///////////////////////////////////////////////////
					// MET LE JOUEUR A CROUPI ET REDUIT CA VITESSE DE DEPLACEMENT
					if(VITESSE_DEPLACEMENT == VITESSE_DEPLACEMENT_DEBOUT )
					{
						HAUTEUR_CAMERA = HAUTEUR_CAMERA_ACCROUPI;
						VITESSE_DEPLACEMENT = VITESSE_DEPLACEMENT_ACCROUPI;
					}
					else
					{
						HAUTEUR_CAMERA = HAUTEUR_CAMERA_DEBOUT;
						VITESSE_DEPLACEMENT = VITESSE_DEPLACEMENT_DEBOUT;
					}

					break;

				///////////////////////////////////////////////////
				// DEFAULT
				default:
					break;
			}
		}

		///////////////////////////////////////////////////
		// IF APPUI CROIS ROUGE
		else if (Event.type == SDL_QUIT)
			*Running = 0;

	}

}


void AfficherText(TTF_Font *font, char *message, SDL_Color color, int x, int y)
{
	////////////////////////////////////////////////
	// ENVOI MATRICE
	glPushMatrix();

	////////////////////////////////////////////////
	// DESACTIVER LES LUMIERE
	glDisable(GL_LIGHTING);
  glLoadIdentity();

	////////////////////////////////////////////////
	// PRECISION SUR LA FENETRE
  gluOrtho2D(0, WinWidth, 0, WinHeight);
	// MOD PROJECTION
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

	////////////////////////////////////////////////
	// DESACTIVATION DU TEST D ARRIERE PLAN
	glDisable(GL_DEPTH_TEST);
	glLoadIdentity();

	////////////////////////////////////////////////
	// BLEND
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// INIT LOAD TEXTURE
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

	////////////////////////////////////////////////
	// CREATION TEXTURE AVEC LE TEXT EN SDL
  SDL_Surface * sFont = TTF_RenderText_Blended(font, message, color);

	////////////////////////////////////////////////
	// PARAMETRE 2D
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// CONVERTION TEXTURE IMAGE
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sFont->w, sFont->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, sFont->pixels);

	////////////////////////////////////////////////
	// SI PARAMS X A -1 ON CENTRE LE TEXT SUR X
	if(x == -1)
		x = WinWidth/2 - sFont->w/2;
	////////////////////////////////////////////////
	// SI PARAMS X A -1 ON CENTRE LE TEXT SUR Y
	if(y == -1)
		y = WinHeight/2 - sFont->h/2;

	////////////////////////////////////////////////
	// DEBUT DU RENDU
  glBegin(GL_QUADS);
  {
    glTexCoord2f(0,0); glVertex2f(x, y);
    glTexCoord2f(1,0); glVertex2f(x + sFont->w, y);
    glTexCoord2f(1,-1); glVertex2f(x + sFont->w, y + sFont->h);
    glTexCoord2f(0,-1); glVertex2f(x, y + sFont->h);
  }
  glEnd();
	////////////////////////////////////////////////

	////////////////////////////////////////////////
	// DESTRUCTUIN DES ELLEMENTS CREE
  glDeleteTextures(1, &texture);
  SDL_FreeSurface(sFont);

	////////////////////////////////////////////////
	// RECUPERATION DE LA MATRICE AVANT MODIF
	glPopMatrix();
	glLoadIdentity();
}

void MessageQuitterRoom()
{
	////////////////////////////////////////////////
	// ENVOI MATRICE
	glPushMatrix();

	////////////////////////////////////////////////
	// DESACTIVER LES LUMIERE
	glDisable(GL_LIGHTING);
	glLoadIdentity();

	////////////////////////////////////////////////
	// PRECISION SUR LA FENETRE
	gluOrtho2D(0, WinWidth, 0, WinHeight);
	// MOD PROJECTION
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	////////////////////////////////////////////////
	// DESACTIVATION DU TEST D ARRIERE PLAN
	glDisable(GL_DEPTH_TEST);
	glLoadIdentity();

	////////////////////////////////////////////////
	// BLEND
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// INIT LOAD TEXTURE
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	////////////////////////////////////////////////
	// CREATION TEXTURE AVEC LE TEXT EN SDL
	// CHOISIR EN FONCTION DE 3 TAILLES D'ECRAN
	SDL_Surface *sImage = NULL;
	if( WinWidth > 2000)
	{
		sImage = IMG_Load("room/exit@3.png");
	}
	else if ( WinWidth > 1300)
	{
		sImage = IMG_Load("room/exit@2.png");
	}
	else
	{
		sImage = IMG_Load("room/exit.png");
	}

	////////////////////////////////////////////////
	// PARAMETRE 2D
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// CONVERTION TEXTURE IMAGE
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sImage->w , sImage->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, sImage->pixels);

	////////////////////////////////////////////////
	// SI PARAMS X A -1 ON CENTRE LE TEXT SUR X
	int x = WinWidth/2 - sImage->w/2;
	////////////////////////////////////////////////
	// SI PARAMS X A -1 ON CENTRE LE TEXT SUR Y
	int y = WinHeight/2 - sImage->h/2;

	////////////////////////////////////////////////
	// DEBUT DU RENDU
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0,0); glVertex2f(x, y);
		glTexCoord2f(1,0); glVertex2f(x + sImage->w, y);
		glTexCoord2f(1,-1); glVertex2f(x + sImage->w, y + sImage->h);
		glTexCoord2f(0,-1); glVertex2f(x, y + sImage->h);
	}
	glEnd();
	////////////////////////////////////////////////

	////////////////////////////////////////////////
	// DESTRUCTUIN DES ELLEMENTS CREE
	glDeleteTextures(1, &texture);
	SDL_FreeSurface(sImage);

	////////////////////////////////////////////////
	// RECUPERATION DE LA MATRICE AVANT MODIF
	glPopMatrix();
	glLoadIdentity();
}

/*

	void assimpInit(const char *filename) {
		int i;
		GLuint ivao = 0;

		// chargement du fichier //
		if ( chargementModel(filename) != 0){
			fprintf(stderr, "Erreur lors du chargement du fichier %s\n", filename);
		}

		if(getenv("MODEL_IS_BROKEN"))
			glFrontFace(GL_CW);

		_textures = malloc((_nbTextures = scene->mNumMaterials) * sizeof *_textures);
		assert(_textures);

		glGenTextures(_nbTextures, _textures);

			for (i = 0; i < scene->mNumMaterials ; i++) {
				const struct aiMaterial* pMaterial = scene->mMaterials[i];
				if (aiGetMaterialTextureCount(pMaterial, aiTextureType_DIFFUSE) > 0) {
					struct aiString tfname;
					char * dir = pathOf(filename), buf[BUFSIZ];
					if (aiGetMaterialTexture(pMaterial, aiTextureType_DIFFUSE, 0, &tfname, NULL, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
						SDL_Surface * t;
						snprintf(buf, sizeof buf, "%s/%s", dir, tfname.data);
						printf("Chargement de %s\n",tfname.data);
						if(!(t = IMG_Load(buf))) {
			 				fprintf(stderr, "Probleme de chargement de textures %s\n", buf);
			  				fprintf(stderr, "\tNouvel essai avec %s\n", tfname.data);
			  				if(!(t = IMG_Load(tfname.data)))
							{
								fprintf(stderr, "Probleme de chargement de textures %s\n", tfname.data); continue;
							}
						}

						glBindTexture(GL_TEXTURE_2D, _textures[i]);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
						#ifdef __APPLE__
							glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t->w, t->h, 0, t->format->BytesPerPixel == 3 ? GL_BGR : GL_BGRA, GL_UNSIGNED_BYTE, t->pixels);
						#else
							glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t->w, t->h, 0, t->format->BytesPerPixel == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, t->pixels);
						#endif
							SDL_FreeSurface(t);
					}
				}
			}

			_nbMeshes= sceneNbMeshes(scene, scene->mRootNode, 0);
			_vaos = malloc(_nbMeshes * sizeof *_vaos);
			assert(_vaos);
		//	glGenVertexArrays(_nbMeshes, _vaos);
			_buffers = malloc(2 * _nbMeshes * sizeof *_buffers);
			assert(_buffers);
			glGenBuffers(2 * _nbMeshes, _buffers);
			_counts = calloc(_nbMeshes, sizeof *_counts);
			assert(_counts);
		//	sceneMkVAOs(scene, scene->mRootNode, &ivao);
			rendu_Model(scene, scene->mRootNode);
	}
*/
