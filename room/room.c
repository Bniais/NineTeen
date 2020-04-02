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
#include <math.h>

// LOCAL LIBRARY
#include "import.h" // YOU NEED ASSIMP LIB FOR import.h (README.dm)
#include "room.h"
#include "../include/libWeb.h"
#include "../games/3_flappy_bird/flappy_bird.h"
// END INCLUDE

//
// SON
#define DIR_SON_ENIRONNEMENT_1 "room/01.wav"
#define DIR_SON_ENIRONNEMENT_2 "room/02.wav"
#define DIR_SON_ENIRONNEMENT_3 "room/03.wav"
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

float VITESSE_DEPLACEMENT = VITESSE_DEPLACEMENT_DEBOUT;
float HAUTEUR_CAMERA = HAUTEUR_CAMERA_DEBOUT;

static int WinWidth = 1280;
static int WinHeight = 720;

#define FPS 60
static const float FRAME_TIME = 1000/FPS;


SDL_Color Text_rouge = {255,0,0};


// lier au son
#define NB_INDICE_PORTER 2
#define MAX_VOLUME_ARCADE 80
typedef enum{ PORTER_10, PORTER_20 }PORTERS;
const float COEF_LOG[2][NB_INDICE_PORTER] = { {270, 200}, {-118, -67.5} };

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
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int room(char *token,struct MeilleureScore_s meilleureScore[], SDL_Window *Window,const C_STRUCT aiScene* scene );




void SDL_GL_AppliquerScene(const C_STRUCT aiScene *scene,struct Camera_s *camera,GLuint *scene_list);
void GL_InitialiserParametre(int width, int height, struct Camera_s camera);
void InitCamera(struct Camera_s *camera, struct Camera_s *cible);
void mouvementCamera(struct Camera_s *camera);
int detectionEnvironnement(float x,float y);
int detecterMachine(float x,float y);
void animationLancerMachine(struct Camera_s camera, struct Camera_s cible,GLuint scene_list,SDL_Window *Window);
void lancerMachine(const C_STRUCT aiScene *scene,int *Running, struct Camera_s camera, struct Camera_s cible[],char *token, struct MeilleureScore_s meilleureScore[],GLuint *scene_list,SDL_Window *Window,SDL_GLContext *Context);




void AfficherText(TTF_Font *font, char *message, SDL_Color color, int x, int y);

void windowMaxSize();


/////////////////////////////////////////////////////
/// \fn int mixerInit(Mix_Chunk **musicEnvironnement)
/// \brief charger et lancer les musiques d'ambiance
///
/// \param Mix_Chunk **musicEnvironnement tableau qui stock les son
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int mixerInit(Mix_Chunk **musicEnvironnement);


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
void reglageVolume(int channel, float xa, float ya, float xb, float yb, int porter);


/////////////////////////////////////////////////////
/// \fn void bruitagePas(struct Camera_s *dernierePosition, struct Camera_s camera, int channel, Mix_Chunk *music)
/// \brief permet de deplacer la source du son des pas et de le jouer
///
/// \param struct Camera_s *dernierePosition derniere position ou le son a etait jouer
/// \param struct Camera_s camera position actuel
/// \param struct int channel channel sur le quelle jouer le son
/// \param struct Mix_Chumk *music son a jouer
///
/// \return void
/////////////////////////////////////////////////////
void bruitagePas(struct Camera_s *dernierePosition, struct Camera_s camera, int channel, Mix_Chunk *music);


