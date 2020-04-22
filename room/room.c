// CODERUNNER COMPILEFLAG
// -std=c99 -framework OpenGL -framework GLUT -lassimp -lm -F/Library/Frameworks -framework SDL2

// GLOBAL LIBRARY
#include <stdio.h>
#ifdef __APPLE__
	#define GL_SILENCE_DEPRECATION
	#include <OpenGL/gl3.h>
	#include <OpenGL/OpenGL.h>
	#include <GLUT/glut.h>
#endif
#ifdef __linux
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
static GLuint * _textures =  NULL, * _vaos = NULL, *_buffers = NULL , *_counts = NULL,_nbTextures = 0, _nbMeshes = 0;



#include "room.h"
#include "../include/libWeb.h"
#include "../games/2_snake/snake.h"
#include "../games/3_flappy_bird/flappy_bird.h"
#include "../games/5_tetris/tetris.h"
#include "../games/7_asteroid/asteroid.h"
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

#define VITESSE_DEPLACEMENT_COURIR 0.14F
#define VITESSE_DEPLACEMENT_DEBOUT 0.09F
#define VITESSE_DEPLACEMENT_ACCROUPI 0.035F
#define SENSIBILITE_CAMERA 0.08F
#define SENSIBILITE_CAMERA_SOURIS 0.003F
#define HAUTEUR_CAMERA_DEBOUT 3.5F
#define HAUTEUR_CAMERA_ACCROUPI 2.7F
#define MAX_Y_AXE_CIBLE 2.8F
const float ANGLE_DETECTION_MACHINE = M_PI/6;

#define START_PX -13.0F //AXE X
#define START_PY 3.5F // hauteur CAMERA
#define START_PZ 7.0F // AXE Y
#define START_CIBLE_Y 0.0F // REGARDE SUR AXE HAUTEUR
#define START_CIBLE_X M_PI/2// REGARDE SUR AXE X
#define START_OUVERTURE 70.0F //OUVERTURE CAMERA


const int SON_PAS = 5;
float HAUTEUR_PERSONNAGE = HAUTEUR_CAMERA_DEBOUT;



#define MIN_INTENSITE 50 // INTENSITE SON STEREO

float VITESSE_DEPLACEMENT = VITESSE_DEPLACEMENT_DEBOUT;
float HAUTEUR_CAMERA = HAUTEUR_CAMERA_DEBOUT;


#define RATIO_WINDOW_SCREEN 0.93
static int WinWidth = 1280;
static int WinHeight = 720;
SDL_Rect bounds;

//////////////////////////////////////////////////
// FIXER NOMBRE FPS MAX
#define FPS 60
// EN DEDUIRE LE DELAI MIN D'UNE FRAME
static const float FRAME_TIME = 1000/FPS;


SDL_Color Text_rouge = {255,0,0};


// lier au son
#define NB_INDICE_PORTER 2
#define MAX_VOLUME_ARCADE 70

// STATIC VAR FOR CAMERA
struct Camera_s
{
	float px,py,pz,cible_py,angle,ouverture;
};



enum { SCORE,FLAPPY_HARD,TETRIS_HARD,ASTEROID_HARD,PACMAN_HARD,SNAKE_HARD,DEMINEUR_HARD,DEMINEUR_EASY,SNAKE_EASY,PACMAN_EASY,ASTEROID_EASY,TETRIS_EASY,FLAPPY_EASY};


#ifdef _WIN32
  #define DIR_TOKEN_FILE "C:\\Windows\\Temp\\.Nineteen"
#else
  #define DIR_TOKEN_FILE "/tmp/.Nineteen"
