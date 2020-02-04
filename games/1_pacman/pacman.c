#include <stdlib.h>

#include <SDL.h>
#include <SDL_image.h>

#include <time.h>

// define temporaire
#define WINDOW_SEND_L 805
#define WINDOW_SEND_H 805
#define FPS 60

// NATIVE RESOLUTION
#define WINDOW_L 800
#define WINDOW_H 800

#define SIZE_TABLEAU_PIECE 20
#define SIZE_TABLEAU_MUR 21
int mur[SIZE_TABLEAU_MUR][SIZE_TABLEAU_MUR];
// mur enum
enum murDirection {AUCUN,HORIZONTAL,VERTICAL};

int piece[SIZE_TABLEAU_PIECE][SIZE_TABLEAU_PIECE];
// piece enum
enum bonus {RIEN,PIECE,BONUS_1,BONUS_2};
// distance entre piece
const int DISTANCE_BETWEEN_PIECE = WINDOW_L/SIZE_TABLEAU_PIECE;
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
    for (int i = 0; i < SIZE_TABLEAU_MUR; i++)
        for(int j = 0 ; j < SIZE_TABLEAU_MUR ; j++)
            mur[i][j] = AUCUN;

    for (int i = 0; i < SIZE_TABLEAU_PIECE; i++)
        for(int j = 0 ; j < SIZE_TABLEAU_PIECE ; j++)
        {
            piece[i][j] = PIECE;
        }
            
}

void carte1()
{
    for (int i = 0; i < SIZE_TABLEAU_MUR; i++)
        for(int j = 0 ; j < SIZE_TABLEAU_MUR ; j++)
        {
        if(!i || i == SIZE_TABLEAU_MUR - 1)
            mur[i][j] = VERTICAL;
        else if(!j || j == SIZE_TABLEAU_MUR - 1)
            mur[i][j] = HORIZONTAL;
        }

    
}

void afficherPiece(SDL_Renderer *renderer)
{
    for(int i=1 ; i< SIZE_TABLEAU_PIECE ; i++)
        for(int j = 1 ; j < SIZE_TABLEAU_PIECE ; j++)
            if( piece[i][j] )
            {
                 SDL_SetRenderDrawColor(renderer, 0xFF, 0, 0, 0xFF);
                 SDL_RenderDrawPoint(renderer,i*DISTANCE_BETWEEN_PIECE,j*DISTANCE_BETWEEN_PIECE);
            }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);        
}

void afficherMur(SDL_Renderer *renderer)
{
    for(int i= 0 ; i< SIZE_TABLEAU_MUR ; i++)
        for(int j = 0 ; j < SIZE_TABLEAU_MUR ; j++)
            if( mur[i][j] == HORIZONTAL)
            {
                 SDL_SetRenderDrawColor(renderer, 0xFF, 0, 0, 0xFF);
                 SDL_Rect target = {i*DISTANCE_BETWEEN_PIECE,j*DISTANCE_BETWEEN_PIECE,DISTANCE_BETWEEN_PIECE,5};
                 SDL_RenderFillRect(renderer,&target);
            }
            else if ( mur[i][j] == VERTICAL)
            {
                 SDL_SetRenderDrawColor(renderer, 0xFF, 0, 0, 0xFF);
                 SDL_Rect target = {i*DISTANCE_BETWEEN_PIECE,j*DISTANCE_BETWEEN_PIECE,5,DISTANCE_BETWEEN_PIECE};
                 SDL_RenderFillRect(renderer,&target);
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

void mangerPiece(int positionX, int positionY)
{
    for (int i = 0; i < SIZE_TABLEAU_PIECE; i++)
        for(int j = 0 ; j < SIZE_TABLEAU_PIECE ; j++)
        {
            if( (positionX + PERSONNAGE.w) >= i*DISTANCE_BETWEEN_PIECE && (positionX + PERSONNAGE.w) < (i+1)*DISTANCE_BETWEEN_PIECE)
                if( (positionY + PERSONNAGE.w) >= j*DISTANCE_BETWEEN_PIECE && (positionY + PERSONNAGE.w) < (j+1)*DISTANCE_BETWEEN_PIECE)
                    piece[i][j] = 0;
        }
            
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
    carte1();
    pacmanPosition pacmanPositionDirection = {WINDOW_L/2,WINDOW_H/2,0,0};
    int mort = 0;
    do
    {
        mangerPiece(pacmanPositionDirection.x,pacmanPositionDirection.y);
        traitementEvenementVariable(&pacmanPositionDirection,&mort);
        SDL_RenderClear(renderer);
        afficherPiece(renderer);
        afficherMur(renderer);
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