void InitMeilleureScore(struct MeilleureScore_s str[]);
void updateMeilleureScore(struct MeilleureScore_s str[] ,char *token);
void messageMachine(struct MeilleureScore_s str[],float x,float y,TTF_Font *font,int afficherMessage);










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
	Mix_Chunk **musicEnvironnement = malloc(sizeof(Mix_Chunk));
	mixerInit(musicEnvironnement);
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
	// VERIFIER EXISTANCE DE LA FENETRE ET CREATION CONTEXT
	if( !Window)
	{
		printf("Impossible de cree la fenetre %s\n",SDL_GetError() );
		return EXIT_FAILURE;
	}
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
	int count_IPS = SDL_GetTicks();
	float ips=0.0;
	int afficherMessage = 0;
	//////////////////////////////////////////////////////////



	while (Running)
	{
		int tempsLancerFrame = SDL_GetTicks();

		//////////////////////////////////////////////////////////
		// REGLAGE SON ENVIRONEMENT AVEC LEUR POSITION
		reglageVolume(0,-4.0,10.5,camera.px,camera.pz,PORTER_10);
		reglageVolume(1,4.0,10.5,camera.px,camera.pz,PORTER_10);
		reglageVolume(2,0.0,0.0,camera.px,camera.pz,PORTER_10);
		//////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////
		// JOUER SON BRUIT DE PAS QUAND C'EST NECESSAIRE
		bruitagePas(&jouerSon,camera,3,*(musicEnvironnement + 3));
		//////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////
		// OpenGL
		//////////////////////////////////////////////////////////
		// APPLIQUER PARAMETRE D'ORIGINE OPENGL
		GL_InitialiserParametre(WinWidth,WinHeight,camera);
		//////////////////////////////////////////////////////////
		// CHARGER LA SCENE
		SDL_GL_AppliquerScene(scene,&camera,&scene_list);
		//////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////
		// OpemGL Text
		//////////////////////////////////////////////////////////
		// AFFICHER CLASSEMENT / SCORE EN HAUT A GAUCHE
		AfficherText(sega,meilleureScore[0].nomJeux,Text_rouge,WinWidth/30,WinHeight - WinWidth/30);
		AfficherText(sega,meilleureScore[0].nomJoueur,Text_rouge,WinWidth/30,WinHeight - WinWidth/15);
		//////////////////////////////////////////////////////////
		// AFFICHAGE MESSAGE A PROXIMITER DES MACHINES
		messageMachine(meilleureScore,camera.px,camera.pz,sega,afficherMessage);
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
		// LIMITER LES FRAMES
		int frame_delay = SDL_GetTicks() - tempsLancerFrame;
			if(frame_delay < FRAME_TIME)
				SDL_Delay(FRAME_TIME - frame_delay );
		//////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////
		// AFFICHER LES FPS DANS LE TERMINAL
		ips++;
		if(SDL_GetTicks() - count_IPS > 1000){
			printf("IPS %f\n",ips );
			count_IPS = SDL_GetTicks();
			afficherMessage = !afficherMessage;
			ips = 0;
		}
		//////////////////////////////////////////////////////////

	}



	//////////////////////////////////////////////////////////
	// LIBERER LA MEMOIRE ALLOUER ET NON NECESSAIRE A LA SUITE
	// LIBERATION DES SONS
	Mix_FreeChunk(*(musicEnvironnement + 0) );
	Mix_FreeChunk(*(musicEnvironnement + 1) );
	Mix_FreeChunk(*(musicEnvironnement + 2) );
	Mix_FreeChunk(*(musicEnvironnement + 3) );
	free(musicEnvironnement);
	//////////////////////////////////////////////////////////
	// LIBERATION DES POLICES
	TTF_CloseFont(font);
	TTF_CloseFont(sega);
	//////////////////////////////////////////////////////////
	// LIBERATION DU CONTEXT
	SDL_GL_DeleteContext(Context);
	//////////////////////////////////////////////////////////
}




































void InitMeilleureScore(struct MeilleureScore_s str[])
{
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
	char reponse[1024];

	while( getCoinsValues(token,reponse) == EXIT_FAILURE );
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

		sprintf(str[0].nomJeux,"SCORE : %d",str[0].score);
		sprintf(str[0].nomJoueur,"CLASSEMENT : %d / %d",temp1,temp2);

}



