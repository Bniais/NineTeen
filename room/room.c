/////////////////////////////////////////////////
///	\file room.c
///	\author Samy M.
///	\version 1.0
///	\date 31 janvier 2020
///	\brief affichage de la room et gestion des evenements liee
/////////////////////////////////////////////////

// CODERUNNER COMPILEFLAG
// -std=c99 -framework OpenGL -framework GLUT -lassimp -lm -F/Library/Frameworks -framework SDL2

// GLOBAL LIBRARY
#include <stdio.h>
#include <stdlib.h>

// EXTERNAL
FILE* EXT_FILE;
int MACOS_VER;


#ifdef __APPLE__
	#define GL_SILENCE_DEPRECATION
	#include <OpenGL/gl.h>
	#include <OpenGL/OpenGL.h>
	#include <GLUT/glut.h>
	#include <OpenCL/cl.h>
	#include <OpenCL/cl_gl.h>
	#include <OpenCL/cl_gl_ext.h>
	#include <OpenCL/cl_ext.h>
#endif
#ifdef __linux__
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glut.h>
  #define M_PI 3.1415
#endif

#ifdef _WIN32
    #include<glew.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <assert.h>

// LOCAL LIBRARY
#include "import.h" // YOU NEED ASSIMP LIB FOR import.h (README.dm)
static GLuint * _textures =  NULL, *_counts = NULL;



#include "room.h"
#include "../include/define.h"
#include "../include/libWeb.h"
#include "../include/communFunctions.h"
#include "../games/2_snake/snake.h"
#include "../games/3_flappy_bird/flappy_bird.h"
#include "../games/4_shooter/shooter.h"
#include "../games/5_tetris/tetris.h"
#include "../games/6_demineur/demineur.h"
#include "../games/7_asteroid/asteroid.h"
#include "leaderboard/leaderboard.h"
// END INCLUDE



//
// SON
#define DIR_SON_ENIRONNEMENT_1 "../room/sounds/borne1.wav"
#define DIR_SON_ENIRONNEMENT_2 "../room/sounds/borne2.wav"
#define DIR_SON_ENIRONNEMENT_3 "../room/sounds/borne3.wav"
#define DIR_SON_ENIRONNEMENT_WALK "../room/sounds/walk.wav"
#define DIR_SON_OUVERTURE_PORTE "../room/sounds/SF-ouvport.wav"
#define DIR_SON_FERMETURE_PORTE "../room/sounds/SF-fermport.wav"
// POLICE
#define DIR_FONT_SEGA "../room/fonts/sega.ttf"
#define DIR_FONT_POLICE "../room/fonts/police.ttf"
//OBJS1
#define DIR_OBJ_LOAD "../room/textures/Girl_1.obj"
// EMPLACEMENT FIHCIER NECESSAIRE

#define VITESSE_DEPLACEMENT_COURIR 0.14F
#define VITESSE_DEPLACEMENT_DEBOUT 0.08F
#define VITESSE_DEPLACEMENT_ACCROUPI 0.035F
#define SENSIBILITE_CAMERA 0.08F
#define SENSIBILITE_CAMERA_SOURIS 0.003F
#define HAUTEUR_CAMERA_DEBOUT 3.5F
#define HAUTEUR_CAMERA_ACCROUPI 2.7F
#define MAX_Y_AXE_CIBLE 2.8F
const float ANGLE_DETECTION_MACHINE = M_PI/6;
#define SON_PAS_DISTANCE 2.2F

#define START_PX 12.5F /// POSTION DEPART AXE X
#define START_PY 3.64F /// POSTION DEPART HAUTEUR CAMERA
#define START_PZ 35.0F /// POSTION DEPART AXE Y
#define START_CIBLE_Y 0.0F /// POSTION DEPART REGARDE SUR AXE HAUTEUR
#define START_CIBLE_X M_PI /// POSTION DEPART REGARDE SUR AXE X
#define START_OUVERTURE 70.0F /// POSTION DEPART OUVERTURE CAMERA

float VITESSE_DEPLACEMENT = VITESSE_DEPLACEMENT_DEBOUT; /// VITESSE DEPLACEMENT DEBOUT ASSIS
float HAUTEUR_CAMERA = HAUTEUR_CAMERA_DEBOUT;


const int SON_PAS = 5; /// VOLUME SON PAS
#define MIN_INTENSITE 100 /// INTENSITE SON STEREO MINI


#define RATIO_WINDOW_SCREEN 0.95 /// TAILLE DE L ECRAN
static int WinWidth = 1280; /// DEFAULT RESOLUTION
static int WinHeight = 720; /// DEFAULT RESOLUTION

SDL_Rect bounds;
int optionFullScreen = 0;
//////////////////////////////////////////////////

#define FPS 60 /// FIXER NOMBRE FPS MAX
static const float FRAME_TIME = 1000/FPS;  /// EN DEDUIRE LE DELAI MIN D'UNE FRAME


SDL_Color Text_rouge = {255,0,0};


// lier au son
#define NB_INDICE_PORTER 2
#define MAX_VOLUME_ARCADE 100

// STATIC VAR FOR CAMERA
/**
*\struct Camera_s
*\brief Contient les informations necessaire pour positionner la camera openGL
*/
struct Camera_s
{
	float px; /*!< \Eyes position X */
	float py; /*!< \Eyes position Y */
	float pz; /*!< \Eyes position Z */
	float cible_py; /*!< \Cible position Y */
	float angle; /*!< \Cible position X */
	float ouverture; /*!< \Ouverture focale */
};

/**
*\struct GL_Quadf
*\brief Contient les coordonner et dimension d'un quadrilataire
*/
struct GL_Quadf
{
	float x; /*!< \ position X */
	float y; /*!< \ position Y */
	float z; /*!< \ position Z */
	float largueur; /*!< \ Largueur */
	float epaisseur; /*!< \ epaisseur */
	float hauteur; /*!< \ hauteur */
};
//////////////////////////////////////////////////////////////////////////////////
// struct toilette femme/homme
enum {FERMER,OUVERTE,FERMETURE,OUVERTURE};
// INIT
struct GL_Quadf toiletteFemme = {20.2  , 8.65 , 1.9 , 3. , 0.1 , 3.9};
struct GL_Quadf toiletteHomme = {20.2  , 1.05 , 1.9 , 3. , 0.1 , 3.9};
//////////////////////////////////////////////////////////////////////////////////
int statutPorteFemme = FERMER;
int statutPorteHomme = FERMER;
//////////////////////////////////////////////////////////////////////////////////



enum { SCORE,FLAPPY_HARD,TETRIS_HARD,ASTEROID_HARD,SHOOTER_HARD,SNAKE_HARD,DEMINEUR_HARD,DEMINEUR_EASY,SNAKE_EASY,SHOOTER_EASY,ASTEROID_EASY,TETRIS_EASY,FLAPPY_EASY};
#define NB_BORNES 16
char adresseFontImg[NB_BORNES][64]={"../room/textures/flappy_hard_font.jpg","../room/textures/tetris_font.jpg","../room/textures/asteroid_font_hard.jpg","../room/textures/shooter_font.jpg","../room/textures/snake_font.jpg","../room/textures/demineur_font.jpg",
														 "../room/textures/demineur_font.jpg","../room/textures/snake_font.jpg","../room/textures/shooter_font.jpg","../room/textures/asteroid_font.jpg","../room/textures/tetris_font.jpg","../room/textures/flappy_easy_font.jpg",
														 "../room/textures/chargement.jpg","../room/textures/chargement.jpg","../room/textures/chargement.jpg","../room/textures/leaderboard_font.jpg"}; /// STOCK LES POSITIONS

int (*lancerJeu[NB_BORNES])(SDL_Renderer *,int , int , int , char *, int , SDL_Texture **) = {flappy_bird, tetris, asteroid, shooter, snake, demineur, demineur, snake, shooter, asteroid, tetris, flappy_bird};

#ifdef _WIN32
  #define DIR_TOKEN_FILE "C:\\Windows\\Temp\\.Nineteen"
#else
  #define DIR_TOKEN_FILE "/tmp/.Nineteen"
#endif

int FIRST_FRAME = 0;
















#include "dir.h"
SDL_Texture * textures[NB_MAX_TEXTURES];

/**
*\struct loadGame
*\brief Contient les paramètres pour le thread de chargement des textures du jeu
*/
typedef struct loadGame_s{
	int id_jeu;  /*!< \brief  L'identifiant du jeu */
	SDL_Renderer *pRenderer;  /*!< \brief Le renderer */
}loadGame;

/**
*\fn int loadGameTexture(loadGame * load)
*\brief Charge les textures d'un jeu
*\param load La structure contenant l'id du jeu et le renderer
*\return Faux en cas de problème sinon vrai
*/
int loadGameTexture(loadGame * load){
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
	    //asteroid
		switch(load->id_jeu){
			case ASTEROID_EASY:
            case ASTEROID_HARD:
				for(int i=0; i< NB_ASTEROID_TEXTURES; i++){
                        int textureFloueAsteroid[NB_ASTEROID_TEXTURES] ={0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0};

					if(textureFloueAsteroid[i])
						SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
					else
						SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

					textures[i] = IMG_LoadTexture(load->pRenderer, DIR_TEXTURES_ASTEROID[i]);
					if( textures[i] == NULL ){
						printf("Erreur lors de la creation de texture %s", SDL_GetError());
						return SDL_FALSE;
					}
				}
				break;

			case FLAPPY_EASY:
            case FLAPPY_HARD:
				for(int i=0; i< NB_FLAPPY_TEXTURES; i++){
					 textures[i] = IMG_LoadTexture(load->pRenderer, DIR_TEXTURES_FLAPPY[i]);
					 if( textures[i] == NULL ){
						printf("Erreur lors de la creation de texture %s", SDL_GetError());
						return SDL_FALSE;
					}
				}
				break;

			case SNAKE_EASY:
            case SNAKE_HARD:
				for(int i=0; i< NB_SNAKE_TEXTURES; i++){
					 textures[i] = IMG_LoadTexture(load->pRenderer, DIR_TEXTURES_SNAKE[i]);
					 if( textures[i] == NULL ){
						printf("Erreur lors de la creation de texture %s", SDL_GetError());
						return SDL_FALSE;
					}
				}
				break;

			case TETRIS_EASY:
            case TETRIS_HARD:
				for(int i=0; i< NB_TETRIS_TEXTURES; i++){
					 textures[i] = IMG_LoadTexture(load->pRenderer, DIR_TEXTURES_TETRIS[i]);
					 if( textures[i] == NULL ){
						printf("Erreur lors de la creation de texture %s", SDL_GetError());
						return SDL_FALSE;
					}
				}
				break;

			case SHOOTER_EASY:
            case SHOOTER_HARD:
				for(int i=0; i< NB_SHOOTER_TEXTURES; i++){
					if(textureFloueShooter[i])
						SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
					else
						SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

					textures[i] = IMG_LoadTexture(load->pRenderer, DIR_TEXTURES_SHOOTER[i]);
					if( textures[i] == NULL ){
						printf("Erreur lors de la creation de texture %s", SDL_GetError());
						return SDL_FALSE;
					}
				}
				break;
		}
		return SDL_TRUE;
}



/////////////////////////////////////////////////////
/// \fn int room(char *token,struct MeilleureScore_s meilleureScore[], SDL_Window *Window,const C_STRUCT aiScene* scene )
/// \brief fonction d'appel principal
///
/// \param token cle de connexion
/// \param meilleureScore tableau charger des scores
/// \param Window fenetre initaliser
/// \param  scene scene charger
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
//int room(char *token,struct MeilleureScore_s meilleureScore[], SDL_Window *Window,const C_STRUCT aiScene* scene, int optFullScreen, SDL_Rect borderSize );


/////////////////////////////////////////////////////
/// \fn int windowMaxSize(int optionFullScreen)
/// \brief fonction qui charge fixe la taille max de la fenetre
/// \param optionFullScreen
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
static
int windowMaxSize(int optionFullScreen);


/////////////////////////////////////////////////////
/// \fn void limiterFrame(const float delayLancementFrame,float *_IPS)
/// \brief permet de mettre un nombre d'image max et reglger la vitesse de deplacement en fonction des ips
///
/// \param delayLancementFrame temps au lancement de la frame
/// \param _IPS mise a jour de la constante IPS
///
/// \return void
/////////////////////////////////////////////////////
static
void limiterFrame(const float delayLancementFrame,float *_IPS);

/////////////////////////////////////////////////////
/// \fn int attendreXsecondeMessage(int *compterSeconde, int *afficherMessage,const int MS, const float _IPS)
/// \brief permet d'attendre avant d'afficher message CLIGNOTANT affiche egalement les IPS
///
/// \param compterSeconde delai d'attente mesurer
/// \param afficherMessage booleen vrai ou faux
/// \param MS delai d'attente
/// \param _IPS affichage des IPS
///
/// \return VRAI/FAUX
/////////////////////////////////////////////////////
static
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
/// \param  x coordonner x
/// \param  y coordonner y
///
/// \return TRUE/FALSE
/////////////////////////////////////////////////////
static
int detectionEnvironnement(float x,float y);


/////////////////////////////////////////////////////
/// \fn int detecterRadio(float x,float y,float angle)
/// \brief detection radio a proximite
///
/// \param  x coordonner x
/// \param  y coordonner y
/// \param  angle angle
///
/// \return TRUE/FALSE
/////////////////////////////////////////////////////
static
int detecterRadio(float x,float y,float angle);

/////////////////////////////////////////////////////
/// \fn int detecterPorte(float x,float y,float angle)
/// \brief detection porte a proximite
///
/// \param  x coordonner x
/// \param  y coordonner y
/// \param  angle angle
///
/// \return TRUE/FALSE
/////////////////////////////////////////////////////
static
int detecterPorte(float x,float y,float angle);

/////////////////////////////////////////////////////
/// \fn int detecterOuvertureToilette(float x,float y,float angle)
/// \brief detection des toilettes h/f
///
/// \param  x coordonner x
/// \param  y coordonner y
/// \param  angle angle
///
/// \return TRUE/FALSE
/////////////////////////////////////////////////////
static
int detecterOuvertureToilette(float x,float y,float angle);