#endif


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
void mouvementCamera(SDL_Window * window, struct Camera_s *camera, const float IPS);


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
void SDL_GL_AppliquerScene(SDL_Window * Window, const C_STRUCT aiScene *scene,struct Camera_s *camera,GLuint *scene_list, const float IPS);


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
/// \fn void messageMachine(struct MeilleureScore_s str[] ,struct Camera_s camera,TTF_Font *font,int afficherMessage)
/// \brief affichage un message propre a chaque machine
///
/// \param struct MeilleureScore_s str[] structure stockant les nom et score des meilleure joueur par machine
/// \param struct Camera_s camera Camera du joueur pour detection des machines
/// \param TTF_Font *font pointeur sur la police
/// \param int afficherMessage afficher messages clignotant ou nom
///
/// \return void
/////////////////////////////////////////////////////
void messageMachine(struct MeilleureScore_s str[] ,struct Camera_s camera,TTF_Font *font,int afficherMessage);


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
/// \fn void lleureScore(struct MeilleureScore_s str[] ,char *token)
/// \brief Mes a jours les donnees liee au score de la structure meilleureScore
///
/// \param struct MeilleureScore_s str[] tableau de donner
///
/////////////////////////////////////////////////////
void updateMeilleureScore(struct MeilleureScore_s str[] ,char *token);








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



static int sceneNbMeshes(const struct aiScene *sc, const struct aiNode* nd, int subtotal) {
  int n = 0;
  subtotal += nd->mNumMeshes;
  for(n = 0; n < nd->mNumChildren; ++n)
    subtotal += sceneNbMeshes(sc, nd->mChildren[n], 0);
  return subtotal;
}

static void sceneMkVAOs(const struct aiScene *sc, const struct aiNode* nd, GLuint * ivao ) {
  int i, j, comp;
  unsigned int n = 0;
  static int temp = 0;

  temp++;

  for (; n < nd->mNumMeshes; ++n) {
    GLfloat * vertices = NULL;
    GLuint  * indices  = NULL;
    const struct aiMesh* mesh = sc->mMeshes[nd->mMeshes[n]];
    comp  = mesh->mVertices ? 3 : 0;
    comp += mesh->mNormals ? 3 : 0;
    comp += mesh->mTextureCoords[0] ? 2 : 0;
    if(!comp) continue;

  //  glBindVertexArray(_vaos[*ivao]);
  //  glBindBuffer(GL_ARRAY_BUFFER, _buffers[2 * (*ivao)]);

    vertices = malloc(comp * mesh->mNumVertices * sizeof *vertices);
    assert(vertices);
    i = 0;
  //  glDisableVertexAttribArray(0);
  //  glDisableVertexAttribArray(1);
  //  glDisableVertexAttribArray(2);

    if(mesh->mVertices) {
    //  glEnableVertexAttribArray(0);
    //  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)(i * sizeof *vertices));
      for(j = 0; j < mesh->mNumVertices; ++j) {
				vertices[i++] = mesh->mVertices[j].x;
				vertices[i++] = mesh->mVertices[j].y;
				vertices[i++] = mesh->mVertices[j].z;
      }

    }
    if(mesh->mNormals) {

    //  glEnableVertexAttribArray(1);
    //  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const void *)(i * sizeof *vertices));
      for(j = 0; j < mesh->mNumVertices; ++j) {
				vertices[i++] = mesh->mNormals[j].x;
				vertices[i++] = mesh->mNormals[j].y;
				vertices[i++] = mesh->mNormals[j].z;
      }

    }

    if(mesh->mTextureCoords[0]) {

    //  glEnableVertexAttribArray(2);
    //  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const void *)(i * sizeof *vertices));
      for(j = 0; j < mesh->mNumVertices; ++j) {
				vertices[i++] = mesh->mTextureCoords[0][j].x;
				vertices[i++] = mesh->mTextureCoords[0][j].y;
      }

    }


  //  glBufferData(GL_ARRAY_BUFFER, (i * sizeof *vertices), vertices, GL_STATIC_DRAW);

    free(vertices);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffers[2 * (*ivao) + 1]);
    if(mesh->mFaces) {
      indices = malloc(3 * mesh->mNumFaces * sizeof *indices);
      assert(indices);
      for(i = 0, j = 0; j < mesh->mNumFaces; ++j) {
				assert(mesh->mFaces[j].mNumIndices < 4);
				if(mesh->mFaces[j].mNumIndices != 3) continue;
				indices[i++] = mesh->mFaces[j].mIndices[0];
				indices[i++] = mesh->mFaces[j].mIndices[1];
				indices[i++] = mesh->mFaces[j].mIndices[2];
      }

    //  glBufferData(GL_ELEMENT_ARRAY_BUFFER, i * sizeof *indices, indices, GL_STATIC_DRAW);
      _counts[*ivao] = i;
      free(indices);
    }
  //  glBindVertexArray(0);
    (*ivao)++;
  }
  for (n = 0; n < nd->mNumChildren; ++n) {
    sceneMkVAOs(sc, nd->mChildren[n], ivao);
  }
}



