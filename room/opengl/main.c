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
#include "main.h"
#include "../../include/libWeb.h"
#include "../../games/3_flappy_bird/flappy_bird.h"
// END INCLUDE



GLuint scene_list = 0; // NB SCENE
float VITESSE_DEPLACEMENT = VITESSE_DEPLACEMENT_DEBOUT;
float HAUTEUR_CAMERA = HAUTEUR_CAMERA_DEBOUT;

#define WinWidth 1920
#define WinHeight 1080

#define FPS 60
static const float FRAME_TIME = 1000/FPS;

static SDL_Window *Window = NULL;
static SDL_GLContext Context;


uint32_t WindowFlags = SDL_WINDOW_OPENGL | SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC ;



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
void lancerMachine(const C_STRUCT aiScene *scene,int *Running, struct Camera_s camera, struct Camera_s cible[],char *token, char *chaine);

float distancePoint(float xa, float ya, float xb, float yb);
void reglageVolume(int channel, float xa, float ya, float xb, float yb, float porter);
void bruitagePas(struct Camera_s *dernierePosition, struct Camera_s camera, int channel, Mix_Chunk *music);

void RenderText(TTF_Font *font, char *message, SDL_Color color, int x, int y, float ouverture);

















































int main( int argc, char *argv[ ], char *envp[ ] )
{
	//SDL INIT
	SDL_Init(SDL_INIT_EVERYTHING);
	Window = SDL_CreateWindow("Nineteen", 0, 0, WinWidth, WinHeight, WindowFlags);
	Context = SDL_GL_CreateContext(Window);

	//MIXER Init
	if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 1, 1024 ) == -1 )
			printf("Erreur init SDL_Mixer : %s\n",Mix_GetError() );






	// PRESET VALUE CAMERA //
	static struct Camera_s camera,cible[15],jouerSon;
	initalisation(&camera,cible);
	jouerSon = camera;

	Mix_Chunk **musicEnvironnement = malloc(sizeof(Mix_Chunk));
	*(musicEnvironnement + 0)	= Mix_LoadWAV("01.wav");
	*(musicEnvironnement + 1)	= Mix_LoadWAV("02.wav");
	*(musicEnvironnement + 2)	= Mix_LoadWAV("03.wav");
	*(musicEnvironnement + 3)	= Mix_LoadWAV("walk.wav");

	Mix_Volume(0,0);
	Mix_Volume(1,0);
	Mix_Volume(2,0);
	Mix_Volume(3,SON_PAS);
	Mix_Volume(4,MIX_MAX_VOLUME/5);
	Mix_Volume(5,MIX_MAX_VOLUME/5);
	Mix_Volume(6,MIX_MAX_VOLUME/5);


	Mix_PlayChannel(0 , *(musicEnvironnement + 0), -1);
	Mix_PlayChannel(1 , *(musicEnvironnement + 1), -1);
	Mix_PlayChannel(2 , *(musicEnvironnement + 2) , -1);




	TTF_Init();
	TTF_Font * font = TTF_OpenFont("police.ttf", WinWidth/50);
	SDL_Color color = {255,255,0};




	const C_STRUCT aiScene* scene = NULL;
	aiImportModel("salle.obj",&scene);
	GL_InitialiserParametre(WinWidth,WinHeight,camera);


	int Running = 1;
	int count_IPS = SDL_GetTicks();
	float ips=0.0;




	// temp a mettre dans le load du launcher;
	char *token = "l8Ysl)8LFn1N^kzzMQY^cV@rlr9*5rrng*LN#_oN3VaxBWsNm(XKhe1OCrKDe1q!#)TAN7O(Yo9ZGh28tF-Hk0Vmnv5)scEkZXlmb*yzznV6QDu!Z9NFpy7IqsVOAZ*qxgYUrD_W6kIT4_ZJ82OtgB)S9Sh!scwlEA5vTA)GlWZlV^uZwEOYVbrKtEQhw#-0$_rMAoi1GDSr)IMtca-x1pDr9UElMThViEiy8(fLA$2NR*7tl5mjqqWf!qMSFtoo";
	char chaine[10];
	getCoinsValues(token,chaine);

	int tscore,tclassement,ttotal;
	sscanf(chaine,"%d %d / %d",&tscore,&tclassement,&ttotal);

	char score[30];
	sprintf(score,"SCORE : %d",tscore);
	char classement[30];
	sprintf(classement,"CLASSEMENT : %d/%d",tclassement,ttotal);



	while (Running)
	{
		int times_at_start_frame = SDL_GetTicks();

		reglageVolume(0,-4.0,10.5,camera.px,camera.pz,10.0);
		reglageVolume(1,4.0,10.5,camera.px,camera.pz,10.0);
		reglageVolume(2,0.0,0.0,camera.px,camera.pz,10.0);
		bruitagePas(&jouerSon,camera,3,*(musicEnvironnement + 3));

		GL_InitialiserParametre(WinWidth,WinHeight,camera);
		SDL_GL_AppliquerScene(scene,&camera);

		lancerMachine(scene,&Running,camera,cible,token,chaine);


		sscanf(chaine,"%d %d / %d",&tscore,&tclassement,&ttotal);
		sprintf(score,"SCORE : %d",tscore);
		sprintf(classement,"CLASSEMENT : %d/%d",tclassement,ttotal);

		RenderText(font,score,color,WinWidth/30,WinHeight - WinWidth/30, camera.ouverture);
		RenderText(font,classement,color,WinWidth/30,WinHeight - WinWidth/15, camera.ouverture);





		SDL_GL_SwapWindow(Window);




		// attente FPS
		int frame_delay = SDL_GetTicks() - times_at_start_frame;
			if(frame_delay < FRAME_TIME)
				SDL_Delay(FRAME_TIME - frame_delay );




		ips++;
		if(SDL_GetTicks() - count_IPS > 1000){
		//	printf("IPS %f\n",ips );
			count_IPS = SDL_GetTicks();
			//sprintf(fpsString,"IPS : %.0f",ips);
			ips = 0;
		}

	}



	TTF_CloseFont(font);
	Mix_FreeChunk(*(musicEnvironnement + 0) );
	Mix_FreeChunk(*(musicEnvironnement + 1) );
	Mix_FreeChunk(*(musicEnvironnement + 2) );
	aiReleaseImport(scene);
	SDL_GL_DeleteContext(Context);
	SDL_DestroyWindow(Window);
	SDL_Quit();
	return 0;
}




























