/////////////////////////////////////////////////////
/// \fn detecterMachine(float x,float y,float angle)
/// \brief fonction qui detecte la proxmiter avec une machine de jeu
///
/// \param  x coordonner x
/// \param  y coordonner y
/// \param  angle angle
///
/// \return TRUE/FALSE
/////////////////////////////////////////////////////
static
int detecterMachine(float x,float y,float angle);


/////////////////////////////////////////////////////
/// \fn void mixerInit()
/// \brief initalisation du volume
///
/////////////////////////////////////////////////////
static
void mixerInit();


/////////////////////////////////////////////////////
/// \fn float distancePoint(float xa, float ya, float xb, float yb)
/// \brief renvoi la distance entre deux point
///
/// \param  xa
/// \param  ya
/// \param  xb
/// \param  yb
///
/// \return float distance point a et b
/////////////////////////////////////////////////////
static
float distancePoint(float xa, float ya, float xb, float yb);



/////////////////////////////////////////////////////
/// \fn void reglageVolume(int channel, float xa, float ya, float xb, float yb, float porter, float angleJoueur, int max_volume)
/// \brief regle le volume de l'environement sur un channel particuliere
///
/// \param  channel canal du son a regler
/// \param  xa
/// \param  ya
/// \param  xb
/// \param  yb
/// \param  porter Porter du son
/// \param float angleJoueur
/// \param  max_volume
///
/// \return void
/////////////////////////////////////////////////////
static
void reglageVolume(int channel, float xa, float ya, float xb, float yb, float porter, float angleJoueur, int max_volume);


/////////////////////////////////////////////////////
/// \fn void bruitagePas(struct Camera_s *dernierePosition, struct Camera_s camera, int channel, Mix_Chunk *music)
/// \brief permet de deplacer la source du son des pas et de le jouer
///
/// \param dernierePosition derniere position ou le son a etait jouer
/// \param  camera position actuel
/// \param  channel channel sur le quelle jouer le son
/// \param music son a jouer
/// \param  angleJoueur angle de la camera du joueur
///
/// \return void
/////////////////////////////////////////////////////
static
void bruitagePas(struct Camera_s *dernierePosition, struct Camera_s camera, int channel, Mix_Chunk *music);

/////////////////////////////////////////////////////
/// \fn float calculAngle(float xa, float ya,      float xb, float yb,       float xc, float yc)
/// \brief renvoi l'angle entre 3 points
///
/// \param  xa
/// \param  ya
/// \param  xb
/// \param  yb
/// \param  xc
/// \param  yc
///
/// \return void
/////////////////////////////////////////////////////
static
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
/// \param  width
/// \param  height
/// \param   camera
///
/// \return void
/////////////////////////////////////////////////////
static
void GL_InitialiserParametre(int width, int height, struct Camera_s camera);


/////////////////////////////////////////////////////
/// \fn void InitCamera(struct Camera_s *camera, struct Camera_s *cible)
/// \brief initialisation des variables lier au camera joueur et machines
///
/// \param   camera
/// \param   cible
///
/// \return void
/////////////////////////////////////////////////////
static
void InitCamera(struct Camera_s *camera, struct Camera_s *cible);


/////////////////////////////////////////////////////
/// \fn void mouvementCamera(SDL_Window * window, struct Camera_s *camera,const float IPS)
/// \brief Permet de gerer les deplacements de la camera
///
/// \param   window
/// \param   camera prend l'adresse de la camera
/// \param   IPS
///
/// \return void
/////////////////////////////////////////////////////
static
void mouvementCamera(SDL_Window * window, struct Camera_s *camera, const float IPS);


/////////////////////////////////////////////////////
/// \fn void SDL_GL_AppliquerScene(SDL_Window * Window, const C_STRUCT aiScene *scene,struct Camera_s *camera,GLuint *scene_list, const float IPS)
/// \brief permet d'appliquer la scene dans la window + mouvement camera
///
/// \param    scene scene ou est stocker notre obj
/// \param   camera mise a jour de LookAt
/// \param  scene_list Nombre de scene
///
/// \return void
/////////////////////////////////////////////////////
static
void SDL_GL_AppliquerScene(SDL_Window * Window, const C_STRUCT aiScene *scene,struct Camera_s *camera,GLuint *scene_list, const float IPS);


/////////////////////////////////////////////////////
/// \fn void AfficherText(TTF_Font *font, char *message, SDL_Color color, int x, int y)
/// \brief affichage de text sur un rendu OpenGL
///
/// \param  font police d'ecriture du texte
/// \param  message message a afficher
/// \param  color couleur du texte
/// \param  x position du text sur X -1 = centrer
/// \param  y position du text sur Y -1 = centrer
///
/// \return void
/////////////////////////////////////////////////////
static
void AfficherText(TTF_Font *font, char *message, SDL_Color color, int x, int y);

/////////////////////////////////////////////////////
/// \fn void messageMachine(struct MeilleureScore_s str[] ,struct Camera_s camera,TTF_Font *font,int afficherMessage)
/// \brief affichage un message propre a chaque machine
///
/// \param   str structure stockant les nom et score des meilleure joueur par machine
/// \param   camera Camera du joueur pour detection des machines
/// \param  font pointeur sur la police
/// \param  afficherMessage afficher messages clignotant ou nom
///
/// \return void
/////////////////////////////////////////////////////
static
void messageMachine(struct MeilleureScore_s str[] ,struct Camera_s camera,TTF_Font *font,int afficherMessage);


/////////////////////////////////////////////////////
/// \fn int MessageQuitterRoom(int modeChargement, char path[])
/// \brief permet d'afficher le message avant de quitter ou au chargement de la room
///
/// \param  modeChargement
/// \param  path
///
/// \return EXIT_SUCCESS/EXIT_FAILURE
/////////////////////////////////////////////////////
static
int MessageQuitterRoom(int modeChargement, char path[]);

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
////   FUNCTION LIER A L'ANIMATION   ////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////



/////////////////////////////////////////////////////
/// \fn void lancerMachine(const C_STRUCT aiScene *scene,int *Running, struct Camera_s camera, struct Camera_s cible[],char *token, struct MeilleureScore_s meilleureScore[],GLuint *scene_list,SDL_Window *Window,SDL_GLContext *Context, int *jouerSonPorteFemme,  int *jouerSonPorteHomme, float _IPS, TTF_Font *font);
/// \brief permet de lancer une machine de jeux comprend plusieur fonction annexe
///
/// \param    scene permet de passer la scene a re afficher au retour de la machine
/// \param  Running permet de mettre fin au jeux
/// \param   camera camera du joueur
/// \param   cible camera cible machine
/// \param  token cle du joueur pour mode en ligne
/// \param   meilleureScore strucuteur des score et nom meilleure joueur
/// \param  scene_list liste des scenes
/// \param  Window fenetre sdl/opengl
/// \param  Context context sdl/opengl
/// \param  jouerSonPorteFemme
/// \param  jouerSonPorteHomme
/// \param  _IPS
///
/// \return void
/////////////////////////////////////////////////////
static
void lancerMachine(const C_STRUCT aiScene *scene,int *Running, struct Camera_s camera, struct Camera_s cible[],char *token, struct MeilleureScore_s meilleureScore[],GLuint *scene_list, SDL_Window **Window,SDL_GLContext *Context, int *jouerSonPorteFemme,  int *jouerSonPorteHomme, float _IPS, TTF_Font *font);


/////////////////////////////////////////////////////
/// \fn void animationLancerMachine(struct Camera_s camera, struct Camera_s cible,GLuint scene_list,SDL_Window *Window, float _IPS,float imgAnim)
/// \brief permet d'animer le lancement et retour des machines
///
/// \param   camera camera de depart
/// \param   cible camera de fin
/// \param  scene_list scene
/// \param  Window vue d'affichae
/// \param  _IPS image / s
///
/// \return void
/////////////////////////////////////////////////////
static
void animationLancerMachine(struct Camera_s camera, struct Camera_s cible,GLuint scene_list,SDL_Window *Window, float _IPS, float imgAnim);






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
/// \param  MeilleureScore_s str tableau de donner
///
/////////////////////////////////////////////////////
void InitMeilleureScore(struct MeilleureScore_s str[]);


/////////////////////////////////////////////////////
/// \fn int updateMeilleureScore(struct MeilleureScore_s str[] ,char *token)
/// \brief Mes a jours les donnees liee au score de la structure meilleureScore
///
/// \param   str tableau de donner
///
/// \return EXIT_SUCCESS/EXIT_FAILURE
/////////////////////////////////////////////////////
int updateMeilleureScore(struct MeilleureScore_s str[] ,char *token);


/////////////////////////////////////////////////////
/// \fn void GlDessinerQuad(struct GL_Quadf quad)
/// \brief Dessine un quadrilataire
///
/// \param  quad
///
/// \return EXIT_SUCCESS/EXIT_FAILURE
/////////////////////////////////////////////////////
static
void GlDessinerQuad(struct GL_Quadf quad);


/////////////////////////////////////////////////////
/// \fn void detruireTexture(GLuint nbTextures)
/// \brief detruit les elements initaliser
/// \param  nbTextures
/// \return void
/////////////////////////////////////////////////////
static
void detruireTexture(GLuint nbTextures);



/////////////////////////////////////////////////////
/// \fn void GLlightMode()
/// \brief permet de regler l'intensiter de chaque lampe presente dans la salle
///
///
/// \return void
/////////////////////////////////////////////////////
static
void GLlightMode();


/////////////////////////////////////////////////////
/// \fn void animationPorteToilette(int *statutPorteFemme, int *statutPorteHomme,int *jouerSonPorteFemme,int *jouerSonPorteHomme, int *toiletteFemmeOuverteDelai, int *toiletteHommeOuverteDelai ,Mix_Chunk *sas_ouverture, Mix_Chunk *sas_fermeture,struct Camera_s camera, float IPS);
/// \brief permet de gerer les evenements liee au porte des toilettes
///
/// \param  statutPorteFemme
/// \param  statutPorteHomme
/// \param  jouerSonPorteFemme
/// \param  jouerSonPorteHomme
/// \param  toiletteFemmeOuverteDelai
/// \param  toiletteHommeOuverteDelai
/// \param  sas_ouverture
/// \param  sas_fermeture
/// \param   camera
/// \param  IPS
///
/// \return EXIT_SUCCESS/EXIT_FAILURE
/////////////////////////////////////////////////////
static
void animationPorteToilette(int *statutPorteFemme, int *statutPorteHomme,int *jouerSonPorteFemme,int *jouerSonPorteHomme, int *toiletteFemmeOuverteDelai, int *toiletteHommeOuverteDelai ,Mix_Chunk *sas_ouverture, Mix_Chunk *sas_fermeture,struct Camera_s camera, float IPS);


/////////////////////////////////////////////////////
/// \fn int backgroundClassement(SDL_Surface *sImage)
/// \brief afficher un background au classement pour qu'il soit plus facilement visible
///
/// \param  sImage
///
/// \return EXIT_SUCCESS/EXIT_FAILURE
/////////////////////////////////////////////////////
static
int backgroundClassement(SDL_Surface *sImage);




























