void aiLoadTexture(const char* filename, const C_STRUCT aiScene *_scene)
{
  int i = 0;
	GLuint ivao = 0;


  _textures = malloc( (_nbTextures = _scene->mNumMaterials) * sizeof *_textures);
  assert(_textures);

  glGenTextures(_nbTextures, _textures);

  for (i = 0; i < _scene->mNumMaterials ; i++) {

    const struct aiMaterial* pMaterial = _scene->mMaterials[i];
       if (aiGetMaterialTextureCount(pMaterial, aiTextureType_DIFFUSE) > 0) {
         struct aiString tfname;
         char * dir = pathOf(filename), buf[BUFSIZ];
         if (aiGetMaterialTexture(pMaterial, aiTextureType_DIFFUSE, 0, &tfname, NULL, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
           SDL_Surface * t;
           snprintf(buf, sizeof buf, "%s/%s", dir, tfname.data);
					 printf("%s\n",tfname.data );
           if(!(t = IMG_Load(buf))) {
            fprintf(stderr, "Probleme de chargement de textures %s\n", buf);
            fprintf(stderr, "\tNouvel essai avec %s\n", tfname.data);
            if(!(t = IMG_Load(tfname.data)))
						{
							fprintf(stderr, "Probleme de chargement de textures %s\n", tfname.data); continue;
            }
						printf("texture dans i = %d\n",i );
            glBindTexture(GL_TEXTURE_2D, _textures[i]);
	          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT/* GL_CLAMP_TO_EDGE */);
	          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT/* GL_CLAMP_TO_EDGE */);
            #ifdef __APPLE__
	           glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t->w, t->h, 0, t->format->BytesPerPixel == 3 ? GL_BGR : GL_BGRA, GL_UNSIGNED_BYTE, t->pixels);
            #else
	           glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t->w, t->h, 0, t->format->BytesPerPixel == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, t->pixels);
            #endif

	          SDL_FreeSurface(t);

          }
					printf("texture dans i = %d\n",i );
					glBindTexture(GL_TEXTURE_2D, _textures[i]);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT/* GL_CLAMP_TO_EDGE */);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT/* GL_CLAMP_TO_EDGE */);
					#ifdef __APPLE__
					 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t->w, t->h, 0, GL_RGB, GL_UNSIGNED_BYTE, t->pixels);
					#else
					 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t->w, t->h, 0, t->format->BytesPerPixel == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, t->pixels);
					#endif
					SDL_FreeSurface(t);

         }

       }

  }

	_nbMeshes = sceneNbMeshes(_scene, _scene->mRootNode, 0);
	_vaos = malloc(_nbMeshes * sizeof *_vaos);
	assert(_vaos);
//	glGenVertexArrays(_nbMeshes, _vaos);
	_buffers =  malloc(2 * _nbMeshes * sizeof *_buffers);
	assert(_buffers);

//	glGenBuffers(2 * _nbMeshes,_buffers);
	_counts = calloc(_nbMeshes, sizeof *_counts);
	assert(_counts);
	sceneMkVAOs(_scene, _scene->mRootNode, &ivao);

}





