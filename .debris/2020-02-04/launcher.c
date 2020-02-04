#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "../include/textField.h"
#include "../include/private_image.h"

#define DIR_TOKEN_FILE "file.token" // only for debug realy named ".token"

int dejaConneceter(char token[])
{
	FILE * fp;
    fp = fopen ("file.token", "r");
	if (!fp)
	{
		return 0;
	}
	else 
	{
		fscanf(fp,"%s",token);
		printf("%s\n",token);
		fclose(fp);
		return 1;
	}
	return 0;
}


void launcher(SDL_Renderer* renderer)
{
	char identifiant[32];
	char password[32];
	char token[256];

	dejaConneceter(token);

	SDL_RenderClear(renderer);
}


int main()
{
	SDL_Window *window = SDL_CreateWindow("Nineteen | Launcher", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,800,800,SDL_WINDOW_SHOWN );
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	TTF_Init();
	
	launcher(renderer);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}