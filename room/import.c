#include "import.h"
#include <stdio.h>

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

void aiDessinerScene (const C_STRUCT aiScene *sc, const C_STRUCT aiNode* nd);




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
		return 0;
	}
	else
		printf("Scene introuvable\n" );

	return 1;
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
  C_STRUCT aiColor4D transparence;
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

  set_float4(c, 0.0f, 0.0f, 0.0f, 1.0f);
	if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_TRANSPARENT, &transparence))
		color4_to_float4(&transparence, c);



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



/*

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

static int sceneNbMeshes(const struct aiScene *sc, const struct aiNode* nd, int subtotal) {
	int n = 0;
	subtotal += nd->mNumMeshes;
	for(n = 0; n < nd->mNumChildren; ++n)
		subtotal += sceneNbMeshes(sc, nd->mChildren[n], 0);
	return subtotal;
}

*/





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