int room(char *token,struct MeilleureScore_s meilleureScore[],SDL_Window *Window, const C_STRUCT aiScene* scene, int optFullScreen, SDL_Rect borderSize)
{

	optionFullScreen = optFullScreen;
	//////////////////////////////////////////////////////////
	// EVENT SOURIS INIT

	//////////////////////////////////////////////////////////
	// POSITIONNER LA SOURIS AU CENTRE
	if ( SDL_GetDisplayBounds(0, &bounds) )
		fprintf(EXT_FILE,"room.c : room() : SDL_GetDisplayBounds : %s\n",SDL_GetError() );
	///////////////////////////////////////////////////
	//////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////
	// RECUPERER C'EST VALEUR DES PARAMS A L'AVENIR
	windowMaxSize(optionFullScreen);
	//////////////////////////////////////////////////////////





	//////////////////////////////////////////////////////////
	// INITIALISATION CAMERA / CIBLE SUR MACHINE /
	static struct Camera_s camera,cible[16],jouerSon;
	InitCamera(&camera,cible);
	jouerSon = camera;
	//////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////
	// INITALISATION SON
	mixerInit();
	// CHARGER LES MUSIQUES D"AMBIANCE
	int initFailed = SDL_FALSE;
	Mix_Chunk *music_01	= Mix_LoadWAV(DIR_SON_ENIRONNEMENT_1);
	if( !music_01 )
	{
		fprintf(EXT_FILE,"room.c -> room() : Mix_LoadWAV : %s DIR:%s\n",Mix_GetError(),DIR_SON_ENIRONNEMENT_1 );
		// SET initFailed
		initFailed = SDL_TRUE;
	}
	Mix_Chunk *music_02	= Mix_LoadWAV(DIR_SON_ENIRONNEMENT_2);
	if( !music_02 )
	{
		fprintf(EXT_FILE,"room.c -> room() : Mix_LoadWAV : %s DIR:%s\n",Mix_GetError(),DIR_SON_ENIRONNEMENT_2 );
		// SET initFailed
		initFailed = SDL_TRUE;
	}

	Mix_Chunk *music_03	= Mix_LoadWAV(DIR_SON_ENIRONNEMENT_3);
	if( !music_03 )
	{
		fprintf(EXT_FILE,"room.c -> room() : Mix_LoadWAV : %s DIR:%s\n",Mix_GetError(),DIR_SON_ENIRONNEMENT_3 );
		// SET initFailed
		initFailed = SDL_TRUE;
	}

	Mix_Chunk *music_walk	= Mix_LoadWAV(DIR_SON_ENIRONNEMENT_WALK);
	if( !music_walk )
	{
		fprintf(EXT_FILE,"room.c -> room() : Mix_LoadWAV : %s DIR:%s\n",Mix_GetError(),DIR_SON_ENIRONNEMENT_WALK );
		// SET initFailed
		initFailed = SDL_TRUE;
	}

	Mix_Chunk *sas_ouverture = Mix_LoadWAV(DIR_SON_OUVERTURE_PORTE);
	if( !sas_ouverture )
	{
		fprintf(EXT_FILE,"room.c -> room() : Mix_LoadWAV : %s DIR:%s\n",Mix_GetError(),DIR_SON_OUVERTURE_PORTE);
		// SET initFailed
		initFailed = SDL_TRUE;
	}
	Mix_Chunk *sas_fermeture = Mix_LoadWAV(DIR_SON_FERMETURE_PORTE);
	if( !sas_fermeture )
	{
		fprintf(EXT_FILE,"room.c -> room() : Mix_LoadWAV : %s DIR:%s\n",Mix_GetError(),DIR_SON_FERMETURE_PORTE );
		// SET initFailed
		initFailed = SDL_TRUE;
	}

	// VARIABLE JOUER SON
	int jouerSonPorteFemme = 0;
	int jouerSonPorteHomme = 0;
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
		fprintf(EXT_FILE,"room.c -> room() : TTF_OpenFont : %s DIR:%s\n",TTF_GetError(),DIR_FONT_POLICE );
		// SET initFailed
		initFailed = SDL_TRUE;
	}
	TTF_Font * sega = TTF_OpenFont(DIR_FONT_SEGA, WinWidth/50);
	if(!sega)
	{
		fprintf(EXT_FILE,"room.c -> room() : TTF_OpenFont : %s DIR:%s\n",TTF_GetError(),DIR_FONT_SEGA );
		// SET initFailed
		initFailed = SDL_TRUE;
	}

	//////////////////////////////////////////////////////////




	if(optionFullScreen)
		Window = SDL_CreateWindow("Nineteen", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, WinWidth, WinHeight, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP  );
	else{

		int ww, wh;
		SDL_Rect usablebounds;
		SDL_GetDisplayUsableBounds(0, &usablebounds);

		Window = SDL_CreateWindow("Nineteen", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, WinWidth, WinHeight, SDL_WINDOW_OPENGL );
		SDL_GetWindowPosition(Window, &ww, &wh);
		SDL_SetWindowPosition(Window, ww - (bounds.w -(usablebounds.w+ borderSize.y + borderSize.h))/2, wh - (bounds.h -(usablebounds.h+ borderSize.x + borderSize.w))/2 );
	}




	//////////////////////////////////////////////////////////
	// VERIFIER EXISTANCE DE LA FENETRE ET CREATION CONTEXT
	if( !Window)
	{
		// NETTOYAGE MEMOIRE
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Erreur Fatal",SDL_GetError(),NULL);
		fprintf(EXT_FILE,"room.c -> room() : SDL_CreateWindow : %s \n",SDL_GetError() );
		initFailed = SDL_TRUE;
	}

	//////////////////////////////////////////////////////////
	// CREATION DE L'ICON D'APPLICATION
	SDL_Surface *favicon;
	if( ( favicon=IMG_Load("../assets/image/favicon.png") ) )
	{
		SDL_SetWindowIcon(Window, favicon);
		SDL_FreeSurface(favicon);
	}
	else
		fprintf(EXT_FILE,"room.c : room() : IMG_Load : %s DIR:%s\n",SDL_GetError() , "../assets/image/favicon.png" );

	SDL_Surface *bgClassement = NULL;
	bgClassement = IMG_Load("../room/textures/background_classement.png");
	if(!bgClassement)
	{
		fprintf(EXT_FILE,"room.c : room() : IMG_Load %s DIR:%s\n",SDL_GetError(),"../room/textures/background_classement.png" );
		initFailed = SDL_TRUE;
	}

	SDL_GLContext Context = SDL_GL_CreateContext(Window);
	if( !Context)
	{
		fprintf(EXT_FILE,"room.c : room() : SDL_GL_CreateContext : %s\n",SDL_GetError());
		initFailed = SDL_TRUE;
	}


	////////	//////////////////////////////////////////////////


	// QUITTER LA FONCTION SI IL Y'A EUX UNE ERREUR AVANT
	if(initFailed == SDL_TRUE)
	{
		/////////////////////////
		// NETTOYAGE ALLOCATION DE MEMOIRE
		// AVANT DESTRUCTION DU PROGRAMME
		if(music_01)
		{
			Mix_FreeChunk(music_01);
			music_01 = NULL;
		}
		if(music_02)
		{
			Mix_FreeChunk(music_02);
			music_02 = NULL;
		}
		if(music_03)
		{
			Mix_FreeChunk(music_03);
			music_03 = NULL;
		}
		if(music_walk)
		{
			Mix_FreeChunk(music_walk);
			music_walk = NULL;
		}
		if(sas_ouverture)
		{
			Mix_FreeChunk(sas_ouverture);
			sas_ouverture = NULL;
		}
		if(sas_fermeture)
		{
			Mix_FreeChunk(sas_fermeture);
			sas_fermeture = NULL;
		}
		if(font)
		{
			TTF_CloseFont(font);
			font = NULL;
		}
		if(sega)
		{
			TTF_CloseFont(sega);
			sega = NULL;
		}
		if(Window)
		{
			SDL_DestroyWindow(Window);
			Window = NULL;
		}
		if(bgClassement)
		{
			SDL_FreeSurface(bgClassement);
			bgClassement = NULL;
		}
		// EXIT_FAILURE
		return EXIT_FAILURE;
	}


	//////////////////////////////////////////////////////////
	// VARIABLE DE DEROULEMENT
	int Running = 1;
	int compterSeconde = SDL_GetTicks();
	int afficherMessage = 0;
	float _IPS = FPS;
	int toiletteFemmeOuverteDelai = 0;
	int toiletteHommeOuverteDelai = 0;

	//////////////////////////////////////////////////////////
	// OpenGL
	//////////////////////////////////////////////////////////
	// INITIALISATION LIST SCENE
	GLuint scene_list = 0; // NB SCENE
	//////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////
	// ALLOCATION TEXTURE
	_malloc((void**)&_textures,sizeof(*_textures),(scene->mNumMaterials),EXT_FILE,SDL_MESSAGEBOX_ERROR,"allocation failed","room.c : room() : GLuint *_textures ",Window);
	//////////////////////////////////////////////////////////
	// CHARGER LES TEXTURES
	chargerTexture(DIR_OBJ_LOAD,scene,_textures,&_counts);
	// APPLICATION DE LA SCENE  AVANT LA 1er FRAME
	SDL_GL_AppliquerScene(Window, scene,&camera,&scene_list,_IPS);

	//////////////////////////////////////////////////////////
	// FIXER LA SOURIS AU CENTRE ET LA CACHER POUR MAC/WINDOW
	//////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////
	#ifndef __linux__
		SDL_WarpMouseInWindow(Window, (WinWidth/2)  ,(WinHeight/2) );
		//////////////////////////////////////////////////////////
		// CACHER LA SOURIS
		SDL_ShowCursor(SDL_DISABLE);
	#endif

	fprintf(EXT_FILE, "room.c : room() : fin de l'initalisation debut du while statut : %d\n",Running );


	FIRST_FRAME = 0 ;
	while (Running)
	{



		//////////////////////////////////////////////////////////
		// DEBUT DE LA MESURE DU TEMPS D'UNE FRAME
		int delayLancementFrame = SDL_GetTicks();


		GL_InitialiserParametre(WinWidth,WinHeight,camera);


		//////////////////////////////////////////////////////////
		// REGLAGE SON ENVIRONEMENT AVEC LEUR POSITION
		// MUSIQUE BAR
		if(!Mix_Paused(0))
			reglageVolume(0,-2.0,19.4,camera.px,camera.pz,35.0  - 1.5 - ( (!Mix_Paused(1) + !Mix_Paused(2)) * 10  ) ,camera.angle, MAX_VOLUME_ARCADE  - ( (!Mix_Paused(1) + !Mix_Paused(2)) * 7  )    );
		// MUSIQUE TABLE
		if(!Mix_Paused(1))
			reglageVolume(1,10.0,0.2,camera.px,camera.pz,35.0 +1.5  - ( (!Mix_Paused(0) + !Mix_Paused(2)) * 10  ) ,camera.angle, MAX_VOLUME_ARCADE - ( (!Mix_Paused(0) + !Mix_Paused(2)) * 7  ) );
		// MUSIQUE BILLARD
		if(!Mix_Paused(2))
			reglageVolume(2,-11.5,2.6,camera.px,camera.pz,35.0 +1.25   - ( (!Mix_Paused(0) + !Mix_Paused(1)) * 10  ) ,camera.angle, MAX_VOLUME_ARCADE - ( (!Mix_Paused(0) + !Mix_Paused(1)) * 7  ) );
		//////////////////////////////////////////////////////////



		//////////////////////////////////////////////////////////
		// JOUER SON BRUIT DE PAS QUAND C'EST NECESSAIRE
		bruitagePas(&jouerSon,camera,3,music_walk);
		//////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////
		// OpenGL
		//////////////////////////////////////////////////////////
		// REGLAGE LUMIERE
		//

		//////////////////////////////////////////////////////////
		// LANCEMENT DES MACHINES
		lancerMachine(scene,&Running,camera,cible,token,meilleureScore,&scene_list, &Window,&Context,&jouerSonPorteFemme, &jouerSonPorteHomme, _IPS,sega);
		//////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////
		// CHARGER LA SCENE

		SDL_GL_AppliquerScene(Window, scene,&camera,&scene_list,_IPS);

		//////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////
		// OpemGL Text
		//////////////////////////////////////////////////////////
		// AFFICHER CLASSEMENT / SCORE EN HAUT A GAUCHE
		backgroundClassement(bgClassement);
		AfficherText(sega,meilleureScore[0].nomJeux,Text_rouge,WinWidth/30,WinHeight - WinWidth/30);
		//
		AfficherText(sega,meilleureScore[0].nomJoueur,Text_rouge,WinWidth/30,WinHeight - WinWidth/15);
		//////////////////////////////////////////////////////////
		// AFFICHAGE MESSAGE A PROXIMITER DES MACHINES
		messageMachine(meilleureScore,camera,sega,afficherMessage);
		//////////////////////////////////////////////////////////


		//////////////////////////////////////////////////////////
		// ANIMATION PORTE TOILETTES
		animationPorteToilette(&statutPorteFemme,&statutPorteHomme,&jouerSonPorteFemme,&jouerSonPorteHomme,&toiletteFemmeOuverteDelai,&toiletteHommeOuverteDelai ,sas_ouverture, sas_fermeture,camera, _IPS);

		//////////////////////////////////////////////////////////
		// OpenGL
		//////////////////////////////////////////////////////////
		// APPLIQUER MODIF SUR LA VUE
		SDL_GL_SwapWindow(Window);
		//////////////////////////////////////////////////////////
		// ATTENDRE 1000 MS POUR MESSAGE CLIGNOTANT
		attendreXsecondeMessage(&compterSeconde, &afficherMessage,1000, _IPS);
		//////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////
		// LIMITER LES FRAMES A CONST FPS
		limiterFrame(delayLancementFrame,&_IPS);
		//////////////////////////////////////////////////////////






	}


	fprintf(EXT_FILE, "room.c : room() : liberation de la memoire\n" );
	//////////////////////////////////////////////////////////
	// LIBERER LA MEMOIRE ALLOUER ET NON NECESSAIRE A LA SUITE
	// LIBERATION DES SONS
	Mix_FreeChunk(music_01 );
	Mix_FreeChunk(music_02 );
	Mix_FreeChunk(music_03 );
	Mix_FreeChunk(music_walk );
	Mix_FreeChunk(sas_ouverture );
	Mix_FreeChunk(sas_fermeture );
	//////////////////////////////////////////////////////////
	// LIBERATION DES POLICES
	TTF_CloseFont(font);
	TTF_CloseFont(sega);
	//////////////////////////////////////////////////////////
	// LIBERATION DES SURFACE
	SDL_FreeSurface(bgClassement);
	//////////////////////////////////////////////////////////
	// DESTRUCTION DES EMPLACEMENTS TEXTURES
	detruireTexture(scene->mNumMaterials);
	fprintf(EXT_FILE, "room.c : room() : memoire liberer\n" );
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

	strcpy(str[SHOOTER_HARD].nomJeux,"SHOOTER");

	strcpy(str[SNAKE_HARD].nomJeux,"SNAKE");

	strcpy(str[DEMINEUR_HARD].nomJeux,"DEMINEUR");

	strcpy(str[DEMINEUR_EASY].nomJeux,"DEMINEUR");

	strcpy(str[SNAKE_EASY].nomJeux,"SNAKE");

	strcpy(str[SHOOTER_EASY].nomJeux,"SHOOTER");

	strcpy(str[ASTEROID_EASY].nomJeux,"ASTEROID");

	strcpy(str[TETRIS_EASY].nomJeux,"TETRIS");

	strcpy(str[FLAPPY_EASY].nomJeux,"FLAPPY   BIRD");


	strcpy(str[13].nomJeux,"COMMING   SOON");
	strcpy(str[14].nomJeux,"COMMING   SOON");
	strcpy(str[15].nomJeux,"COMMING   SOON");
}



