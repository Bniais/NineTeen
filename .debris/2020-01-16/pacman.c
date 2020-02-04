#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>

#include <time.h>

// define temporaire
#define WINDOW_SEND_L 800
#define WINDOW_SEND_H 800
#define FPS 60

// NATIVE RESOLUTION
#define WINDOW_L 800
#define WINDOW_H 800

#define SIZE_TABLEAU 20
int mur[SIZE_TABLEAU][SIZE_TABLEAU];
// mur enum
enum murDirection {AUCUN,HORIZONTAL,VERTICAL};

int piece[SIZE_TABLEAU][SIZE_TABLEAU];
// piece enum
enum bonus {RIEN,PIECE,BONUS_1,BONUS_2};
// distance entre piece
const int DISTANCE_BETWEEN_PIECE = WINDOW_L/SIZE_TABLEAU;
//vitesse deplacement 
#define VITESSE_DEPLACEMENT 4

// chargement
#define DIR_PERSONNAGE "Textures/pacman.png"

// direction enum
SDL_Rect PERSONNAGE = {22,22,22,22};
enum direction {DROIT, HAUT, GAUCHE, BAS};

typedef struct {
    int x;
    int y;
    int bouche;
    int direction;
}pacmanPosition;

SDL_Texture * LoadTextureWithErrorCode(SDL_Renderer *renderer ,char directory[])
{

	SDL_Texture* texture = IMG_LoadTexture(renderer,directory);

	if(!texture)
		printf("TEXTURE FAILURE : Erreur lors de la creation de %s SDL_ERROR : %s",directory,SDL_GetError());

	return texture;
}

void initialiserPlateauJeux()
{
    for (int i = 0; i < SIZE_TABLEAU; i++)
        for(int j = 0 ; j < SIZE_TABLEAU ; j++)
        {
            mur[i][j] = AUCUN;
            piece[i][j] = PIECE;
        }
    
}

void afficherPiece(SDL_Renderer *renderer)
{
    for(int i=1 ; i< SIZE_TABLEAU ; i++)
        for(int j = 1 ; j < SIZE_TABLEAU ; j++)
            if( piece[i][j] )
            {
                printf("Je suis la\n");
                 SDL_SetRenderDrawColor(renderer, 0xFF, 0, 0, 0xFF);
                 SDL_RenderDrawPoint(renderer,i*DISTANCE_BETWEEN_PIECE,j*DISTANCE_BETWEEN_PIECE);
            }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);        
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

void traitementEvenementVariable(pacmanPosition *pacmanPositionDirection, int *exit)
{
       SDL_Event mouvement;
        while ( SDL_PollEvent(&mouvement) )
        {
            if (mouvement.type == SDL_KEYDOWN){
                if (mouvement.key.keysym.sym == SDLK_UP )
                {
                    pacmanPositionDirection->direction = HAUT;
                }
                else if ( mouvement.key.keysym.sym == SDLK_DOWN )
                {
                    pacmanPositionDirection->direction = BAS;
                }
                else if ( mouvement.key.keysym.sym == SDLK_LEFT )
                {
                    pacmanPositionDirection->direction = GAUCHE;
                }
                else if ( mouvement.key.keysym.sym == SDLK_RIGHT )
                {
                    pacmanPositionDirection->direction = DROIT;
                }
                else if ( mouvement.key.keysym.sym == SDLK_SPACE )
                {
                    *exit = 1;
                }
                    
            }


        }  

        switch (pacmanPositionDirection->direction)
        {
            case HAUT:{
               pacmanPositionDirection->y -= VITESSE_DEPLACEMENT / (FPS/30);
            }break;
            case BAS:{
                pacmanPositionDirection->y += VITESSE_DEPLACEMENT / (FPS/30);
            }break;
            case GAUCHE:{
                pacmanPositionDirection->x -= VITESSE_DEPLACEMENT / (FPS/30);
            }break;
            case DROIT:{
                pacmanPositionDirection->x += VITESSE_DEPLACEMENT / (FPS/30);
            }break;
        }

}

void pacman(SDL_Renderer *renderer)
{
    SDL_Texture *texturePersonnage = LoadTextureWithErrorCode(renderer,DIR_PERSONNAGE);
    initialiserPlateauJeux();
    pacmanPosition pacmanPositionDirection = {WINDOW_L/2,WINDOW_H/2,0,0};
    int mort = 0;
    do
    {
        traitementEvenementVariable(&pacmanPositionDirection,&mort);
        SDL_RenderClear(renderer);
        afficherPiece(renderer);
        afficherPacman(renderer,texturePersonnage, pacmanPositionDirection.x,pacmanPositionDirection.y,pacmanPositionDirection.bouche, pacmanPositionDirection.direction);
        SDL_RenderPresent(renderer);
        SDL_Delay(1000/FPS);


    } while (!mort);
    
    

   

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