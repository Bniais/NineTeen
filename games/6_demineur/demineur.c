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

static const int FRAME_TIME = 1000 / FRAMES_PER_SECOND;

static void myInit(){

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

static const SDL_Point ESPACE_DISPLAY_WINDOW = { 100, 100};

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


int coor_valide(int x, int y){
	if((x>=0)&&(x<TAILLE_GRILLE_LIGNE)&&(y>=0)&&(y<TAILLE_GRILLE_COLONNE))
		return 1;
	else return 0;
}

int nb_bombes_autour(int grille[TAILLE_GRILLE_LIGNE][TAILLE_GRILLE_COLONNE], int x, int y){

	int nb_bombes_autour=0;
	if(grille[x][y]==MASQUE_AVEC_BOMBES)
		return -1;
	if((coor_valide(x-1, y-1))&&((grille[x-1][y-1]==MASQUE_AVEC_BOMBES)||(grille[x-1][y-1]==DRAPEAU_AVEC_BOMBES)||(grille[x-1][y-1]==BOMBE)))
		nb_bombes_autour++;
	if((coor_valide(x-1, y))&&((grille[x-1][y]==MASQUE_AVEC_BOMBES)||(grille[x-1][y]==DRAPEAU_AVEC_BOMBES)||(grille[x-1][y]==BOMBE)))
		nb_bombes_autour++;
	if((coor_valide(x-1, y+1))&&((grille[x-1][y+1]==MASQUE_AVEC_BOMBES)||(grille[x-1][y+1]==DRAPEAU_AVEC_BOMBES)||(grille[x-1][y+1]==BOMBE)))
		nb_bombes_autour++;
	if((coor_valide(x, y-1))&&((grille[x][y-1]==MASQUE_AVEC_BOMBES)||(grille[x][y-1]==DRAPEAU_AVEC_BOMBES)||(grille[x][y-1]==BOMBE)))
		nb_bombes_autour++;
	if((coor_valide(x, y+1))&&((grille[x][y+1]==MASQUE_AVEC_BOMBES)||(grille[x][y+1]==DRAPEAU_AVEC_BOMBES)||(grille[x][y+1]==BOMBE)))
		nb_bombes_autour++;
	if((coor_valide(x+1, y-1))&&((grille[x+1][y-1]==MASQUE_AVEC_BOMBES)||(grille[x+1][y-1]==DRAPEAU_AVEC_BOMBES)||(grille[x+1][y-1]==BOMBE)))
		nb_bombes_autour++;
	if((coor_valide(x+1, y))&&((grille[x+1][y]==MASQUE_AVEC_BOMBES)||(grille[x+1][y]==DRAPEAU_AVEC_BOMBES)||(grille[x+1][y]==BOMBE)))
		nb_bombes_autour++;
	if((coor_valide(x+1, y+1))&&((grille[x+1][y+1]==MASQUE_AVEC_BOMBES)||(grille[x+1][y+1]==DRAPEAU_AVEC_BOMBES)||(grille[x+1][y+1]==BOMBE)))
		nb_bombes_autour++;

	return nb_bombes_autour;
}

int danger_avec_libre_autour(int grille[TAILLE_GRILLE_LIGNE][TAILLE_GRILLE_COLONNE], int x, int y){

	if((coor_valide(x-1, y-1))&&(nb_bombes_autour(grille, x-1, y-1)==0))
		return 1;
	if((coor_valide(x-1, y))&&(nb_bombes_autour(grille, x-1, y)==0))
		return 1;
	if((coor_valide(x-1, y+1))&&(nb_bombes_autour(grille, x-1, y+1)==0))
		return 1;
	if((coor_valide(x, y-1))&&(nb_bombes_autour(grille, x, y-1)==0))
		return 1;
	if((coor_valide(x, y+1))&&(nb_bombes_autour(grille, x, y+1)==0))
		return 1;
	if((coor_valide(x+1, y-1))&&(nb_bombes_autour(grille, x+1, y-1)==0))
		return 1;
	if((coor_valide(x+1, y))&&(nb_bombes_autour(grille, x+1, y)==0))
		return 1;
	if((coor_valide(x+1, y+1))&&(nb_bombes_autour(grille, x+1, y+1)==0))
		return 1;

	else return 0;
}

void etat(int grille[TAILLE_GRILLE_LIGNE][TAILLE_GRILLE_COLONNE], int x, int y){
	if(coor_valide(x,y)){
		if((grille[x][y]==MASQUE)||(grille[x][y]==DRAPEAU_SANS_BOMBES))
		{
			if(nb_bombes_autour(grille, x, y)==0){
				grille[x][y]=LIBRE;
				etat(grille, x-1, y-1);
				etat(grille, x-1, y);
				etat(grille, x-1, y+1);
				etat(grille, x, y-1);
				etat(grille, x, y+1);
				etat(grille, x+1, y-1);
				etat(grille, x+1, y);
				etat(grille, x+1, y+1);
			}
			else if(danger_avec_libre_autour(grille, x, y)){
				grille[x][y]=DANGER_AVEC_LIBRE_AUTOUR;
				if(nb_bombes_autour(grille, x-1, y-1)==0)
					etat(grille, x-1, y-1);
				if(nb_bombes_autour(grille, x-1, y)==0)
					etat(grille, x-1, y);
				if(nb_bombes_autour(grille, x-1, y+1)==0)
					etat(grille, x-1, y+1);
				if(nb_bombes_autour(grille, x, y-1)==0)
					etat(grille, x, y-1);
				if(nb_bombes_autour(grille, x, y+1)==0)
					etat(grille, x, y+1);
				if(nb_bombes_autour(grille, x+1, y-1)==0)
					etat(grille, x+1, y-1);
				if(nb_bombes_autour(grille, x+1, y)==0)
					etat(grille, x+1, y);
				if(nb_bombes_autour(grille, x+1, y+1)==0)
					etat(grille, x+1, y+1);
			}
			else grille[x][y]=DANGER_SANS_LIBRE_AUTOUR;
		}
	}
}

void init_grille(int grille[TAILLE_GRILLE_LIGNE][TAILLE_GRILLE_COLONNE], int x, int y){

	int i, j;
	int bombes=NOMBRE_BOMBES_GRILLE;
	printf("nb bombes : %d", bombes);

	for(i=0;i<TAILLE_GRILLE_LIGNE;i++){
		for(j=0;j<TAILLE_GRILLE_COLONNE;j++){
				grille[i][j]=-1;
		}
	}

	grille[x][y]=MASQUE;

	if(coor_valide(x-1, y-1))
		grille[x-1][y-1]=MASQUE;
	if(coor_valide(x-1, y))
		grille[x-1][y]=MASQUE;
	if(coor_valide(x-1, y+1))
		grille[x-1][y+1]=MASQUE;
	if(coor_valide(x, y-1))
		grille[x][y-1]=MASQUE;
	if(coor_valide(x, y+1))
		grille[x][y+1]=MASQUE;
	if(coor_valide(x+1, y-1))
		grille[x+1][y-1]=MASQUE;
	if(coor_valide(x+1, y))
		grille[x+1][y]=MASQUE;
	if(coor_valide(x+1, y+1))
		grille[x+1][y+1]=MASQUE;

	while(bombes>0){
		i=rand()%TAILLE_GRILLE_LIGNE;
		j=rand()%TAILLE_GRILLE_COLONNE;
		if((grille[i][j]!=MASQUE_AVEC_BOMBES)&&(grille[i][j]!=MASQUE)){
			grille[i][j]=MASQUE_AVEC_BOMBES;
			--bombes;
		}
	}

	for(i=0;i<TAILLE_GRILLE_LIGNE;i++){
		for(j=0;j<TAILLE_GRILLE_COLONNE;j++){
			if(grille[i][j]!=MASQUE_AVEC_BOMBES){
				grille[i][j]=MASQUE;
			}
		}
	}
	etat(grille, x, y);
}

int fin_jeu(int grille[TAILLE_GRILLE_LIGNE][TAILLE_GRILLE_COLONNE]){
	int i, j;
	for(i=0; i<TAILLE_GRILLE_LIGNE; i++){
		for(j=0; j<TAILLE_GRILLE_COLONNE; j++){
			if((grille[i][j]==MASQUE)||(grille[i][j]==DRAPEAU_SANS_BOMBES))
				return 0;
		}
	}
	return 1;
}

void afficher_texte(SDL_Renderer * renderer, char *message, TTF_Font * police, int x, int y)
{

	SDL_Color couleur={255, 255, 255, 255};

	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(police, message, couleur);
	SDL_Texture* textureMessage = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	SDL_Rect dest={x,y,0,0};
	SDL_QueryTexture(textureMessage,NULL,(int*)SDL_TEXTUREACCESS_STATIC,&(dest.w), &(dest.h) );

	SDL_RenderCopy(renderer, textureMessage, NULL, &dest);

	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(textureMessage);


}

void afficher_grille(SDL_Renderer *renderer, int grille[TAILLE_GRILLE_LIGNE][TAILLE_GRILLE_COLONNE], int premier_click, int click_bombe, SDL_Texture * texture){

	SDL_Rect case_dem={50,50,TAILLE_CASE,TAILLE_CASE};
	//SDL_SetRenderDrawColor(renderer,0,0,0,255);

	SDL_Rect src={0,0,54,54};

	if(premier_click==0){
		for(int i=0;i<TAILLE_GRILLE_LIGNE;i++){
			case_dem.x=50;
			for(int j=0;j<TAILLE_GRILLE_COLONNE;j++){
				src.x=54*3; src.y=54;
				SDL_RenderCopy(renderer, texture, &src, &case_dem);
					case_dem.x+=TAILLE_CASE;
			}
			case_dem.y+=TAILLE_CASE;
		}
	}
	else{
		for(int i=0;i<TAILLE_GRILLE_LIGNE;i++){
			case_dem.x=50;
			for(int j=0;j<TAILLE_GRILLE_COLONNE;j++){
				if(grille[i][j]==MASQUE_AVEC_BOMBES){
					src.x=54*3; src.y=54;
					SDL_RenderCopy(renderer, texture, &src, &case_dem);
				}
				else if(grille[i][j]==BOMBE){
					src.x=54*5; src.y=54;
					SDL_RenderCopy(renderer, texture, &src, &case_dem);
				}
				else if(grille[i][j] == LIBRE)
				{
					src.x=0; src.y=0;
					SDL_RenderCopy(renderer, texture, &src, &case_dem);

				}
				else if(grille[i][j] == DANGER_AVEC_LIBRE_AUTOUR)
				{
					if(nb_bombes_autour(grille, i, j)==1){
						src.x=54*3; src.y=0;
						SDL_RenderCopy(renderer, texture, &src, &case_dem);
					}
					else if(nb_bombes_autour(grille, i, j)==2){
						src.x=54*4; src.y=0;
						SDL_RenderCopy(renderer, texture, &src, &case_dem);
					}
					else if(nb_bombes_autour(grille, i, j)==3){
						src.x=54; src.y=54;
						SDL_RenderCopy(renderer, texture, &src, &case_dem);
					}
					else if(nb_bombes_autour(grille, i, j)==4){
						src.x=54*2; src.y=54;
						SDL_RenderCopy(renderer, texture, &src, &case_dem);
					}
					else if(nb_bombes_autour(grille, i, j)==5){
						src.x=54*4; src.y=54;
						SDL_RenderCopy(renderer, texture, &src, &case_dem);
					}
					else if(nb_bombes_autour(grille, i, j)==6){
						src.x=54*4; src.y=54*3;
						SDL_RenderCopy(renderer, texture, &src, &case_dem);
					}
					else if(nb_bombes_autour(grille, i, j)==7){
						src.x=54*3; src.y=54*3;
						SDL_RenderCopy(renderer, texture, &src, &case_dem);
					}
					else if(nb_bombes_autour(grille, i, j)==8){
						src.x=54; src.y=54*3;
						SDL_RenderCopy(renderer, texture, &src, &case_dem);
					}

				}
				else if(grille[i][j] == DANGER_SANS_LIBRE_AUTOUR)
				{
					if(nb_bombes_autour(grille, i, j)==1){
						src.x=54*3; src.y=0;
						SDL_RenderCopy(renderer, texture, &src, &case_dem);
					}
					else if(nb_bombes_autour(grille, i, j)==2){
						src.x=54*4; src.y=0;
						SDL_RenderCopy(renderer, texture, &src, &case_dem);
					}
					else if(nb_bombes_autour(grille, i, j)==3){
						src.x=54; src.y=54;
						SDL_RenderCopy(renderer, texture, &src, &case_dem);
					}
					else if(nb_bombes_autour(grille, i, j)==4){
						src.x=54*2; src.y=54;
						SDL_RenderCopy(renderer, texture, &src, &case_dem);
					}
					else if(nb_bombes_autour(grille, i, j)==5){
						src.x=54*4; src.y=54;
						SDL_RenderCopy(renderer, texture, &src, &case_dem);
					}
					else if(nb_bombes_autour(grille, i, j)==6){
						src.x=54*4; src.y=54*3;
						SDL_RenderCopy(renderer, texture, &src, &case_dem);
					}
					else if(nb_bombes_autour(grille, i, j)==7){
						src.x=54*3; src.y=54*3;
						SDL_RenderCopy(renderer, texture, &src, &case_dem);
					}
					else if(nb_bombes_autour(grille, i, j)==8){
						src.x=54; src.y=54*3;
						SDL_RenderCopy(renderer, texture, &src, &case_dem);
					}

				}
				else if((grille[i][j]==DRAPEAU_AVEC_BOMBES)&&(click_bombe==0))
				{
					src.x=0; src.y=54*2;
					SDL_RenderCopy(renderer, texture, &src, &case_dem);
				}
				else if((grille[i][j]==DRAPEAU_AVEC_BOMBES)&&(click_bombe==1))
				{
					src.x=54*5; src.y=54;
					SDL_RenderCopy(renderer, texture, &src, &case_dem);
				}
				else if(grille[i][j]==DRAPEAU_SANS_BOMBES){
					src.x=0; src.y=54*2;
					SDL_RenderCopy(renderer, texture, &src, &case_dem);
				}
				else if(grille[i][j] == MASQUE){
					src.x=54*3; src.y=54;
					SDL_RenderCopy(renderer, texture, &src, &case_dem);
				}
				case_dem.x+=TAILLE_CASE;
			}
			case_dem.y+=TAILLE_CASE;
		}
	}
}

int demineur(SDL_Renderer *renderer, int score, int WinWidht, int WinHeight, char *token, int hardcore){
/////////////////////
/// MISE EN PLACE ///``
/////////////////////
	myInit();

	////////////
	/// Vars ///`
	////////////
	int i,j;
	char message[30];
	float ratioWindowSize = 1;
	SDL_Rect displayBounds;
	SDL_GetDisplayBounds(0, &displayBounds);
	SDL_Point maxWindowSize = maximizeWindow(displayBounds, &ratioWindowSize);
	int clique_gauche = SDL_FALSE;
	int relache_gauche = SDL_FALSE;
	int clique_droit = SDL_FALSE;
	int relache_droit = SDL_FALSE;
	int click_bombe = 0;
	int premier_click=0;
	TTF_Font * police=NULL;
	int temps_final=0;
	police = TTF_OpenFont("../assets/font/police.ttf", 40);
	float ratio_fen=(float)maxWindowSize.x/(PLAYGROUND_SIZE_W + 2 * HUD_W);
	printf("ratio_fen -->%d %d %d %f\n",maxWindowSize.x, HUD_W, (PLAYGROUND_SIZE_W + 2 * HUD_W), ratio_fen);
	//Keyboard
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	int temps_jeu=0;
	int tempo;
	int minutes, secondes;
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


/*
	SDL_Window *myWindow = SDL_CreateWindow("Démineur", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, maxWindowSize.x ,maxWindowSize.y, WINDOW_FLAG);
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
*/

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
	SDL_Texture *hudTexture = IMG_LoadTexture(renderer, "../games/2_snake/Textures/hud.png");
	if( hudTexture == NULL ){
		printf("Erreur lors de la creation de texture");
		return EXIT_FAILURE;
	}

	SDL_Texture * texture;
	texture=IMG_LoadTexture(renderer, "../games/6_demineur/demineur.png");

	int grille[TAILLE_GRILLE_LIGNE][TAILLE_GRILLE_COLONNE];



	///////////////////////
	/// Initialize vars ///`
	///////////////////////


/////////////////////
/// BOUCLE DU JEU ///``
/////////////////////

	while( premier_click==0 ){

		// Init input
		SDL_GetMouseState(&(mouseCoor.x), &(mouseCoor.y));
		relache_gauche = SDL_FALSE;
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
				case SDL_KEYDOWN:
				{
					if(event.key.keysym.sym == SDLK_ESCAPE)
					{
						// fermer
						return 0;
					}
				}
				case SDL_MOUSEBUTTONDOWN:
					if(event.button.button == SDL_BUTTON_LEFT){
						clique_gauche = SDL_TRUE;
					}break;

				case SDL_MOUSEBUTTONUP:
					if((event.button.button == SDL_BUTTON_LEFT)&&(clique_gauche==SDL_TRUE)){
						relache_gauche=SDL_TRUE;
						premier_click=1;
					}break;
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

	if(relache_gauche){
		mouseCoor.x-=ratio_fen*HUD_W;
		mouseCoor.y-=ratio_fen*HUD_H;
		SDL_Point case_coor = {(int)(mouseCoor.x/(TAILLE_CASE*ratio_fen)), (int)(mouseCoor.y/(TAILLE_CASE*ratio_fen))};
		SDL_Rect case_dem={case_coor.x*TAILLE_CASE,case_coor.y*TAILLE_CASE,TAILLE_CASE,TAILLE_CASE};
		SDL_SetRenderDrawColor(renderer,255,0,0,255);
		SDL_RenderFillRect(renderer, &case_dem);
		case_coor.y--;case_coor.x--;
		init_grille(grille, case_coor.y, case_coor.x);
		etat(grille, case_coor.y, case_coor.x);
	}

		afficher_texte(renderer, "Temps jeu", police, 1350, 50);
		afficher_texte(renderer, "0 sec", police, 1400, 100);
		afficher_grille(renderer,grille, premier_click, click_bombe, texture);

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

	tempo=SDL_GetTicks();

	while((!fin_jeu(grille))&&(click_bombe==0))
	{

		// Init input
		SDL_GetMouseState(&(mouseCoor.x), &(mouseCoor.y));
		relache_gauche = SDL_FALSE;
		relache_droit = SDL_FALSE;
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
				case SDL_KEYDOWN:
				{
					if(event.key.keysym.sym == SDLK_ESCAPE)
					{
						// fermer
						return 0;
					}
				}

				case SDL_MOUSEBUTTONDOWN:
					if(event.button.button == SDL_BUTTON_LEFT){
						clique_gauche = SDL_TRUE;
					}
					else if(event.button.button==SDL_BUTTON_RIGHT){
						clique_droit= SDL_TRUE;
					}break;

				case SDL_MOUSEBUTTONUP:
					if((event.button.button == SDL_BUTTON_LEFT)&&(clique_gauche==SDL_TRUE)){
						relache_gauche=SDL_TRUE;
						//premier_click=1;
					}
					else if((event.button.button == SDL_BUTTON_RIGHT)&&(clique_droit==SDL_TRUE)){
						relache_droit=SDL_TRUE;
					}break;
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

	if(relache_gauche){
		mouseCoor.x-=ratio_fen*HUD_W;
		mouseCoor.y-=ratio_fen*HUD_H;
		SDL_Point case_coor = {(int)(mouseCoor.x/(TAILLE_CASE*ratio_fen)), (int)(mouseCoor.y/(TAILLE_CASE*ratio_fen))};
		SDL_Rect case_dem={case_coor.x*TAILLE_CASE,case_coor.y*TAILLE_CASE,TAILLE_CASE,TAILLE_CASE};
		SDL_SetRenderDrawColor(renderer,255,0,0,255);
		SDL_RenderFillRect(renderer, &case_dem);
		case_coor.x--;
		case_coor.y--;
		if((grille[case_coor.y][case_coor.x] ==MASQUE_AVEC_BOMBES)||(grille[case_coor.y][case_coor.x]==DRAPEAU_AVEC_BOMBES)){
			for(i=0;i<TAILLE_GRILLE_LIGNE;i++){
				for(j=0;j<TAILLE_GRILLE_COLONNE;j++){
					if(grille[i][j]==MASQUE_AVEC_BOMBES)
						grille[i][j]=BOMBE;
				}
			}
			click_bombe=1;
		}
		else{
			etat(grille, case_coor.y, case_coor.x);
		}
	}
	if(relache_droit){
		mouseCoor.x-=ratio_fen*HUD_W;
		mouseCoor.y-=ratio_fen*HUD_H;
		SDL_Point case_coor = {(int)(mouseCoor.x/(TAILLE_CASE*ratio_fen)), (int)(mouseCoor.y/(TAILLE_CASE*ratio_fen))};
		SDL_Rect case_dem={case_coor.x*TAILLE_CASE,case_coor.y*TAILLE_CASE,TAILLE_CASE,TAILLE_CASE};
		SDL_SetRenderDrawColor(renderer,255,0,0,255);
		SDL_RenderFillRect(renderer, &case_dem);
		case_coor.x--;
		case_coor.y--;
		if(grille[case_coor.y][case_coor.x]==MASQUE){
			grille[case_coor.y][case_coor.x]=DRAPEAU_SANS_BOMBES;
		}
		else if (grille[case_coor.y][case_coor.x]==MASQUE_AVEC_BOMBES){
			grille[case_coor.y][case_coor.x]=DRAPEAU_AVEC_BOMBES;
		}
		else if (grille[case_coor.y][case_coor.x]==DRAPEAU_AVEC_BOMBES){
			grille[case_coor.y][case_coor.x]=MASQUE_AVEC_BOMBES;
		}
		else if(grille[case_coor.y][case_coor.x]==DRAPEAU_SANS_BOMBES){
			grille[case_coor.y][case_coor.x]=MASQUE;
		}

	}



		temps_jeu=(SDL_GetTicks()-tempo)/1000;
		afficher_texte(renderer, "Temps jeu", police, 1350, 50);

		if(temps_jeu<60)
		{
			sprintf(message, "%d sec", temps_jeu);
			afficher_texte(renderer, message, police, 1400, 100);
		}

		else if(temps_jeu>=60)
		{
			minutes=temps_jeu/60;
			secondes=temps_jeu%60;
			sprintf(message, "%d min %d sec", minutes, secondes);
			afficher_texte(renderer, message, police, 1350, 100);
		}

		afficher_grille(renderer,grille, premier_click, click_bombe, texture);
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

	temps_final=temps_jeu;

	while(1)
	{
		SDL_Event event;
		while( SDL_PollEvent(&event) ){
			switch( event.type ){
				case SDL_QUIT:
					// fermer
					return 0;
					break;
				case SDL_KEYDOWN:
				{
					if(event.key.keysym.sym == SDLK_ESCAPE)
					{
						// fermer
						return 0;
					}
				}

			}
		}


	////////////////////////////
	// Handle Keyboard inputs //`
	////////////////////////////
		SDL_PumpEvents();

		if(fin_jeu(grille)){
			afficher_grille(renderer,grille, premier_click, click_bombe, texture);
			afficher_texte(renderer, "Temps jeu", police, 1350, 50);
			if(temps_final<60)
			{
				sprintf(message, "%d sec", temps_final);
				afficher_texte(renderer, message, police, 1350, 100);
			}

			else if(temps_final>=60)
			{
				minutes=temps_final/60;
				secondes=temps_final%60;
				sprintf(message, "%d min %d sec", minutes, secondes);
				afficher_texte(renderer, message, police, 1350, 100);
			}
			afficher_texte(renderer, "Bravo", police, 1350, 150);
		}

		else if(click_bombe==1)
		{
			afficher_grille(renderer,grille, premier_click, click_bombe, texture);
			afficher_texte(renderer, "Tu as perdu...", police, 1350, 50);
		}

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
//	TTF_CloseFont(police);
////	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(hudTexture);
	SDL_DestroyTexture(texture);
	//SDL_DestroyWindow(myWindow);
//	SDL_Quit();
	return 0;
}
