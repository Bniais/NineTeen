/////////////////////////////////////////////////
///	\file demineur.c
///	\author David L.
///	\version 1.0
///	\date 31 janvier 2020
///	\brief Programme permettant de jouer au demineur
/////////////////////////////////////////////////

#include "../../define/define.h"
#include "config.h"
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

static const int FRAME_TIME = 1000 / FRAMES_PER_SECOND;


/*Fonction qui permet de vérifier que les coordonnées de la case rentrées en paramètre sont bien dans la grille de démineur*/
int coor_valide(int x, int y){
	if((x>=0)&&(x<TAILLE_GRILLE_LIGNE)&&(y>=0)&&(y<TAILLE_GRILLE_COLONNE))
		return 1;
	else return 0;
}

/*Fonction qui renvoi le nombre de bombes autour d'une case (les 8 cases qui l'entoure)*/
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
/*Fonction qui vérifie si la case donnée en paramètre possède au moins une case libre autour d'elle
Une case est dite libre si elle n'a aucune bombe autour d'elle (voir config.h)
Cette fonction servira dans le programme principal car, si le joueur clique sur une case qui ne possède aucune case libre autour d'elle, alors le démineur affichera cette case seule, sans chercher à démasquer les autres cases autour*/
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

/*Fonction récursive qui affiche les cases (ou non) de la grille en fonction des coordonnées où l'utilisateur a cliqué
C'est la fonction principale du jeu, celle qui détermine les cases de la grille à démasquer en fonction de leur type (si il y a une bombe autour etc...)*/
void etat(int grille[TAILLE_GRILLE_LIGNE][TAILLE_GRILLE_COLONNE], int x, int y){
	//Vérifie dans un premier temps si les coordonnées du clique sont valides
	if(coor_valide(x,y)){
		//cette fonction agit sur les cases masquées, ce que vérifie la condiion ci-dessous
		if((grille[x][y]==MASQUE)||(grille[x][y]==DRAPEAU_SANS_BOMBES))
		{
			/*si la case concernée n'a aucune bombe autour, elle est libre,
			lorsqu'un case est libre cela veut dire qu'on la dévoile et qu'on cherche à dévoiler les cases qui sont autour,
			d'où l'appel récursif */
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
				/*Si une case vérifie cette condition, alors elle a au moins une case libre et une bombe autour d'elle
				la fonction cherche la/les cases libres autour pour la/les dévoiler et ainsi de suite*/
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
			//Si la case concernée n'a aucune case libre autour d'elle, la fonction arrête de dévoiler des cases
			else grille[x][y]=DANGER_SANS_LIBRE_AUTOUR;
		}
	}
}
/*La fonction suivante sert à initiliser la grille à l'aide du premier click de l'utilisateur
En effet, lors d'une partie de démineur, la grille s'initialise en fonction des coordonnées du premier click de l'utilisateur
(Le premier click ne peut pas être sur une bombe, sinon la partie est perdue sans que l'utilisateur ait pu jouer)*/
void init_grille(int grille[TAILLE_GRILLE_LIGNE][TAILLE_GRILLE_COLONNE], int x, int y){

	int i, j;
	int bombes=NOMBRE_BOMBES_GRILLE;
	printf("nb bombes : %d", bombes);
	//Initialisation de toutes les cases à -1 afin qu'elles soient toutes neutres au départ
	for(i=0;i<TAILLE_GRILLE_LIGNE;i++){
		for(j=0;j<TAILLE_GRILLE_COLONNE;j++){
				grille[i][j]=-1;
		}
	}

	//On met des cases de type MASQUE à l'endroit du premier click, afin d'éviter que des bombes s'y glisse
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

	//La fonction place des bombes aléatoirement sur des cases de la grille
	//sauf sur les cases qui sont autour du premier click
	while(bombes>0){
		i=rand()%TAILLE_GRILLE_LIGNE;
		j=rand()%TAILLE_GRILLE_COLONNE;
		if((grille[i][j]!=MASQUE_AVEC_BOMBES)&&(grille[i][j]!=MASQUE)){
			grille[i][j]=MASQUE_AVEC_BOMBES;
			--bombes;
		}
	}
	//la fonction remplit les cases restantes
	for(i=0;i<TAILLE_GRILLE_LIGNE;i++){
		for(j=0;j<TAILLE_GRILLE_COLONNE;j++){
			if(grille[i][j]!=MASQUE_AVEC_BOMBES){
				grille[i][j]=MASQUE;
			}
		}
	}
	//on lance la fonction etat, qui découvre les premières cases, où l'utilisateur vient de cliquer
	etat(grille, x, y);
}

