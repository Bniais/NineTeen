#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>

#include <time.h>

// define temporaire
#define WINDOW_SEND_L 1920
#define WINDOW_SEND_H 1080

// NATIVE RESOLUTION
#define WINDOW_L 1920
#define WINDOW_H 1080

#define SIZE_TABLEAU 20

#define MUR 1
#define PIECE 2


void afficherTout(SDL_Renderer* renderer)
{
    system("clear");
}

void initialisationPlateau()
{

}

int pacman(SDL_Renderer *renderer)
{
    srand( time(NULL) );
    int tableDeJeu[SIZE_TABLEAU][SIZE_TABLEAU];
    return 0;
}


int main()
{
    SDL_Init(SDL_INIT_EVERYTHING );

    SDL_Window* window = SDL_CreateWindow("Nineteen | Flappy Bird", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_SEND_L, WINDOW_SEND_H, SDL_WINDOW_SHOWN  );
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

           pacman(renderer);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}