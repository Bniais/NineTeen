#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "../include/textField.h"
#include "../include/private_image.h"

#include "../include/libWeb.h"

#define TRUE 1
#define FALSE 0

#define DIR_TOKEN_FILE ".token" // only for debug realy named ".token"


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

int dejaConneceter(char token[])
{
	FILE * fp;
    fp = fopen (DIR_TOKEN_FILE, "r");
	if (!fp)
	{
		return 0;
	}
	else
	{
		fscanf(fp,"%s",token);
		fclose(fp);

		if ( !connectWithKey(token) ){
			return 1;
		}
	}
	return 0;
}

int sauvegarderToken(char *token)
{
	FILE *fp;
	fp = fopen(DIR_TOKEN_FILE,"w");
	if (!fp)
	{
		return 0;
	}
	else
	{
		fprintf(fp, "%s\n",token );
		fclose(fp);
		return 1;
	}

}

void affichageForMac()
{
    int attendre = 5;
    do
    {
        SDL_Event ev;
        while ( SDL_PollEvent(&ev) )
        {
            attendre--;

        }
    } while (attendre);
}

void attendreEvenementAppuyer(int event)
{
    int attendre = 1;
    do
    {
        SDL_Event ev;
        while ( SDL_PollEvent(&ev) )
        {
            if (ev.type == SDL_KEYDOWN){

                if(ev.key.keysym.sym == event)
                {
                    attendre = 0;
                }

            }

        }
    } while (attendre);
}

void launcher(SDL_Renderer* renderer)
{
	char identifiant[32];
	char password[32];
	char *token;
	int i = 0;


	if ( !dejaConneceter(token) )
	{
		do{
			printf("\nIdentifiant : " );
			scanf("%s",identifiant );
			printf("\nMot de passe : " );
			scanf("%s",password );
			printf("\nSaisi terminer. Connexion en cours...\n" );
		}while( connectWithUsername(&token,identifiant,password) && i++ < 0);
		sauvegarderToken(token);
	}

	printf("Connect\n");
	free(token);


}



int main()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	// size of screen
	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);
	int largueur = DM.h;
	int hauteur = largueur*0.66667;

	SDL_Window *window = SDL_CreateWindow("Nineteen | Launcher", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,largueur,hauteur,SDL_WINDOW_SHOWN );
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	TTF_Init();

	launcher(renderer);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