float distancePoint(float xa, float ya, float xb, float yb)
{
	float resultat = sqrt( pow( (xb - xa) , 2 ) + pow( (yb - ya) , 2 ) ) ;
	if (resultat < 0)
		return resultat * -1;
	else
	 	return resultat;
}


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
}

void bruitagePas(struct Camera_s *dernierePosition, struct Camera_s camera, int channel, Mix_Chunk *music)
{
	if ( distancePoint(dernierePosition->px,dernierePosition->pz, camera.px, camera.pz) > 1.6F )
	{
		*dernierePosition = camera;
		Mix_PlayChannel(channel,music,0);
	}
}


void SDL_GL_AppliquerScene(const C_STRUCT aiScene *scene,struct Camera_s *camera)
{
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	SDL_Color color = {0,0,0};

	mouvementCamera(camera);


	if(scene_list == 0) {
		scene_list = glGenLists(1);
		glNewList(scene_list, GL_COMPILE);
		aiDessinerScene(scene, scene->mRootNode);
		glEndList();
	}

	glCallList(scene_list);
}


void GL_InitialiserParametre(int width, int height, struct Camera_s camera)
{
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
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

	gluPerspective(camera.ouverture,(float)(width)/(float)(height),0.1,100);	//Pour les explications, lire le tutorial sur OGL et win

	//Initialize Modelview Matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
	glLoadIdentity();
}