int updateMeilleureScore(struct MeilleureScore_s str[] ,char *token)
{
	///////////////////////////////////////////////////////////
	// INITALISATION D'UNE CHAINE POUR STOCKER LA REPONSE
	char *reponse = NULL;
	if ( _malloc((void**)&reponse,sizeof(char),2048,EXT_FILE,SDL_MESSAGEBOX_ERROR,"allocation failed","room.c : updateMeilleureScore() : char*reponse ",NULL) )
		return EXIT_FAILURE;

	///////////////////////////////////////////////////////////
	// RECUPERATION DU SCORE JUSQU'A REUSSITE
	while( updateMeilleureScoreStruct(token,reponse) == EXIT_FAILURE );

	///////////////////////////////////////////////////////////
	// PARSING DANS LA CHAINE DE DONNER RECU
	int temp1,temp2;
	sscanf(reponse,"%d %d / %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", &str[0].score,&temp1,&temp2,
																																str[FLAPPY_HARD].nomJoueur,&str[FLAPPY_HARD].score,
																																str[TETRIS_HARD].nomJoueur,&str[TETRIS_HARD].score,
																																str[ASTEROID_HARD].nomJoueur,&str[ASTEROID_HARD].score,
																																str[SHOOTER_HARD].nomJoueur,&str[SHOOTER_HARD].score,
																																str[SNAKE_HARD].nomJoueur,&str[SNAKE_HARD].score,
																																str[DEMINEUR_HARD].nomJoueur,&str[DEMINEUR_HARD].score,
																																str[DEMINEUR_EASY].nomJoueur,&str[DEMINEUR_EASY].score,
																																str[SNAKE_EASY].nomJoueur,&str[SNAKE_EASY].score,
																																str[SHOOTER_EASY].nomJoueur,&str[SHOOTER_EASY].score,
																																str[ASTEROID_EASY].nomJoueur,&str[ASTEROID_EASY].score,
																																str[TETRIS_EASY].nomJoueur,&str[TETRIS_EASY].score,
																																str[FLAPPY_EASY].nomJoueur,&str[FLAPPY_EASY].score,
																																str[13].nomJoueur,&str[13].score,
																																str[14].nomJoueur,&str[14].score,
																																str[15].nomJoueur,&str[15].score,
																																&str[1].scoreJoueurActuel,
																																&str[2].scoreJoueurActuel,
																																&str[3].scoreJoueurActuel,
																																&str[4].scoreJoueurActuel,
																																&str[5].scoreJoueurActuel,
																																&str[6].scoreJoueurActuel,
																																&str[7].scoreJoueurActuel,
																																&str[8].scoreJoueurActuel,
																																&str[9].scoreJoueurActuel,
																																&str[10].scoreJoueurActuel,
																																&str[11].scoreJoueurActuel,
																																&str[12].scoreJoueurActuel,
																																&str[13].scoreJoueurActuel,
																																&str[14].scoreJoueurActuel,
																																&str[15].scoreJoueurActuel,
																																&str[1].multiplicator,
																																&str[2].multiplicator,
																																&str[3].multiplicator,
																																&str[4].multiplicator,
																																&str[5].multiplicator,
																																&str[6].multiplicator,
																																&str[7].multiplicator,
																																&str[8].multiplicator,
																																&str[9].multiplicator,
																																&str[10].multiplicator,
																																&str[11].multiplicator,
																																&str[12].multiplicator,
																																&str[13].multiplicator,
																																&str[14].multiplicator,
																																&str[15].multiplicator
																															);


		///////////////////////////////////////////////////////////
		// REMPLISSAGE DE L'EMPLACEMENT 0 PAR MEILLEURE SCORE ET CLASSEMENT DU JOUEUR
		sprintf(str[0].nomJeux,"SCORE : %d",str[0].score);
		sprintf(str[0].nomJoueur,"CLASSEMENT : %d / %d",temp1,temp2);
		str[0].multiplicator = temp2; // STOCKER NB JOOUEUR ICI

		return EXIT_SUCCESS;
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




int windowMaxSize(int optionFullScreen)
{
    /////////////////////////////////////////////////////
    // CREATION VARIABLE
    SDL_Rect dm;

    /////////////////////////////////////////////////////
    // RECUPRATION DE LA TAILLE D'ECRAN SI CA ECHOU ON RECUPERER L'ERREUR
	if(optionFullScreen){
		if (SDL_GetDisplayBounds(0,&dm) != 0)
	    {
	        fprintf(EXT_FILE,"room.c : windowMaxSize() : SDL_GetDisplayBounds : %s\n",SDL_GetError() );
	        return EXIT_FAILURE;
	    }
	}
	else{
		if (SDL_GetDisplayUsableBounds(0,&dm) != 0)
	    {
	        fprintf(EXT_FILE,"room.c : windowMaxSize() : SDL_GetDisplayBounds : %s\n",SDL_GetError() );
	        return EXIT_FAILURE;
	    }
	}


	if(!optionFullScreen){
		dm.w *= RATIO_WINDOW_SCREEN;
	    dm.h *= RATIO_WINDOW_SCREEN;
	}


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

		fprintf(EXT_FILE,"room.c : windowMaxSize() : %dx%d\n",WinWidth,WinHeight);
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
	if(1000.0/delayFrame > 1)
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
		fprintf(EXT_FILE,"room.c : attendreXsecondeMessage() : FPS %f\n",_IPS );
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



void reglageVolume(int channel, float xa, float ya, float xb, float yb, float porter, float angleJoueur, int max_volume)
{
	////////////////////////////////////////////////////
	// FIX VOLUME MAX PAR DEFAULT
	float volume = max_volume;
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
	if ( distancePoint(dernierePosition->px,dernierePosition->pz, camera.px, camera.pz) > SON_PAS_DISTANCE )
	{
		////////////////////////////////////////////////////
		// SI ON A PARCOURU 1.6M ON JOUE UN SONS
		// MISE A JOUR DE LA DERNIERE POSITION OU ON A JOUER LE SON
		*dernierePosition = camera;
		Mix_PlayChannel(channel,music,0);
	}
}


void GlDessinerQuad(struct GL_Quadf quad)
{
	glBegin(GL_QUADS);

		glVertex3f( quad.x + quad.largueur - (quad.largueur/2.0) ,  quad.z  + quad.hauteur   - (quad.hauteur/2.0) , quad.y + quad.epaisseur  - (quad.epaisseur/2.0) );
		glVertex3f( quad.x + quad.largueur - (quad.largueur/2.0) ,  quad.z  + quad.hauteur   - (quad.hauteur/2.0) , quad.y                   - (quad.epaisseur/2.0) );
		glVertex3f( quad.x                 - (quad.largueur/2.0) ,  quad.z  + quad.hauteur   - (quad.hauteur/2.0) , quad.y                   - (quad.epaisseur/2.0) );
		glVertex3f( quad.x                 - (quad.largueur/2.0) ,  quad.z  + quad.hauteur   - (quad.hauteur/2.0) , quad.y + quad.epaisseur  - (quad.epaisseur/2.0) );

		glVertex3f( quad.x + quad.largueur - (quad.largueur/2.0) ,  quad.z                   - (quad.hauteur/2.0) , quad.y  + quad.epaisseur - (quad.epaisseur/2.0) );
		glVertex3f( quad.x + quad.largueur - (quad.largueur/2.0) ,  quad.z                   - (quad.hauteur/2.0) , quad.y                   - (quad.epaisseur/2.0) );
		glVertex3f( quad.x + quad.largueur - (quad.largueur/2.0) ,  quad.z  + quad.hauteur   - (quad.hauteur/2.0) , quad.y                   - (quad.epaisseur/2.0) );
		glVertex3f( quad.x + quad.largueur - (quad.largueur/2.0) ,  quad.z  + quad.hauteur   - (quad.hauteur/2.0) , quad.y + quad.epaisseur  - (quad.epaisseur/2.0) );

		glVertex3f( quad.x                 - (quad.largueur/2.0) ,  quad.z                   - (quad.hauteur/2.0) , quad.y  + quad.epaisseur - (quad.epaisseur/2.0) );
		glVertex3f( quad.x                 - (quad.largueur/2.0) ,  quad.z                   - (quad.hauteur/2.0) , quad.y                   - (quad.epaisseur/2.0) );
		glVertex3f( quad.x + quad.largueur - (quad.largueur/2.0) ,  quad.z                   - (quad.hauteur/2.0) , quad.y                   - (quad.epaisseur/2.0) );
		glVertex3f( quad.x + quad.largueur - (quad.largueur/2.0) ,  quad.z                   - (quad.hauteur/2.0) , quad.y + quad.epaisseur  - (quad.epaisseur/2.0) );

		glVertex3f( quad.x                 - (quad.largueur/2.0) ,  quad.z  + quad.hauteur   - (quad.hauteur/2.0) , quad.y  + quad.epaisseur - (quad.epaisseur/2.0) );
		glVertex3f( quad.x                 - (quad.largueur/2.0) ,  quad.z  + quad.hauteur   - (quad.hauteur/2.0) , quad.y                   - (quad.epaisseur/2.0) );
		glVertex3f( quad.x                 - (quad.largueur/2.0) ,  quad.z                   - (quad.hauteur/2.0) , quad.y                   - (quad.epaisseur/2.0) );
		glVertex3f( quad.x                 - (quad.largueur/2.0) ,  quad.z                   - (quad.hauteur/2.0) , quad.y  + quad.epaisseur - (quad.epaisseur/2.0) );

		glVertex3f( quad.x + quad.largueur - (quad.largueur/2.0) ,  quad.z  + quad.hauteur   - (quad.hauteur/2.0) , quad.y                   - (quad.epaisseur/2.0) );
		glVertex3f( quad.x + quad.largueur - (quad.largueur/2.0) ,  quad.z                   - (quad.hauteur/2.0) , quad.y                   - (quad.epaisseur/2.0) );
		glVertex3f( quad.x                 - (quad.largueur/2.0) ,  quad.z                   - (quad.hauteur/2.0) , quad.y                   - (quad.epaisseur/2.0) );
		glVertex3f( quad.x                 - (quad.largueur/2.0) ,  quad.z  + quad.hauteur   - (quad.hauteur/2.0) , quad.y                   - (quad.epaisseur/2.0) );

		glVertex3f( quad.x + quad.largueur - (quad.largueur/2.0) ,  quad.z                   - (quad.hauteur/2.0) , quad.y + quad.epaisseur  - (quad.epaisseur/2.0) );
		glVertex3f( quad.x + quad.largueur - (quad.largueur/2.0) ,  quad.z  + quad.hauteur   - (quad.hauteur/2.0) , quad.y + quad.epaisseur  - (quad.epaisseur/2.0) );
		glVertex3f( quad.x                 - (quad.largueur/2.0) ,  quad.z  + quad.hauteur   - (quad.hauteur/2.0) , quad.y + quad.epaisseur  - (quad.epaisseur/2.0) );
		glVertex3f( quad.x                 - (quad.largueur/2.0) ,  quad.z                   - (quad.hauteur/2.0) , quad.y + quad.epaisseur  - (quad.epaisseur/2.0) );

		glEnd();
}

void SDL_GL_AppliquerScene(SDL_Window * Window, const C_STRUCT aiScene *scene,struct Camera_s *camera,GLuint *scene_list,const float IPS)
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
	mouvementCamera(Window, camera,IPS);
	////////////////////////////////////////////////////
	// GERER LES LUMIERE PRESENTE DANS LA SALLE
	GLlightMode();
	////////////////////////////////////////////////////
	// SI LE RENDU DE LA SCENE N'EST PAS FAIT LE FAIRE
	GlDessinerQuad(toiletteFemme);
	GlDessinerQuad(toiletteHomme);

	if(*scene_list == 0) {
		*scene_list = glGenLists(1);
		glNewList(*scene_list, GL_COMPILE);
		GLuint ivao = 0;
		afficherScene(scene, scene->mRootNode,&ivao,_textures,_counts);
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
	gluPerspective(camera.ouverture,(float)(width)/(float)(height),0.2,200);

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


	cible[FLAPPY_HARD-1].px = -6.562877;
	cible[FLAPPY_HARD-1].pz = 12.924735;
	cible[FLAPPY_HARD-1].py = 3.609998;
	cible[FLAPPY_HARD-1].cible_py = -0.332000;
	cible[FLAPPY_HARD-1].angle = M_PI;
	cible[FLAPPY_HARD-1].ouverture =70;

	cible[TETRIS_HARD-1].px = -4.612497;
	cible[TETRIS_HARD-1].pz = 12.926734;
	cible[TETRIS_HARD-1].py = 3.609998;
	cible[TETRIS_HARD-1].cible_py = -.329500;
	cible[TETRIS_HARD-1].angle = M_PI;
	cible[TETRIS_HARD-1].ouverture =70;

	cible[ASTEROID_HARD-1].px = -2.680496;
	cible[ASTEROID_HARD-1].pz = 12.925740;
	cible[ASTEROID_HARD-1].py = 3.610998;
	cible[ASTEROID_HARD-1].cible_py = -.329000;
	cible[ASTEROID_HARD-1].angle = M_PI;
	cible[ASTEROID_HARD-1].ouverture =70;




	cible[SHOOTER_HARD-1].px = -6.56;
	cible[SHOOTER_HARD-1].pz = 9.195632;
	cible[SHOOTER_HARD-1].py = 3.609998;
	cible[SHOOTER_HARD-1].cible_py = -.329000;
	cible[SHOOTER_HARD-1].angle = 0.0;
	cible[SHOOTER_HARD-1].ouverture =70;

	cible[SNAKE_HARD-1].px = -4.627522;
	cible[SNAKE_HARD-1].pz = 9.194634;
	cible[SNAKE_HARD-1].py = 3.612998;
	cible[SNAKE_HARD-1].cible_py = -.330000;
	cible[SNAKE_HARD-1].angle = 0.001000;
	cible[SNAKE_HARD-1].ouverture =70;

	cible[DEMINEUR_HARD-1].px = -2.677498;
	cible[DEMINEUR_HARD-1].pz = 9.194127;
	cible[DEMINEUR_HARD-1].py = 3.609998;
	cible[DEMINEUR_HARD-1].cible_py = -.328500;
	cible[DEMINEUR_HARD-1].angle = 0;
	cible[DEMINEUR_HARD-1].ouverture =70;


	// // // // //


	cible[DEMINEUR_EASY-1].px = 2.703498;
	cible[DEMINEUR_EASY-1].pz = 12.925235;
	cible[DEMINEUR_EASY-1].py = 3.609998;
	cible[DEMINEUR_EASY-1].cible_py = -.327500;
	cible[DEMINEUR_EASY-1].angle = M_PI;
	cible[DEMINEUR_EASY-1].ouverture =70;

	cible[SNAKE_EASY-1].px = 4.651513;
	cible[SNAKE_EASY-1].pz = 12.924240;
	cible[SNAKE_EASY-1].py = 3.614998;
	cible[SNAKE_EASY-1].cible_py = -.331000;
	cible[SNAKE_EASY-1].angle = M_PI;
	cible[SNAKE_EASY-1].ouverture =70;

	cible[SHOOTER_EASY-1].px = 6.587511;
	cible[SHOOTER_EASY-1].pz = 12.924235;
	cible[SHOOTER_EASY-1].py = 3.609998;
	cible[SHOOTER_EASY-1].cible_py = -.332000;
	cible[SHOOTER_EASY-1].angle = M_PI;
	cible[SHOOTER_EASY-1].ouverture =70;



	cible[ASTEROID_EASY-1].px = 2.706498;
	cible[ASTEROID_EASY-1].pz = 9.193630;
	cible[ASTEROID_EASY-1].py = 3.612498;
	cible[ASTEROID_EASY-1].cible_py = -.331500;
	cible[ASTEROID_EASY-1].angle = 0.0;
	cible[ASTEROID_EASY-1].ouverture =70;

	cible[TETRIS_EASY-1].px = 4.637994;
	cible[TETRIS_EASY-1].pz = 9.192131;
	cible[TETRIS_EASY-1].py = 3.612498;
	cible[TETRIS_EASY-1].cible_py = -.329000;
	cible[TETRIS_EASY-1].angle = 0;
	cible[TETRIS_EASY-1].ouverture =70;

	cible[FLAPPY_EASY-1].px = 6.59;
 	cible[FLAPPY_EASY-1].pz = 9.195626;
 	cible[FLAPPY_EASY-1].py = 3.613497;
 	cible[FLAPPY_EASY-1].cible_py = -.331000;
	cible[FLAPPY_EASY-1].angle = 0;
	cible[FLAPPY_EASY-1].ouverture =70;



	cible[12].px = -13.10;
	cible[12].pz = 10.50;
	cible[12].py = 3.609998;
	cible[12].cible_py = -.332000;
	cible[12].angle = 3*M_PI/2;
	cible[12].ouverture =70;

	cible[13].px = -13.10;
	cible[13].pz = 12.44;
	cible[13].py = 3.609998;
	cible[13].cible_py = -.332000;
	cible[13].angle = 3*M_PI/2;
	cible[13].ouverture =70;

	cible[14].px = 0.0;
	cible[14].pz = 0.82;
	cible[14].py = 3.609998 + 0.3F;
	cible[14].cible_py = -.332000;
	cible[14].angle = M_PI;
	cible[14].ouverture =70;

	cible[15].px = 0.000;
	cible[15].pz = 19.523005;
	cible[15].py = 3.665997;
	cible[15].cible_py = -.152400;
	cible[15].angle = 0.0;
	cible[15].ouverture =70;
}

/* ---------------------------------------------------------------------------- */


void mouvementCamera(SDL_Window * Window, struct Camera_s *camera, const float IPS)
{
	///////////////////////////////////////////////////
	// REGLAGE EN FONCTION DES FPS FIXER
	// VALEUR BASER SUR 60 FPS DE BASE

	// DETECTER COURSE A PIED



	//float _SENSIBILITE_CAMERA = 0.0005;//SENSIBILITE_CAMERA * (IPS/FPS);
	//float _VITESSE_DEPLACEMENT = 0.0005;//VITESSE_DEPLACEMENT * (IPS/FPS);

    float _SENSIBILITE_CAMERA = SENSIBILITE_CAMERA * (FPS/IPS);
	float _VITESSE_DEPLACEMENT = VITESSE_DEPLACEMENT * (FPS/IPS);
	///////////////////////////////////////////////////
	// INITALISATION D'UN EVENEMENT SDL
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);


	//DETECTER LA COURSE A PIED
	if( keystate[SDL_SCANCODE_LSHIFT] )
	{
		_VITESSE_DEPLACEMENT = VITESSE_DEPLACEMENT_COURIR * (FPS/IPS);
	}

	///////////////////////////////////////////////////
	// MOUVEMENT CAMERA SUR X ET Z


	////////////////////////////
	// RECUPERER POSITION SOURIS
	SDL_PumpEvents();

	#ifndef __linux__


	// FIRST_FRAME
	// PERMET DE BLOQUER LE MOUVEMENT DE LA SOURIS TEMPS QUE LA PREMIERE FRAME N EST PAS ATTEINTE
	if ( ( SDL_GetWindowFlags(Window) & (SDL_WINDOW_INPUT_FOCUS ) )  && FIRST_FRAME   ){
		int mouseX,mouseY;
        SDL_RaiseWindow(Window);
		SDL_GetMouseState(&mouseX, &mouseY);

		///////////////////////////////////////////////////
		// RECUPERER TAILLE ECRAN
		// CELA PERMET DE COMPENSER LA DIFFERENCE ENTRE POSITION SOURIS ET PLACEMENT GLOBAL DE LA SOURIS
		// AFIN DEVITER DE PASSER EN PARAMETRE WINDOW

		///////////////////////////////////////////////////
		// APPLIQUER LA DIFFERENCE DE DEPLACEMENT A LA CAMERA
		camera->angle -= ( (mouseX  + (bounds.w-WinWidth) /2) - ((WinWidth/2) + (bounds.w-WinWidth) /2 ) )* SENSIBILITE_CAMERA_SOURIS;

		///////////////////////////////////////////////////
		// VERIFIER QU"ON NE VAS PAS DEPASSER LA LIMITE FIXER
		if(camera->cible_py - (( (mouseY  + (bounds.h-WinHeight) /2) - ((WinHeight/2) + (bounds.h-WinHeight) /2 ) )* SENSIBILITE_CAMERA_SOURIS) < MAX_Y_AXE_CIBLE && camera->cible_py - (( (mouseY  + (bounds.h-WinHeight) /2) - ((WinHeight/2) + (bounds.h-WinHeight) /2 ) )* SENSIBILITE_CAMERA_SOURIS) > -MAX_Y_AXE_CIBLE)
			// APPLIQUER LA DIFFERENCE DE DEPLACEMENT A LA CAMERA
			camera->cible_py -= ( (mouseY  + (bounds.h-WinHeight) /2) - ((WinHeight/2) + (bounds.h-WinHeight) /2 ) )* SENSIBILITE_CAMERA_SOURIS;

		///////////////////////////////////////////////////
		// RECENTRAGE DE CAMERA

		SDL_WarpMouseInWindow(Window, (WinWidth/2)  ,(WinHeight/2) );

	}
	else if(!FIRST_FRAME)
	{
		SDL_WarpMouseInWindow(Window, (WinWidth/2)  ,(WinHeight/2) );
		FIRST_FRAME = 1;
	}



	#endif
		///////////////////////////////////////////////////

	// REDUIT L'ECART D ANGLE A UN ANGLE IDENTIQUE
	// COMPRIS DANS UN INTERVALE
	while( camera->angle > 2 * M_PI)
		camera->angle -= 2*M_PI;
	while( camera->angle < 0)
		camera->angle += 2*M_PI;



	// APUI FLECHE GAUCHE
	if( keystate[SDL_SCANCODE_LEFT] )
	{
		camera->angle += _SENSIBILITE_CAMERA;
		///////////////////////////////////////////////////
	}

	///////////////////////////////////////////////////
	// APUI FLECHE DROITE
	if( keystate[SDL_SCANCODE_RIGHT] )
	{
		camera->angle -= _SENSIBILITE_CAMERA;
		///////////////////////////////////////////////////
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
	if (camera->px <= 2.1 && camera->px >= -3.0 && camera->pz <= 3.5 && camera->pz >= -3.5)
		camera->py = 0.6F + HAUTEUR_CAMERA;
	else
		camera->py = HAUTEUR_CAMERA;




	/*// RECENTRE LA SOURIS
	if(mouseX <= WinWidth/2 - M_PI*SENSIBILITE_CAMERA_SOURIS)
	{
		SDL_WarpMouseGlobal(mouseX + 2*M_PI*SENSIBILITE_CAMERA_SOURIS + (bounds.w-WinWidth) /2  ,mouseY + (bounds.h-WinHeight) /2);
	}
	if(mouseX >= WinWidth/2 + M_PI*SENSIBILITE_CAMERA_SOURIS)
	{
		SDL_WarpMouseGlobal(mouseX - 2*M_PI*SENSIBILITE_CAMERA_SOURIS + (bounds.w-WinWidth) /2 ,mouseY + (bounds.h-WinHeight) /2);
	}*/




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
	if( x <= 6.0 && x >= -6.0 && y >= 19.5 && y < 25.0)
		return 0;
	// MUR DERRIERE LE BAR NINETEEN
	if(x > -9.0 && x < 9.0 && y > 23.8 && y < 24.5)
		return 0;

	///////////////////////////////////////////////////
	// FUNTION POUR MUR AVEC ANGLE A PROXIMITER DE NINETEEN
	// CELUI DE DROITE FACE A LA MACHINE
	if(  y >= 14.0 &&  x <= -(-0.4736842105*y + 19.36842105)    )
		return 0;

	///////////////////////////////////////////////////
	// FUNTION POUR MUR AVEC ANGLE A PROXIMITER DE NINETEEN
	// CELUI DE GAUCHE FACE A LA MACHINE
	///////////////////////////////////////////////////
	// COTER GAUCHE PORTE
	if( y >= 14.0 && y < 17.5  && x >= -0.4736842105*y + 19.36842105  )
		return 0;
	// COTER DROIT PORTE
	if( y >= 19.2  && x >= -0.4736842105*y + 19.36842105 && x < 11.3  )
		return 0;

	///////////////////////////////////////////////////
	// LOT 12 MACHINE MILIEU DE SALLE
	if( y >= 8.5 && y <= 13.5 && (  (x <= 8.0 && x >= 1.4) || (x > -8.0 && x <= -1.4)   )   )
		return 0;

	///////////////////////////////////////////////////
	// DOUBLE MACHINE A GAUCHE DE LA SALLE
	if( x <= -12.5 && y <= 13.72 && y >= 9.0)
		return 0;

	///////////////////////////////////////////////////
	// MACHINE SEUL CENTRAL
	if( x >= -1.5 && x <= 1.5 && y >= -1.5 && y <= 1.5)
		return 0;



	///////////////////////////////////////////////////
	// OUVERTURE ENTRE POUR ALLER DANS LA SALLE TOILETTE
	if( (y >= 7.0 || y <= 2.5) && x >= 14.0 && x <= 16.0)
		return 0;

	///////////////////////////////////////////////////
	// MUR QUI SEPARE LA SALLE ET LES TOILETTES
	if( x >= 18.0 && x <= 19.0 && y >= -0.5 && y <= 10.5 )
		return 0;

	///////////////////////////////////////////////////
	// SEPRATION CANAPER / SALLE
	if( x > 2.5 && x < 4.45 && y < 5.6)
		return 0;

	///////////////////////////////////////////////////
	// HITOBX CANAPER 1
	if( x > 4.4 && x < 11.0 && y < 4.2 && y > 0.9)
		return 0;

	///////////////////////////////////////////////////
	// HITOBX CANAPER 2
	if( x > 4.4 && x < 7.2 && y < 0.9)
			return 0;

	///////////////////////////////////////////////////
	// HITOBX TABLE CANAPER
	if( x > 7.7 && x < 11.0 && y < 0.2 && y > -3.05)
			return 0;

	///////////////////////////////////////////////////
	// BILLARD
	if( x <= -5.0 && x >= -13.5 && y <= 2.5 && y >= -2.5)
		return 0;

	///////////////////////////////////////////////////
	// DIMESION SALLE DES TOILETTE
	if(x > 15.0 && (  y < -2.6 || y > 12.0  )  )
		return 0;

	///////////////////////////////////////////////////
	// SEPARATION ENTRE TOILETTE
	if( x > 21.0 && y < 9.0 && y > 0.5)
		return 0;

	///////////////////////////////////////////////////
	// SEPARATION AXE Y TOILETTE
	if( x > 18.0 && y > 4.5 && y < 5.5)
		return 0;

	///////////////////////////////////////////////////
	// PORTE TOILETTE FEMME OBSTACLE
	if( y < 9.1 && y > 8.2 && x > toiletteFemme.x - 2.2)
		return 0;

	///////////////////////////////////////////////////
	// PORTE TOILETTE HOMME OBSTACLE
	if( y < 1.4 && y > 0.4 && x > toiletteHomme.x - 2.2)
		return 0;


	///////////////////////////////////////////////////
	// DIMESION SALLE DE BASE
	// HAUT DE LA SALLE
	if( y < -4.5 )
		return 0;
	// BAS DE LA SALLE
	if( y > 35.2 )
		return 0;
	// GAUCHE DE LA SALLE
	if( x < -14.0 )
		return 0;
	// DROIT DE LA SALLE
	if ( x > 22.8 )
		return 0;
	return 1;
}


int detecterRadio(float x,float y,float angle)
{

	///////////////////////////////////////////////////
	// DETECTER D'UN ANGLE FACE A LA RADIO
	if( ( angle > ( 2*M_PI - (ANGLE_DETECTION_MACHINE) ) && angle <= 2*M_PI  ) ||    (    angle >= 0 && angle < 0 + (ANGLE_DETECTION_MACHINE)   )   )
	{
		////////////////////////////////////////
		// DETECTION RADIO MUSIC 1
		if( x > -2.3 && x < -1.2 && y > 19.0 && y < 19.55)
		{
			return 1;
		}
	}

	////////////////////////////////////////
	// DETECTION RADIO MUSIC 2
	if( x < 11.6 && x > 11.0 && y > -1.0 && y < 0.0)
	{
		return 2;
	}

	///////////////////////////////////////////////////
	// DETECTER D'UN ANGLE FACE A LA RADIO
	if(angle > M_PI - (ANGLE_DETECTION_MACHINE) && angle < M_PI + (ANGLE_DETECTION_MACHINE) )
	{
		////////////////////////////////////////
		// DETECTION RADIO MUSIC 3
		if(x > -12.5 && x < -11.5 && y < 3.0  && y > 2.4)
			return 3;
	}

	return 0;
}

int detecterPorte(float x,float y,float angle)
{

	///////////////////////////////////////////////////
	// DETECTER D'UN ANGLE FACE A LA RADIO
	if( ( angle > ( 2*M_PI - (ANGLE_DETECTION_MACHINE) ) && angle <= 2*M_PI  ) ||    (    angle >= 0 && angle < 0 + (ANGLE_DETECTION_MACHINE)   )   )
	{
		////////////////////////////////////////
		// DETECTION RADIO MUSIC 1
		if( y > 33)
		{
			return 1;
		}
	}

	return 0;
}

int detecterOuvertureToilette(float x,float y,float angle)
{

	////////////////////////////////////////
	// DETECTION TOILETTE FEMME
	if( x > 18.9 && x < 21.8 && y > 8.5 && y < 10.5)
	{
		return 1;
	}



	////////////////////////////////////////
	// DETECTION TOILETTE HOMME
	if( x > 18.9 && x < 21.8 && y > -0.2 && y < 0.8)
	{
		return 2;
	}


	////////////////////////////////////////
	// OUVERTURE AUTOMATIQUE FEMME
	if( x > 19.0 && x < 21.0 && y > 7.5 && y < 8.2)
	{
		return 3;
	}
	////////////////////////////////////////
	// OUVERTURE AUTOMATIQUE HOMME
	if( x > 19.0 && x < 21.0 && y > 1.0 && y < 2.0)
	{
		return 4;
	}

	return 0;
}

int detecterMachine(float x,float y,float angle)
{
	///////////////////////////////////////////////////
	// DETECTION MACHINE
	///////////////////////////////////////////////////
	// DETECTER RANGER 6 MACHINE A GAUCHE
	if(x > -8.0 && x < -1.5 && y > 7.0 && y < 15.0)
	{
		///////////////////////////////////////////////////
		// DETECTER SI RANGER NORD / SUD
		if( y > 11.0)
		{
			///////////////////////////////////////////////////
			// DETECTER SI ON A UN ANGLE MAX DE 60 DEGRES
			if(angle > M_PI - (ANGLE_DETECTION_MACHINE) && angle < M_PI + (ANGLE_DETECTION_MACHINE) )
			{
				///////////////////////////////////////////////////
				// DETECTER PRECICEMENT LA MACHINE PARMIS LES 3
				if( x < -5.7 )
					return 1;
				else if ( x < -3.7)
					return 2;
				else
					return 3;//3
			}
		}
		///////////////////////////////////////////////////
		// RANGER SUD
		else
		{
			///////////////////////////////////////////////////
			// DETECTER SI ON A UN ANGLE MAX DE 60 DEGRES
			if( ( angle > ( 2*M_PI - (ANGLE_DETECTION_MACHINE) ) && angle <= 2*M_PI  ) ||    (    angle >= 0 && angle < 0 + (ANGLE_DETECTION_MACHINE)   )   )
			{
				///////////////////////////////////////////////////
				// DETECTER PRECICEMENT LA MACHINE PARMIS LES 3
				if( x < -5.7 )
					return 4;//4;
				else if ( x < -3.7)
					return 5;
				else
					return 6; //6
			}
		}
	}


	///////////////////////////////////////////////////
	// DETECTER RANGER 6 MACHINE A DROITE
	if(x < 8.0 && x > 1.5 && y > 7.0 && y < 15.0)
	{
		///////////////////////////////////////////////////
		// DETECTER SI RANGER NORD / SUD
		if( y > 11.0) // vrai = face
		{
			///////////////////////////////////////////////////
			// DETECTER SI ON A UN ANGLE MAX DE 60 DEGRES
			if(angle > M_PI - (ANGLE_DETECTION_MACHINE) && angle < M_PI + (ANGLE_DETECTION_MACHINE) )
			{
				if( x < 3.4 )
					return 7; //7
				else if ( x < 5.8)
					return 8;
				else
					return 9; //9
			}
		}
		///////////////////////////////////////////////////
		// RANGER SUD
		else
		{
			///////////////////////////////////////////////////
			// DETECTER SI ON A UN ANGLE MAX DE 60 DEGRES
			if( ( angle > ( 2*M_PI - (ANGLE_DETECTION_MACHINE) ) && angle <= 2*M_PI  ) ||    (    angle >= 0 && angle < 0 + (ANGLE_DETECTION_MACHINE)   )   )
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
		if(angle > 3*M_PI/2 - (ANGLE_DETECTION_MACHINE) && angle < 3*M_PI/2 + (ANGLE_DETECTION_MACHINE))
		{
			if(y < 11.25)
				return 0; //13
			else
				return 0; //14
		}
	}

	///////////////////////////////////////////////////
	// DETECTER MACHINE CENTRAL
	if( x > -1.0 && x < 1.0 && y > 1.0 && y < 3.0)
		///////////////////////////////////////////////////
		// DETECTER SI ON A UN ANGLE MAX DE 60 DEGRES
		if(angle > M_PI - (ANGLE_DETECTION_MACHINE) && angle < M_PI + (ANGLE_DETECTION_MACHINE) )
			return 0; //15

	//////////////////////////////////////////////////
	// DETECTER ORDINATEUR
	if( x > -1.0 && x < 1.0 && y > 18.5 && y < 19.5)
	{
		///////////////////////////////////////////////////
		// DETECTER SI ON A UN ANGLE MAX DE 60 DEGRES
		if( ( angle > ( 2*M_PI - (ANGLE_DETECTION_MACHINE) ) && angle <= 2*M_PI  ) ||    (    angle >= 0 && angle < 0 + (ANGLE_DETECTION_MACHINE)   )   )
		{
			return 16;
		}
	}



	return 0;
}

void messageMachine(struct MeilleureScore_s str[], struct Camera_s camera,TTF_Font *font,int afficherMessage)
{
	SDL_Color white = {255,255,255};
	SDL_Color yellow = {255,255,0};
	///////////////////////////////////////////////////////////////////
	// RETOURNE LE CODE DE LA MACHINE DETECTER 0 SI NON DETECTER
	int detection = detecterMachine(camera.px, camera.pz,camera.angle);
	if(detection > 0 && detection != 16)
	{

		// BUFFER POUR LE MESSAGE DES RECORDS
		char message[60];
		// CONCATENATION DES SCORES ET NOM DES JOUEUR DANS UNE MEME CHAINE
		sprintf(message, "RECORD   :  %d  PAR  %s",str[detection].score,str[detection].nomJoueur);
		// AFFICHAGE DU TEXT DU RECORD
		AfficherText(font,message,white,-1,WinHeight/8);
		// AFFICHAGE DU TEXT DU NOM DU JEU
		AfficherText(font,str[detection].nomJeux,white,-1,WinHeight/6);
		// CONCATENATION MEILLEURE SCORE DU JOUEUR
		sprintf(message, "RECORD  PERSO  :   %d",str[detection].scoreJoueurActuel);
		// AFFICHER RECORD PERSONNEL
		AfficherText(font,message,white,-1,WinHeight*0.9);

		// CONCATENATION MULTIPLICATEURS
		sprintf(message, "MULTIPLICATEUR  :   x%.2f   (  %d  )",str[detection].multiplicator, (int)(str[detection].scoreJoueurActuel*str[detection].multiplicator) );
		// AFFICHER MULTIPLICATEURS
		AfficherText(font,message,yellow,-1,WinHeight*0.85);

		////////////////////////////////////////////////
		// AFFICHAGE CLIGNOTANT
		if(afficherMessage)
				AfficherText(font,"APPUYER   SUR   E",white,-1,-1);

	}
	if(detection == 16)
	{
		if(afficherMessage)
				AfficherText(font,"APPUYER   SUR   E",white,-1,-1);
	}

	////////////////////////////////////////////////
	// MESSAGE OUVERTURE PORTE TOILETTE
	detection = detecterOuvertureToilette(camera.px,camera.pz,camera.angle);
	if(detection == 1 || detection == 2)
	{
		SDL_Color yellow = {230,50,50};
		////////////////////////////////////////////////
		// AFFICHAGE CLIGNOTANT
		AfficherText(font,"APPUYER   SUR   E",yellow,-1,-1);
	}


	////////////////////////////////////////////////
	// MESSAGE STATUT SON DANS LA SALLE
	detection = detecterRadio(camera.px,camera.pz,camera.angle);
	if(detection)
	{
		////////////////////////////////////////////////
		// AFFICHAGE CLIGNOTANT
		if(afficherMessage)
			AfficherText(font,"APPUYER   SUR   E",white,-1,-1);

		////////////////////////////////////////////////
		// STATUT RADIO
		if( Mix_Paused(detection - 1) )
			AfficherText(font,"MUSIC : OFF",white,-1,WinHeight*0.4);
		else
			AfficherText(font,"MUSIC : ON",white,-1,WinHeight*0.4);
	}


	detection = detecterPorte(camera.px,camera.pz,camera.angle);
	if(detection)
	{
		////////////////////////////////////////////////
		// AFFICHAGE CLIGNOTANT
		if(afficherMessage)
			AfficherText(font,"APPUYER   SUR   E",white,-1,-1);
        AfficherText(font,"QUITTER",white,-1,WinHeight*0.4);
	}

}


void animationLancerMachine(struct Camera_s camera, struct Camera_s cible,GLuint scene_list,SDL_Window *Window, float _IPS, float imgAnim)
{

	// FIXER DUREE ANIMATION
	float DUREE_ANIM = imgAnim * (_IPS / FPS );

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
		#ifndef __linux__
		if (SDL_GetWindowFlags(Window) & (SDL_WINDOW_INPUT_FOCUS ))
		  SDL_WarpMouseInWindow(Window, (WinWidth/2)  ,(WinHeight/2) );
		#endif
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
		// REGLAGE DE LA LUMIERE APRES LE DEPLACEMENT
		// DE LA CAMERA
		GLlightMode();
		// APPEL LIST SCENE
		glCallList(scene_list);
		// APPLICATION A LA WINDOW
		SDL_GL_SwapWindow(Window);

	}


}

void show_flags(int flags) {

    printf("\nFLAGS ENABLED: ( %d )\n", flags);
    printf("=======================\n");
    if(flags & SDL_WINDOW_FULLSCREEN)         printf("SDL_WINDOW_FULLSCREEN\n");
    if(flags & SDL_WINDOW_OPENGL)             printf("SDL_WINDOW_OPENGL\n");
    if(flags & SDL_WINDOW_SHOWN)              printf("SDL_WINDOW_SHOWN\n");
    if(flags & SDL_WINDOW_HIDDEN)             printf("SDL_WINDOW_HIDDEN\n");
    if(flags & SDL_WINDOW_BORDERLESS)         printf("SDL_WINDOW_BORDERLESS\n");
    if(flags & SDL_WINDOW_RESIZABLE)          printf("SDL_WINDOW_RESIZABLE\n");
    if(flags & SDL_WINDOW_MINIMIZED)          printf("SDL_WINDOW_MINIMIZED\n");
    if(flags & SDL_WINDOW_MAXIMIZED)          printf("SDL_WINDOW_MAXIMIZED\n");
    if(flags & SDL_WINDOW_INPUT_GRABBED)      printf("SDL_WINDOW_INPUT_GRABBED\n");
    if(flags & SDL_WINDOW_INPUT_FOCUS)        printf("SDL_WINDOW_INPUT_FOCUS\n");
    if(flags & SDL_WINDOW_MOUSE_FOCUS)        printf("SDL_WINDOW_MOUSE_FOCUS\n");
    if(flags & SDL_WINDOW_FULLSCREEN_DESKTOP) printf("SDL_WINDOW_FULLSCREEN_DESKTOP\n");
    if(flags & SDL_WINDOW_FOREIGN)            printf("SDL_WINDOW_FOREIGN\n");
}

void lancerMachine(const C_STRUCT aiScene *scene,int *Running, struct Camera_s camera, struct Camera_s cible[], char *token, struct MeilleureScore_s meilleureScore[],GLuint *scene_list, SDL_Window **Window,SDL_GLContext *Context, int *jouerSonPorteFemme , int *jouerSonPorteHomme, float _IPS, TTF_Font *font)
{


	///////////////////////////////////////////////////
	// GESTION EVENEMENT
	SDL_Event Event;
	while (SDL_PollEvent(&Event))
	{
		///////////////////////////////////////////////////
		// EVENEMENT APPUI TOUCHE
		if (Event.type == SDL_KEYDOWN || Event.type == SDL_MOUSEBUTTONDOWN)
		{
				///////////////////////////////////////////////////
				// TOUCHE ECHAPE METTRE FIN AU JEUX
				if(Event.key.keysym.sym == SDLK_ESCAPE || ((Event.key.keysym.sym == SDLK_SPACE || Event.key.keysym.sym == SDLK_e || Event.type == SDL_MOUSEBUTTONDOWN) && detecterPorte(camera.px,camera.pz,camera.angle)))
				{
					fprintf(EXT_FILE, "room.c : lancerMachine() : Afficher menu escape (Appui touche echap) \n" );
					//////////////////////////////////////////////////////////
					// AFFICHER LA SOURIS
					SDL_ShowCursor(SDL_ENABLE);

					///////////////////////////////////////////////////
					// INIT AFFICHAGE DU MESSAGE
					SDL_GL_AppliquerScene(*Window, scene,&camera,scene_list,FPS);
					MessageQuitterRoom(0,"../room/textures/exit.png");
					SDL_GL_SwapWindow(*Window);

					///////////////////////////////////////////////////
					// VIDER LA LISTE DES EVENEMENTS
					if(Event.key.keysym.sym == SDLK_ESCAPE)
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
							if (Event.type == SDL_KEYDOWN) {
								switch (Event.key.keysym.sym)
								{
									case SDLK_ESCAPE:
										fprintf(EXT_FILE, "room.c : lancerMachine() :Quitter le menu (Appui touche echap) \n" );
										decision = 0;
										FIRST_FRAME = 0;
										SDL_GL_AppliquerScene(*Window, scene,&camera,scene_list,FPS);
										break;
									case SDLK_q:
										fprintf(EXT_FILE, "room.c : lancerMachine() : Quitter le jeu (Appui touche q) \n" );
										decision = 0;
										*Running = 0;
										break;
									case SDLK_d:
										fprintf(EXT_FILE, "room.c : lancerMachine() : Quitter le jeu en ce deconnectant (Appui touche d) \n" );
										decision = 0;
										remove(DIR_TOKEN_FILE);
										*Running = 0;
										break;
									default:break;
								}
							}
						}
					}

					//////////////////////////////////////////////////////////
					// ON DESACTIVER L"AFFICHAGE DE LA SOURIS
					if(*Running != 0)
					{
						//////////////////////////////////////////////////////////
						// CACHER LA SOURIS
						#ifndef __linux__
						if (SDL_GetWindowFlags(*Window) & (SDL_WINDOW_INPUT_FOCUS ) ){
						 	SDL_ShowCursor(SDL_DISABLE);
							///////////////////////////////////////////////////
							// RECENTRAGE DE CAMERA
							SDL_WarpMouseInWindow(*Window, (WinWidth/2)  ,(WinHeight/2) );
						}
						#endif
													///////////////////////////////////////////////////
					}

				}

				///////////////////////////////////////////////////
				// TOUCHE E ENTRER DANS UN JEUX OU CLIQUE GAUCHE
				if(Event.key.keysym.sym == SDLK_SPACE || Event.key.keysym.sym == SDLK_e || Event.type == SDL_MOUSEBUTTONDOWN)
				{
						///////////////////////////////////////////////////
						// VERIFIER SI ON EST PROCHES D UNE MACHINE
						// RENVOIE LE CODE DE LA MACHINE
						int machine = detecterMachine(camera.px, camera.pz, camera.angle);
						if ( machine)
						{
							fprintf(EXT_FILE, "room.c : lancerMachine() : lancement de la machine numero %d \n",machine );

							///////////////////////////////////////////////////
							// CREATION D'UN RENDU AUTRE QUE OPENGL CAR NON COMPATIBLE

							printf("Renderer value 1 : %p FLAG : %d\n", SDL_GetRenderer(*Window) , SDL_GetWindowFlags(*Window) );
							show_flags( SDL_GetWindowFlags(*Window) );
							///////////////////////////////////////////////////
							// ANIMATION CENTRAGE SUR MACHINE
							animationLancerMachine(camera,cible[machine-1],*scene_list,*Window, _IPS,60.0);

							MessageQuitterRoom(1,adresseFontImg[machine-1]);
							MessageQuitterRoom(1,"../room/textures/chargement.png");

							SDL_GL_SwapWindow(*Window);

							///////////////////////////////////////////////////
							// DESTRUCTION DES TEXTURES ET DU CONTEXT
							detruireTexture(scene->mNumMaterials);
							SDL_GL_DeleteContext(*Context);
							
							int x,y;
							SDL_GetWindowPosition(*Window,&x,&y);

							///////////////////////////////////////////////////
							// CREATION DU RENDU POUR CHARGEMENT DES TEXTURES
							SDL_Renderer * pRenderer = SDL_CreateRenderer(*Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC |SDL_RENDERER_TARGETTEXTURE );
							printf("Renderer value 2 : %p FLAG : %d\n", SDL_GetRenderer(*Window) , SDL_GetWindowFlags(*Window) );
							show_flags( SDL_GetWindowFlags(*Window) );
							if(!pRenderer)
							{
								SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Erreur Fatal creation render.",SDL_GetError(),*Window);
								fprintf(EXT_FILE, "room.c : lancerMachine() : Erreur fatal : Impossible de cree un rendu : %s \n",SDL_GetError() );
								break;
							}


							loadGame lg = {machine, pRenderer};
							loadGameTexture(&lg);



              				#ifdef _WIN32
								if(optionFullScreen ){
									SDL_MinimizeWindow(Window);
									SDL_MaximizeWindow(Window);
								}
							#endif

							SDL_ShowCursor(SDL_TRUE);

							if(machine <= 12){
								lancerJeu[machine-1](pRenderer, meilleureScore[machine].scoreJoueurActuel,WinWidth,WinHeight,token,machine <= 6, textures);
								updateMeilleureScore(meilleureScore,token);
							}
							else if(machine == 16)
								leaderboard(pRenderer,WinWidth,WinHeight,meilleureScore[0].multiplicator);

							SDL_ShowCursor(SDL_FALSE);


							///////////////////////////////////////////////////
							// DESTRUCTION DU RENDU ET CONTEXT POUR RECREATION CONTEXT OPENGL
							SDL_DestroyRenderer(pRenderer);
							pRenderer = NULL;
							printf("Renderer value 3 : %p FLAG : %d\n", SDL_GetRenderer(*Window) , SDL_GetWindowFlags(*Window) );
							show_flags( SDL_GetWindowFlags(*Window) );
							SDL_DestroyWindow(*Window);
							*Window = SDL_CreateWindow("Nineteen", x,y, WinWidth, WinHeight, SDL_WINDOW_OPENGL );
							*Context = SDL_GL_CreateContext(*Window);
							printf("Renderer value 4 : %p FLAG : %d\n", SDL_GetRenderer(*Window) , SDL_GetWindowFlags(*Window) );
							show_flags( SDL_GetWindowFlags(*Window) );
							///////////////////////////////////////////////////
							// AFFICHAGE DE LA SCENE
							// RECHARGEMENT DES IMAGES
							///////////////////////////////////////////////////
							// SET BLACK COLOR BACKGROUND
							glClearColor(0.0,0.0,0.0,1.0);
							glClear(GL_COLOR_BUFFER_BIT);
							///////////////////////////////////////////////////
							glLoadIdentity();
							GL_InitialiserParametre(WinWidth,WinHeight,camera);
							MessageQuitterRoom(1,adresseFontImg[machine-1]);
							MessageQuitterRoom(1,"../room/textures/chargement.png");

							SDL_GL_SwapWindow(*Window);

							// ATTENTE POUR MAC OS AFIN DE VOIR L'ANIMATION
							while(SDL_PollEvent(&Event));
							_malloc((void**)&_textures,sizeof(*_textures),(scene->mNumMaterials),EXT_FILE,SDL_MESSAGEBOX_ERROR,"allocation failed","room.c : room() : GLuint *_textures ",*Window);
							chargerTexture(DIR_OBJ_LOAD,scene,_textures,&_counts);

							// REMISE A ZERO DE LA SCENE
							*scene_list = 0;
							static struct Camera_s camera2;
							SDL_GL_AppliquerScene(*Window, scene,&camera2,scene_list,FPS);
							// ANIMATION DE RETOUR SUR MACHINE
							animationLancerMachine(cible[machine-1],camera,*scene_list,*Window, _IPS,60.0);
							// VIDER POLL EVENEMENT
							while(SDL_PollEvent(&Event));


						}


						////////////////////////////////////////////////////////////////////
						// OUVRIR / FERMER LES TOILETTE
						machine = detecterOuvertureToilette(camera.px,camera.pz,camera.angle);
						if(machine)
						{
							switch (machine) {
								case 1:{
									if(!statutPorteFemme)
									{
										*jouerSonPorteFemme = 1;
										statutPorteFemme = OUVERTURE;
									}
								}break;
								case 2:{
									if(!statutPorteHomme)
									{
										*jouerSonPorteHomme = 1;
										statutPorteHomme = OUVERTURE;
									}
								}break;
								default:break;
							}
						}

						////////////////////////////////////////////////////////////////////
						// COUPER LANCER MUSIQUE
						machine = detecterRadio(camera.px,camera.pz,camera.angle);
						if(machine)
						{
							fprintf(EXT_FILE, "room.c : lancerMachine() : changement statut channel %d, statut pause : %d\n",machine - 1 , Mix_Paused(machine - 1) );
							if(Mix_Paused(machine - 1) )
								Mix_Resume(machine - 1);
							else
								Mix_Pause(machine - 1);
						}

					}



				///////////////////////////////////////////////////
				// TOUCHE C CE METTRE A CROUPI
				if(Event.key.keysym.sym == SDLK_c)
				{
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
				}
		}

		///////////////////////////////////////////////////
		// IF APPUI CROIS ROUGE
		else if (Event.type == SDL_QUIT)
		{
			fprintf(EXT_FILE, "room.c : lancerMachine() : Appui sur le croix, quitter le programme\n" );
			*Running = 0;
		}


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



void animationPorteToilette(int *statutPorteFemme, int *statutPorteHomme,int *jouerSonPorteFemme,int *jouerSonPorteHomme, int *toiletteFemmeOuverteDelai, int *toiletteHommeOuverteDelai ,Mix_Chunk *sas_ouverture, Mix_Chunk *sas_fermeture,struct Camera_s camera, float IPS)
{
	// ANIMATION FAITE POUR 50 _FPS
	float variateurFPSanimation = (25.0/IPS) * 0.1;
	//////////////////////////////////////////////////////////
	///////// SI IL FAUT JOUER UN SON POUR LES FEMMES
	if(*jouerSonPorteFemme == 1)
	{
		Mix_PlayChannel(4,sas_ouverture,0);
		*jouerSonPorteFemme = 0;
	}
	if(*jouerSonPorteFemme == 2)
	{
		Mix_PlayChannel(4,sas_fermeture,0);
		*jouerSonPorteFemme = 0;
	}
	//////////////////////////////////////////////////////////
	///////// SI IL FAUT JOUER UN SON POUR LES HOMME
	if(*jouerSonPorteHomme == 1)
	{
		Mix_PlayChannel(15,sas_ouverture,0);
		*jouerSonPorteHomme = 0;
	}
	if(*jouerSonPorteHomme == 2)
	{
		Mix_PlayChannel(15,sas_fermeture,0);
		*jouerSonPorteHomme = 0;
	}


	//////////////////////////////////////////////////////////
	// GESTION DES STATUS DE CHAQUE PORTE
	//////////////////////////////////////////////////////////
	// PORTE EN COURS D OUVERTURE FEMME
	if( *statutPorteFemme == OUVERTURE )
	{
		//////////////////////////////////////////////////////////
		// REGLAGE SON TOILETTE FEMME
		reglageVolume(4,toiletteFemme.x,toiletteFemme.y, camera.px, camera.pz,8.0,camera.angle, 70);

		if( toiletteFemme.x >= 23.0 )
		{
			*statutPorteFemme = OUVERTE;
			*toiletteFemmeOuverteDelai = SDL_GetTicks();
		}
		else
		{
			toiletteFemme.x += variateurFPSanimation;
		}
	}
	//////////////////////////////////////////////////////////
	// PORTE EN COURS D OUVERTURE HOMME
	if( *statutPorteHomme == OUVERTURE )
	{
		//////////////////////////////////////////////////////////
		// REGLAGE SON TOILETTE HOMME
		reglageVolume(15,toiletteHomme.x,toiletteHomme.y, camera.px, camera.pz,8.0,camera.angle, 70);


		if( toiletteHomme.x >= 23.0 )
		{
			*statutPorteHomme = OUVERTE;
			*toiletteHommeOuverteDelai = SDL_GetTicks();
		}
		else
		{
			toiletteHomme.x += variateurFPSanimation;
		}

	}

	//////////////////////////////////////////////////////////
	// PORTE EN COURS DE FERMETURE FEMME
	if( *statutPorteFemme == FERMETURE )
	{
		//////////////////////////////////////////////////////////
		// REGLAGE SON TOILETTE FEMME
		reglageVolume(4,toiletteFemme.x,toiletteFemme.y, camera.px, camera.pz,8.0,camera.angle, 70);
		////////////////////////////////////////////////////////
		// RE OUVRIR SI PASSAGE DEVANT LA PORTE DURANT LA FERMETURE
		if(camera.px > 19.0 && camera.pz < 9.0 && camera.pz > 8.0)
		{
			printf("RE OUVRIR\n" );
			*statutPorteFemme = OUVERTURE;
			*toiletteFemmeOuverteDelai = 0;
			*jouerSonPorteFemme = 1;
		}
		////////////////////////////////////////////////////////
		// BOUCLE DE FERMETURE
		else
		{
			if( toiletteFemme.x < 20.3 )
			{
				toiletteFemme.x = 20.2;
				*statutPorteFemme = FERMER;
			}
			else
				toiletteFemme.x -= variateurFPSanimation;
		}

	}
	//////////////////////////////////////////////////////////
	// PORTE EN COURS DE FERMETURE HOMME
	if( *statutPorteHomme == FERMETURE )
	{
		//////////////////////////////////////////////////////////
		// REGLAGE SON TOILETTE HOMME
		reglageVolume(15,toiletteHomme.x,toiletteHomme.y, camera.px, camera.pz,8.0,camera.angle, 70);
		////////////////////////////////////////////////////////
		// RE OUVRIR SI PASSAGE DEVANT LA PORTE DURANT LA FERMETURE
		if(camera.px > 19.0 && camera.pz < 1.5 && camera.pz > 0.4)
		{
			printf("RE OUVRIR\n" );
			*statutPorteHomme = OUVERTURE;
			*toiletteHommeOuverteDelai = 0;
			*jouerSonPorteHomme = 1;
		}
		////////////////////////////////////////////////////////
		// BOUCLE DE FERMETURE
		else
		{
			if( toiletteHomme.x < 20.3 )
			{
				toiletteHomme.x = 20.2;
				*statutPorteHomme = FERMER;
			}

			else
				toiletteHomme.x -= variateurFPSanimation;
		}

	}


	//////////////////////////////////////////////////////////
	// FERMETURE AUTOMATIQUE FEMME
	if( *statutPorteFemme == OUVERTE )
	{
		if( ( *toiletteFemmeOuverteDelai + 3000) < SDL_GetTicks() )
		{
			*statutPorteFemme = FERMETURE;
			*toiletteFemmeOuverteDelai = 0;
			*jouerSonPorteFemme = 2;
		}
	}
	//////////////////////////////////////////////////////////
	// FERMETURE AUTOMATIQUE FEMME
	if( *statutPorteHomme == OUVERTE )
	{
		if( ( *toiletteHommeOuverteDelai + 3000) < SDL_GetTicks() )
		{
			*statutPorteHomme = FERMETURE;
			*toiletteHommeOuverteDelai = 0;
			*jouerSonPorteHomme = 2;
		}
	}


	//////////////////////////////////////////////////////////
	// OUBERTURE INTERIEUR AUTOMATIQUE
	int toilette = detecterOuvertureToilette(camera.px,camera.pz,camera.angle);
	switch (toilette) {
		case 3:{
			if( *statutPorteFemme != OUVERTE && *statutPorteFemme != OUVERTURE)
			{
				*statutPorteFemme = OUVERTURE;
				*jouerSonPorteFemme = 1;
			}
		}break;
		case 4:{
			if( *statutPorteHomme != OUVERTE && *statutPorteHomme != OUVERTURE)
			{
				*statutPorteHomme = OUVERTURE;
				*jouerSonPorteHomme = 1;
			}
		}break;
	}
}




int MessageQuitterRoom(int modeChargement,char path[])
{
	////////////////////////////////////////////////
	// ENVOI MATRICE
	glPushMatrix();
	glLoadIdentity();
	////////////////////////////////////////////////
	// DESACTIVER LES LUMIERE
	glDisable(GL_LIGHTING);
	glLoadIdentity();

	////////////////////////////////////////////////
	// PRECISION SUR LA FENETRE
	gluOrtho2D(0, WinWidth, 0, WinHeight);
	// MOD PROJECTION
	glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();

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
	sImage = IMG_Load(path);

	if(!sImage)
	{
		fprintf(EXT_FILE,"room.c : MessageQuitterRoom() : IMG_Load %s DIR:%s\n",SDL_GetError(),path );
		return EXIT_FAILURE;
	}


	////////////////////////////////////////////////
	// PARAMETRE 2D
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// CONVERTION TEXTURE IMAGE

	if(modeChargement)
	{
		#ifdef __APPLE__
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sImage->w, sImage->h, 0, sImage->format->BytesPerPixel == 3 ? GL_RGB : GL_BGRA, GL_UNSIGNED_BYTE, sImage->pixels);
		#else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sImage->w, sImage->h, 0, sImage->format->BytesPerPixel == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, sImage->pixels);
		#endif
	}
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sImage->w , sImage->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, sImage->pixels);



	// RESCALE IMG
	if(!modeChargement)
	{
		sImage->w *= (WinWidth/2560.0);
		sImage->h *= (WinHeight/1440.0);
	}
	else
	{
		sImage->w = WinWidth;
		sImage->h = WinHeight;
	}

	////////////////////////////////////////////////
	// POSITIONNEMENT DE LA FENETRE QUITTER X
	int x = WinWidth/2 - sImage->w/2;
	////////////////////////////////////////////////
	// POSITIONNEMENT DE LA FENETRE QUITTER Y
	int y = WinHeight/2 - sImage->h/2;
	if(modeChargement)
	{
		x = 0;
		y = 0;
	}

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

	return EXIT_SUCCESS;
}


