// CODERUNNER COMPILEFLAG 
// -std=c99 glm/glm_util.c glm/glmimg.c -framework OpenGL -framework GLUT -lm



#include <stdio.h>
#ifdef __APPLE__
	#define GL_SILENCE_DEPRECATION
	#include <OpenGL/gl.h>
	#include <GLUT/glut.h>
#endif
#ifdef __linux
	#include <GL/gl.h>
	#include <GL/glu.h>
#endif

#include <math.h>

/* assimp include files. These three are usually needed. */
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

const C_STRUCT aiScene* scene = NULL; // MODEL/SCENE
GLuint scene_list = 0; // NB SCENE


// STATIC VAR FOR CAMERA
struct Camera_s
{
	float px,py,pz,angle,ouverture;
};
static struct Camera_s camera;
// END VAR CAMERA

#define VITESSE_DEPLACEMENT 0.5F


void framerate (void)
{
	static GLint prev_time = 0;
	static GLint prev_fps_time = 0;
	static int frames = 0;

	int time = glutGet(GLUT_ELAPSED_TIME);
	prev_time = time;

	frames += 1;
	if ((time - prev_fps_time) > 1000) /* update every seconds */
    {
        int current_fps = frames * 1000 / (time - prev_fps_time);
        printf("%d fps\n", current_fps);
        frames = 0;
        prev_fps_time = time;
    }


	glutPostRedisplay ();
}


// ASSIMP RENDU AND LOAD //

int chargementModel (const char* path)
{
	/* we are taking one of the postprocessing presets to avoid
	   spelling out 20+ single postprocessing flags here. */
	scene = aiImportFile(path,aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene) {
		return 0;
	}
	return 1;
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
void appliquerTexture(const C_STRUCT aiMaterial *mtl)
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

void rendu_Model (const C_STRUCT aiScene *sc, const C_STRUCT aiNode* nd)
{
	unsigned int i;
	unsigned int n = 0, t;
	C_STRUCT aiMatrix4x4 m = nd->mTransformation;

	/* update transform */
	//aiTransposeMatrix4(&m);
	//glPushMatrix();
	//glMultMatrixf((float*)&m);

	/* draw all meshes assigned to this node */
	for (; n < nd->mNumMeshes; ++n) {
		const C_STRUCT aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];

		appliquerTexture(sc->mMaterials[mesh->mMaterialIndex]); // COLOR TEXTURE

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

	/* draw all children */
	for (n = 0; n < nd->mNumChildren; ++n) {
		rendu_Model(sc, nd->mChildren[n]);
	}

	glPopMatrix();
}


void Reshape(int width, int height)
{	 
 	glViewport(0,0,width,height);
 	glMatrixMode(GL_PROJECTION);
 	glLoadIdentity();
 	gluPerspective(45,(float)(width)/(float)(height),0.1,100);	//Pour les explications, lire le tutorial sur OGL et win
 	glMatrixMode(GL_MODELVIEW);	//Optionnel
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
}

void boucleVue()
{	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	
	
	 gluLookAt(camera.px                   ,camera.py    ,camera.pz                  ,
			  camera.px+sin(camera.angle) ,camera.py    , camera.pz+cos(camera.angle),
			  0.0                          ,1.0         ,0.0)                        ;


	if(scene_list == 0) {
		scene_list = glGenLists(1);
		glNewList(scene_list, GL_COMPILE);
			/* now begin at the root node of the imported data and traverse
			the scenegraph by multiplying subsequent local transforms
			together on GL's matrix stack. */
		rendu_Model(scene, scene->mRootNode);
		glEndList();
	}
	
	
	glCallList(scene_list);
	glutSwapBuffers();
	

//	framerate();
}



void InitGL(){
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);    /* Uses default lighting parameters */
	
	glEnable(GL_DEPTH_TEST);

}




/// EVENT

/* Fonction executee lors de la frappe          */
/* d'une touche alphanumerique du clavier       */

void keyboard(unsigned char key,int x,int y) {
	printf("\n%f",camera.angle);
	switch ( key ) {
		
		case 'w'      : camera.px += VITESSE_DEPLACEMENT *sin(camera.angle);
						camera.pz += VITESSE_DEPLACEMENT *cos(camera.angle);
						glutPostRedisplay();
						break;
		case 's'    :   camera.px -= VITESSE_DEPLACEMENT *sin(camera.angle);
						camera.pz -= VITESSE_DEPLACEMENT *cos(camera.angle);
						glutPostRedisplay();
						break;
		case 'a'      : camera.px += VITESSE_DEPLACEMENT *sin(camera.angle + M_PI/2);
						camera.pz += VITESSE_DEPLACEMENT *cos(camera.angle + M_PI/2);
						glutPostRedisplay();
						break;
		case 'd'    : 	camera.px -= VITESSE_DEPLACEMENT *sin(camera.angle + M_PI/2);
						camera.pz -= VITESSE_DEPLACEMENT *cos(camera.angle + M_PI/2);
						glutPostRedisplay();
						break;
						
						
		case 'c'	:  	printf("\nA croupie")	;break;

		}
}



/* Fonction executee lors de la frappe          */
/* d'une touche non alphanumerique du clavier   */

void special(int key,int x,int y) {
	printf("\n%f",camera.angle);
  switch ( key ) {
	case GLUT_KEY_UP      : camera.px += 0.1F*sin(camera.angle);
							camera.pz += 0.1F*cos(camera.angle);
							glutPostRedisplay();
							break;
	case GLUT_KEY_DOWN    : camera.px -= 0.1F*sin(camera.angle);
							camera.pz -= 0.1F*cos(camera.angle);
							glutPostRedisplay();
							break;
	case GLUT_KEY_RIGHT   : camera.angle -= 0.03F;
							glutPostRedisplay();
							break;
	case GLUT_KEY_LEFT    : camera.angle += 0.03F;
							glutPostRedisplay();
							break; }
}

int main( int argc, char *argv[ ], char *envp[ ] )
{	 
	// PRESET VALUE CAMERA //
	
		camera.px = 0.0F;
		camera.py = 3.0F;
		camera.pz = 0.0F;
		
		camera.angle = 0.0F;
		camera.ouverture = 60.0F;
		
	
	// END PRESET VALUE //
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(640,480);	//Optionnel
	glutInitWindowPosition(300,300);
	int WindowName = glutCreateWindow("Ma première fenêtre OpenGL !");
	
	
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutDisplayFunc(boucleVue);
	
	chargementModel("salle.obj");
	
	InitGL();
	
	glutMainLoop();
	
	aiReleaseImport(scene);
 
	return 0;
 
}	