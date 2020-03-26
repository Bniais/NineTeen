// CODERUNNER COMPILEFLAG
// -std=c99 -framework OpenGL -framework GLUT -lassimp -lm -F/Library/Frameworks -framework SDL2



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


static SDL_Window *Window = NULL;
static SDL_GLContext Context;
#define WinWidth 1920
#define WinHeight 1080
typedef int32_t i32;
typedef uint32_t u32;
typedef int32_t b32;
u32 WindowFlags = SDL_WINDOW_OPENGL;

#include <assert.h>
#include <math.h>

/* assimp include files. These three are usually needed. */
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../../games/3_flappy_bird/flappy_bird.h"


const C_STRUCT aiScene* scene = NULL; // MODEL/SCENE
GLuint scene_list = 0; // NB SCENE


// STATIC VAR FOR CAMERA
struct Camera_s
{
	float px,py,pz,cible_py,angle,ouverture;
};


#define VITESSE_DEPLACEMENT 0.18F
#define SENSIBILITE_CAMERA 0.08F
#define HAUTEUR_CAMERA_DEBOUT 3.5F
#define MAX_Y_AXE_CIBLE 2.8F


static GLuint * _vaos = NULL, * _buffers = NULL, * _counts = NULL, * _textures = NULL, _nbMeshes = 0, _nbTextures = 0;

// ASSIMP RENDU AND LOAD //
char * pathOf(const char * path) {
	int spos = -1;
	char * tmp, * ptr;
	tmp = malloc((strlen(path) + 1) * sizeof * tmp); assert(tmp); strcpy(tmp, path); //strdup(path);
	ptr = tmp;
	while(*ptr) {
		if(*ptr == '/' || *ptr == '\\')
			spos = ptr - tmp;
		++ptr;
	}
	tmp[spos >= 0 ? spos : 0] = 0;
	return tmp;
}


int chargementModel (const char* path)
{
	/* we are taking one of the postprocessing presets to avoid
	   spelling out 20+ single postprocessing flags here. */
	scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality |
						 aiProcess_CalcTangentSpace       |
			       		 aiProcess_Triangulate            |
			       	     aiProcess_JoinIdenticalVertices  |
			             aiProcess_SortByPType);

	if (scene) {
		return 0;
	}
	return 1;
}

static int sceneNbMeshes(const struct aiScene *sc, const struct aiNode* nd, int subtotal) {
	int n = 0;
	subtotal += nd->mNumMeshes;
	for(n = 0; n < nd->mNumChildren; ++n)
		subtotal += sceneNbMeshes(sc, nd->mChildren[n], 0);
	return subtotal;
}


/* ---------------------------------------------------------------------------- */
void color4_to_float4(const C_STRUCT aiColor4D *c, float f[4])
{
	f[0] = c->r;
	f[1] = c->g;
	f[2] = c->b;
	f[3] = c->a;
}

/* ---------------------------------------------------------------------------- */
void set_float4(float f[4], float a, float b, float c, float d)
{
	f[0] = a;
	f[1] = b;
	f[2] = c;
	f[3] = d;
}

/* ---------------------------------------------------------------------------- */
void TextureOpenGL(const C_STRUCT aiMaterial *mtl)
{
	float c[4];

	GLenum fill_mode;
	int ret1, ret2;
	C_STRUCT aiColor4D diffuse;
	C_STRUCT aiColor4D specular;
	C_STRUCT aiColor4D ambient;
	C_STRUCT aiColor4D emission;
	ai_real shininess, strength;
	int two_sided;
	int wireframe;
	unsigned int max;

	set_float4(c, 0.8f, 0.8f, 0.8f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
		color4_to_float4(&diffuse, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);

	set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
		color4_to_float4(&specular, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);

	set_float4(c, 0.2f, 0.2f, 0.2f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
		color4_to_float4(&ambient, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c);

	set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
		color4_to_float4(&emission, c);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c);


	max = 1;
	ret1 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max);
	if(ret1 == AI_SUCCESS) {
    	max = 1;
    	ret2 = aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, &max);
		if(ret2 == AI_SUCCESS)
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess * strength);
        else
        	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    }
	else {
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 0.0f);
		set_float4(c, 0.0f, 0.0f, 0.0f, 0.0f);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);
	}

	max = 1;
	if(AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_ENABLE_WIREFRAME, &wireframe, &max))
		fill_mode = wireframe ? GL_LINE : GL_FILL;
	else
		fill_mode = GL_FILL;
	glPolygonMode(GL_FRONT_AND_BACK, fill_mode);

	max = 1;
	if((AI_SUCCESS == aiGetMaterialIntegerArray(mtl, AI_MATKEY_TWOSIDED, &two_sided, &max)) && two_sided)
		glDisable(GL_CULL_FACE);
	else
		glEnable(GL_CULL_FACE);

}