int backgroundClassement(SDL_Surface *sImage)
{
	////////////////////////////////////////////////
	// ENVOI MATRICE
	glPushMatrix();
	glLoadIdentity();
	////////////////////////////////////////////////
	// DESACTIVER LES LUMIERE
	glDisable(GL_LIGHTING);
	glLoadIdentity();

	////////////////////////////////////////////////
	// PRECISION SUR LA FENETRE
	gluOrtho2D(0, WinWidth, 0, WinHeight);
	// MOD PROJECTION
	glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();

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
	// PARAMETRE 2D
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// CONVERTION TEXTURE IMAGE
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sImage->w , sImage->h, 0, GL_BGRA, GL_UNSIGNED_BYTE, sImage->pixels);

	// RESCALE IMG
	int tmpW= sImage->w * (WinWidth/2560.0);
	int tmpH = sImage->h * (WinHeight/1440.0);

	////////////////////////////////////////////////
	// POSITIONNEMENT DE LA FENETRE QUITTER X
	int x = 0;
	////////////////////////////////////////////////
	// POSITIONNEMENT DE LA FENETRE QUITTER Y
	int y = WinHeight - tmpH;

	////////////////////////////////////////////////
	// DEBUT DU RENDU
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0,0); glVertex2f(x, y);
		glTexCoord2f(1,0); glVertex2f(x + tmpW, y);
		glTexCoord2f(1,-1); glVertex2f(x + tmpW, y + tmpH);
		glTexCoord2f(0,-1); glVertex2f(x, y + tmpH);
	}
	glEnd();
	////////////////////////////////////////////////

	////////////////////////////////////////////////
	// DESTRUCTUIN DES ELLEMENTS CREE
	glDeleteTextures(1, &texture);
	////////////////////////////////////////////////
	// RECUPERATION DE LA MATRICE AVANT MODIF
	glPopMatrix();
	glLoadIdentity();

	return EXIT_SUCCESS;
}



