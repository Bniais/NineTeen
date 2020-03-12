#include "../../define/define.h"
#include "config.h"
#include <time.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_image.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void myInit(){

	// SDL Init
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	//audio
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	if( Mix_OpenAudio( MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 1024 ) == -1 )
		printf("Erreur init SDL_Mixer\n" );

	// rand
	srand(time(NULL));
}

SDL_Point maximizeWindow(SDL_Rect displayBounds, float* ratioWindowSize){
	SDL_Point maxW = {(PLAYGROUND_SIZE_W + 2 * HUD_W), (PLAYGROUND_SIZE_H + 2 * HUD_H)};
	if( maxW.x > displayBounds.w - ESPACE_DISPLAY_WINDOW.x  ||  maxW.y > (displayBounds.h - ESPACE_DISPLAY_WINDOW.y) - ESPACE_DISPLAY_WINDOW.y){
		if( (float)maxW.x/maxW.y > (float)(displayBounds.w - ESPACE_DISPLAY_WINDOW.x)/(displayBounds.h - ESPACE_DISPLAY_WINDOW.y) ){
			*ratioWindowSize = (float)maxW.x / (displayBounds.w - ESPACE_DISPLAY_WINDOW.x);
			maxW.y /= (float)maxW.x / (displayBounds.w - ESPACE_DISPLAY_WINDOW.x);
			maxW.x = displayBounds.w - ESPACE_DISPLAY_WINDOW.x;
		}
		else{
			*ratioWindowSize = (float)maxW.y / (displayBounds.h - ESPACE_DISPLAY_WINDOW.y);
			maxW.x /= (float)maxW.y / (displayBounds.h - ESPACE_DISPLAY_WINDOW.y);
			maxW.y = (displayBounds.h - ESPACE_DISPLAY_WINDOW.y);
		}
	}
	return maxW;
}

void afficherPauseMenu(SDL_Renderer *renderer, SDL_Point mouseCoor, SDL_Texture *voileTexture, SDL_Texture *pauseMenuTexture){
	//voile noir
	SDL_RenderCopy(renderer, voileTexture, NULL, NULL);
	SDL_Rect dest = BOUTON_PAUSE_BASE;
	SDL_Rect src = {0, 0, BOUTON_PAUSE_SIZE_W, BOUTON_PAUSE_SIZE_H};

	//boutons liste
	for(int i = 0; i < NB_BOUTON_PAUSE; i++){
		if( mouseCoor.x >= dest.x && mouseCoor.x <= dest.x + dest.w && mouseCoor.y >= dest.y && mouseCoor.y <= dest.y + dest.h)
			src.y = BOUTON_PAUSE_SIZE_H;
		else
			src.y = 0;

		SDL_RenderCopy(renderer, pauseMenuTexture, &src, &dest);
		src.x += BOUTON_PAUSE_SIZE_W;
		dest.y += BOUTON_PAUSE_SIZE_H + ESPACEMENT_BOUTON_PAUSE;
	}
}
void init_grille(int grille[TAILLE_GRILLE_LIGNE][TAILLE_GRILLE_COLONNE]){
	for(int i=0;i<TAILLE_GRILLE_LIGNE;i++)
		for(int j=0;j<TAILLE_GRILLE_COLONNE;j++)
			grille[i][j]=MASQUE;

	int bombes=NB_BOMBES;
	while(bombes!=0){
		int x=rand()%TAILLE_GRILLE_LIGNE;
		int y=rand()%TAILLE_GRILLE_COLONNE;
		if(grille[x][y]==MASQUE){
			grille[x][y]=BOMBE;
			bombes--;
		}
	}
}

void afficher_grille(SDL_Renderer *renderer, int grille[TAILLE_GRILLE_LIGNE][TAILLE_GRILLE_COLONNE], int click_bombe){
	SDL_Rect case_dem={50,50,TAILLE_CASE,TAILLE_CASE};
	SDL_SetRenderDrawColor(renderer,0,0,0,255);

	for(int i=0;i<TAILLE_GRILLE_LIGNE;i++){
		case_dem.x=50;
		for(int j=0;j<TAILLE_GRILLE_COLONNE;j++){
			if(grille[i][j]==BOMBE && click_bombe==0){
				SDL_SetRenderDrawColor(renderer,89,89,89,255);
				SDL_RenderFillRect(renderer, &case_dem);
			}
			else if(grille[i][j]==BOMBE && click_bombe==1){
				SDL_SetRenderDrawColor(renderer,255,0,0,255);
				SDL_RenderFillRect(renderer, &case_dem);
			}
			else if(grille[i][j] == VIDE)
			{
				SDL_SetRenderDrawColor(renderer,255,255,255,255);
				SDL_RenderFillRect(renderer, &case_dem);
				SDL_SetRenderDrawColor(renderer,0,0,0,255);
			}
			else if(grille[i][j] == MASQUE){
				SDL_SetRenderDrawColor(renderer, 89, 89, 89, 255);
				SDL_RenderFillRect(renderer, &case_dem);
			}
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderDrawRect(renderer, &case_dem);
			case_dem.x+=TAILLE_CASE;
		}
		case_dem.y+=TAILLE_CASE;
	}
}


