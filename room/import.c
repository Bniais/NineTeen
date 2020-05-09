#include "import.h"
#include <stdio.h>
#include <stdlib.h>
FILE *EXT_FILE;
#include <SDL2/SDL_image.h>
#include <assert.h>
/**
*
* \file import.c
*
* \brief permet de convertir un fichier OBJ en vertex baser sur la lib assimp
* \author Mahi.s
* \version 1.0
* \date 27 mars 2020
*/

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

/////////////////////////////////////////////////////
/// \fn void chargerCouleur(const C_STRUCT aiMaterial *mtl);
/// \brief chargement des couleurs des objets
///
/// \param const C_STRUCT aiMaterial *mtl
///
/////////////////////////////////////////////////////
void chargerCouleur(const C_STRUCT aiMaterial *mtl);

/////////////////////////////////////////////////////
/// \fn void chargerTexture(const char* filename, const C_STRUCT aiScene *_scene, GLuint textures[], GLuint ** counts );
/// \brief chargement des textures de la room
///
/// \param const char* filename
/// \param const C_STRUCT aiScene *_scene
/// \param GLuint textures[]
/// \param GLuint ** counts
///
/////////////////////////////////////////////////////
void chargerTexture(const char* filename, const C_STRUCT aiScene *_scene, GLuint textures[], GLuint ** counts );

/////////////////////////////////////////////////////
/// \fn int nombreMeshesScenes(const struct aiScene *scene, const struct aiNode* node, int sousTotal)
/// \brief retourne le nombre de mesh dans une scene
///
/// \param const struct aiScene *scene
/// \param const struct aiNode* node
/// \param int sousTotal
///
/// \return int nombre de mesh
/////////////////////////////////////////////////////
int nombreMeshesScenes(const struct aiScene *scene, const struct aiNode* node, int sousTotal);


void sceneMkVAOs(const struct aiScene *sc, const struct aiNode* nd, GLuint * ivao , GLuint counts[]);

/////////////////////////////////////////////////////
/// \fn void color4_to_float4(const C_STRUCT aiColor4D *c, float f[4])
/// \brief convertie un type aiColor4D en float [4]
///
/// \param const C_STRUCT aiColor4D *c
/// \param float f[4]
///
/// \return void
/////////////////////////////////////////////////////
void color4_to_float4(const C_STRUCT aiColor4D *c, float f[4]);

/////////////////////////////////////////////////////
/// \fn void set_float4(float f[4], float a, float b, float c, float d)
/// \brief convertie 4 float en tableau de 4 float
///
/// \param float f[4]
/// \param float a
/// \param float b
/// \param float c
/// \param float d
///
/// \return void
/////////////////////////////////////////////////////
void set_float4(float f[4], float a, float b, float c, float d);



char * pathOf(const char * path);

















/////////////////////////////////////////////////////
/// \fn int aiImportModel (const char* path,const C_STRUCT aiScene **scene)
/// \brief charge un fichier 3D
///
/// \param const char* path
/// \param const C_STRUCT aiScene **scene
///
/// \return EXIT_SUCCESS/EXIT_FAILURE
/////////////////////////////////////////////////////
int aiImportModel (const char* path,const C_STRUCT aiScene **scene)
{
	///////////////////////////////////////////////////////////////////////
  // CHARGEMENT DE LA SCENE
  // AVEC DIFFERENT PARAMETRE
	*scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality |
						 aiProcess_CalcTangentSpace       |
			       		 aiProcess_Triangulate            |
			       	     aiProcess_JoinIdenticalVertices  |
			             aiProcess_SortByPType);

  ///////////////////////////////////////////////////////////////////////
  // VERIFIER SI CA A BIEN FONCTIONNER
	if (*scene) {
		return EXIT_SUCCESS;
	}
	else
		fprintf(EXT_FILE,"import.c : aiImportModel() : aiImportFile : impossible de charger %s\n",path );

	return EXIT_FAILURE;
}








/////////////////////////////////////////////////////
/// \fn void chargerCouleur(const C_STRUCT aiMaterial *mtl);
/// \brief chargement des couleurs des objets
///
/// \param const C_STRUCT aiMaterial *mtl
///
/////////////////////////////////////////////////////

void chargerCouleur(const C_STRUCT aiMaterial *mtl)
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