void GLlightMode()
{
	glEnable(GL_LIGHTING);	// Active l'éclairage


	// LUMIERE BILLARD
	// ALLUMMAGE
	glEnable(GL_LIGHT0);	// Allume la lumière n°0
	// REGLAGE COULEUR AMBIANT/SPECULARITE/DIFFUSION
	GLfloat light0_billard_ambiant[] = { 1.0, 0.917 , 0.173 , 1.0 };
	GLfloat light0_billard_diffuse[] = { 1.0, 1.0 , 1.0 , 1.0 };
	GLfloat light0_billard_specular[] = { 1.0, 1.0 , 1.0 , 1.0 };
	glLightfv(GL_LIGHT0  , GL_AMBIENT, light0_billard_ambiant);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_billard_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_billard_specular);
	// POSITIONNEMENT DE LA LAMPE
	GLfloat light0_billard_position[]={-9.3 , 5.4 , 0.0 , 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light0_billard_position);
	// DIRECTION DU SPOT
	GLfloat light0_billard_spot_direction[]={ 0.0 , -1.0 , 0.0 };
	glLightf(GL_LIGHT0 , GL_SPOT_CUTOFF, 90.0);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light0_billard_spot_direction);
	glLightf(GL_LIGHT0 , GL_SPOT_EXPONENT, 2.0);
	// ATTENUATION DU SPOT
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.f);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1);
	glLightf(GL_LIGHT0 , GL_QUADRATIC_ATTENUATION, 0.f);

