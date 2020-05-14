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
/// \fn void afficherScene(const struct aiScene *scene, const struct aiNode* node, GLuint * ivao, GLuint textures[], GLuint counts[])
/// \brief permet d'afficher la scene dans openGL
///
/// \param const struct aiScene *scene
/// \param const struct aiNode* node\
/// \param GLuint * ivao
/// \param GLuint textures[]
/// \param GLuint counts[]
///
/// \return void
/////////////////////////////////////////////////////
void afficherScene(const struct aiScene *sc, const struct aiNode* nd, GLuint * ivao, GLuint textures[], GLuint counts[]);


////////////////////////////
//////// CHARGEMENT ////////
////////////////////////////

/////////////////////////////////////////////////////
/// \fn void chargerCouleur(const C_STRUCT aiMaterial *mtl);
/// \brief chargement des couleurs des objets
///
/// \param const C_STRUCT aiMaterial *mtl
///
/////////////////////////////////////////////////////
void chargerCouleur(const C_STRUCT aiMaterial *mtl);

/////////////////////////////////////////////////////
/// \fn void chargerTexture(const char* filename, const C_STRUCT aiScene *scene, GLuint textures[], GLuint ** counts );
/// \brief chargement des textures de la room
///
/// \param const char* filename
/// \param const C_STRUCT aiScene *_scene
/// \param GLuint textures[]
/// \param GLuint ** counts
///
/////////////////////////////////////////////////////
void chargerTexture(const char* filename, const C_STRUCT aiScene *scene, GLuint textures[], GLuint ** counts );


////////////////////////////
//////// COMPTAGE //////////
////////////////////////////

/////////////////////////////////////////////////////
/// \fn void nombreVertexObjetScene(const struct aiScene *scene, const struct aiNode* node, GLuint * ivao , GLuint counts[]);
/// \brief Compte le nombre de vertex dans un node et le note dans un tableau counts a l'indice de l objet
///
/// \param const struct aiScene *scene
/// \param const struct aiNode* node
/// \param GLuint * ivao
/// \param GLuint counts[]
/// \return void
/////////////////////////////////////////////////////
void nombreVertexObjetScene(const struct aiScene *scene, const struct aiNode* node, GLuint * ivao , GLuint counts[]);


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


////////////////////////////
//////// CONVERTION ////////
////////////////////////////


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