void click_vide(int grille[TAILLE_GRILLE_LIGNE][TAILLE_GRILLE_COLONNE], int x, int y){
	int nb_bombes=0;
	if(grille[x-1][y-1]==BOMBE)
		nb_bombes++;
	if(grille[x-1][y]==BOMBE)
		nb_bombes++;
	if(grille[x-1][y+1]==BOMBE)
		nb_bombes++;
	if(grille[x][y-1]==BOMBE)
		nb_bombes++;
	if(grille[x][y+1]==BOMBE)
		nb_bombes++;
	if(grille[x+1][y-1]==BOMBE)
		nb_bombes++;
	if(grille[x+1][y]==BOMBE)
		nb_bombes++;
	if(grille[x+1][y+1]==BOMBE)
		nb_bombes++;

	if(nb_bombes!=0){
		grille[x][y]=VIDE;
	}

	else if(nb_bombes==0){
		grille[x][y]=VIDE;
		click_vide(grille, x-1, y-1);
		click_vide(grille, x-1, y);
		click_vide(grille, x-1, y+1);
		click_vide(grille, x, y-1);
		click_vide(grille, x, y+1);
		click_vide(grille, x+1, y-1);
		click_vide(grille, x+1, y);
		click_vide(grille, x+1, y+1);
	}


}
// int launchSnake(SDL_Window *myWindow, SDL_Renderer* renderer, char *identifiant, char *token){
int main(){
/////////////////////
/// MISE EN PLACE ///``
/////////////////////
	myInit();

	////////////
	/// Vars ///`
	////////////
	float ratioWindowSize = 1;
	SDL_Rect displayBounds;
	SDL_GetDisplayBounds(0, &displayBounds);
		SDL_Point maxWindowSize = maximizeWindow(displayBounds, &ratioWindowSize);
	int jaiclique = SDL_FALSE;
	int jairelache = SDL_FALSE;
	int click_bombe = 0;

	float ratio_fen=(float)maxWindowSize.x/(PLAYGROUND_SIZE_W + 2 * HUD_W);
	printf("ratio_fen -->%d %d %d %f\n",maxWindowSize.x, HUD_W, (PLAYGROUND_SIZE_W + 2 * HUD_W), ratio_fen);
	//Keyboard
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);

	//Time
	unsigned int lastTime = 0, currentTime;

	//Fonts
	/*TTF_Font* scoreFont = TTF_OpenFont("./fonts/flappy.ttf", OPEN_FONT_SIZE);
	if( scoreFont == NULL ){
		printf("TTF_OpenFont() Failed: %s\n", TTF_GetError());
		return EXIT_FAILURE;
	}*/

	//audio
	/*Mix_Chunk *flap_wav = Mix_LoadWAV( "../3_flappy_bird/Sounds/flap.wav" );
	if( !flap_wav)
		printf("Erreur chargement des sons\n");*/

	//Window and renderer



	SDL_Window *myWindow = SDL_CreateWindow("Snake", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, maxWindowSize.x ,maxWindowSize.y, WINDOW_FLAG);
	if( myWindow == NULL ){
		printf("Erreur lors de la creation de la fenêtre : %s", SDL_GetError());
		return EXIT_FAILURE;
	}

	printf("%d, %d\n", maxWindowSize.x ,maxWindowSize.y );

	SDL_Renderer *renderer = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED);
	if( renderer == NULL ){
		printf("Erreur lors de la creation d'un renderer : %s", SDL_GetError());
		return EXIT_FAILURE;
	}

	//Views
	SDL_Rect playgroundView = {HUD_W/ratioWindowSize, HUD_H/ratioWindowSize, PLAYGROUND_SIZE_W, PLAYGROUND_SIZE_H};
	SDL_Rect hudView = {0, 0, (PLAYGROUND_SIZE_W + 2 * HUD_W), (PLAYGROUND_SIZE_H + 2 * HUD_H)};
	SDL_Rect hudDraw = {0, 0, (PLAYGROUND_SIZE_W + 2 * HUD_W)/ratioWindowSize, (PLAYGROUND_SIZE_H + 2 * HUD_H)/ratioWindowSize};

	//hud and menus
	int paused = 0;
	int rdyToPause = 1;

	//mouse
	SDL_Point mouseCoor;

	//Textures
	SDL_Texture *hudTexture = IMG_LoadTexture(renderer, DIR_HUD);
	if( hudTexture == NULL ){
		printf("Erreur lors de la creation de texture");
		return EXIT_FAILURE;
	}

	int grille[TAILLE_GRILLE_LIGNE][TAILLE_GRILLE_COLONNE];
	init_grille(grille);




	///////////////////////
	/// Initialize vars ///`
	///////////////////////


