#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "../define/define.h"

#define TAILLE_L 19
#define TAILLE_H 9

#define SOL 0
#define PERSONNAGE -1
#define MACHINE 1
#define MUR -2

int salon[19][9];

void initialisation_salon(){
	//Mettre les murs
	for(int i=0 ; i<TAILLE_H ; i++){
		for(int j=0 ; j<TAILLE_L ; j++){
			if(j==0||j==TAILLE_L-1 || i==0 || i== TAILLE_H-1)
				salon[i][j]=MUR;
			else
				salon[i][j]=SOL;
		}
	}

	//Mettre les machines

}

int apply_renderer_texture_rect(SDL_Renderer* renderer , SDL_Texture * texture, SDL_Rect rect){


		if(SDL_RenderCopy(renderer, texture, NULL, &rect))
		{
			printf("Error () : SDL_RenderCopy failed %s\n", IMG_GetError());
			return FALSE;
		} else {


			return TRUE;

		}


}

SDL_Texture *load_texture_jpg(SDL_Renderer* renderer, char directory[],SDL_Rect rect){

	//Load texture from jpg
	int flags=IMG_INIT_JPG;
	int initted=IMG_Init(flags);

/*	if(initted&flags != flags){
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
		apply_renderer_texture_rect(renderer, texture,rect);
		return texture;

	}


}

SDL_Texture *load_texture_png_(SDL_Renderer* renderer, char directory[],SDL_Rect rect){

	//Load texture from png
/*	int flags=IMG_INIT_PNG;
	int initted=IMG_Init(flags);

	if(initted&flags != flags) {
		printf("IMG_Init: Failed to init required png support!\n");
		printf("IMG_Init: %s\n", IMG_GetError());
		return NULL;
	} */

	SDL_Texture * texture = IMG_LoadTexture(renderer, directory);
	if(!texture)
	{
		printf("texture load failed: %s\n", IMG_GetError());
		return NULL;
	} else {
		apply_renderer_texture_rect(renderer, texture,rect);
		return texture;

	}


}

void print_room(SDL_Renderer* renderer){

	SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer,0,0,0,255);

	SDL_Rect rect = {0,0,1920/ TAILLE_L,1080 / TAILLE_H};

	for(int i= 0 ; i<TAILLE_L ; i++){



		for(int j=0 ; j<TAILLE_H ; j++){

			if (i ==0 && j == 0 )
				load_texture_png_(renderer,"../assets/texture/mur_h_g.png",rect);
			else if(i == 0 && j == TAILLE_H - 1)
				load_texture_png_(renderer,"../assets/texture/mur_b_g.png",rect);
			else if ( i == 0 )
				load_texture_png_(renderer,"../assets/texture/mur_g.png",rect);
			else if ( j == 0 && i == TAILLE_L - 1 )
				load_texture_png_(renderer,"../assets/texture/mur_h_d.png",rect);
			else if ( i == TAILLE_L -1 && j == TAILLE_H -1 )
				load_texture_png_(renderer,"../assets/texture/mur_b_d.png",rect);
			else if ( j == 0 )
				load_texture_png_(renderer,"../assets/texture/mur_h.png",rect);
			else if ( i == TAILLE_L - 1 )
				load_texture_png_(renderer,"../assets/texture/mur_d.png",rect);
			else if ( j == TAILLE_H - 1 )
				load_texture_png_(renderer,"../assets/texture/mur_b.png",rect);
			else
				load_texture_png_(renderer,"../assets/texture/sol.png",rect);

			if ( salon[i][j] == -1)
				load_texture_png_(renderer,"../assets/texture/personnage.png",rect);
			if( salon[i][j] >= 1)
				load_texture_png_(renderer,"../assets/texture/borne_arcade.png",rect);
			rect.y += rect.h;

		}

		rect.x += rect.w;
		rect.y = 0;

	}

	SDL_RenderPresent(renderer);

}

int appartient_a(int x, int y){

	return (x >= 0 && x < TAILLE_L && y >= 0 && y < TAILLE_H);
}

int deplacer_joueur(int p_x, int p_y,int x, int y){



	if (appartient_a(p_x+x,p_y+y)) {

		if ( salon[p_x+x][p_y+y] < 1 && salon[p_x+x][p_y+y] != -2 ){

			printf("\ndeplacement sur %d,%d\n",p_x+x,p_y+y);
			//salon[p_x][p_y]=0;
			//salon[p_x + x][p_y + y]=-1;
			return 1;
		} else {

			return 0;

		}


	} else {
		return 0;
	}


}