void RenduOpenGL (const C_STRUCT aiScene *sc, const C_STRUCT aiNode* nd)
{
	unsigned int i;
	unsigned int n = 0, t;
	C_STRUCT aiMatrix4x4 m = nd->mTransformation;

	/* update transform */
	aiTransposeMatrix4(&m);
	glPushMatrix();
	glMultMatrixf((float*)&m);

	/* draw all meshes assigned to this node */
	for (; n < nd->mNumMeshes; ++n) {
		const C_STRUCT aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];

		TextureOpenGL(sc->mMaterials[mesh->mMaterialIndex]); // COLOR TEXTURE

			for (t = 0; t < mesh->mNumFaces; ++t) {
				const C_STRUCT aiFace* face = &mesh->mFaces[t];
				GLenum face_mode;

				switch(face->mNumIndices) {
					case 1: face_mode = GL_POINTS; break;
					case 2: face_mode = GL_LINES; break;
					case 3: face_mode = GL_TRIANGLES; break;

					default: face_mode = GL_POLYGON; break;
			}

			glBegin(face_mode);

			for(i = 0; i < face->mNumIndices; i++) {
				int index = face->mIndices[i];
				if(mesh->mColors[0] != NULL)
					glColor4fv((GLfloat*)&mesh->mColors[0][index]);
				if(mesh->mNormals != NULL)
					glNormal3fv(&mesh->mNormals[index].x);
				glVertex3fv(&mesh->mVertices[index].x);
			}

			glEnd();
		}

	}


	for (n = 0; n < nd->mNumChildren; ++n) {
		RenduOpenGL(sc, nd->mChildren[n]);
	}

	glPopMatrix();
}



void InitGL(int width, int height, struct Camera_s camera){
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

void lancerMachine(int *Running, struct Camera_s camera)
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
				case SDLK_SPACE:
					{
						// verifier si on est proche d'une machine //
								// si oui renvoi le code de la machine

						// centrer sur la machine //


						// zoomer sur la machine //


						// lancer la machine
						SDL_Renderer *pRenderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
						printf( "\nEXIT CODE = %d\n" , flappy_bird( pRenderer, 50,WinWidth,WinHeight));
						SDL_DestroyRenderer(pRenderer);
						SDL_DestroyWindow(Window);

						// retour sur la Window 3D.

						Window = SDL_CreateWindow("OpenGL Test", 0, 0, WinWidth, WinHeight, WindowFlags );
						Context = SDL_GL_CreateContext(Window);

						InitGL(WinWidth,WinHeight,camera);
						scene_list = 0;

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

int detectionEnvironnement(float x,float y)
{
	printf("X = %f Z = %f\n",x,y);
	
	if( x < 5.0 && x > -5.0 && y > 20.0)
		return 0;
	if( y > 8.5 && y < 13.5 && (  (x < 8.0 && x > 1.5) || (x > -8.0 && x < -1.5)   )   )
		return 0;
	if( y < -5.0 )
		return 0;
	if( y > 24.0 )
		return 0;
	if( x < -14.5 )
		return 0;
	if ( x > 14.5 )
		return 0;
	return 1;
}

void mouvementCamera(struct Camera_s *camera)
{
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);

	if( keystate[SDL_SCANCODE_LEFT] )
				camera->angle += SENSIBILITE_CAMERA;
	if( keystate[SDL_SCANCODE_RIGHT] )
				camera->angle -= SENSIBILITE_CAMERA;
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



int main( int argc, char *argv[ ], char *envp[ ] )
{
	// PRESET VALUE CAMERA //
	static struct Camera_s camera;
	camera.px = 0.0F;
	camera.py = HAUTEUR_CAMERA_DEBOUT;
	camera.pz = 0.0F;
	camera.cible_py = 0.0F;

	camera.angle = 0.0F;
	camera.ouverture = 70.0F;



	SDL_Init(SDL_INIT_EVERYTHING);
	Window = SDL_CreateWindow("OpenGL Test", 0, 0, WinWidth, WinHeight, WindowFlags);
	
	Context = SDL_GL_CreateContext(Window);


	chargementModel("salle.obj");
	InitGL(WinWidth,WinHeight,camera);


	b32 Running = 1;

	while (Running)
	{

		lancerMachine(&Running,camera);

		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		mouvementCamera(&camera);

		float LightPos[4]={0,10,0,1};
		glLightfv(GL_LIGHT0,GL_POSITION,LightPos);
		glMateriali(GL_FRONT_AND_BACK,GL_SHININESS,40);	//définit la taille de la tache spéculaire

		if(scene_list == 0) {
			scene_list = glGenLists(1);
			glNewList(scene_list, GL_COMPILE);
			RenduOpenGL(scene, scene->mRootNode);
			glEndList();
		}

		glCallList(scene_list);

		SDL_GL_SwapWindow(Window);
	}

	aiReleaseImport(scene);
	SDL_GL_DeleteContext(Context);
	SDL_DestroyWindow(Window);
	SDL_Quit();
	return 0;
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
