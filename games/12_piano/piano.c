#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

// temporaire define parametre a l'appelle du main de piano
#define WINDOW_L 640
#define WINDOW_H 900

typedef struct {
   int touche1,touche2,touche3,touche4,longueurNote;
}Piano;
int taille = 0;

#define SEPARATEUR_TOUCHE 5 // distance de separation des touche
#define NOMBRE_TOUCHE_PIANO 4
const SDL_Rect TOUCHE_PIANO = {WINDOW_L/4,WINDOW_H,WINDOW_L/4,0};
#define FPS 60
#define DIR_BACKGROUND "Textures/background.png"
#define MUSIC_1 "Sounds/Starfire.mp3"


Piano *chargerMusique(char nom[])
{
    Piano *piano = malloc( sizeof(Piano) );
    FILE *fichier = fopen(nom,"r");
    if ( !fichier  )
        printf("Fichier introuvable\n");
    else
    {
        while( !feof(fichier) )
        {
            piano = realloc(piano, sizeof(Piano)* (taille+1) );
            fscanf(fichier,"%d %d %d %d %d ",&(piano + taille)->touche1,&(piano + taille)->touche2,&(piano + taille)->touche3,&(piano + taille)->touche4,&(piano + taille)->longueurNote);
            taille++;
        }
    }

    return piano;
}

void chargementVue(SDL_Renderer *renderer, Piano *piano, long long progression, int appuiTouch_1, int appuiTouch_2, int appuiTouch_3, int appuiTouch_4)
{
    SDL_Rect target = TOUCHE_PIANO;
    int decalageCarrer = 0;
    for(int i=0 ; i < taille ; i++)
    {
        if(i)
            decalageCarrer -= (piano + (i-1) )->longueurNote;


        if( (piano + i)->touche1 )
        {
            target.x = TOUCHE_PIANO.x * 0 + SEPARATEUR_TOUCHE;
            target.y = TOUCHE_PIANO.y - (piano + i)->longueurNote + decalageCarrer + SEPARATEUR_TOUCHE + progression;
            target.w = TOUCHE_PIANO.w - SEPARATEUR_TOUCHE;
            target.h = (piano + i)->longueurNote - SEPARATEUR_TOUCHE;
            if(!i && piano->touche1 == 2)
                SDL_SetRenderDrawColor(renderer, 20, 150, 20, 245);
            else if(!i && piano->touche1 == -1)
                SDL_SetRenderDrawColor(renderer, 150, 20, 20, 245);
            else
                SDL_SetRenderDrawColor(renderer, 20, 20, 20, 245);

            SDL_RenderFillRect(renderer,&target);
        }

        if( (piano + i)->touche2 )
        {
            target.x = TOUCHE_PIANO.x * 1 + SEPARATEUR_TOUCHE;
            target.y = TOUCHE_PIANO.y - (piano + i)->longueurNote + decalageCarrer + SEPARATEUR_TOUCHE + progression;
            target.w = TOUCHE_PIANO.w - SEPARATEUR_TOUCHE;
            target.h = (piano + i)->longueurNote - SEPARATEUR_TOUCHE;
            if(!i && piano->touche2 == 2)
                SDL_SetRenderDrawColor(renderer, 20, 150, 20, 245);
            else if(!i && piano->touche2 == -1)
                SDL_SetRenderDrawColor(renderer, 150, 20, 20, 245);
            else
                SDL_SetRenderDrawColor(renderer, 20, 20, 20, 245);

            SDL_RenderFillRect(renderer,&target);
        }
        if( (piano + i)->touche3 )
        {
            target.x = TOUCHE_PIANO.x * 2 + SEPARATEUR_TOUCHE;
            target.y = TOUCHE_PIANO.y - (piano + i)->longueurNote + decalageCarrer + SEPARATEUR_TOUCHE + progression;
            target.w = TOUCHE_PIANO.w - SEPARATEUR_TOUCHE;
            target.h = (piano + i)->longueurNote - SEPARATEUR_TOUCHE;
            if(!i && piano->touche3 == 2)
                SDL_SetRenderDrawColor(renderer, 20, 150, 20, 245);
            else if(!i && piano->touche3 == -1)
                SDL_SetRenderDrawColor(renderer, 150, 20, 20, 245);
            else
                SDL_SetRenderDrawColor(renderer, 20, 20, 20, 245);

            SDL_RenderFillRect(renderer,&target);
        }
        if( (piano + i)->touche4 )
        {
            target.x = TOUCHE_PIANO.x * 3 + SEPARATEUR_TOUCHE;
            target.y = TOUCHE_PIANO.y - (piano + i)->longueurNote + decalageCarrer + SEPARATEUR_TOUCHE + progression;
            target.w = TOUCHE_PIANO.w - SEPARATEUR_TOUCHE;
            target.h = (piano + i)->longueurNote - SEPARATEUR_TOUCHE;
            if(!i && piano->touche4 == 2)
                SDL_SetRenderDrawColor(renderer, 20, 150, 20, 245);
            else if(!i && piano->touche4 == -1)
                SDL_SetRenderDrawColor(renderer, 150, 20, 20, 245);
            else
                SDL_SetRenderDrawColor(renderer, 20, 20, 20, 245);

            SDL_RenderFillRect(renderer,&target);
        }

    }

}

