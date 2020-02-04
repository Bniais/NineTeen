#include "launcher.h"
#include  <string.h>

#define NB_FILE 11

char file[NB_FILE][10];

void init_file(){

	strcpy(file[0],"assets/f1");
	strcpy(file[1],"assets/f2");
	strcpy(file[2],"assets/f3");
	strcpy(file[3],"assets/f4");
	strcpy(file[4],"assets/f5");	
	strcpy(file[5],"assets/f6");
	strcpy(file[6],"assets/f7");
	strcpy(file[7],"assets/f8");
	strcpy(file[8],"assets/f9");
	strcpy(file[9],"assets/f10");

}

int apply_renderer_texture(SDL_Renderer* renderer , SDL_Texture * texture){
	
		
		if(SDL_RenderCopy(renderer, texture, NULL, NULL))
		{
			printf("Error () : SDL_RenderCopy failed %s\n", IMG_GetError());
			return FALSE;
		} else {
			
			SDL_RenderPresent(renderer);
			return TRUE;
			
		}

}

SDL_Texture *load_texture_png(SDL_Renderer* renderer, char directory[]){
	
	//Load texture from png
/*	int flags=IMG_INIT_PNG;
	int initted=IMG_Init(flags);
	
	if(initted&flags != flags) {
		printf("IMG_Init: Failed to init required png support!\n");
		printf("IMG_Init: %s\n", IMG_GetError());
		return NULL;
	}*/

	SDL_Texture * texture = IMG_LoadTexture(renderer, directory);
	if(!texture)
	{
		printf("texture load failed: %s\n", IMG_GetError());
		return NULL;
	} else {
		
		apply_renderer_texture(renderer, texture);
		return texture;
		
	}
	
	
}

int verify_file_existe(char name[]){
	int retour = FALSE;

	FILE *fichier = fopen(name,"rb");

	if (fichier){

		retour = TRUE;
		fclose(fichier);

	} else {
		printf("ERROR (%d) : Le fichier %s est introuvable \n",FILE_NOT_FOUND,name );
	}

	return retour;
}

SDL_Renderer* init(SDL_Window* window, SDL_Renderer* renderer){

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);		

	SDL_SetRenderDrawColor(renderer,255,255,255,255);
	SDL_RenderClear(renderer);

	SDL_Texture *texture = load_texture_png(renderer, "assets/image/launcher.png");



	SDL_Rect bg_chargement = {30,LAUNCHER_HEIGHT_SIZE-50,LAUNCHER_WIDTH_SIZE-60,30};
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Couleur fond
	SDL_RenderFillRect(renderer, &bg_chargement);

	SDL_RenderPresent(renderer);

	return renderer;
}

void pauseClavier(Uint32 tempsMax)//-1=infini
{
 SDL_Event ev;
 //on attend un event et si cet event est l'appuie d'une touche, on sort de la boucle.
 //si tempsMax est dépassé, on sort aussi de la boucle (WaitEventTimeout reverra 0 == faux pour une condition  )
 while(SDL_WaitEventTimeout(&ev,tempsMax)&&ev.type!=SDL_KEYDOWN);
}

SDL_Texture* print_text(SDL_Renderer *renderer, TTF_Font *police,SDL_Rect position, SDL_Color color, char text[]){

	
	SDL_Surface *pSurf= TTF_RenderText_Blended(police, text, color); // valeur du texte 
	
		
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, pSurf);	
	SDL_RenderCopy(renderer, texture, NULL, &position); // rendu texte

	

	return texture;

}

int check_(SDL_Renderer *renderer,SDL_Window *window, TTF_Font *police){

	int i;
	int value = TRUE;

	
	//SDL_Color black = {255,255,255};

	//SDL_Rect position_file = {240,LAUNCHER_HEIGHT_SIZE-70,15,20};	
	SDL_Rect chargement = {35, LAUNCHER_HEIGHT_SIZE-45 , LAUNCHER_WIDTH_SIZE-70 ,20};


	for(i = 0 ; i <= NB_FILE && value; i++){

		// verifier file exist
		value = verify_file_existe("assets/image/launcher.png");


		if (value) {
			
			chargement.w= ( (LAUNCHER_WIDTH_SIZE-70)/NB_FILE ) * i;

			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100); // Couleur fond
			SDL_RenderFillRect(renderer, &chargement);

			//position_file.w = 15;
			//position_file.w *= strlen(file[i]);

			//SDL_Texture * text = print_text(renderer,police,position_file,black,file[i]);

			//SDL_DestroyTexture(text);

			SDL_RenderPresent(renderer);

			SDL_Delay(50);
			

		}
		
		

	}

	
	return (--i == NB_FILE);
	
}