void aiDessinerImage(const struct aiScene *sc, const struct aiNode* nd, GLuint * ivao)
{
	glEnable(GL_TEXTURE_2D);
	unsigned int n = 0,i,t;
	struct aiMatrix4x4 m = nd->mTransformation;


	/* update transform */
	aiTransposeMatrix4(&m);
	glPushMatrix();
	glMultMatrixf((float*)&m);

	/* draw all meshes assigned to this node */
	for (; n < nd->mNumMeshes; ++n) {
		const struct aiMesh* mesh = sc->mMeshes[nd->mMeshes[n]];
		// BLIND DE LA TEXTURE SI BESOIN
		if (aiGetMaterialTextureCount(sc->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE) > 0) {
			glBindTexture(GL_TEXTURE_2D, _textures[mesh->mMaterialIndex]);
		}
		if(_counts[*ivao]) {

		//	glBindVertexArray(_vaos[*ivao]);
			aiAppliquerCouleur(sc->mMaterials[mesh->mMaterialIndex]);

			for (t = 0; t < mesh->mNumFaces; ++t) {
				const C_STRUCT aiFace* face = &mesh->mFaces[t];
				GLenum face_mode;

					switch(face->mNumIndices) {
						case 1: face_mode = GL_POINTS; break;
						case 2: face_mode = GL_LINES; break;
						case 3: face_mode = GL_TRIANGLES; break;
						default: face_mode = GL_POLYGON; break;
					}


					if (aiGetMaterialTextureCount(sc->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE) > 0) {
						glBindTexture(GL_TEXTURE_2D, _textures[mesh->mMaterialIndex]);
					}

					glBegin(face_mode);
					for(i = 0; i < face->mNumIndices; i++) {

						int index = face->mIndices[i];

						if(mesh->mColors[0] != NULL)
						{
							glColor4fv((GLfloat*)&mesh->mColors[0][index]);
						}
						if(mesh->mNormals != NULL)
						{
							glNormal3fv(&mesh->mNormals[index].x);
						}


						if (aiGetMaterialTextureCount(sc->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE) > 0) {
							glTexCoord2f(mesh->mTextureCoords[0][index].x, 1- mesh->mTextureCoords[0][index].y);
						}

						glVertex3fv(&mesh->mVertices[index].x);

					}
					glEnd();


			}
		}

		(*ivao)++;
	}


	for (n = 0; n < nd->mNumChildren; ++n) {
		aiDessinerImage(sc, nd->mChildren[n],ivao);
	}

	glPopMatrix();

}


void detruireTexture()
{

  if(_counts) {
    free(_counts);
    _counts = NULL;
  }
  if(_textures) {
    glDeleteTextures(_nbTextures, _textures);
    free(_textures);
    _textures = NULL;
  }
  if(_vaos) {
    glDeleteVertexArrays(_nbMeshes, _vaos);
    free(_vaos);
    _vaos = NULL;
  }
  if(_buffers) {
    glDeleteBuffers(2 * _nbMeshes, _buffers);
    free(_buffers);
    _buffers = NULL;
  }
}
void GLlightMode()
{
	glEnable(GL_LIGHTING);	// Active l'éclairage
	glEnable(GL_LIGHT0);	// Allume la lumière n°1
	glEnable(GL_LIGHT1);	// Allume la lumière n°1
	//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	// COULEUR DES LAMPES
	/////////////////////
	//
	GLfloat ambiant1[] = { 1.0, 0.917 , 0.173 , 1.0 };
	GLfloat diffuse[] = { 1.0, 1.0 , 1.0 , 1.0 };
	GLfloat specular[] = { 1.0, 1.0 , 1.0 , 1.0 };


	GLfloat position1[]={-9.3 , 5.4 , 0.0 , 1.0 };
	GLfloat spot_direction1[]={ 0.0 , -1.0 , 0.0 };

	GLfloat position2[]={3.4 , 4.2 , -4.8 , 1.0 };
	GLfloat spot_direction2[]={ 0.0 , 0.0 , -1.0 };


	glLightfv(GL_LIGHT0  , GL_AMBIENT, ambiant1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, position1);


	GLfloat ambiant2[] = { 0.89, 0.33 , 0.27 , 1.0 };
	glLightfv(GL_LIGHT1  , GL_AMBIENT, ambiant1);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT1, GL_POSITION, position2);


	// ATENUATION


	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.0);
	glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.01);
	glLightf(GL_LIGHT1 , GL_QUADRATIC_ATTENUATION, 0.1);

	glLightf(GL_LIGHT0 , GL_SPOT_CUTOFF, 90.0);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction1);
	glLightf(GL_LIGHT0 , GL_SPOT_EXPONENT, 2.0);

	glLightf(GL_LIGHT0 , GL_SPOT_CUTOFF, 90.0);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction2);
	glLightf(GL_LIGHT0 , GL_SPOT_EXPONENT, 2.0);
}




