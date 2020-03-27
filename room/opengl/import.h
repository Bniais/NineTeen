#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl.h>

int aiImportModel (const char* path,const C_STRUCT aiScene **scene);

void aiDessinerScene (const C_STRUCT aiScene *sc, const C_STRUCT aiNode* nd);
