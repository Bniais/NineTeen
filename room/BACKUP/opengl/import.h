#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
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


//#include <OpenGL/gl.h>

int aiImportModel (const char* path,const C_STRUCT aiScene **scene);

void aiDessinerScene (const C_STRUCT aiScene *sc, const C_STRUCT aiNode* nd);
