// include define //
//#include "define/define.h"
// end include define //

// library include //
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
// end library include //

// library SQL //
//#include <//>
		// je ne sais pas comment elle fonctionne encore
		// voir si il faut aussi passer en params des fonctions une variable pour garder la connexion a la base
// end library SQL //



// include launcher //
#include "launcher/launcher.h"
// end include launcher //


// include room //
#include "room/room.h"
// end include room //

// include des jeux //
#include "games/1_pacman/pacman.h"      // JEU 1
#include "games/2_snake/snake.h"       // JEU 2
#include "games/3_flappy_bird/flappy_bird.h"  // JEU 3
#include "games/4_space_invaders/space_invaders.h"  // JEU 3
#include "games/5_tetris/tetris.h"      // JEU 5
// end include jeu //


int main(void){

	SDL_Init( SDL_INIT_EVERYTHING );
	TTF_Init();

	// var

	SDL_Window * window;
	SDL_Renderer * renderer;

	char *identifiant;
	char *token;

	int choice;

	//launcher est une fonction blocante si pas d'utilisateur connecter
	if ( launcher(renderer, identifiant, token) ) {

		//SDL_SetWindowFullscreen(SDL_Window* window, Uint32      flags)

		window = SDL_CreateWindow("Nineteen | Launcher", SDL_WINDOWPOS_CENTERED,
									 SDL_WINDOWPOS_CENTERED,
									 1920,
									 1080,
									 SDL_WINDOW_SHOWN  );


		renderer = SDL_CreateRenderer(window, -1, 0);

		while ( (choice = room(window,renderer,identifiant,token)  ) != -1)
		{

			switch (choice){

				//case 1: launchSnake(window,renderer,identifiant,token);

			}

		}


	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
}



/* MAIN TEST BACKUP



int main(int argc, char *argv[])
{
	//initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING );

	//Creation fenêtre
	SDL_Window* myWindow=SDL_CreateWindow("Test", WINDOW_X, WINDOW_Y,	WINDOW_W, WINDOW_H, WINDOW_FLAG);



	SDL_Renderer* renderer = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); // Création du renderer
  if(renderer == NULL) // Gestion des erreurs
  {
    printf("Erreur lors de la creation d'un renderer : %s",SDL_GetError());
    return EXIT_FAILURE;
  }

	SDL_SetRenderDrawColor(renderer,255,255,255,255);
	SDL_RenderClear(renderer);


	// BEGIN COMMENT
	//Load texture from BMP
	SDL_Surface * image = SDL_LoadBMP("Textures_generales/testA.bmp");
	if(!image) {
	  printf("SDL_LoadBMP failed %s\n", SDL_GetError());
	  return -1;
	}

	SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, image);
	if(!texture)
	{
		printf("texture load failed: %s\n", IMG_GetError());
		return -1;
	}

	// END COMMENT

	//Load texture from png
	int flags=IMG_INIT_PNG;
	int initted=IMG_Init(flags);
	if(initted&flags != flags) {
		printf("IMG_Init: Failed to init required png support!\n");
		printf("IMG_Init: %s\n", IMG_GetError());
		return -1;
	}

	SDL_Texture * texture = IMG_LoadTexture(renderer, "Textures_generales/logo.png");
	if(!texture)
	{
		printf("texture load failed: %s\n", IMG_GetError());
		return -1;
	}



	//charger le launcher
	// params : fenetre SDL, id, token //
	// retourne vrai si pas d'erreur //
//	if (! launcher(WINDOW *window, char * id, char * token) ){
		// message d'erreur mettre tuer l'application
//	}


	// charger le salon
	// params : fenetre SDL
	// return 0 si erreur, retourne 1 a 19 pour le jeu charger parmis les 19 du salon //
	// fonction blocante //
int choice; //= room_load(WINDOW *window);
int done=0;
while (!done) {
    SDL_Event event;

		//Dessiner
		if(SDL_RenderCopy(renderer, texture, NULL, NULL))
		{
			printf("SDL_RenderCopy failed %s\n", IMG_GetError());
			return -1;
		}

    SDL_RenderPresent(renderer);
		SDL_Delay(200);


		//Events
    while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
				//fermer
				 	return 0;
					break;

				case SDL_MOUSEBUTTONDOWN:
				//gestion souris
					if(event.button.button == SDL_BUTTON_LEFT) //test
						return 0;
					break;

			}
    }
  }
}


*/