int room(char *token,struct MeilleureScore_s meilleureScore[],SDL_Window *Window, const C_STRUCT aiScene* scene)
{
	//////////////////////////////////////////////////////////
	// EVENT SOURIS INIT
	//////////////////////////////////////////////////////////
	// CACHER LA SOURIS
	SDL_ShowCursor(SDL_DISABLE);
	//////////////////////////////////////////////////////////
	// POSITIONNER LA SOURIS AU CENTRE
	SDL_GetDisplayBounds(0, &bounds);
	///////////////////////////////////////////////////
	//////////////////////////////////////////////////////////


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

	SDL_WarpMouseInWindow(Window, (WinWidth/2)  ,(WinHeight/2) );

	glLoadIdentity();
	GL_InitialiserParametre(WinWidth,WinHeight,camera);

	aiLoadTexture(DIR_OBJ_LOAD,scene);



	while (Running)
	{
		glLoadIdentity();
		GL_InitialiserParametre(WinWidth,WinHeight,camera);


		int delayLancementFrame = SDL_GetTicks();

		//////////////////////////////////////////////////////////
		// REGLAGE SON ENVIRONEMENT AVEC LEUR POSITION
		// MUSIQUE LOT MACHINE GAUCHE
		reglageVolume(0,-5.0,11.0,camera.px,camera.pz,10.0,camera.angle);
		// MUSIQUE LOT MACHINE DROITE
		reglageVolume(1,5.0,11.0,camera.px,camera.pz,10.0,camera.angle);
		// MUSIQUE MACHINE SEUL
		reglageVolume(2,0.0,0.0,camera.px,camera.pz,10.0,camera.angle);
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
		// CHARGER LA SCENE
		SDL_GL_AppliquerScene(Window, scene,&camera,&scene_list,_IPS);

		//GL_InitialiserParametre(WinWidth,WinHeight,camera);
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
	// DESTRUCTION DES EMPLACEMENTS TEXTURES
	detruireTexture();
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

	strcpy(str[TETRIS_EASY].nomJeux,"TETRIS");

	strcpy(str[FLAPPY_EASY].nomJeux,"FLAPPY   BIRD");


	strcpy(str[13].nomJeux,"COMMING   SOON");
	strcpy(str[14].nomJeux,"COMMING   SOON");
	strcpy(str[15].nomJeux,"COMMING   SOON");
}



void updateMeilleureScore(struct MeilleureScore_s str[] ,char *token)
{
	///////////////////////////////////////////////////////////
	// INITALISATION D'UNE CHAINE POUR STOCKER LA REPONSE
	char reponse[2048];

	///////////////////////////////////////////////////////////
	// RECUPERATION DU SCORE JUSQU'A REUSSITE
	while( updateMeilleureScoreStruct(token,reponse) == EXIT_FAILURE );

	///////////////////////////////////////////////////////////
	// PARSING DANS LA CHAINE DE DONNER RECU
	int temp1,temp2;
	sscanf(reponse,"%d %d / %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", &str[0].score,&temp1,&temp2,
																																str[FLAPPY_HARD].nomJoueur,&str[FLAPPY_HARD].score,
																																str[TETRIS_HARD].nomJoueur,&str[TETRIS_HARD].score,
																																str[ASTEROID_HARD].nomJoueur,&str[ASTEROID_HARD].score,
																																str[PACMAN_HARD].nomJoueur,&str[PACMAN_HARD].score,
																																str[SNAKE_HARD].nomJoueur,&str[SNAKE_HARD].score,
																																str[DEMINEUR_HARD].nomJoueur,&str[DEMINEUR_HARD].score,
																																str[DEMINEUR_EASY].nomJoueur,&str[DEMINEUR_EASY].score,
																																str[SNAKE_EASY].nomJoueur,&str[SNAKE_EASY].score,
																																str[PACMAN_EASY].nomJoueur,&str[PACMAN_EASY].score,
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
																																&str[15].scoreJoueurActuel

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
	mouvementCamera(Window, camera,60.0);

	////////////////////////////////////////////////////
	// SI LE RENDU DE LA SCENE N'EST PAS FAIT LE FAIRE

	if(*scene_list == 0) {
		// FIXER LA SCENE A 1
		*scene_list = glGenLists(1);
		glNewList(*scene_list, GL_COMPILE);
		GLuint ivao = 0;
		aiDessinerImage(scene, scene->mRootNode,&ivao);
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

	cible[TETRIS_EASY-1].px = 4.65;
	cible[TETRIS_EASY-1].pz = 9.25;
	cible[TETRIS_EASY-1].py = 3.45;
	cible[TETRIS_EASY-1].cible_py = -.34;
	cible[TETRIS_EASY-1].angle = 0;
	cible[TETRIS_EASY-1].ouverture =70;

	cible[FLAPPY_EASY-1].px = 6.59;
 	cible[FLAPPY_EASY-1].pz = 9.25;
 	cible[FLAPPY_EASY-1].py = 3.45;
 	cible[FLAPPY_EASY-1].cible_py = -.34;
	cible[FLAPPY_EASY-1].angle = 0;
	cible[FLAPPY_EASY-1].ouverture =70;

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
	cible[14].py = 4.05;
	cible[14].cible_py = -.34;
	cible[14].angle = M_PI;
	cible[14].ouverture =70;
}

/* ---------------------------------------------------------------------------- */


void mouvementCamera(SDL_Window * Window, struct Camera_s *camera, const float IPS)
{
	///////////////////////////////////////////////////
	// REGLAGE EN FONCTION DES FPS FIXER
	// VALEUR BASER SUR 60 FPS DE BASE

	// DETECTER COURSE A PIED



	float _SENSIBILITE_CAMERA = SENSIBILITE_CAMERA * (IPS/FPS);
	float _VITESSE_DEPLACEMENT = VITESSE_DEPLACEMENT * (IPS/FPS);


	///////////////////////////////////////////////////
	// INITALISATION D'UN EVENEMENT SDL
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);


	//DETECTER LA COURSE A PIED
	if( keystate[SDL_SCANCODE_LSHIFT] )
	{
		_VITESSE_DEPLACEMENT = VITESSE_DEPLACEMENT_COURIR * (IPS/FPS);
	}

	///////////////////////////////////////////////////
	// MOUVEMENT CAMERA SUR X ET Z


	////////////////////////////
	// RECUPERER POSITION SOURIS
	SDL_PumpEvents();
	int mouseX,mouseY;
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
	if (camera->px <= 4 && camera->px >= -4 && camera->pz <= 4 && camera->pz >= -4)
		camera->py = 0.6F + HAUTEUR_CAMERA;
	else
		camera->py = HAUTEUR_CAMERA;



/*
	// RECENTRE LA SOURIS
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
	if( (y >= 7.0 || y <= 2.5) && x >= 14.0 && x <= 16.0)
		return 0;

	///////////////////////////////////////////////////
	// MUR QUI SEPARE LA SALLE ET LES TOILETTES
	if( x >= 18.0 && x <= 19.0 && y >= -0.5 && y <= 10.5 )
		return 0;

	///////////////////////////////////////////////////
	// MACHINE VOITURE
	if ( x >= 4.0 && x <= 10.0 && y <= 3.6 && y >= - 3.6)
		return 0;

	///////////////////////////////////////////////////
	// BILLARD
	if( x <= -5.0 && x >= -13.5 && y <= 2.5 && y >= -2.5)
		return 0;

	///////////////////////////////////////////////////
	// DIMESION SALLE DES TOILETTE
	if(x > 15.0 && (  y < -2.0 || y > 12.0  )  )
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
	if( y < -4.5 )
		return 0;
	// BAS DE LA SALLE
	if( y > 23.5 )
		return 0;
	// GAUCHE DE LA SALLE
	if( x < -14.0 )
		return 0;
	// DROIT DE LA SALLE
	if ( x > 23.5 )
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
			if(angle > M_PI - (ANGLE_DETECTION_MACHINE) && angle < M_PI + (ANGLE_DETECTION_MACHINE) )
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
			if( ( angle > ( 2*M_PI - (ANGLE_DETECTION_MACHINE) ) && angle <= 2*M_PI  ) ||    (    angle >= 0 && angle < 0 + (ANGLE_DETECTION_MACHINE)   )   )
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
			if(angle > M_PI - (ANGLE_DETECTION_MACHINE) && angle < M_PI + (ANGLE_DETECTION_MACHINE) )
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
		if(angle > M_PI - (ANGLE_DETECTION_MACHINE) && angle < M_PI + (ANGLE_DETECTION_MACHINE) )
			return 15;



	return 0;
}

void messageMachine(struct MeilleureScore_s str[], struct Camera_s camera,TTF_Font *font,int afficherMessage)
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
		// CONCATENATION MEILLEURE SCORE DU JOUEUR
		sprintf(message, "RECORD  PERSO  :   %d",str[detection].scoreJoueurActuel);
		// AFFICHER RECORD PERSONNEL
		AfficherText(font,message,white,-1,WinHeight*0.9);

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
		SDL_WarpMouseInWindow(Window, (WinWidth/2)  ,(WinHeight/2) );
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
		if (Event.type == SDL_KEYDOWN || Event.type == SDL_MOUSEBUTTONDOWN)
		{
				///////////////////////////////////////////////////
				// TOUCHE ESPACE METTRE FIN AU JEUX
				if(Event.key.keysym.sym == SDLK_ESCAPE)
				{
					//////////////////////////////////////////////////////////
					// AFFICHER LA SOURIS
					SDL_ShowCursor(SDL_ENABLE);
						///////////////////////////////////////////////////
						// INIT AFFICHAGE DU MESSAGE
						GL_InitialiserParametre(WinWidth,WinHeight,camera);
						SDL_GL_AppliquerScene(Window, scene,&camera,scene_list,FPS);
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
								if (Event.type == SDL_KEYDOWN) {
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
											FILE *fp = fopen(DIR_TOKEN_FILE,"w");
											fclose(fp);
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
							SDL_ShowCursor(SDL_DISABLE);
							///////////////////////////////////////////////////
							// RECENTRAGE DE CAMERA
							SDL_WarpMouseInWindow(Window, (WinWidth/2)  ,(WinHeight/2) );
														///////////////////////////////////////////////////
						}
				}

				///////////////////////////////////////////////////
				// TOUCHE E ENTRER DANS UN JEUX OU CLIQUE GAUCHE
				if(Event.key.keysym.sym == SDLK_e || Event.type == SDL_MOUSEBUTTONDOWN)
				{
						///////////////////////////////////////////////////
						// VERIFIER SI ON EST PROCHES D UNE MACHINE
						// RENVOIE LE CODE DE LA MACHINE
						int machine = detecterMachine(camera.px, camera.pz, camera.angle);
						if ( machine)
						{

							//SDL_WarpMouseGlobal( (WinWidth/2) + (bounds.w-WinWidth) /2  ,(WinHeight/2) + (bounds.h-WinHeight) /2);

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
									printf( "\nEXIT CODE = %d\n" , flappy_bird( pRenderer, meilleureScore[FLAPPY_HARD].scoreJoueurActuel,WinWidth,WinHeight,token,1));
									updateMeilleureScore(meilleureScore,token);
								break;
								case 2:
									tetris( pRenderer ,meilleureScore[TETRIS_HARD].scoreJoueurActuel, 1920./WinWidth,token,1);
									updateMeilleureScore(meilleureScore,token);
									break;
								case 3:
									asteroid( pRenderer ,meilleureScore[ASTEROID_HARD].scoreJoueurActuel, 1920./WinWidth,token,1);
									updateMeilleureScore(meilleureScore,token);
									break;
								case 4: SDL_Delay(500);break;
								case 5:
									snake( pRenderer ,meilleureScore[SNAKE_HARD].scoreJoueurActuel, WinWidth/1920.,token,1);
									updateMeilleureScore(meilleureScore,token);
								case 6: SDL_Delay(500);break;
								case 7: SDL_Delay(500);break;
								case 8:
									snake( pRenderer ,meilleureScore[SNAKE_EASY].scoreJoueurActuel, WinWidth/1920.,token,0);
									updateMeilleureScore(meilleureScore,token);
								case 9: SDL_Delay(500);break;
								case 10:
									asteroid( pRenderer ,meilleureScore[ASTEROID_EASY].scoreJoueurActuel, 1920./WinWidth,token,0);
									updateMeilleureScore(meilleureScore,token);
									break;
								case 11: {
									tetris( pRenderer ,meilleureScore[TETRIS_EASY].scoreJoueurActuel, 1920./WinWidth,token,0);
									updateMeilleureScore(meilleureScore,token);
									break;
								}break;
								case 12:
									printf( "\nEXIT CODE = %d\n" , flappy_bird( pRenderer, meilleureScore[FLAPPY_EASY].scoreJoueurActuel,WinWidth,WinHeight,token,0));
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



							#ifdef __linux__
								detruireTexture();

								SDL_GL_DeleteContext(*Context);
								///////////////////////////////////////////////////
								*Context = SDL_GL_CreateContext(Window);
								///////////////////////////////////////////////////
								// REMISE A ZERO DE LA SCENE
								*scene_list = 0;
								// ATTENTE POUR MAC OS AFIN DE VOIR L'ANIMATION
								while(SDL_PollEvent(&Event));
								// AFFICHAGE DE LA SCENE
								SDL_WarpMouseInWindow(Window, (WinWidth/2)  ,(WinHeight/2) );
								// RECHARGEMENT DES IMAGES
								aiLoadTexture(DIR_OBJ_LOAD,scene);
							#else
								// REMISE A ZERO DE LA SCENE
								*scene_list = 0;
								// ATTENTE POUR MAC OS AFIN DE VOIR L'ANIMATION
								while(SDL_PollEvent(&Event));
								// AFFICHAGE DE LA SCENE
								SDL_WarpMouseInWindow(Window, (WinWidth/2)  ,(WinHeight/2) );
							#endif



							SDL_GL_AppliquerScene(Window, scene,&camera,scene_list,FPS);
							// ANIMATION DE RETOUR SUR MACHINE
							animationLancerMachine(cible[machine-1],camera,*scene_list,Window);
							// VIDER POLL EVENEMENT
							while(SDL_PollEvent(&Event));


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
