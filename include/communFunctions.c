/////////////////////////////////////////////////
///	\file communFunctions.c
///	\author Samy M.
///	\version 1.0
///	\date 31 janvier 2020
///	\brief regroupe un certain nombre de fonction utiliser par different fichier
/////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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


FILE* EXT_FILE;




/////////////////////////////////////////////////////
/// \fn void informationPreciseCPUGPU()
/// \brief permet d'afficher les info CPU/GPU
///
///
/////////////////////////////////////////////////////
void informationPreciseCPUGPU()
{

	#ifdef __APPLE__
		system("system_profiler SPSoftwareDataType | grep -e \"System Version\" | sed 's/ //g' | sed 's/SystemVersion://g' >> /tmp/Nineteen.tmp");
		system("sysctl -n machdep.cpu.brand_string | sed 's/ //g' >> /tmp/Nineteen.tmp");
		system("system_profiler SPDisplaysDataType | grep -e Chipset -e VRAM | sed 's/ //g' | sed 's/ChipsetModel://' | sed 's/VRAM(Total)://' >> /tmp/Nineteen.tmp");

		char buf[512];
		FILE *fp = fopen("/tmp/Nineteen.tmp", "r");

		fscanf(fp, "%s\n",buf);
		fprintf(EXT_FILE," - System : %s\n",buf);

		fscanf(fp, "%s\n",buf);
		fprintf(EXT_FILE," - ChipsetCPU : %s\n",buf);

		fscanf(fp, "%s\n",buf);
		fprintf(EXT_FILE," - ChipsetGPU : %s\n",buf);

		fscanf(fp, "%s\n",buf);
		fprintf(EXT_FILE," - VRAM GPU = %s\n",buf);

		fclose(fp);
		system("rm /tmp/Nineteen.tmp");
	#endif

	/*#ifdef _WIN32

		system("ver >> C:\\Windows\\Temp\\Nineteen.tmp");
		system("wmic cpu get Name | findstr /v Name >> C:\\Windows\\Temp\\Nineteen.tmp");
		system("wmic path win32_VideoController get name | findstr /v Name >> C:\\Windows\\Temp\\Nineteen.tmp");

		char buf[512], totalbuf[1024];

		FILE *fp = fopen("C:\\Windows\\Temp\\Nineteen.tmp", "r");

		while(fscanf(fp, "%*s %[^\n]",buf) != EOF ){
			strcat(totalbuf, buf);
			strcat(totalbuf, "\n");
		}
		fclose(fp);
        system( " del C:\\Windows\\Temp\\Nineteen.tmp" );

		char * system = strtok ( totalbuf, "\n" );
		fprintf(EXT_FILE," - System : %s\n",system);

		char * chipset = strtok ( NULL, "\n" );
		fprintf(EXT_FILE," - ChipsetCPU : %s",chipset);

		char * gpu = strtok ( NULL, "\n" );
		fprintf(EXT_FILE," - ChipsetGPU : %s",gpu);

	#endif */

}


/////////////////////////////////////////////////////
/// \fn int _malloc( void **ptr ,int type, int size , FILE *fp , Uint32 flags,const char* title, const char* message,SDL_Window* window )
/// \brief permet de faire une allocation dynamqiue avec message d'erreur
/// \param void **ptr element a allouer
/// \param int type sizeof(type)
/// \param FILE *fp pointeur sur element sorti erreur
/// \param Uint32 flags flags message SDL
/// \paran char *title if set NULL no message on SDL
/// \param char *message message pour fprintf
/// \param SDL_Window *window null si hors d'une fenetre
///
/// \return EXIT_SUCCESS/EXIT_FAILURE
/////////////////////////////////////////////////////
int _malloc( void **ptr ,int type, int size , FILE *fp , Uint32 flags,const char* title, const char* message,SDL_Window* window )
{
	*ptr = malloc(sizeof(type) * size);
	if(!*ptr)
	{
		if(!title)
			SDL_ShowSimpleMessageBox(flags,title,message,window);
		fprintf(fp,"%s",message);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

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
	if(frameAnim){
		SDL_SetTextureAlphaMod(loadingTexture, 255);
		SDL_Rect dest = {windowW - 3/2. * ratioSize* LOADING_SIZE + shiftX* ratioSize , windowH - 3/2. * ratioSize* LOADING_SIZE + shiftY* ratioSize, ratioSize* LOADING_SIZE, ratioSize* LOADING_SIZE};
		SDL_SetTextureColorMod(loadingTexture, color.r, color.g, color.b);
		SDL_RenderCopyEx(renderer, loadingTexture, &SRC_LOADING, &dest, (frameAnim-1) * LOADING_ANGLE_PER_FRAME, NULL, SDL_FLIP_NONE);
	}
}


void afficherRetour(SDL_Renderer * renderer, SDL_Texture * loadingTexture,TTF_Font * font, SDL_Color color, int shiftX, int shiftY, int frameAnim, int windowW, int windowH, int referenceW){

	float ratioSize = (float)referenceW/BASE_WINDOW_W;
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

char* REPLAY = "Appuyer sur Espace pour jouer !";
#define SIZE_REPLAY 43.
SDL_Color WHITE = {255,255,255,255};
/**
*\fn void drawReplay(SDL_Renderer* renderer, TTF_Font* font)
*\brief Affiche un texte invitant le joueur à rejouer
*\param renderer Le renderer où afficher
*\param font La police d'écriture
*/
void drawReplay(SDL_Renderer* renderer, TTF_Font* font, float posY, int viewW, int viewH){
	char * text = REPLAY;

	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, WHITE);
	SDL_Rect dest;
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	SDL_QueryTexture(Message,NULL,(int*)SDL_TEXTUREACCESS_STATIC,&(dest.w), &(dest.h) );
	dest.w /= (OPEN_FONT_SIZE / SIZE_REPLAY);
	dest.h /= (OPEN_FONT_SIZE / SIZE_REPLAY);

	//centrer
	dest.x = viewW/2 - dest.w/2;
	dest.y = (viewH/2 - dest.h)*(1-posY);

	SDL_RenderCopy(renderer, Message, NULL, &dest);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
}

//quit
static char * QUIT = "Echap";
static SDL_Rect QUIT_DEST = {10,10,0,0};
#define SIZE_QUIT 28.
void drawQuit(SDL_Renderer* renderer, TTF_Font* font, float ratioWindowSize, SDL_Color color){
	char * text = QUIT;

	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, color);
	SDL_Rect dest = QUIT_DEST;
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	SDL_QueryTexture(Message,NULL,(int*)SDL_TEXTUREACCESS_STATIC,&(dest.w), &(dest.h) );
	dest.w /= (OPEN_FONT_SIZE / SIZE_QUIT);
	dest.h /= (OPEN_FONT_SIZE / SIZE_QUIT);
	dest.w *= ratioWindowSize;
	dest.h *= ratioWindowSize;
	dest.x *= ratioWindowSize;
	dest.y *= ratioWindowSize;

	SDL_RenderCopy(renderer, Message, NULL, &dest);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
}

int lenNum(int n)
{
	int count=1;

	while(n >= 10){
		n /=10;
		count++;
	}

	return count;
}

int randSign(){
	return (rand()%2 ? 1 : -1);
}
