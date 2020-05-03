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

int aiImportModel (const char* path,const C_STRUCT aiScene **scene);

void aiAppliquerCouleur(const C_STRUCT aiMaterial *mtl);




// fonction permettant de convertir des couleurs en different type
void color4_to_float4(const C_STRUCT aiColor4D *c, float f[4]);
void set_float4(float f[4], float a, float b, float c, float d);













































int aiImportModel (const char* path,const C_STRUCT aiScene **scene)
{
	/* we are taking one of the postprocessing presets to avoid
	   spelling out 20+ single postprocessing flags here. */
	*scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality |
						 aiProcess_CalcTangentSpace       |
			       		 aiProcess_Triangulate            |
			       	     aiProcess_JoinIdenticalVertices  |
			             aiProcess_SortByPType);

	if (*scene) {
		return EXIT_SUCCESS;
	}
	else
		fprintf(EXT_FILE,"import.c : aiImportModel() : aiImportFile : impossible de charger %s\n",path );

	return EXIT_FAILURE;
}



void aiAppliquerCouleur(const C_STRUCT aiMaterial *mtl)
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


/////////////////////////////////////////////////////
/// \fn int sceneNbMeshes(const struct aiScene *sc, const struct aiNode* nd, int subtotal)
/// \brief retourne le nombre de mesh dans une scene
///
/// \param const struct aiScene *sc
/// \param const struct aiNode* nd
/// \param const struct aiScene *sc
///
/// \return int nombre de mesh
/////////////////////////////////////////////////////
int sceneNbMeshes(const struct aiScene *sc, const struct aiNode* nd, int subtotal) {
  int n = 0;
  subtotal += nd->mNumMeshes;
  for(n = 0; n < nd->mNumChildren; ++n)
    subtotal += sceneNbMeshes(sc, nd->mChildren[n], 0);
  return subtotal;
}

void sceneMkVAOs(const struct aiScene *sc, const struct aiNode* nd, GLuint * ivao , GLuint counts[]) {
  int i, j; //comp;
  unsigned int n = 0;
  //static int temp = 0;

  //temp++;

  for (; n < nd->mNumMeshes; ++n) {
   // GLfloat * vertices = NULL;
    GLuint  * indices  = NULL;
    const struct aiMesh* mesh = sc->mMeshes[nd->mMeshes[n]];
  //  comp  = mesh->mVertices ? 3 : 0;
  //  comp += mesh->mNormals ? 3 : 0;
  //  comp += mesh->mTextureCoords[0] ? 2 : 0;
  //  if(!comp) continue;

  //  glBindVertexArray(_vaos[*ivao]);
  //  glBindBuffer(GL_ARRAY_BUFFER, _buffers[2 * (*ivao)]);

  //  vertices = malloc(comp * mesh->mNumVertices * sizeof *vertices);
  //  assert(vertices);
  //  i = 0;
  //  glDisableVertexAttribArray(0);
  //  glDisableVertexAttribArray(1);
  //  glDisableVertexAttribArray(2);

/*    if(mesh->mVertices) {
    //  glEnableVertexAttribArray(0);
    //  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const void *)(i * sizeof *vertices));
      for(j = 0; j < mesh->mNumVertices; ++j) {
				vertices[i++] = mesh->mVertices[j].x;
				vertices[i++] = mesh->mVertices[j].y;
				vertices[i++] = mesh->mVertices[j].z;
      }
    }*/
/*    if(mesh->mNormals) {
    //  glEnableVertexAttribArray(1);
    //  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const void *)(i * sizeof *vertices));
      for(j = 0; j < mesh->mNumVertices; ++j) {
				vertices[i++] = mesh->mNormals[j].x;
				vertices[i++] = mesh->mNormals[j].y;
				vertices[i++] = mesh->mNormals[j].z;
      }
    } */

/*
    //  glEnableVertexAttribArray(2);
    //  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const void *)(i * sizeof *vertices));
      for(j = 0; j < mesh->mNumVertices; ++j) {
				vertices[i++] = mesh->mTextureCoords[0][j].x;
				vertices[i++] = mesh->mTextureCoords[0][j].y;
      }
    } */


  //  glBufferData(GL_ARRAY_BUFFER, (i * sizeof *vertices), vertices, GL_STATIC_DRAW);

//    free(vertices);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffers[2 * (*ivao) + 1]);
    if(mesh->mFaces) {
      indices = malloc(3 * mesh->mNumFaces * sizeof *indices);
  //    assert(indices);
      for(i = 0, j = 0; j < mesh->mNumFaces; ++j) {
	//			assert(mesh->mFaces[j].mNumIndices < 4);
				if(mesh->mFaces[j].mNumIndices != 3) continue;
				indices[i++] = mesh->mFaces[j].mIndices[0];
				indices[i++] = mesh->mFaces[j].mIndices[1];
				indices[i++] = mesh->mFaces[j].mIndices[2];
      }

    //  glBufferData(GL_ELEMENT_ARRAY_BUFFER, i * sizeof *indices, indices, GL_STATIC_DRAW);
      counts[*ivao] = i;
      free(indices);
    }
  //  glBindVertexArray(0);
    (*ivao)++;
  }
  for (n = 0; n < nd->mNumChildren; ++n) {
    sceneMkVAOs(sc, nd->mChildren[n], ivao,counts);
  }
}