/////////////////////
/// BOUCLE DU JEU ///``
/////////////////////

	while( 1 ){

		// Init input
		SDL_GetMouseState(&(mouseCoor.x), &(mouseCoor.y));
		jairelache = SDL_FALSE;
	////////////
	// Events //`
	////////////
		SDL_Event event;
		while( SDL_PollEvent(&event) ){
			switch( event.type ){
				case SDL_QUIT:
					// fermer
					return 0;
					break;

				case SDL_MOUSEBUTTONDOWN:
					if(event.button.button == SDL_BUTTON_LEFT){
						jaiclique = SDL_TRUE;
					}break;

				case SDL_MOUSEBUTTONUP:
					if((event.button.button == SDL_BUTTON_LEFT)&&(jaiclique==SDL_TRUE)){
						jairelache=SDL_TRUE;
					}break;


				case SDL_KEYUP:
					if ( event.key.keysym.sym == SDLK_ESCAPE )
						rdyToPause = 1;
					break;

				case SDL_KEYDOWN:
					if ( event.key.keysym.sym == SDLK_ESCAPE && rdyToPause ){
						paused = !paused;
						rdyToPause = 0;
					}
					break;
			}
		}


	////////////////////////////
	// Handle Keyboard inputs //`
	////////////////////////////
		SDL_PumpEvents();

	//////////////
	// Gameplay //`
	//////////////


	///////////////////
	// Check hitboxs //`
	///////////////////


	//////////
	// Draw //`
	//////////

	if(jairelache){
		mouseCoor.x-=ratio_fen*HUD_W;
		mouseCoor.y-=ratio_fen*HUD_H;
		SDL_Point case_coor = {(int)(mouseCoor.x/(TAILLE_CASE*ratio_fen)), (int)(mouseCoor.y/(TAILLE_CASE*ratio_fen))};
		SDL_Rect case_dem={case_coor.x*TAILLE_CASE,case_coor.y*TAILLE_CASE,TAILLE_CASE,TAILLE_CASE};
		SDL_SetRenderDrawColor(renderer,255,0,0,255);
		SDL_RenderFillRect(renderer, &case_dem);

		if(grille[case_coor.y-1][case_coor.x-1] == BOMBE){
			click_bombe=1;
		}
		else if(grille[case_coor.y-1][case_coor.x-1] == MASQUE){
			click_vide(grille, case_coor.y-1, case_coor.x-1);
		}
		printf("x-->%d    y-->%d \n", mouseCoor.x,mouseCoor.y);
	}


	afficher_grille(renderer,grille, click_bombe);

		//hud
		SDL_RenderSetScale(renderer, 1, 1);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderSetViewport(renderer, &hudView);
		SDL_RenderCopy(renderer, hudTexture, &hudView, &hudDraw);
		SDL_RenderSetViewport(renderer, &playgroundView);

		//afficher
		SDL_RenderSetScale(renderer, 1. / ratioWindowSize, 1. / ratioWindowSize);
		SDL_RenderPresent(renderer);


	////////////////
	// Next frame //`
	////////////////


		//regulateFPS
		currentTime = SDL_GetTicks();
		while( currentTime - lastTime < FRAME_TIME )
			currentTime = SDL_GetTicks();

		if( currentTime - lastTime > FRAME_TIME )
			printf(" TIME FRAME : %d\n", currentTime - lastTime);

		lastTime = currentTime;

		// On efface
		SDL_SetRenderDrawColor(renderer, 0, 40, 200, 255);
		SDL_RenderClear(renderer);
	}
	//printf("Waw t'es nul, %d\n", score);
	return 0;
}