////////////////////////////////////////////////////////////////////////////////

	// LUMIERE MURALE
	// ALLUMMAGE
	glEnable(GL_LIGHT1);	// Allume la lumière n°1
	// REGLAGE COULEUR AMBIANT/SPECULARITE/DIFFUSION
	GLfloat light1_murale_ambiant[] = { 1.0, 0.917 , 0.173 , 1.0 };
	GLfloat light1_murale_diffuse[] = { 1.0, 1.0 , 1.0 , 1.0 };
	GLfloat light1_murale_specular[] = { 1.0, 1.0 , 1.0 , 1.0 };
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_murale_ambiant);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_murale_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, light1_murale_specular);
	// POSITIONNEMENT DE LA LAMPE
	GLfloat light1_murale_position[]={3.4 , 3.8 , -4.8 , 1.0 };
	glLightfv(GL_LIGHT1, GL_POSITION, light1_murale_position);
	// DIRECTION DU SPOT
	GLfloat light1_murale_spot_direction[]={ 0.0 , 0.0 , -1.0 };
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light1_murale_spot_direction);
	// ATTENUATION DU SPOT
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.1f);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.01);
	glLightf(GL_LIGHT1 , GL_QUADRATIC_ATTENUATION, 0.1);

////////////////////////////////////////////////////////////////////////////////

	// LUMIERE MURALE
	// ALLUMMAGE
	glEnable(GL_LIGHT2);	// Allume la lumière n°2
	// REGLAGE COULEUR AMBIANT/SPECULARITE/DIFFUSION UTILISATION PRESET LAMP 1
	glLightfv(GL_LIGHT2, GL_AMBIENT, light1_murale_ambiant);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, light1_murale_diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, light1_murale_specular);
	// POSITIONNEMENT DE LA LAMPE
	GLfloat light2_murale_position[]={-2.5  , 3.8 , -4.8 , 1.0 };
	glLightfv(GL_LIGHT2, GL_POSITION, light2_murale_position);
	// DIRECTION DU SPOT UTILISATION PRESET LAMP 1
	glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light1_murale_spot_direction);
	// ATTENUATION DU SPOT
	glLightf(GL_LIGHT2, GL_CONSTANT_ATTENUATION, 0.1f);
	glLightf(GL_LIGHT2, GL_LINEAR_ATTENUATION, 0.01);
	glLightf(GL_LIGHT2 , GL_QUADRATIC_ATTENUATION, 0.1);