/////////////////////////////////////////////////////
/// \fn void chargerTexture(const char* filename, const C_STRUCT aiScene *_scene, GLuint textures[], GLuint ** counts );
/// \brief chargement des textures de la room
///
/// \param const char* filename
/// \param const C_STRUCT aiScene *_scene
/// \param GLuint textures[]
/// \param GLuint ** counts
///
/////////////////////////////////////////////////////
void chargerTexture(const char* filename, const C_STRUCT aiScene *_scene, GLuint textures[], GLuint ** counts )
{

  fprintf(EXT_FILE,"import.c : chargerTexture() : Nombre de textures %d\n", _scene->mNumMaterials );
  glGenTextures(_scene->mNumMaterials, textures);

  for (int i = 0; i < _scene->mNumMaterials ; i++) {

    const struct aiMaterial* pMaterial = _scene->mMaterials[i];

    if (aiGetMaterialTextureCount(pMaterial, aiTextureType_DIFFUSE) > 0) {

      struct aiString tfname;
      if (aiGetMaterialTexture(pMaterial, aiTextureType_DIFFUSE, 0, &tfname, NULL, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {

        SDL_Surface * t;
        char * dir = pathOf(filename), buf[BUFSIZ];
        snprintf(buf, sizeof buf, "%s/%s", dir, tfname.data);

        if(!(t = IMG_Load(buf))) {
          fprintf(EXT_FILE, "import.c : chargerTexture() : IMG_Load : Probleme de chargement de textures %s\n", buf);
          fprintf(EXT_FILE, "import.c : chargerTexture() : Nouvel essai avec %s\n", tfname.data);
          if(!(t = IMG_Load(tfname.data)))
					{
						fprintf(EXT_FILE, "import.c : chargerTexture() : IMG_Load : Probleme de chargement de textures %s\n", tfname.data); continue;
          }
          glBindTexture(GL_TEXTURE_2D, textures[i]);
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

					glBindTexture(GL_TEXTURE_2D, textures[i]);
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

	int nbMeshes = nombreMeshesScenes(_scene, _scene->mRootNode, 0);
	*counts = calloc( nbMeshes, sizeof (*counts));
	assert(*counts);

  GLuint ivao = 0;
	sceneMkVAOs(_scene, _scene->mRootNode, &ivao,*counts);

}



/////////////////////////////////////////////////////
/// \fn int nombreMeshesScenes(const struct aiScene *scene, const struct aiNode* node, int sousTotal)
/// \brief retourne le nombre de mesh dans une scene
///
/// \param const struct aiScene *scene
/// \param const struct aiNode* node
/// \param int sousTotal
///
/// \return int nombre de mesh
/////////////////////////////////////////////////////
int nombreMeshesScenes(const struct aiScene *scene, const struct aiNode* node, int sousTotal) {
  int n = 0;
  sousTotal += node->mNumMeshes;
  for(n = 0; n < node->mNumChildren; ++n)
    sousTotal += nombreMeshesScenes(scene, node->mChildren[n], 0);
  return sousTotal;
}





void sceneMkVAOs(const struct aiScene *sc, const struct aiNode* nd, GLuint * ivao , GLuint counts[]) {
  int i, j;
  unsigned int n = 0;

  for (; n < nd->mNumMeshes; ++n) {
    GLuint  * indices  = NULL;
    const struct aiMesh* mesh = sc->mMeshes[nd->mMeshes[n]];

    if(mesh->mFaces) {
      indices = malloc(3 * mesh->mNumFaces * sizeof *indices);
      for(i = 0, j = 0; j < mesh->mNumFaces; ++j) {
				if(mesh->mFaces[j].mNumIndices != 3) continue;
				indices[i++] = mesh->mFaces[j].mIndices[0];
				indices[i++] = mesh->mFaces[j].mIndices[1];
				indices[i++] = mesh->mFaces[j].mIndices[2];
      }

      counts[*ivao] = i;
      free(indices);
    }
    (*ivao)++;
  }
  for (n = 0; n < nd->mNumChildren; ++n) {
    sceneMkVAOs(sc, nd->mChildren[n], ivao,counts);
  }
}



void afficherScene(const struct aiScene *sc, const struct aiNode* nd, GLuint * ivao, GLuint textures[], GLuint counts[])
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

			glBindTexture(GL_TEXTURE_2D, textures[mesh->mMaterialIndex]);
		if(counts[*ivao]) {

			chargerCouleur(sc->mMaterials[mesh->mMaterialIndex]);

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
						glBindTexture(GL_TEXTURE_2D, textures[mesh->mMaterialIndex]);
					}

					glBegin(face_mode);
					for(i = 0; i < face->mNumIndices; i++) {

						int index = face->mIndices[i];

						if (aiGetMaterialTextureCount(sc->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE) > 0) {
							glTexCoord2f(mesh->mTextureCoords[0][index].x, 1- mesh->mTextureCoords[0][index].y);
						}
						if(mesh->mNormals != NULL)
						{
							glNormal3fv(&mesh->mNormals[index].x);
						}

						glVertex3fv(&mesh->mVertices[index].x);

					}
					glEnd();


			}
		}
		(*ivao)++;
	}


	for (n = 0; n < nd->mNumChildren; ++n) {
		afficherScene(sc, nd->mChildren[n],ivao,textures,counts);
	}

	glPopMatrix();

}

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










/////////////////////////////////////////////////////
/// \fn void color4_to_float4(const C_STRUCT aiColor4D *c, float f[4])
/// \brief convertie un type aiColor4D en float [4]
///
/// \param const C_STRUCT aiColor4D *c
/// \param float f[4]
///
/// \return void
/////////////////////////////////////////////////////
void color4_to_float4(const C_STRUCT aiColor4D *c, float f[4])
{
	f[0] = c->r;
	f[1] = c->g;
	f[2] = c->b;
	f[3] = c->a;
}

/////////////////////////////////////////////////////
/// \fn void set_float4(float f[4], float a, float b, float c, float d)
/// \brief convertie 4 float en tableau de 4 float
///
/// \param float f[4]
/// \param float a
/// \param float b
/// \param float c
/// \param float d
///
/// \return void
/////////////////////////////////////////////////////
void set_float4(float f[4], float a, float b, float c, float d)
{
	f[0] = a;
	f[1] = b;
	f[2] = c;
	f[3] = d;
}
