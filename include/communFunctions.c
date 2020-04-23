#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "communFunctions.h"
#include "../define/define.h"
#define LOADING_SIZE 54
#define LOADING_ANGLE_PER_FRAME (360/60)
#define SIZE_ECHEC 62.
const SDL_Rect SRC_LOADING = {0,0,222,222};
const SDL_Color COLOR_ERROR_LOADING = {0xfb,0x17,0x17};

const int ALPHA[2*(int)FRAME_ANIM_RETOUR] = { 10, 30, 55, 70, 100, 140, 170, 200, 220, 225, 230, 230, 230, 230, 230, 230,230, 230, 230, 230, 230,230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 230, 225,220,200,170,140,100,70,55,30,10};

/////////////////////////////////////////////////////
/// \fn afficherMessageSysteme(char message[])
/// \brief affiche une popup systeme
/// \param char message[] message a afficher a l'ecran
/////////////////////////////////////////////////////
void afficherMessageSysteme(char message[])
{
	#ifdef _WIN32
		char commande[80 + strlen(message) ];
		sprintf(commande, "mshta vbscript:Execute(\"msgbox \"\"%s\"\":close\") " , message);
		system(commande);
	#endif

	#ifdef __APPLE__
		char commande[160 + strlen(message) ];
		sprintf(commande, "osascript -e 'tell application (path to frontmost application as text) to display dialog \"%s\" buttons {\"QUITTER\"} with icon stop'" , message);
		system(commande);
	#endif

	#ifdef __linux
		char commande[60 + strlen(message) ];
		sprintf(commande, "zenity --warning --text \"%s\"" , message);
		system(commande);
	#endif
}

void afficherLoading(SDL_Renderer * renderer, SDL_Texture * loadingTexture, SDL_Color color, int shiftX, int shiftY, int frameAnim, int windowW, int windowH, int referenceW){

	float ratioSize = (float)referenceW/BASE_WINDOW_W;
		printf("ar %f\n", ratioSize );
	if(frameAnim){
		SDL_SetTextureAlphaMod(loadingTexture, 255);
		SDL_Rect dest = {windowW - 3/2. * ratioSize* LOADING_SIZE + shiftX* ratioSize , windowH - 3/2. * ratioSize* LOADING_SIZE + shiftY* ratioSize, ratioSize* LOADING_SIZE, ratioSize* LOADING_SIZE};
		SDL_SetTextureColorMod(loadingTexture, color.r, color.g, color.b);
		SDL_RenderCopyEx(renderer, loadingTexture, &SRC_LOADING, &dest, (frameAnim-1) * LOADING_ANGLE_PER_FRAME, NULL, SDL_FLIP_NONE);
	}
}


void afficherRetour(SDL_Renderer * renderer, SDL_Texture * loadingTexture,TTF_Font * font, SDL_Color color, int shiftX, int shiftY, int frameAnim, int windowW, int windowH, int referenceW){

	float ratioSize = (float)referenceW/BASE_WINDOW_W;
	printf("ar %f\n", ratioSize );
	int fLogo= frameAnim;
	if(fLogo < 0){
		fLogo += FRAME_ANIM_RETOUR;
		if(fLogo > 0)
			fLogo = 0;
	}

	//logo
	if(fLogo){


		int w, h;
		SDL_GetRendererOutputSize(renderer, &w, &h);
		SDL_Rect src = SRC_LOADING;

		src.y =( fLogo>0 ? 2 : 1 ) * src.h;

		if(frameAnim < 0 )
			SDL_SetTextureColorMod(loadingTexture, COLOR_ERROR_LOADING.r, COLOR_ERROR_LOADING.g, COLOR_ERROR_LOADING.b);
		else
			SDL_SetTextureColorMod(loadingTexture, color.r, color.g, color.b);

		int frameAlpha = -abs(abs(fLogo)-FRAME_ANIM_RETOUR/2) + FRAME_ANIM_RETOUR/2;
		SDL_SetTextureAlphaMod(loadingTexture, 10+220*frameAlpha/(FRAME_ANIM_RETOUR/2));
		SDL_Rect dest = {windowW - 3/2. * ratioSize* LOADING_SIZE  + shiftX* ratioSize , windowH - 3/2. * ratioSize* LOADING_SIZE + shiftY* ratioSize, ratioSize* LOADING_SIZE, ratioSize* LOADING_SIZE};

		SDL_RenderCopy(renderer, loadingTexture, &src, &dest);

	}

	//text
	if(frameAnim < 0 ){
		char * text = "L'envoi de score a echoue !";

		SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, COLOR_ERROR_LOADING);
		SDL_Rect dest;
		SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
		SDL_QueryTexture(Message,NULL,(int*)SDL_TEXTUREACCESS_STATIC,&(dest.w), &(dest.h) );
		dest.w /= (OPEN_FONT_SIZE / SIZE_ECHEC)* ratioSize;
		dest.h /= (OPEN_FONT_SIZE / SIZE_ECHEC) *ratioSize;

		//centrer
		dest.x = PLAYGROUND_SIZE_W/2 - dest.w/2;
		dest.y = PLAYGROUND_SIZE_H/3 - dest.h/2;

		SDL_SetTextureAlphaMod(Message, ALPHA[-frameAnim-1]);
		SDL_RenderCopy(renderer, Message, NULL, &dest);
		SDL_FreeSurface(surfaceMessage);
		SDL_DestroyTexture(Message);
	}
}