int mixerInit(Mix_Chunk **musicEnvironnement)
{
		// REGLER VOLUME DES PISTES AUDIO
		Mix_Volume(0,0);
		Mix_Volume(1,0);
		Mix_Volume(2,0);
		Mix_Volume(3,SON_PAS);
		Mix_Volume(4,MIX_MAX_VOLUME/5);
		Mix_Volume(5,MIX_MAX_VOLUME/5);
		Mix_Volume(6,MIX_MAX_VOLUME/5);


		// CHARGER LES MUSIQUES D"AMBIANCE
		*(musicEnvironnement + 0)	= Mix_LoadWAV(DIR_SON_ENIRONNEMENT_1);
		if( !*(musicEnvironnement + 0) )
		{
			printf("Erreur de chargement son %s\n",DIR_SON_ENIRONNEMENT_1);
			return EXIT_FAILURE;
		}

		*(musicEnvironnement + 1)	= Mix_LoadWAV(DIR_SON_ENIRONNEMENT_2);
		if( !*(musicEnvironnement + 1) )
		{
			printf("Erreur de chargement son %s\n",DIR_SON_ENIRONNEMENT_2);
			return EXIT_FAILURE;
		}

		*(musicEnvironnement + 2)	= Mix_LoadWAV(DIR_SON_ENIRONNEMENT_3);
		if( !*(musicEnvironnement + 2) )
		{
			printf("Erreur de chargement son %s\n",DIR_SON_ENIRONNEMENT_3);
			return EXIT_FAILURE;
		}

		*(musicEnvironnement + 3)	= Mix_LoadWAV(DIR_SON_ENIRONNEMENT_WALK);
		if( !*(musicEnvironnement + 3) )
		{
			printf("Erreur de chargement son %s\n",DIR_SON_ENIRONNEMENT_WALK);
			return EXIT_FAILURE;
		}


		// LANCER LES MUSIQUE D'AMBIANCE //
		Mix_PlayChannel(0 , *(musicEnvironnement + 0), -1);
		Mix_PlayChannel(1 , *(musicEnvironnement + 1), -1);
		Mix_PlayChannel(2 , *(musicEnvironnement + 2) , -1);

		return EXIT_SUCCESS;
}




void windowMaxSize()
{
	SDL_DisplayMode dm;

	if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
	{
     SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());

	 }

WinWidth = dm.w*0.9;
WinHeight = dm.h*0.9;
printf("SIZE : %d %d\n", WinWidth, WinHeight);
}

float distancePoint(float xa, float ya, float xb, float yb)
{
	float resultat = sqrt( pow( (xb - xa) , 2 ) + pow( (yb - ya) , 2 ) ) ;
	if (resultat < 0)
		return resultat * -1;
	else
	 	return resultat;
}


void reglageVolume(int channel, float xa, float ya, float xb, float yb, int porter)
{
    float distance = distancePoint(xa,ya,xb,yb);
    float volume = (COEF_LOG[0][porter] + COEF_LOG[1][porter] * log(distance)) * MAX_VOLUME_ARCADE/128.;

    if(volume < 0)
        volume = 0;
    else if(volume >MAX_VOLUME_ARCADE)
        volume = MAX_VOLUME_ARCADE;

    Mix_Volume(channel, (int)volume);
}

/*
void reglageVolume(int channel, float xa, float ya, float xb, float yb, float porter)
{
	float volume = MIX_MAX_VOLUME;
	float distance = distancePoint(xa,ya,xb,yb);

	if(distance > porter)
		volume = 0;
	else
	{
		float _distance = porter - distance;
		volume = ( volume /porter) * _distance;
	}

	Mix_Volume(channel, (int)volume);
}*/



void bruitagePas(struct Camera_s *dernierePosition, struct Camera_s camera, int channel, Mix_Chunk *music)
{
	if ( distancePoint(dernierePosition->px,dernierePosition->pz, camera.px, camera.pz) > 1.6F )
	{
		*dernierePosition = camera;
		Mix_PlayChannel(channel,music,0);
	}
}


void SDL_GL_AppliquerScene(const C_STRUCT aiScene *scene,struct Camera_s *camera,GLuint *scene_list)
{
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();



	mouvementCamera(camera);


	if(*scene_list == 0) {
		*scene_list = glGenLists(1);
		glNewList(*scene_list, GL_COMPILE);
		aiDessinerScene(scene, scene->mRootNode);
		glEndList();
	}

	glCallList(*scene_list);
}


void GL_InitialiserParametre(int width, int height, struct Camera_s camera)
{
	GLfloat mat_shininess[] = { 50.0 };


	GLfloat light_position0[] = { camera.px, camera.pz, 0.0, 1.0 };
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel (GL_SMOOTH);

	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position0);


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);


	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	gluPerspective(camera.ouverture,(float)(width)/(float)(height),0.2,500);	//Pour les explications, lire le tutorial sur OGL et win

	//Initialize Modelview Matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
	glLoadIdentity();
}