SDL_Rect print_animate(SDL_Renderer *renderer){

	SDL_Rect retour;

	//SDL_RenderClear(renderer);

	SDL_SetRenderDrawColor(renderer,0,0,0,255);

	SDL_Rect rect = {0,0,1920/ TAILLE_L,1080 / TAILLE_H};

	for(int i= 0 ; i<TAILLE_L ; i++){



		for(int j=0 ; j<TAILLE_H ; j++){

			if (i ==0 && j == 0 )
				load_texture_png_(renderer,"../assets/texture/mur_h_g.png",rect);
			else if(i == 0 && j == TAILLE_H - 1)
				load_texture_png_(renderer,"../assets/texture/mur_b_g.png",rect);
			else if ( i == 0 )
				load_texture_png_(renderer,"../assets/texture/mur_g.png",rect);
			else if ( j == 0 && i == TAILLE_L - 1 )
				load_texture_png_(renderer,"../assets/texture/mur_h_d.png",rect);
			else if ( i == TAILLE_L -1 && j == TAILLE_H -1 )
				load_texture_png_(renderer,"../assets/texture/mur_b_d.png",rect);
			else if ( j == 0 )
				load_texture_png_(renderer,"../assets/texture/mur_h.png",rect);
			else if ( i == TAILLE_L - 1 )
				load_texture_png_(renderer,"../assets/texture/mur_d.png",rect);
			else if ( j == TAILLE_H - 1 )
				load_texture_png_(renderer,"../assets/texture/mur_b.png",rect);
			else
				load_texture_png_(renderer,"../assets/texture/sol.png",rect);
			if( salon[i][j] >= 1)
				load_texture_png_(renderer,"../assets/texture/borne_arcade.png",rect);
			if(salon[i][j] == -1 ){

				retour.x = rect.x;
				retour.y = rect.y;
				retour.w = rect.w;
				retour.h = rect.h;

			}

			rect.y += rect.h;




		}

		rect.x += rect.w;
		rect.y = 0;

	}

	return retour;
}



int fonction_principal(int px, int py,SDL_Renderer *renderer){

	int choice = 1;
	print_room(renderer);

	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	int continuer = TRUE;

	SDL_Event event;



	while(continuer){

		SDL_WaitEvent(&event);

		switch(event.type)
		{

			case SDL_KEYUP: {
				if ( event.key.keysym.sym == SDLK_ESCAPE )
				{
					continuer = FALSE;
					printf("CANCEL\n");
					choice = -1;
				}

				if ( event.key.keysym.sym == SDLK_LEFT )
				{
					if ( deplacer_joueur(px,py,-1,0) ){
					//	animate_deplacement(renderer,1,50);
						salon[px][py]=SOL;
						px += -1;
						salon[px][py]=PERSONNAGE;
					}
				}

				if ( event.key.keysym.sym == SDLK_RIGHT )
				{
					if ( deplacer_joueur(px,py,1,0) ){
				//		animate_deplacement(renderer,2,50);
						salon[px][py]=SOL;
						px += 1;
						salon[px][py]=PERSONNAGE;
					}
				}

				if ( event.key.keysym.sym == SDLK_UP )
				{
					if ( deplacer_joueur(px,py,0,-1) ){
					//	animate_deplacement(renderer,3,50);
						salon[px][py]=SOL;
						py += -1;
						salon[px][py]=PERSONNAGE;
					}
				}

				if ( event.key.keysym.sym == SDLK_DOWN )
				{
					if ( deplacer_joueur(px,py,0,1) ){
					//	animate_deplacement(renderer,4,50);
						salon[px][py]=SOL;
						py += 1;
						salon[px][py]=PERSONNAGE;
					}
				}

				if ( event.key.keysym.sym == SDLK_SPACE )
				{
					if(salon[px][py-1] >= 1){
						continuer = FALSE;
						choice = salon[px][py-1];
					}
				}

			};break;



		}

	}
	SDL_RenderClear(renderer);



	return choice;

}





int room(SDL_Window *window, SDL_Renderer* renderer,char * identifiant,char * token){


	// variable init
	int px =0;
	int py=1;


	initialisation_salon();

	salon[0][1]=PERSONNAGE;
	salon[3][5]=MACHINE;
	salon[11][7]=MACHINE;
	// end init //



	return fonction_principal(px,py,renderer);

}

int main()
{
	SDL_Window *window = SDL_CreateWindow("Nineteen | Launcher", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1920,1080,SDL_WINDOW_SHOWN );
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	TTF_Init();
	room(window, renderer,"1","2");
}
