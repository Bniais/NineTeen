#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#define LOADING_SIZE 50
#define LOADING_ANGLE_PER_FRAME (360/60)

void afficherLoading(SDL_Renderer * renderer, SDL_Texture * loadingTexture, SDL_Color color, int frameAnim){
	if(frameAnim){
		int w, h;
		SDL_GetRendererOutputSize(renderer, &w, &h);

		SDL_Rect dest = {w - 3/2. * LOADING_SIZE , h - 3/2. * LOADING_SIZE, LOADING_SIZE, LOADING_SIZE};
		SDL_SetTextureColorMod(loadingTexture, color.r, color.g, color.b);
		SDL_RenderCopyEx(renderer, loadingTexture, NULL, &dest, (frameAnim-1) * LOADING_ANGLE_PER_FRAME, NULL, SDL_FLIP_NONE);
	}
}