void InitCamera(struct Camera_s *camera, struct Camera_s *cible)
{
	camera->px = START_PX;
	camera->py = START_PY;
	camera->pz = START_PZ;
	camera->cible_py = START_CIBLE_Y;
	camera->angle = START_CIBLE_X;
	camera->ouverture = START_OUVERTURE;


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


void mouvementCamera(struct Camera_s *camera)
{
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);

	if( keystate[SDL_SCANCODE_LEFT] )
	{
		camera->angle += SENSIBILITE_CAMERA;
		if( camera->angle > 2 * M_PI)
			camera->angle -= 2*M_PI;
	}

	if( keystate[SDL_SCANCODE_RIGHT] )
	{
		camera->angle -= SENSIBILITE_CAMERA;
		if( camera->angle < 0)
			camera->angle += 2*M_PI;
	}

	if( keystate[SDL_SCANCODE_UP] )
				if(camera->cible_py + SENSIBILITE_CAMERA < MAX_Y_AXE_CIBLE )
					camera->cible_py += SENSIBILITE_CAMERA;
	if( keystate[SDL_SCANCODE_DOWN] )
				if(camera->cible_py - SENSIBILITE_CAMERA > -MAX_Y_AXE_CIBLE )
					camera->cible_py -= SENSIBILITE_CAMERA;





	if( keystate[SDL_SCANCODE_W] )
	{

		if( detectionEnvironnement( (camera->px + VITESSE_DEPLACEMENT *sin(camera->angle) ),
									( camera->pz +  VITESSE_DEPLACEMENT *cos(camera->angle) ) )   )
									{
										camera->px += VITESSE_DEPLACEMENT *sin(camera->angle);
										camera->pz += VITESSE_DEPLACEMENT *cos(camera->angle);
									}

	}

	if( keystate[SDL_SCANCODE_S] )
	{
		if(
			detectionEnvironnement(  camera->px - VITESSE_DEPLACEMENT *sin(camera->angle),
									 camera->pz - VITESSE_DEPLACEMENT *cos(camera->angle)
								  )
		)
		{
				camera->px -= VITESSE_DEPLACEMENT *sin(camera->angle);
				camera->pz -= VITESSE_DEPLACEMENT *cos(camera->angle);
		}

	}
	if( keystate[SDL_SCANCODE_A] )
	{
		if(
			detectionEnvironnement(  camera->px + VITESSE_DEPLACEMENT *sin(camera->angle + M_PI/2),
									 camera->pz + VITESSE_DEPLACEMENT *cos(camera->angle + M_PI/2)
								  )
		)
		{
				camera->px += VITESSE_DEPLACEMENT *sin(camera->angle + M_PI/2);
				camera->pz += VITESSE_DEPLACEMENT *cos(camera->angle + M_PI/2);
		}


	}
	if( keystate[SDL_SCANCODE_D] )
	{
		if(
			detectionEnvironnement(  camera->px - VITESSE_DEPLACEMENT *sin(camera->angle + M_PI/2),
									 camera->pz - VITESSE_DEPLACEMENT *cos(camera->angle + M_PI/2)
								  )
		)
		{
			camera->px -= VITESSE_DEPLACEMENT *sin(camera->angle + M_PI/2);
			camera->pz -= VITESSE_DEPLACEMENT *cos(camera->angle + M_PI/2);
		}

	}



	// gestion des hauteur dans l'espace
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
	//printf("X = %f Z = %f\n",x,y);


	// HITBOX

	// accueil nineteen
	if( x <= 5.0 && x >= -5.0 && y >= 20.0)
		return 0;

	// lot de 12 machine au milieu de la salle
	if( y >= 8.5 && y <= 13.5 && (  (x <= 8.0 && x >= 1.5) || (x > -8.0 && x <= -1.5)   )   )
		return 0;

	// double machine a gauche de la salle
	if( x <= -12.5 && y <= 13.5 && y >= 9.0)
		return 0;

	// machine central
	if( x >= -1.0 && x <= 1.0 && y >= -1.5 && y <= 1.5)
		return 0;

	// coter non droit au abord de l'accueil de la room
	if(  y >= 14.0 &&  (    ( x >= -0.4736842105*y + 19.36842105  ) || (  x <= -(-0.4736842105*y + 19.36842105))    )   )
		return 0;

	// ouverture entre toilette et salle
	if( (y >= 7.0 || y <= 2.0) && x >= 14.5 && x <= 15.5)
		return 0;

	// mur toilette // salle
	if( x >= 18.0 && x <= 19.0 && y >= -0.5 && y <= 10.0 )
		return 0;

	// machine voiture
	if ( x >= 4.0 && x <= 10.0 && y <= 3.6 && y >= - 3.6)
		return 0;

	// billard
	if( x <= -7.5 && x >= -12.0 && y <= 5.0 && y >= -2.3)
		return 0;
	// BASE DE LA SALLE

	// dimension toilette
	if(x > 15.0 && (  y < -3.0 || y > 12.5  )  )
		return 0;

	// separation toilette
	if( x > 21.0 && x < 22.0 && y < 9.0 && y > 0.5)
		return 0;

	// separation toilette 2
	if( x > 18.0 && y > 4.5 && y < 5.5)
		return 0;

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

int detecterMachine(float x,float y)
{
	//ranger 6 machine a gauche
	if(x > -8.0 && x < -1.0 && y > 7.0 && y < 15.0)
	{
		// machine face/dos
		if( y > 11.0) // vrai = face
		{
			if( x < -5.7 )
				return 1;
			else if ( x < -3.7)
				return 2;
			else
				return 3;
		}
		else
		{
			if( x < -5.7 )
				return 4;
			else if ( x < -3.7)
				return 5;
			else
				return 6;
		}
	}


	// ranger de 6 machine a droite
	if(x < 8.0 && x > 1.0 && y > 7.0 && y < 15.0)
	{
		// machine face/dos
		if( y > 11.0) // vrai = face
		{
			if( x < 3.4 )
				return 7;
			else if ( x < 5.8)
				return 8;
			else
				return 9;
		}
		else
		{
			if( x < 3.4 )
				return 10;
			else if ( x < 5.8)
				return 11;
			else
				return 12;
		}
	}

	// 2 machine sur la gauche
	if( x < -11 && y > 9.0 && y < 13.5)
	{
		if(y < 11.25)
			return 13;
		else
			return 14;
	}

	// machine centrale
	if( x > -1.0 && x < 1.0 && y > 1.0 && y < 3.0)
		return 15;



	return 0;
}

void messageMachine(struct MeilleureScore_s str[],float x,float y,TTF_Font *font,int afficherMessage)
{
	int detection = detecterMachine(x, y);
	if(detection)
	{
		SDL_Color white = {255,255,255};
		char message[60];
		sprintf(message, "RECORD   :  %d  PAR  %s",str[detection].score,str[detection].nomJoueur);
		AfficherText(font,message,white,-1,WinHeight/8);
		AfficherText(font,str[detection].nomJeux,white,-1,WinHeight/6);

		if(afficherMessage)
		{
					AfficherText(font,"APPUYER   SUR   E",white,-1,-1);
		}

	}
}

void animationLancerMachine(struct Camera_s camera, struct Camera_s cible,GLuint scene_list,SDL_Window *Window)
{
	float DUREE_ANIM = 60.0F;
	printf("CAM %f CIBLE %f\n",camera.angle,cible.angle );

	int i = 0;
	float x = (cible.px - camera.px)/DUREE_ANIM ;
	float z = (cible.pz - camera.pz)/DUREE_ANIM ;
	float cib = (cible.cible_py - camera.cible_py) / DUREE_ANIM;
	float y = (cible.py - camera.py)/DUREE_ANIM;

	float angle = cible.angle - camera.angle;


	// correction angle negatif
	if( angle < -M_PI){
		printf("CORRECTION ANGLE NEGATIF %f\n",camera.angle);
		cible.angle = 2*M_PI;
		angle = cible.angle - camera.angle;
	}
	//correction angle positu
	else if ( angle > M_PI){
		printf("CORRECTION ANGLE POSITIF\n");
		angle -= 2*M_PI;
	}
	angle /= DUREE_ANIM ;

	while( i++ < 60)
	{
		camera.px += x;
		camera.pz += z;
		camera.cible_py += cib;
		camera.py += y;
		camera.angle += angle;

		glDisable(GL_LIGHTING);
	  glLoadIdentity();
	  gluOrtho2D(0, WinWidth, 0, WinHeight); // m_Width and m_Height is the resolution of window
		GL_InitialiserParametre(WinWidth,WinHeight,camera);

		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();


		gluLookAt(camera.px                   ,camera.py    ,camera.pz                  ,
				  camera.px+sin(camera.angle) ,camera.py + camera.cible_py    , camera.pz+cos(camera.angle),
				  0.0
				               ,1.0         ,0.0)                        ;

		glCallList(scene_list);

		SDL_GL_SwapWindow(Window);

	}

}

void lancerMachine(const C_STRUCT aiScene *scene,int *Running, struct Camera_s camera, struct Camera_s cible[], char *token, struct MeilleureScore_s meilleureScore[],GLuint *scene_list,SDL_Window *Window,SDL_GLContext *Context)
{

	SDL_Event Event;
	while (SDL_PollEvent(&Event))
	{
		if (Event.type == SDL_KEYDOWN)
		{
			switch (Event.key.keysym.sym)
			{
				case SDLK_ESCAPE:
					*Running = 0;
					break;
				case SDLK_e:
					{
						// verifier si on est proche d'une machine //
								// si oui renvoi le code de la machine

								int machine = detecterMachine(camera.px, camera.pz);
								if ( machine)
								{
									animationLancerMachine(camera,cible[machine-1],*scene_list,Window);
									// centrer sur la machine //
									// zoomer sur la machine //



									// lancer la machine
									SDL_Renderer *pRenderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC |SDL_RENDERER_TARGETTEXTURE);

									switch (machine) {
										case 1: {
											printf( "\nEXIT CODE = %d\n" , flappy_bird( pRenderer, meilleureScore[FLAPPY_HARD].score,WinWidth,WinHeight,token,1));
											updateMeilleureScore(meilleureScore,token);
										}break;
										case 2: SDL_Delay(500);break;
										case 3: SDL_Delay(500);break;
										case 4: SDL_Delay(500);break;
										case 5: SDL_Delay(500);break;
										case 6: SDL_Delay(500);break;
										case 7: SDL_Delay(500);break;
										case 8: SDL_Delay(500);break;
										case 9: SDL_Delay(500);break;
										case 10: SDL_Delay(500);break;
										case 11: {
											printf( "\nEXIT CODE = %d\n" , flappy_bird( pRenderer, meilleureScore[FLAPPY_EASY].score,WinWidth,WinHeight,token,0));
											updateMeilleureScore(meilleureScore,token);
										}break;
										case 12: SDL_Delay(500);break;
										case 13: SDL_Delay(500);break;
										case 14: SDL_Delay(500);break;
										case 15: SDL_Delay(500);break;
										default:break;
									}

									SDL_DestroyRenderer(pRenderer);
									SDL_GL_DeleteContext(*Context);
									// retour sur la Window 3D.

									*Context = SDL_GL_CreateContext(Window);
									*scene_list = 0;
									while(SDL_PollEvent(&Event));
									SDL_GL_AppliquerScene(scene,&camera,scene_list);
									animationLancerMachine(cible[machine-1],camera,*scene_list,Window);

								}



					}break;
				case SDLK_c:
				{
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

				}break;
				default:
					break;
			}
		}
		else if (Event.type == SDL_QUIT)
		{
			*Running = 0;
		}
	}

}


void AfficherText(TTF_Font *font, char *message, SDL_Color color, int x, int y)
{
	glPushMatrix();


	glDisable(GL_LIGHTING);
  glLoadIdentity();


  gluOrtho2D(0, WinWidth, 0, WinHeight); // m_Width and m_Height is the resolution of window
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();




  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  SDL_Surface * sFont = TTF_RenderText_Blended(font, message, color);


  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sFont->w, sFont->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, sFont->pixels);

	if(x == -1)
		x = WinWidth/2 - sFont->w/2;
	if(y == -1)
		y = WinHeight/2 - sFont->h/2;

  glBegin(GL_QUADS);
  {
    glTexCoord2f(0,0); glVertex2f(x, y);
    glTexCoord2f(1,0); glVertex2f(x + sFont->w, y);
    glTexCoord2f(1,-1); glVertex2f(x + sFont->w, y + sFont->h);
    glTexCoord2f(0,-1); glVertex2f(x, y + sFont->h);
  }
  glEnd();



  glDeleteTextures(1, &texture);
  SDL_FreeSurface(sFont);

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