int nettoyageStructure(Piano *piano, int *progression)
{
    if(taille)
        if( piano->longueurNote < *progression)
        {
            // verifer que tous les valeurs on etait appuyer
            if( (piano->touche1 == 2 || !piano->touche1 ) && (piano->touche2 == 2 || !piano->touche2 ) && (piano->touche3 == 2 || !piano->touche3 ) && (piano->touche4 == 2 || !piano->touche4 ) )
            {
                *progression -= piano->longueurNote;
                for (int i = 1; i < taille; i++)
                {
                    piano[i-1] = piano[i];
                }
                taille--;
                return 0;
            } else
                return 1;
        }

    return 0;
}

void verificationTouch(Piano *piano,int progression, int appuiTouch_1, int appuiTouch_2, int appuiTouch_3, int appuiTouch_4)
{
    // mettre la valeur a 2 si toucher
    // mettre a -1 si toucher alors qu'il ne fallait pas

    if( piano->touche1 == 1 && appuiTouch_1)
    {
           piano->touche1 = 2;
    }
    else if ( !piano->touche1 && appuiTouch_1)
    {
        piano->touche1 = -1;
    }

    if( piano->touche2 == 1 && appuiTouch_2)
    {
           piano->touche2 = 2;
    }
    else if ( !piano->touche2 && appuiTouch_2)
    {
        piano->touche2 = -1;
    }

    if( piano->touche3 == 1 && appuiTouch_3)
    {
           piano->touche3 = 2;
    }
    else if ( !piano->touche3 && appuiTouch_3)
    {
        piano->touche3 = -1;
    }

    if( piano->touche4 == 1 && appuiTouch_4)
    {
           piano->touche4 = 2;
    }
    else if ( !piano->touche4 && appuiTouch_4)
    {
        piano->touche4 = -1;
    }
}

int analyseStructure(Piano *piano)
{
    // fin du jeux
    return (piano->touche1 == -1 || piano->touche2 == -1 || piano->touche3 == -1 || piano->touche4 == -1);
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

void pianoTiles(SDL_Renderer *renderer)
{
    Piano *piano = malloc( sizeof(Piano) );
    int progression = 0;
    int exit = 0;
    int appuiTouch_1 = 0 ,appuiTouch_2 = 0 ,appuiTouch_3 = 0 ,appuiTouch_4 = 0;


    piano = chargerMusique("musique.txt");
    printf("Il y'a %d note \n",taille);
    //Mix_Music* music = Mix_LoadMUS("Sounds/Invincible.mp3");
    Mix_Chunk* music = Mix_LoadWAV("Sounds/Invincible.wav");


// pre affichage avant de commencer
    SDL_SetRenderDrawColor(renderer,255,255,255,255);
    SDL_RenderClear(renderer);
    chargementVue(renderer,piano,progression,appuiTouch_1,appuiTouch_2,appuiTouch_3,appuiTouch_4);
    SDL_RenderPresent(renderer);
    attendreEvenementAppuyer(SDLK_SPACE);
// demarrage //

    Mix_PlayChannel(1,music,1);

    do
    {
        SDL_Event evenement;
        if (SDL_PollEvent(&evenement) )
        {
            if(evenement.type == SDL_KEYDOWN )
            {
                if(evenement.key.keysym.sym == SDLK_c)
                    appuiTouch_1 = 1;
                if(evenement.key.keysym.sym == SDLK_v)
                    appuiTouch_2 = 1;
                if(evenement.key.keysym.sym == SDLK_b)
                    appuiTouch_3 = 1;
                if(evenement.key.keysym.sym == SDLK_n)
                    appuiTouch_4 = 1;
            } else if(evenement.type == SDL_KEYUP )
            {
                if(evenement.key.keysym.sym == SDLK_c)
                    appuiTouch_1 = 0;
                if(evenement.key.keysym.sym == SDLK_v)
                    appuiTouch_2 = 0;
                if(evenement.key.keysym.sym == SDLK_b)
                    appuiTouch_3 = 0;
                if(evenement.key.keysym.sym == SDLK_n)
                    appuiTouch_4 = 0;
            }

        }

        progression += 8/(FPS/60);
        verificationTouch(piano,progression,appuiTouch_1,appuiTouch_2,appuiTouch_3,appuiTouch_4);
        exit = analyseStructure(piano);
        if(!exit)
            exit = nettoyageStructure(piano,&progression);
                if(exit)
                    Mix_HaltChannel(1);


        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderClear(renderer);
        chargementVue(renderer,piano,progression,appuiTouch_1,appuiTouch_2,appuiTouch_3,appuiTouch_4);
        SDL_RenderPresent(renderer);
        SDL_Delay(1000/FPS);
    } while (!exit && taille != 0);
    SDL_Delay(2000);
    free(piano);
    piano = NULL;
}

// temporaire
int main()
{
    SDL_Init(SDL_INIT_EVERYTHING );

    SDL_Window* window = SDL_CreateWindow("Nineteen | Piano Tils", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_L, WINDOW_H, SDL_WINDOW_SHOWN  );
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) < 0)
    {
        SDL_Log("Erreur initialisation SDL_mixer : %s", Mix_GetError());
        SDL_Quit();
        return -1;
    }

    pianoTiles(renderer);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}