void aiDessinerImage(const struct aiScene *sc, const struct aiNode* nd, GLuint * ivao, GLuint textures[], GLuint counts[])
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

	//	if (aiGetMaterialTextureCount(sc->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE) > 0) {
			glBindTexture(GL_TEXTURE_2D, textures[mesh->mMaterialIndex]);
	//	}
		if(counts[*ivao]) {

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
						glBindTexture(GL_TEXTURE_2D, textures[mesh->mMaterialIndex]);
					}

					glBegin(face_mode);
					for(i = 0; i < face->mNumIndices; i++) {

						int index = face->mIndices[i];

						if (aiGetMaterialTextureCount(sc->mMaterials[mesh->mMaterialIndex], aiTextureType_DIFFUSE) > 0) {
							glTexCoord2f(mesh->mTextureCoords[0][index].x, 1- mesh->mTextureCoords[0][index].y);
						}
						if(mesh->mColors[0] != NULL)
						{
						//	glColor4fv((GLfloat*)&mesh->mColors[0][index]);
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
		aiDessinerImage(sc, nd->mChildren[n],ivao,textures,counts);
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


void aiLoadTexture(const char* filename, const C_STRUCT aiScene *_scene, GLuint textures[], GLuint ** counts )
{
  int i = 0;



//  _textures = malloc( (_nbTextures = _scene->mNumMaterials) * sizeof *_textures);
//  assert(_textures);
  glGenTextures(_scene->mNumMaterials, textures);

  for (i = 0; i < _scene->mNumMaterials ; i++) {

    const struct aiMaterial* pMaterial = _scene->mMaterials[i];

    if (aiGetMaterialTextureCount(pMaterial, aiTextureType_DIFFUSE) > 0) {


         struct aiString tfname;
         if (aiGetMaterialTexture(pMaterial, aiTextureType_DIFFUSE, 0, &tfname, NULL, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {

           SDL_Surface * t;
           char * dir = pathOf(filename), buf[BUFSIZ];
           snprintf(buf, sizeof buf, "%s/%s", dir, tfname.data);

           if(!(t = IMG_Load(buf))) {
            fprintf(stderr, "Probleme de chargement de textures %s\n", buf);
            fprintf(stderr, "\tNouvel essai avec %s\n", tfname.data);
            if(!(t = IMG_Load(tfname.data)))
						{
							fprintf(stderr, "Probleme de chargement de textures %s\n", tfname.data); continue;
            }
            glBindTexture(GL_TEXTURE_2D, textures[i]);
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

	int nbMeshes = sceneNbMeshes(_scene, _scene->mRootNode, 0);
  	printf("NB mesh = %d\n",nbMeshes );
//	_vaos = malloc(_nbMeshes * sizeof *_vaos);
//	assert(_vaos);
//	glGenVertexArrays(_nbMeshes, _vaos);

//	glGenBuffers(2 * _nbMeshes,_buffers);
	*counts = calloc( nbMeshes, sizeof (*counts));
	assert(*counts);

  GLuint ivao = 0;
	sceneMkVAOs(_scene, _scene->mRootNode, &ivao,*counts);

}