void connexion_view(SDL_Renderer *renderer, TTF_Font *police){

SDL_Color black = {255,255,255};

						SDL_Texture *texture = load_texture_png(renderer, "assets/image/launcher_no_font.png");
						
						

						SDL_Rect position = {40,90,LAUNCHER_WIDTH_SIZE-80,LAUNCHER_HEIGHT_SIZE-180};
						
						SDL_SetRenderDrawColor(renderer, 0,0,0,100);
						SDL_RenderFillRect(renderer,&position);


						position.x = 20;
						position.y = 20;
						position.w = 200;
						position.h = 40;

						print_text(renderer,police,position,black,"Connexion");


						position.x = 60;
						position.y = 110;
						position.w += 120;
						print_text(renderer,police,position,black,"Nom d'utilisateur");

						position.y += 60;
						position.w = LAUNCHER_WIDTH_SIZE-120;
						SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100); // Couleur fond
						SDL_RenderFillRect(renderer, &position);

						position.x = 60;
						position.y += 70;
						position.w = 200;
						print_text(renderer,police,position,black,"Mot de passe");

						position.y += 60;
						position.w = LAUNCHER_WIDTH_SIZE-120;
						SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100); // Couleur fond
						SDL_RenderFillRect(renderer, &position);

						
						position.y += 70;
						position.x = LAUNCHER_HEIGHT_SIZE - 100;
						position.w = 200;
						position.h = 40;
						SDL_SetRenderDrawColor(renderer, 50, 145, 255, 100); // Couleur fond
						SDL_RenderFillRect(renderer, &position);
						print_text(renderer,police,position,black,"Se connecter");
						
						SDL_RenderPresent(renderer);

}


int launcher(SDL_Renderer* renderer ,char *identifiant, char *token){


	init_file();

	int retour = FALSE;
	
	TTF_Font *police = TTF_OpenFont("assets/font/magma.ttf",11);

	SDL_Window* window = SDL_CreateWindow("Nineteen | Launcher", SDL_WINDOWPOS_CENTERED,
								 SDL_WINDOWPOS_CENTERED,
								 LAUNCHER_WIDTH_SIZE,
								 LAUNCHER_HEIGHT_SIZE,
								 SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE );
	
	
	if ( verify_file_existe("assets/image/launcher.png") ){
		
		// cree une fenetre d'affichage
		

		if( window ){

				// preset init view //
				renderer = init(window,renderer);

				SDL_Color black = {255,255,255};
				SDL_Rect position_text = {35,LAUNCHER_HEIGHT_SIZE-70,200,20};
				print_text(renderer,police,position_text,black,"Chargement :");
				
				// end init view //

				if ( check_(renderer,window,police) ){

					// blackscreen
					SDL_RenderClear(renderer);
					

					// page de connexion

					char username[16];
					char password[128];


					FILE *user =fopen("config/.user","rb");
					if (user){
						SDL_Texture *texture = load_texture_png(renderer, "assets/image/launcher.png");
						SDL_Texture * text = print_text(renderer,police,position_text,black,"Connexion en cours");
						SDL_RenderPresent(renderer);
						fscanf(user,"username = %s\n",username);
						fscanf(user,"password = %s\n",password);

						// connexion en cours
						SDL_Delay(2000);
						retour = TRUE;

						// if connexion success end of launcher
						
						// else connexion fonction

					} else {

						connexion_view(renderer,police);
						// tentative de connexion
						SDL_Delay(10000);
						retour = TRUE;
						// cree le fichier config/.user 
						// avec les infos user/password
						

					}

				}
				
				
				SDL_DestroyRenderer(renderer);

				retour = TRUE;

		} else {
			
			printf("Error (%d) : Erreur SDL_CreateWindow : %s",SDL_CREATE_WINDOW_ECHEC, SDL_GetError() );
			
		}
		
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	return retour;
}
