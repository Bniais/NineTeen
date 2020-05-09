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
#endif

#ifdef _WIN32
    #include<glew.h>
#endif



/////////////////////////////////////////////////////
/// \fn int aiImportModel (const char* path,const C_STRUCT aiScene **scene)
/// \brief charge un fichier 3D
///
/// \param const char* path
/// \param const C_STRUCT aiScene **scene
///
/// \return EXIT_SUCCESS/EXIT_FAILURE
/////////////////////////////////////////////////////
int aiImportModel (const char* path,const C_STRUCT aiScene **scene);

void afficherScene(const struct aiScene *sc, const struct aiNode* nd, GLuint * ivao, GLuint textures[], GLuint counts[]);

void chargerTexture(const char* filename, const C_STRUCT aiScene *_scene, GLuint textures[], GLuint ** counts );
