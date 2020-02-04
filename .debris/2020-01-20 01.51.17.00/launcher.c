#include <SDL.h>
#include <SDL_ttf.h>

#include "../include/textField.h"
#include "../include/private_image.h"


int launcher(SDL_Renderer* renderer)
{
	
}


int main()
{
	SDL_Window *window = SDL_CreateWindow("Nineteen | Launcher", SDL_WINDOWPOS_CENTERED,
								 SDL_WINDOWPOS_CENTERED,
								 800,
								 800,
								 SDL_WINDOW_SHOWN );


	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	
	launcher(renderer);
}