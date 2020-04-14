#include <SDL2/SDL_ttf.h>
#define FRAME_ANIM_RETOUR 25.
#define DIR_LOADING "assets/texture/loading.png"
void afficherLoading(SDL_Renderer * renderer, SDL_Texture * loadingTexture, SDL_Color color, int shiftX, int shiftY, int frameAnim, int windowW, int windowH, int referenceW);
void afficherRetour(SDL_Renderer * renderer, SDL_Texture * loadingTexture,TTF_Font * font, SDL_Color color, int shiftX, int shiftY, int frameAnim, int windowW, int windowH, int referenceW);