/////////////////////////////////////////////////////
/// \fn char * pathOf(const char * path)
/// \brief trouve l'arborescene repertoire
///
/// \param const char * path
///
/// \return char * pointeur sur la chaine cree
/////////////////////////////////////////////////////
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
/// \fn void afficherScene(const struct aiScene *scene, const struct aiNode* node, GLuint * ivao, GLuint textures[], GLuint counts[])
/// \brief permet d'afficher la scene dans openGL
///
/// \param const struct aiScene *scene
/// \param const struct aiNode* node\
/// \param GLuint * ivao
/// \param GLuint textures[]
/// \param GLuint counts[]
///
/// \return void
/////////////////////////////////////////////////////
void afficherScene(const struct aiScene *scene, const struct aiNode* node, GLuint * ivao, GLuint textures[], GLuint counts[])
{
	glEnable(GL_TEXTURE_2D);
	unsigned int n = 0,i,t;
	struct aiMatrix4x4 m = node->mTransformation;


	// APPLICATION DE TRANSFORMATION SUR LA MATRISE GL
	aiTransposeMatrix4(&m);
	glPushMatrix();
	glMultMatrixf((float*)&m);


	// DESSINER TOUTES LES MESHES ASSIGNER A CE NODE
	for (; n < node->mNumMeshes; ++n) {
		const struct aiMesh* mesh = scene->mMeshes[node->mMeshes[n]];

    // UNIQUEMENT SI IL Y A DES VERTISE
		if(counts[*ivao]) {
      // CHARGEMENT DES COULEURS DES OBJETS NON TEXTURE ET TEXTURER EGALEMENT
			chargerCouleur(scene->mMaterials[mesh->mMaterialIndex]);
      // CHARGEMENT DE LA TEXTURE
      glBindTexture(GL_TEXTURE_2D, textures[mesh->mMaterialIndex]);

			for (t = 0; t < mesh->mNumFaces; ++t) {
				const C_STRUCT aiFace* face = &mesh->mFaces[t];

        // DETECTION DU TYPE D OBJ A DESSINER
        GLenum face_mode;
				switch(face->mNumIndices) {
					case 1: face_mode = GL_POINTS; break;
					case 2: face_mode = GL_LINES; break;
					case 3: face_mode = GL_TRIANGLES; break;
					default: face_mode = GL_POLYGON; break;
				}

        // MODE DESSIN OPENGL
				glBegin(face_mode);
            // PARCOUR DE TOUTE LES MESHES PRESENTE DANS CE NODE
				    for(i = 0; i < face->mNumIndices; i++)
            {
              // INDICE DE LA MESHES A DESSINER
              int index = face->mIndices[i];

              // SI DETECTION DE TEXTURE ON L APPLIQUE EGALEMENT
              if (aiGetMaterialTextureCount(scene->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE) > 0)
							   glTexCoord2f(mesh->mTextureCoords[0][index].x, 1- mesh->mTextureCoords[0][index].y);
              // DESSINER NORMAL MESH
						  if(mesh->mNormals != NULL)
						     glNormal3fv(&mesh->mNormals[index].x);
              // DESSINER VERTEX MESH
              if(mesh->mVertices != NULL)
                 glVertex3fv(&mesh->mVertices[index].x);

					  }
				 glEnd();


			}
		}
		(*ivao)++;
	}

  // APPEL RECURSIF POUR TOUS LES NODES PRESENT
	for (n = 0; n < node->mNumChildren; ++n)
		afficherScene(scene, node->mChildren[n],ivao,textures,counts);

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


  // tableau de float pour contenir des preset
  float c[4];

  // CHARGEMENT DES PARAMETRE DE DIFFUSION
  C_STRUCT aiColor4D diffuse;
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse))
  {
    color4_to_float4(&diffuse, c);
	  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, c);
  }
  // CHARGEMENT DES PARAMETRE DE SPECULARITER
  C_STRUCT aiColor4D specular;
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &specular))
  {
    color4_to_float4(&specular, c);
	  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, c);
  }
  // CHARGEMENT DES PARAMETRE AMBIENTE
  C_STRUCT aiColor4D ambient;
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &ambient))
  {
    color4_to_float4(&ambient, c);
	  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, c);
  }
  // CHARGEMENT DES PARAMETRE D EMISSION
  C_STRUCT aiColor4D emission;
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_EMISSIVE, &emission))
  {
    color4_to_float4(&emission, c);
	  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, c);
  }




	unsigned int max = 1;
  // RECUPERATION DES PARAMETRE SHININESS
  ai_real shininess;
	if( AI_SUCCESS == aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS, &shininess, &max))
  {
    // RECUPERATION DES PARAMETRE strength ET APPLICATION DES DEUX OU DE L'UN OU L AUTRE
    ai_real strength;
		if(AI_SUCCESS == aiGetMaterialFloatArray(mtl, AI_MATKEY_SHININESS_STRENGTH, &strength, &max))
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess * strength);
    else
      glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  }

}