//Cette fonction renvoit 1 lorsque les seules cases masquées restantes sur la grille sont des bombes,
//l'utilisateur a alors gagné
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

/*Cette fonction sert à écrire du texte sur la fenêtre SDL aux coordonnées indiquées en paramètre,
avec le message rentré et la police choisie*/
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

/*Fonction assez longue mais néanmoins asses simple, affiche les cases en fonction de leur type
Les images mises sur les cases proviennent de la texture en paramètre, initialisé dans le main..
Cette fonction est appelée en boucle dans le programme principale*/
void afficher_grille(SDL_Renderer *renderer, int grille[TAILLE_GRILLE_LIGNE][TAILLE_GRILLE_COLONNE], int premier_click, int click_bombe, SDL_Texture * texture){

	SDL_Rect case_dem={50,50,TAILLE_CASE,TAILLE_CASE};

	SDL_Rect src={0,0,54,54};

	/*Avant le premier click, les cases de la grille n'ont aucun type,
	 on affiche juste une grille entière de cases masquées*/
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
//	myInit();

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


/////////////////////
/// BOUCLES DU JEU ///``
/////////////////////

//première boucle, qui se fermera lorsque l'utilisateur aura effectué son premier click
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

/*Lorsque l'utilisateur effectue un click,
un renderer de la taille d'une case est créé,
ce renderer se "colle" forcement à une case*/
	if(relache_gauche){
		mouseCoor.x-=ratio_fen*HUD_W;
		mouseCoor.y-=ratio_fen*HUD_H;
		SDL_Point case_coor = {(int)(mouseCoor.x/(TAILLE_CASE*ratio_fen)), (int)(mouseCoor.y/(TAILLE_CASE*ratio_fen))};
		SDL_Rect case_dem={case_coor.x*TAILLE_CASE,case_coor.y*TAILLE_CASE,TAILLE_CASE,TAILLE_CASE};
		case_coor.y--;case_coor.x--;
		/*Vérifie que l'utilisateur click bien dans la grille, sinon sans effet
		Le premier click lance l'initialisation de la grille et sort de cette boucle
		la partie est maintenant lancée*/
		if(coor_valide(case_coor.y, case_coor.x)){
			SDL_SetRenderDrawColor(renderer,255,0,0,255);
			SDL_RenderFillRect(renderer, &case_dem);
			init_grille(grille, case_coor.y, case_coor.x);
			etat(grille, case_coor.y, case_coor.x);
			premier_click=1;
		}
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

	//cette variable servira plus tard dans le programme pour connaître le temps de jeu réel (temps de jeu qui commencer après le premier click)
	tempo=SDL_GetTicks();

	//Boucle principale du jeu, se termine lorsque la partie est gagnée ou perdue
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

	/*comme la boucle précédente, un clique gauche regardera l'état de la case
	si c'est une bombe, la partie se termine*/
	if(relache_gauche){
		mouseCoor.x-=ratio_fen*HUD_W;
		mouseCoor.y-=ratio_fen*HUD_H;
		SDL_Point case_coor = {(int)(mouseCoor.x/(TAILLE_CASE*ratio_fen)), (int)(mouseCoor.y/(TAILLE_CASE*ratio_fen))};
		SDL_Rect case_dem={case_coor.x*TAILLE_CASE,case_coor.y*TAILLE_CASE,TAILLE_CASE,TAILLE_CASE};
		case_coor.x--;
		case_coor.y--;
		if(coor_valide(case_coor.y,case_coor.x)){
			SDL_SetRenderDrawColor(renderer,255,0,0,255);
			SDL_RenderFillRect(renderer, &case_dem);

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
	}
	//Un click droit permet de placer un drapeau sur une case (ou de l'enlever)
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


		/*Pour connaitre le temps effectif du jeu, on prend le temps depuis que la fenêtre SDL est ouverte
		et on soustrait le temps que l'utilisateur à pris pour son premier click*/
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

	//Dernière boucle du jeu, affiche un message en fonction de si l'utilisateur a gagné ou a perdu
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