////////////////////////////////////////////////////////////////////////////////

	// LUMIERE MURALE
	// ALLUMMAGE
	glEnable(GL_LIGHT3);	// Allume la lumière n°3
	// REGLAGE COULEUR AMBIANT/SPECULARITE/DIFFUSION UTILISATION PRESET LAMP 1
	glLightfv(GL_LIGHT3, GL_AMBIENT, light1_murale_ambiant);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, light1_murale_diffuse);
	glLightfv(GL_LIGHT3, GL_SPECULAR, light1_murale_specular);
	// POSITIONNEMENT DE LA LAMPE
	GLfloat light3_murale_position[]={9.2  , 3.8 , -4.8 , 1.0 };
	glLightfv(GL_LIGHT3, GL_POSITION, light3_murale_position);
	// DIRECTION DU SPOT UTILISATION PRESET LAMP 1
	glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, light1_murale_spot_direction);
	// ATTENUATION DU SPOT
	glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION, 0.1f);
	glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, 0.01);
	glLightf(GL_LIGHT3 , GL_QUADRATIC_ATTENUATION, 0.1);


////////////////////////////////////////////////////////////////////////////////

	// LUMIERE MURALE
	// ALLUMMAGE
	glEnable(GL_LIGHT4);	// Allume la lumière n°4
	// REGLAGE COULEUR AMBIANT/SPECULARITE/DIFFUSION UTILISATION PRESET LAMP 1
	glLightfv(GL_LIGHT4, GL_AMBIENT, light1_murale_ambiant);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, light1_murale_diffuse);
	glLightfv(GL_LIGHT4, GL_SPECULAR, light1_murale_specular);
	// POSITIONNEMENT DE LA LAMPE
	GLfloat light4_murale_position[]={-8.5  , 3.8 , -4.8 , 1.0 };
	glLightfv(GL_LIGHT4, GL_POSITION, light4_murale_position);
	// DIRECTION DU SPOT UTILISATION PRESET LAMP 1
	glLightfv(GL_LIGHT4, GL_SPOT_DIRECTION, light1_murale_spot_direction);
	// ATTENUATION DU SPOT
	glLightf(GL_LIGHT4, GL_CONSTANT_ATTENUATION, 0.1f);
	glLightf(GL_LIGHT4, GL_LINEAR_ATTENUATION, 0.01);
	glLightf(GL_LIGHT4 , GL_QUADRATIC_ATTENUATION, 0.1);

////////////////////////////////////////////////////////////////////////////////

	// LUMIERE MURALE
	// ALLUMMAGE
	glEnable(GL_LIGHT5);	// Allume la lumière n°5
	// REGLAGE COULEUR AMBIANT/SPECULARITE/DIFFUSION UTILISATION PRESET LAMP 1
	glLightfv(GL_LIGHT5, GL_AMBIENT, light1_murale_ambiant);
	glLightfv(GL_LIGHT5, GL_DIFFUSE, light1_murale_diffuse);
	glLightfv(GL_LIGHT5, GL_SPECULAR, light1_murale_specular);
	// POSITIONNEMENT DE LA LAMPE
	GLfloat light5_murale_position[]={-14.0  , 3.8 , 1.4 , 1.0 };
	glLightfv(GL_LIGHT5, GL_POSITION, light5_murale_position);
	// DIRECTION DU SPOT
	GLfloat light5_murale_spot_direction[] = { -1.0 , 1.0 , 0.0 };
	glLightfv(GL_LIGHT5, GL_SPOT_DIRECTION, light5_murale_spot_direction);
	// ATTENUATION DU SPOT
	glLightf(GL_LIGHT5, GL_CONSTANT_ATTENUATION, 0.1f);
	glLightf(GL_LIGHT5, GL_LINEAR_ATTENUATION, 0.01);
	glLightf(GL_LIGHT5 , GL_QUADRATIC_ATTENUATION, 0.1);

////////////////////////////////////////////////////////////////////////////////

// LUMIERE MURALE
	// ALLUMMAGE
	glEnable(GL_LIGHT6);	// Allume la lumière n°6
	// REGLAGE COULEUR AMBIANT/SPECULARITE/DIFFUSION UTILISATION PRESET LAMP 1
	glLightfv(GL_LIGHT6, GL_AMBIENT, light1_murale_ambiant);
	glLightfv(GL_LIGHT6, GL_DIFFUSE, light1_murale_diffuse);
	glLightfv(GL_LIGHT6, GL_SPECULAR, light1_murale_specular);
	// POSITIONNEMENT DE LA LAMPE
	GLfloat light6_murale_position[]={-14.0  , 3.8 , 8.1 , 1.0 };
	glLightfv(GL_LIGHT6, GL_POSITION, light6_murale_position);
	// DIRECTION DU SPOT UTILISATION PRESET LAMP 6
	glLightfv(GL_LIGHT6, GL_SPOT_DIRECTION, light5_murale_spot_direction);
	// ATTENUATION DU SPOT
	glLightf(GL_LIGHT6, GL_CONSTANT_ATTENUATION, 0.1f);
	glLightf(GL_LIGHT6, GL_LINEAR_ATTENUATION, 0.01);
	glLightf(GL_LIGHT6 , GL_QUADRATIC_ATTENUATION, 0.1);

////////////////////////////////////////////////////////////////////////////////

	// LUMIERE LOGO NINETEEN
	// ALLUMMAGE
	glEnable(GL_LIGHT7 );	// Allume la lumière n°8
	// REGLAGE COULEUR AMBIANT/SPECULARITE/DIFFUSION
	GLfloat light7_nineteen_ambiant[] = { 0.7, 0.5 , 0.9 , 1.0 };
	GLfloat light7_nineteen_diffuse[] = { 1.0, 1.0 , 1.0 , 1.0 };
	GLfloat light7_nineteen_specular[] = { 1.0, 1.0 , 1.0 , 1.0 };
	glLightfv(GL_LIGHT7, GL_AMBIENT, light7_nineteen_ambiant);
	glLightfv(GL_LIGHT7, GL_DIFFUSE, light7_nineteen_diffuse);
	glLightfv(GL_LIGHT7, GL_SPECULAR, light7_nineteen_specular);
	// POSITIONNEMENT DE LA LAMPE
	GLfloat light7_nineteen_position[]={0.f , 4.4 , 24.5 , 1.0 };
	glLightfv(GL_LIGHT7, GL_POSITION, light7_nineteen_position);
	// DIRECTION DU SPOT
	GLfloat light7_nineteen_spot_direction[]={ 0.0 , 0.0 , -1.0 };

	glLightfv(GL_LIGHT7, GL_SPOT_DIRECTION, light7_nineteen_spot_direction);

	// ATTENUATION DU SPOT
	glLightf(GL_LIGHT7, GL_CONSTANT_ATTENUATION, 0.01f);
	glLightf(GL_LIGHT7, GL_LINEAR_ATTENUATION, 0.01);
	glLightf(GL_LIGHT7 , GL_QUADRATIC_ATTENUATION, 0.02f);
}



void detruireTexture(GLuint nbTextures)
{

  if(_counts) {
    free(_counts);
    _counts = NULL;
  }
  if(_textures) {
    glDeleteTextures(nbTextures, _textures);
    free(_textures);
    _textures = NULL;
  }
}
