#include <SDL2/SDL_ttf.h>
#define FRAME_ANIM_RETOUR 25.
#define DIR_LOADING "../assets/texture/loading.png"
void afficherLoading(SDL_Renderer * renderer, SDL_Texture * loadingTexture, SDL_Color color, int shiftX, int shiftY, int frameAnim, int windowW, int windowH, int referenceW);
void afficherRetour(SDL_Renderer * renderer, SDL_Texture * loadingTexture,TTF_Font * font, SDL_Color color, int shiftX, int shiftY, int frameAnim, int windowW, int windowH, int referenceW);
void drawReplay(SDL_Renderer* renderer, TTF_Font* font, float posY, int viewW, int viewH);
void drawQuit(SDL_Renderer* renderer, TTF_Font* font, float ratioWindowSize, SDL_Color color);





/////////////////////////////////////////////////////
/// \fn int _malloc( void **ptr ,int type, int size , FILE *fp , Uint32 flags,const char* title, const char* message,SDL_Window* window )
/// \brief permet de faire une allocation dynamqiue avec message d'erreur
/// \param void **fp element a allouer
/// \param int type sizeof(type)
/// \param FILE *ptr pointeur sur element sorti erreur
/// \param Uint32 flags flags message SDL
/// \paran char *title if set NULL no message on SDL
/// \param char *message message pour fprintf
/// \param SDL_Window *window null si hors d'une fenetre
///
/// \return EXIT_SUCCESS/EXIT_FAILURE
/////////////////////////////////////////////////////
int _malloc( void **ptr ,int type, int size , FILE *fp , Uint32 flags,const char* title, const char* message,SDL_Window* window );

/////////////////////////////////////////////////////
/// \fn afficherMessageSysteme(char message[])
/// \brief affiche une popup systeme
/// \param char message[] message a afficher a l'ecran
/////////////////////////////////////////////////////
void afficherMessageSysteme(char message[]);

int lenNum(int n);
