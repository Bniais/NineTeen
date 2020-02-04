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

#define SIZE_TABLEAU 11

#define MUR 1
#define PIECE 2


// chargement
#define DIR_PERSONNAGE "Textures/pacman.png"

// direction enum
enum direction {DROIT, HAUT, GAUCHE, BAS};

SDL_Rect PERSONNAGE = {22,22,22,22};

SDL_Texture * LoadTextureWithErrorCode(SDL_Renderer *renderer ,char directory[])
{

	SDL_Texture* texture = IMG_LoadTexture(renderer,directory);

	if(!texture)
		printf("TEXTURE FAILURE : Erreur lors de la creation de %s SDL_ERROR : %s",directory,SDL_GetError());

	return texture;
}

void afficherPacman(SDL_Renderer *renderer,SDL_Texture *texturePersonnage ,int positionX, int positionY,int boucheOuverte,int direction)
{
    SDL_Rect cible = PERSONNAGE;
    SDL_Rect target = {positionX,positionY,PERSONNAGE.w,PERSONNAGE.h};
    cible.x *= direction;
    cible.y *= boucheOuverte;
    SDL_RenderCopy(renderer,texturePersonnage,&cible,&target);
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

void pacman(SDL_Renderer *renderer)
{
    SDL_Texture *texturePersonnage = LoadTextureWithErrorCode(renderer,DIR_PERSONNAGE);
    SDL_RenderClear(renderer);
    afficherPacman(renderer,texturePersonnage, 50,50,0, BAS);
    SDL_RenderPresent(renderer);
    attendreEvenementAppuyer(SDLK_SPACE);

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