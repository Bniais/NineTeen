#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "../include/textField.h"

#include "../include/libWeb.h"

#define TRUE 1
#define FALSE 0

#define DIR_TOKEN_FILE ".token" // only for debug realy named ".token"

const SDL_Color rouge = {255,0,0};
const	SDL_Color noir = {0,0,0};
const	SDL_Color blanc = {255,255,255};
const	SDL_Color blanc_foncer = {200,200,200};
const	SDL_Color vert = {76,196,66};
const SDL_Color bleu_foncer = {66,100,196};

// RESOLUTION
#define WIN_LARGUEUR 1440
#define WIN_HAUTEUR 960

#define SIZE_SESSION 256
#define URL_REGISTRATION "https://nineteen.recognizer.fr/"


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

int dejaConneceter(char **token)
{
	FILE * fp;
    fp = fopen (DIR_TOKEN_FILE, "r");
	if (!fp)
	{
		return 0;
	}
	else
	{
		fscanf(fp,"%s",*token);
		fclose(fp);

		if ( !connectWithKey(*token) ){
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

void connexion(SDL_Renderer *renderer, char **token)
{
	SDL_Texture* background = IMG_LoadTexture(renderer,"../assets/image/launcher_no_font.png");

	TTF_Font *police = NULL;
	TTF_Font *ttf_pwd = NULL;
	police = TTF_OpenFont("../assets/font/police.ttf",100);

	ttf_pwd = TTF_OpenFont("../assets/font/password.ttf",100);


	char identifiant[24]="";
	char motDePasse[24]="";


	SDL_Rect targetId = { WIN_LARGUEUR/5.5 , WIN_HAUTEUR/3, WIN_LARGUEUR/1.7 , WIN_HAUTEUR/14};
	SDL_Rect targetIdLabel = { WIN_LARGUEUR/6.5 , WIN_HAUTEUR/4 , WIN_LARGUEUR/1.7 , WIN_HAUTEUR/14};

	SDL_Rect targetPwd = { WIN_LARGUEUR/5.5 , WIN_HAUTEUR/1.9 , WIN_LARGUEUR/1.7 , WIN_HAUTEUR/14 };
	SDL_Rect targetPwdLabel = { WIN_LARGUEUR/6.3 , WIN_HAUTEUR/2.3 , WIN_LARGUEUR/1.7 , WIN_HAUTEUR/14 };

	SDL_Rect targetConnect = { WIN_LARGUEUR/1.87, WIN_HAUTEUR/1.5 , 30* (strlen("Connexion")+2)  , 60};
	SDL_Rect targetInscription = {WIN_LARGUEUR/4, WIN_HAUTEUR/1.5, 30* (strlen("Inscription")+2), 60};

	SDL_Rect targetUIView = {WIN_LARGUEUR/6.5,WIN_HAUTEUR/4.8, WIN_HAUTEUR,WIN_HAUTEUR/2};


	SDL_RenderCopy(renderer, background, NULL, NULL);
	SDL_SetRenderDrawColor(renderer, noir.r , noir.g, noir.b,200);
	SDL_RenderFillRect(renderer,&targetUIView);
	SDL_SetRenderDrawColor(renderer, blanc_foncer.r , blanc_foncer.g, blanc_foncer.b,255);
	SDL_RenderFillRect(renderer,&targetId);
	SDL_RenderFillRect(renderer,&targetPwd);
	renduTextField(renderer,"Identifiant",police,blanc_foncer,targetIdLabel);
	renduTextField(renderer,"Mot de passe",police,blanc_foncer,targetPwdLabel);

	SDL_SetRenderDrawColor(renderer, vert.r , vert.g, vert.b,255);
	SDL_RenderFillRect(renderer,&targetConnect);
	renduTextField(renderer,"Connexion",police,noir,targetConnect);

	SDL_SetRenderDrawColor(renderer, bleu_foncer.r , bleu_foncer.g, bleu_foncer.b,255);
	SDL_RenderFillRect(renderer,&targetInscription);
	renduTextField(renderer,"Inscription",police,noir,targetInscription);


	SDL_RenderPresent(renderer);
	int etatIdentifant = RESPONDER_TRUE;
	int etatMotDePasse = RESPONDER_FALSE;
	int pressConnexion = SDL_FALSE;

	SDL_Point mouse = {0,0};
	int pressMaj = SDL_FALSE;
	int connecter = SDL_FALSE;

	do
	{
		SDL_RenderCopy(renderer, background, NULL, NULL);
		if(etatIdentifant != RESPONDER_FALSE)
			etatIdentifant = textField(renderer, police, blanc_foncer ,identifiant, strlen(identifiant) ,&targetId , &mouse,&pressMaj);
		else if(etatMotDePasse != RESPONDER_FALSE)
			etatMotDePasse = textField(renderer, ttf_pwd, blanc_foncer ,motDePasse, strlen(motDePasse) ,&targetPwd , &mouse,&pressMaj);


		if(mouse.x && mouse.y)
		{
			// si on match avec les coordonner d'une des deux textfield on le met en RESPONDER_TRUE et l'autre en RESPONDER_FALSE
			if ( TF_ClickIn( targetId , mouse) )
			{
				etatIdentifant = RESPONDER_TRUE;
				etatMotDePasse = RESPONDER_FALSE;
			}
			else if ( TF_ClickIn( targetPwd , mouse) )
			{
				etatMotDePasse = RESPONDER_TRUE;
				etatIdentifant = RESPONDER_FALSE;
			}
			else if ( TF_ClickIn( targetConnect , mouse) )
			{
				time_t t = time(NULL);
				struct tm tm = *localtime(&t);
				printf("IL EST : %d-%02d-%02d %02d:%02d:%d\n",tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour , tm.tm_min, tm.tm_sec );

				if ( !connectWithUsername(token,identifiant,motDePasse) )
				{
					pressConnexion = SDL_TRUE;
					etatMotDePasse = RESPONDER_FALSE;
					etatIdentifant = RESPONDER_FALSE;
				}
				mouse.x = 0;
				mouse.y = 0;
			}
			else if ( TF_ClickIn( targetInscription , mouse) )
			{
				pressConnexion = SDL_TRUE;
				system("open "URL_REGISTRATION);
				mouse.x = 0;
				mouse.y = 0;
			}
			else
			{
				mouse.x = 0;
				mouse.y = 0;
			}
		}

		SDL_RenderCopy(renderer, background, NULL, NULL);
		SDL_SetRenderDrawColor(renderer, noir.r , noir.g, noir.b,200);
		SDL_RenderFillRect(renderer,&targetUIView);
		SDL_SetRenderDrawColor(renderer, blanc_foncer.r , blanc_foncer.g, blanc_foncer.b,255);
		SDL_RenderFillRect(renderer,&targetId);
		SDL_RenderFillRect(renderer,&targetPwd);
		renduTextField(renderer,"Identifiant",police,blanc_foncer,targetIdLabel);
		renduTextField(renderer,"Mot de passe",police,blanc_foncer,targetPwdLabel);

		SDL_SetRenderDrawColor(renderer, vert.r , vert.g, vert.b,255);
		SDL_RenderFillRect(renderer,&targetConnect);
		renduTextField(renderer,"Connexion",police,noir,targetConnect);

		SDL_SetRenderDrawColor(renderer, bleu_foncer.r , bleu_foncer.g, bleu_foncer.b,255);
		SDL_RenderFillRect(renderer,&targetInscription);
		renduTextField(renderer,"Inscription",police,noir,targetInscription);

		if( strlen(motDePasse) >= 1)
			renduTextField(renderer,motDePasse,ttf_pwd,noir,targetPwd);
		if( strlen(identifiant) >= 1)
			renduTextField(renderer,identifiant,police,noir,targetId);

		SDL_RenderPresent(renderer);
		SDL_RenderClear(renderer);

		} while( !pressConnexion ) ;
}

int chargementFichier()
{
	return SDL_TRUE;
}

int launcher(SDL_Renderer* renderer, char **token)
{

	printf("TON PING %d\n",ping() );

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	if ( !dejaConneceter(token) )
	{
		connexion(renderer,token);
		sauvegarderToken(*token);

  }

	// chargement puis envoi vers room
	chargementFichier();


	return 0;
}




int main()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	// size of screen
/*	SDL_DisplayMode DM;
	SDL_GetCurrentDisplayMode(0, &DM);
	int largueur = DM.h;
	int hauteur = largueur*0.66667; */
	char *token = malloc(sizeof(char) * SIZE_SESSION + 1);

	SDL_Window *window = SDL_CreateWindow("Nineteen | Launcher", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIN_LARGUEUR,WIN_HAUTEUR,SDL_WINDOW_SHOWN );
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
	TTF_Init();

	launcher(renderer,&token);

	TTF_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
