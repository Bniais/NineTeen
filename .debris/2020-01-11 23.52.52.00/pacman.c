#include <SDL.h>
#include <SDL_image.h>

// define temporaire
#define WINDOW_SEND_L 1920
#define WINDOW_SEND_H 1080

// NATIVE RESOLUTION
#define WINDOW_L 1920
#define WINDOW_H 1080



void afficherTout(SDL_Renderer* renderer)
{
    system("clear");
}

int pacman(SDL_Renderer *renderer)
{
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