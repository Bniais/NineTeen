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
#include <math.h>

// LOCAL LIBRARY
#include "import.h" // YOU NEED ASSIMP LIB FOR import.h (README.dm)
#include "main.h"
#include "../../games/3_flappy_bird/flappy_bird.h"
// END INCLUDE



GLuint scene_list = 0; // NB SCENE



#define WinWidth 1600
#define WinHeight 900

#define FPS 60
static const float FRAME_TIME = 1000/FPS;


static SDL_Window *Window = NULL;
static SDL_GLContext Context;


uint32_t WindowFlags = SDL_WINDOW_OPENGL;



// STATIC VAR FOR CAMERA
struct Camera_s
{
	float px,py,pz,cible_py,angle,ouverture;
};


void SDL_GL_AppliquerScene(const C_STRUCT aiScene *scene,struct Camera_s *camera);
void GL_InitialiserParametre(int width, int height, struct Camera_s camera);
void initalisation(struct Camera_s *camera, struct Camera_s *cible);
void mouvementCamera(struct Camera_s *camera);
int detectionEnvironnement(float x,float y);
int detecterMachine(float x,float y);
void animationLancerMachine(struct Camera_s camera, struct Camera_s cible);
void lancerMachine(const C_STRUCT aiScene *scene,int *Running, struct Camera_s camera, struct Camera_s cible[]);





int main( int argc, char *argv[ ], char *envp[ ] )
{
	const C_STRUCT aiScene* scene = NULL; // MODEL/SCENE

	// PRESET VALUE CAMERA //
	static struct Camera_s camera,cible[15];
	initalisation(&camera,cible);

	//SDL INIT
	SDL_Init(SDL_INIT_EVERYTHING);
	Window = SDL_CreateWindow("Nineteen", 0, 0, WinWidth, WinHeight, WindowFlags);
	Context = SDL_GL_CreateContext(Window);

	aiImportModel("salle.obj",&scene);
	GL_InitialiserParametre(WinWidth,WinHeight,camera);

	int Running = 1;

	while (Running)
	{
		int times_at_start_frame = SDL_GetTicks();

		lancerMachine(scene,&Running,camera,cible);

		SDL_GL_AppliquerScene(scene,&camera);


		// attente FPS
		int frame_delay = SDL_GetTicks() - times_at_start_frame;
			if(frame_delay < FRAME_TIME)
				SDL_Delay(FRAME_TIME - frame_delay );
	}



	aiReleaseImport(scene);
	SDL_GL_DeleteContext(Context);
	SDL_DestroyWindow(Window);
	SDL_Quit();
	return 0;
}



































void SDL_GL_AppliquerScene(const C_STRUCT aiScene *scene,struct Camera_s *camera)
{
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	mouvementCamera(camera);

	glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,40);	//définit la taille de la tache spéculaire


	if(scene_list == 0) {
		scene_list = glGenLists(1);
		glNewList(scene_list, GL_COMPILE);
		aiDessinerScene(scene, scene->mRootNode);
		glEndList();
	}


	glCallList(scene_list);

	SDL_GL_SwapWindow(Window);
}


void GL_InitialiserParametre(int width, int height, struct Camera_s camera){
	glEnable(GL_LIGHTING);

	//
	glEnable(GL_LIGHT0);    /* Uses default lighting parameters */

	//
	glEnable(GL_DEPTH_TEST);


	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	gluPerspective(camera.ouverture,(float)(width)/(float)(height),0.1,100);	//Pour les explications, lire le tutorial sur OGL et win
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

	//Initialize Modelview Matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();


	//Initialize clear color
	glClearColor( 0.f, 0.f, 0.f, 1.f );

}



void initalisation(struct Camera_s *camera, struct Camera_s *cible)
{
	camera->px = START_PX;
	camera->py = START_PY;
	camera->pz = START_PZ;
	camera->cible_py = START_CIBLE_Y;
	camera->angle = START_CIBLE_X;
	camera->ouverture = START_OUVERTURE;


	cible[1].px = -6.56;
	cible[1].pz = 12.9;
	cible[1].py = 3.45;
	cible[1].cible_py = -.34;
	cible[1].angle = M_PI;
	cible[1].ouverture =60;
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
		camera->py = 4.5F;
	else
		camera->py = HAUTEUR_CAMERA_DEBOUT;




	// MISE A JOURS DE LA POSITION DE LA CAMERA
	gluLookAt(camera->px                   ,camera->py    ,camera->pz                  ,
			  camera->px+sin(camera->angle) ,camera->py + camera->cible_py    , camera->pz+cos(camera->angle),
			  0.0
			               ,1.0         ,0.0)                        ;
}

int detectionEnvironnement(float x,float y)
{
	printf("X = %f Z = %f\n",x,y);


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
	if( x >= -1.0 && x <= 1.0 && y >= -1.0 && y <= 1.0)
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
	if( x <= -7.5 && x >= -12.0 && y <= 5.0 && y >= -1.5)
		return 0;
	// BASE DE LA SALLE

	// dimension toilette
	if(x > 15.0 && (  y < -3.0 || y > 12.5  )  )
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
		printf("\n%f\n",y);
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
		printf("\n%f\n",y);
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

void animationLancerMachine(struct Camera_s camera, struct Camera_s cible)
{
	float DUREE_ANIM = 60.0F;
	printf("CAM %f CIBLE %f\n",camera.angle,cible.angle );

	int i = 0;
	float x = (cible.px - camera.px)/DUREE_ANIM ;
	float z = (cible.pz - camera.pz)/DUREE_ANIM ;
	float angle = ( (cible.angle - camera.angle)/DUREE_ANIM ) ;
	float cib = (cible.cible_py - camera.cible_py) / DUREE_ANIM;
	float y = (cible.py - camera.py)/DUREE_ANIM;


	while( i++ < 60)
	{
		camera.px += x;
		camera.pz += z;
		camera.angle += angle;
		camera.cible_py += cib;
		camera.py += y;


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

void lancerMachine(const C_STRUCT aiScene *scene,int *Running, struct Camera_s camera, struct Camera_s cible[])
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
								printf("SPACE_BAR \n");

								int machine = detecterMachine(camera.px, camera.pz);
								if ( machine == 1 )
								{
									animationLancerMachine(camera,cible[machine]);
									// centrer sur la machine //
									// zoomer sur la machine //

									// lancer la machine
									SDL_Renderer *pRenderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
									printf( "\nEXIT CODE = %d\n" , flappy_bird( pRenderer, 50,WinWidth,WinHeight));
									SDL_DestroyRenderer(pRenderer);
									SDL_DestroyWindow(Window);
									// retour sur la Window 3D.




									// recration du context jeu + fenetre afin de revenir ou nous en etions
									Window = SDL_CreateWindow("Nineteen", 0, 0, WinWidth, WinHeight, WindowFlags );
									Context = SDL_GL_CreateContext(Window);
									scene_list = 0;
									GL_InitialiserParametre( WinWidth,WinHeight,cible[machine] );
									SDL_GL_AppliquerScene(scene,&camera);
									lancerMachine(scene,Running,camera,cible);
									animationLancerMachine(cible[machine],camera);
								}



					}
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
