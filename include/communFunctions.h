#include <SDL2/SDL_ttf.h>
#define FRAME_ANIM_RETOUR 25.
void afficherLoading(SDL_Renderer * renderer, SDL_Texture * loadingTexture, SDL_Color color, int shiftX, int shiftY, int frameAnim, int windowW, int windowH, int referenceW);
void afficherRetour(SDL_Renderer * renderer, SDL_Texture * loadingTexture,TTF_Font * font, SDL_Color color, int shiftX, int shiftY, int frameAnim, int windowW, int windowH, int referenceW);
void drawReplay(SDL_Renderer* renderer, TTF_Font* font);
void drawQuit(SDL_Renderer* renderer, TTF_Font* font, float ratioWindowSize, SDL_Color color);

/////////////////////////////////////////////////////
/// \fn afficherMessageSysteme(char message[])
/// \brief affiche une popup systeme
/// \param char message[] message a afficher a l'ecran
/////////////////////////////////////////////////////
void afficherMessageSysteme(char message[]);

int lenNum(int n);