void initalisation(struct Camera_s *camera, struct Camera_s *cible)
{
	camera->px = START_PX;
	camera->py = START_PY;
	camera->pz = START_PZ;
	camera->cible_py = START_CIBLE_Y;
	camera->angle = START_CIBLE_X;
	camera->ouverture = START_OUVERTURE;


	cible[0].px = -6.56;
	cible[0].pz = 12.9;
	cible[0].py = 3.45;
	cible[0].cible_py = -.34;
	cible[0].angle = M_PI;
	cible[0].ouverture =70;

	cible[1].px = -4.62;
	cible[1].pz = 12.85;
	cible[1].py = 3.45;
	cible[1].cible_py = -.34;
	cible[1].angle = M_PI;
	cible[1].ouverture =70;

	cible[2].px = -2.68;
	cible[2].pz = 12.85;
	cible[2].py = 3.45;
	cible[2].cible_py = -.34;
	cible[2].angle = M_PI;
	cible[2].ouverture =70;

	cible[3].px = -6.56;
	cible[3].pz = 9.25;
	cible[3].py = 3.45;
	cible[3].cible_py = -.34;
	cible[3].angle = 0.0;
	cible[3].ouverture =70;

	cible[4].px = -4.62;
	cible[4].pz = 9.25;
	cible[4].py = 3.45;
	cible[4].cible_py = -.34;
	cible[4].angle = 0;
	cible[4].ouverture =70;

	cible[5].px = -2.68;
	cible[5].pz = 9.25;
	cible[5].py = 3.45;
	cible[5].cible_py = -.34;
	cible[5].angle = 0;
	cible[5].ouverture =70;

	cible[6].px = 2.68;
	cible[6].pz = 12.85;
	cible[6].py = 3.45;
	cible[6].cible_py = -.34;
	cible[6].angle = M_PI;
	cible[6].ouverture =70;

	cible[7].px = 4.62;
	cible[7].pz = 12.85;
	cible[7].py = 3.45;
	cible[7].cible_py = -.34;
	cible[7].angle = M_PI;
	cible[7].ouverture =70;

	cible[8].px = 6.56;
	cible[8].pz = 12.85;
	cible[8].py = 3.45;
	cible[8].cible_py = -.34;
	cible[8].angle = M_PI;
	cible[8].ouverture =70;

	cible[9].px = 2.71;
	cible[9].pz = 9.25;
	cible[9].py = 3.45;
	cible[9].cible_py = -.34;
	cible[9].angle = 0.0;
	cible[9].ouverture =70;

	cible[10].px = 4.65;
	cible[10].pz = 9.25;
	cible[10].py = 3.45;
	cible[10].cible_py = -.34;
	cible[10].angle = 0;
	cible[10].ouverture =70;

	cible[11].px = 6.59;
	cible[11].pz = 9.25;
	cible[11].py = 3.45;
	cible[11].cible_py = -.34;
	cible[11].angle = 0;
	cible[11].ouverture =70;

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

void lancerMachine(const C_STRUCT aiScene *scene,int *Running, struct Camera_s camera, struct Camera_s cible[], char *token, char *chaine)
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
								if ( machine)
								{
									animationLancerMachine(camera,cible[machine-1]);
									// centrer sur la machine //
									// zoomer sur la machine //



									// lancer la machine
									SDL_Renderer *pRenderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC |SDL_RENDERER_TARGETTEXTURE);

									switch (machine) {
										case 1: printf( "\nEXIT CODE = %d\n" , flappy_bird( pRenderer, 1,WinWidth,WinHeight,token));break;
										case 2: SDL_Delay(1500);break;
										case 3: SDL_Delay(1500);break;
										case 4: SDL_Delay(1500);break;
										case 5: SDL_Delay(1500);break;
										case 6: SDL_Delay(1500);break;
										case 7: SDL_Delay(1500);break;
										case 8: SDL_Delay(1500);break;
										case 9: SDL_Delay(1500);break;
										case 10: SDL_Delay(1500);break;
										case 11: SDL_Delay(1500);break;
										case 12: SDL_Delay(1500);break;
										case 13: SDL_Delay(1500);break;
										case 14: SDL_Delay(1500);break;
										case 15: SDL_Delay(1500);break;
										default:break;
									}

									SDL_Renderer *ptr_renderer = pRenderer;
									SDL_Window *ptr_Window = Window;
									// retour sur la Window 3D.


									getCoinsValues(token,chaine);


									// recration du context jeu + fenetre afin de revenir ou nous en etions
									Window = SDL_CreateWindow("Nineteen", 0, 0, WinWidth, WinHeight, WindowFlags );
									Context = SDL_GL_CreateContext(Window);
									scene_list = 0;



									while(SDL_PollEvent(&Event) );
									SDL_GL_AppliquerScene(scene,&camera);
									while(SDL_PollEvent(&Event) );
									animationLancerMachine(cible[machine-1],camera);
									while(SDL_PollEvent(&Event) );

									SDL_DestroyRenderer(ptr_renderer);
								//	SDL_DestroyWindow(ptr_Window);
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


void RenderText(TTF_Font *font, char *message, SDL_Color color, int x, int y, float ouverture)
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