/////////////////////////////////////////////////////
/// \fn void chargerTexture(const char* filename, const C_STRUCT aiScene *scene, GLuint textures[], GLuint ** counts );
/// \brief chargement des textures de la room
///
/// \param const char* filename
/// \param const C_STRUCT aiScene *_scene
/// \param GLuint textures[]
/// \param GLuint ** counts
///
/////////////////////////////////////////////////////
void chargerTexture(const char* filename, const C_STRUCT aiScene *scene, GLuint textures[], GLuint ** counts )
{

  fprintf(EXT_FILE,"import.c : chargerTexture() : Nombre de textures %d\n", scene->mNumMaterials );
  glGenTextures(scene->mNumMaterials, textures);

  // PARCOURIR TOUTES LES TEXTURE PRESENTE DANS LA STRUCT
  for (int i = 0; i < scene->mNumMaterials ; i++) {

    const struct aiMaterial* pMaterial = scene->mMaterials[i];

    // VERIFIER LA PRESENCE D UNE TEXTURE POUR TEL INDICE
    if (aiGetMaterialTextureCount(pMaterial, aiTextureType_DIFFUSE) > 0) {

      // RECUPERATION DU NOM DU FICHIER
      struct aiString tfname;
      if (aiGetMaterialTexture(pMaterial, aiTextureType_DIFFUSE, 0, &tfname, NULL, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {

        char * dir = pathOf(filename);
        char buf[BUFSIZ];
        // LIEN COMPLET VER LE FICHIER
        snprintf(buf, sizeof buf, "%s/%s", dir, tfname.data);


        SDL_Surface * img = IMG_Load(buf);
        if(!img)
          // SI FICHIER INTROUVABLE ESSAYER AVEC UNE AUTRE
          fprintf(EXT_FILE, "import.c : chargerTexture() : IMG_Load : Probleme de chargement de textures %s\n", buf);
        else
        {
          // MISE EN TAMPO DE LA TEXTURE DANS LE CONTEXT
          glBindTexture(GL_TEXTURE_2D, textures[i]);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          #ifdef __APPLE__
           glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->w, img->h, 0, GL_RGB, GL_UNSIGNED_BYTE, img->pixels);
          #else
           glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->w, img->h, 0, img->format->BytesPerPixel == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, img->pixels);
          #endif

          // DESTRUCTION DE LA SURFACE
          SDL_FreeSurface(img);
        }

      }

    }

  }

	int nbMeshes = nombreMeshesScenes(scene, scene->mRootNode, 0);
	*counts = calloc( nbMeshes, sizeof (*counts));
	if(!counts)
    fprintf(EXT_FILE, "import.c : chargerTexture() : allocation failed counts\n");

  GLuint ivao = 0;
	nombreVertexObjetScene(scene, scene->mRootNode, &ivao,*counts);

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




/////////////////////////////////////////////////////
/// \fn void nombreVertexObjetScene(const struct aiScene *scene, const struct aiNode* node, GLuint * ivao , GLuint counts[]);
/// \brief Compte le nombre de vertex dans un node et le note dans un tableau counts a l'indice de l objet
///
/// \param const struct aiScene *scene
/// \param const struct aiNode* node
/// \param GLuint * ivao
/// \param GLuint counts[]
/// \return void
/////////////////////////////////////////////////////
void nombreVertexObjetScene(const struct aiScene *scene, const struct aiNode* node, GLuint * ivao , GLuint counts[]) {
  int i, j;
  unsigned int n = 0;

  // PARCOUR DU NODE
  for (; n < node->mNumMeshes; ++n) {
    const struct aiMesh* mesh = scene->mMeshes[node->mMeshes[n]];

    // DETECTION D UNE FACE
    if(mesh->mFaces) {
      // POUR CHAQUE FACE
      for(i = 0, j = 0; j < mesh->mNumFaces; ++j) {
        // AJOUT ++ a i si un indice existe pour cette ELLEMENTS
        // PERMET DE COMPTER LE NOMBRE D INDICE PRESENT DANS LA FACE
				if(mesh->mFaces[j].mNumIndices != 3)continue;
        //
        if(mesh->mFaces[j].mIndices[0])
          i++;
        if(mesh->mFaces[j].mIndices[1])
          i++;
        if(mesh->mFaces[j].mIndices[2])
          i++;
      }
      // AJOUT DE L INDICE i DANS counts
      counts[*ivao] = i;
    }
    // INCREMENTATION DE L INDICE DU TAB COUNTS
    (*ivao)++;
  }

  // PARCOUR RECUSSIF
  for (n = 0; n < node->mNumChildren; ++n) {
    nombreVertexObjetScene(scene, node->mChildren[n], ivao,counts);
  }
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

/////////////////////////////////////////////////////
/// \fn char * pathOf(const char * path)
/// \brief trouve l'arborescene repertoire
///
/// \param const char * path
///
/// \return char * pointeur sur la chaine cree
/////////////////////////////////////////////////////
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
