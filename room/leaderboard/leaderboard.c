#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "../../include/libWeb.h"

#define LIMITE_FPS 240
const float FRAME_TIME_LEADERBOARD = 1000.0/LIMITE_FPS*1.f;

#define NATIF_W 1280
#define NATIF_H 720
float inverseRatio = 1.f;


#define NOMBRE_CELLULE_FENETRE 10
#define PRELOAD_CELLULE_MAX 15
#define NOMBRE_CELLULE_LIST_SCROLL 6

int AFFICHER_PLUS = 1;

const int HUD_SIZE = NATIF_W * 0.085;
const SDL_Color BACKGROUND_C = {254, 254, 226};


///////////////////////////////////////////////
// LES SELECTION
#define SCREEN 0
#define LIST_SCROLL 1
#define LIST_SCOLL_ELEMENT 2
#define TEXT_FIELD_USERNAME 3
#define SEARCH_PLAYER 4
///////////////////////////////////////////////

struct classement
{
	int score;
	char username[32];
};

#define NOMBRE_ELEMENT_LIST 7
const char nomList[NOMBRE_ELEMENT_LIST][30]={"GLOBAL","FLAPPY BIRD","TETRIS","ASTEROID","PACMAN","SNAKE","DEMINEUR"};

SDL_Color noirCOLOR = {0,0,0};
SDL_Color noirC = {50,50,50};


void chargementDonner(int gameID, char *username, int offset, struct classement donner[])
{
	//////////////////////////////////
	// CAST
	char _gameID[5];
	sprintf(_gameID,"%d",gameID);

	char _offset[5];
	sprintf(_offset,"%d",offset);

	char *retour = malloc(sizeof(char) * 2048);
	if(!retour)
		printf("Erreur allocation\n" );
	while ( getLeaderboard(_gameID,username,_offset,retour) != EXIT_SUCCESS );

	sscanf(retour,"%s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d %s %d ", donner[0 + offset].username,&donner[0 + offset].score,
																																																							donner[1 + offset].username,&donner[1 + offset].score,
																																																							donner[2 + offset].username,&donner[2 + offset].score,
																																																							donner[3 + offset].username,&donner[3 + offset].score,
																																																							donner[4 + offset].username,&donner[4 + offset].score,
																																																							donner[5 + offset].username,&donner[5 + offset].score,
																																																							donner[6 + offset].username,&donner[6 + offset].score,
																																																							donner[7 + offset].username,&donner[7 + offset].score,
																																																							donner[8 + offset].username,&donner[8 + offset].score,
																																																							donner[9 + offset].username,&donner[9 + offset].score,
																																																							donner[10 + offset].username,&donner[10 + offset].score,
																																																							donner[11 + offset].username,&donner[11 + offset].score,
																																																							donner[12 + offset].username,&donner[12 + offset].score,
																																																							donner[13 + offset].username,&donner[13 + offset].score,
																																																							donner[14 + offset].username,&donner[14 + offset].score
																																																						);
	free(retour);
}



void _limiterFrame(const float delayLancementFrame,float *_IPS)
{
	float delayFrame = SDL_GetTicks() - delayLancementFrame;
	// MISE A JOURS DU NOMBRE DE FPS
	*_IPS = 1000.0/delayFrame;
	// ATTENDRE SI NECESSAIRE POUR LIMITER A 60 FPS
	if(delayFrame < FRAME_TIME_LEADERBOARD)
	{
		SDL_Delay(FRAME_TIME_LEADERBOARD - delayFrame );
		// MAJ IPS APRES ATTENTE
		*_IPS = 1000.0/ ( delayFrame + (FRAME_TIME_LEADERBOARD - delayFrame) );
	}

}

void scrollEvent(int *halt, int *xMouse, int *yMouse, int *scrollPositionWindow, int *scrollPositionList, int *_SELECTION, char rechercher[], struct classement donner[], int selectionScrollingList)
{

  SDL_Event event;
  while( SDL_PollEvent( &event ) )
  {
		if(event.type == SDL_KEYDOWN)
		{
			if(event.key.keysym.sym == SDLK_ESCAPE)
	    {
				printf("JE QUITTE LE PROGRAMME SEUL\n");
	     *halt = SDL_TRUE;
	    }
		}
    if(event.type == SDL_MOUSEWHEEL)
    {
			/////////////////////////////////////////
			// RECUPERER POSITION SOURIS POUR SAVOIR OU SCROLL SI LES DEUX VUE SCROLL SON OUVERTE
			int x,y;
			SDL_GetMouseState(&x,&y);
			x *= inverseRatio;
			y *= inverseRatio;
			SDL_Rect listDeroulante = {NATIF_W*0.05 , HUD_SIZE * 0.15, NATIF_W*0.35, HUD_SIZE*4.2};

			if(*_SELECTION != LIST_SCROLL || !(x > listDeroulante.x && x < listDeroulante.x + listDeroulante.w && y > listDeroulante.y && y < listDeroulante.h + listDeroulante.y))
			{
				/////////////////////////////////
				// UPDATE VIEW SCROLL
				*scrollPositionWindow += event.wheel.y;
				/////////////////////////////////
				// DEPASSEMENT HAUT
				if(*scrollPositionWindow > 0)
					*scrollPositionWindow = 0;

				if( *_SELECTION == SEARCH_PLAYER)
				{
					if (*scrollPositionWindow < -(int)(  (NATIF_H/10) * (PRELOAD_CELLULE_MAX - NOMBRE_CELLULE_FENETRE  + 1) + HUD_SIZE ) )
					{
						*scrollPositionWindow = -(int)(  (NATIF_H/10) * (PRELOAD_CELLULE_MAX - NOMBRE_CELLULE_FENETRE  +1 ) + HUD_SIZE );
						printf("NOUVEAU = %d\n",PRELOAD_CELLULE_MAX*AFFICHER_PLUS );
						//chargementDonner(selectionScrollingList, "", PRELOAD_CELLULE_MAX*AFFICHER_PLUS,donner);
						AFFICHER_PLUS += 1;
					}

				}
				else
				{
					if (*scrollPositionWindow < -(int)(  (NATIF_H/10) * ( (PRELOAD_CELLULE_MAX * AFFICHER_PLUS) - NOMBRE_CELLULE_FENETRE  ) + HUD_SIZE ) ) // -1 POUR SELECTION ACTUEL
					{
						*scrollPositionWindow = -(int)(  (NATIF_H/10) * ( (PRELOAD_CELLULE_MAX * AFFICHER_PLUS) - NOMBRE_CELLULE_FENETRE  ) + HUD_SIZE );
						chargementDonner(selectionScrollingList, "", PRELOAD_CELLULE_MAX*AFFICHER_PLUS,donner);
						printf("NOUVEAU = %d\n",PRELOAD_CELLULE_MAX );
						AFFICHER_PLUS += 1;
					}

				}

			}
			else
			{
				/////////////////////////////////
				// UPDATE VIEW SCROLL
				*scrollPositionList += event.wheel.y;
				// DEPASSEMENT HAUT
				if(*scrollPositionList > 0)
					*scrollPositionList = 0;
				// DEPASSEMENT BAS
				else if (*scrollPositionList < -(int)( HUD_SIZE*0.60 * (NOMBRE_ELEMENT_LIST - NOMBRE_CELLULE_LIST_SCROLL - 1) ) ) // -1 POUR SELECTION ACTUEL
					*scrollPositionList = -(int)( HUD_SIZE*0.60 * (NOMBRE_ELEMENT_LIST - NOMBRE_CELLULE_LIST_SCROLL - 1) );
			}

    }
		// RECUPERE LA POSITION DE LA SOURIE QUAND ON CLIQUE
		if(event.type == SDL_MOUSEBUTTONUP)
		{
			SDL_GetMouseState(xMouse,yMouse);
			*xMouse *= inverseRatio;
			*yMouse *= inverseRatio;
		}

		if(*_SELECTION == TEXT_FIELD_USERNAME)
		{
			switch (event.type)
			{
				case SDL_TEXTINPUT:
					 strcat(rechercher, event.text.text);break;
				case SDL_KEYDOWN:{
					if(event.key.keysym.sym == SDLK_BACKSPACE)
					{
							if(strlen(rechercher) > 0)
								rechercher[strlen(rechercher) - 1] = '\0';
					}
					if(event.key.keysym.sym == SDLK_RETURN)
					{
						if(strlen(rechercher) > 0)
						{
							*_SELECTION = SEARCH_PLAYER;

							// REPLACER VUE A 0
							*scrollPositionWindow = 0;
							printf("LANCER RECHERCHE\n" );
						}

					}

				}break;
				default:break;
			}

		}



  }

}

void ecrireText(SDL_Renderer* renderer, TTF_Font* font, char * text, SDL_Color couleur, int xPosition, int yPosition, float scale, int estHUD){

	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, couleur);
	SDL_Rect dest;
	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	SDL_QueryTexture(Message,NULL,(int*)SDL_TEXTUREACCESS_STATIC,&(dest.w), &(dest.h) );
	SDL_Rect src = dest;
	src.x = 0;
	src.y = 0;

	dest.w *= scale;
	dest.h *= scale;


	//centrer
	if( xPosition == -1)
		dest.x = NATIF_W/2 - dest.w/2;
	else
		dest.x = xPosition - dest.w/2;
	if( yPosition == -1)
		dest.y = NATIF_H/2 - dest.h/2;
	else
		dest.y = yPosition - dest.h/2;


	if(estHUD)
	{
		if(dest.y + dest.h > HUD_SIZE*4.35 )
		{
			src.h +=  HUD_SIZE*4.35 - (dest.y + dest.h);
			dest.h = src.h * scale;
		}
	}




	SDL_RenderCopy(renderer, Message, &src, &dest);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
}




void afficherCellule(SDL_Renderer *renderer, int cellulePosition, char *username , int score, TTF_Font * police, int decallageY, int estRecherche)
{
	SDL_Rect cellule = {NATIF_W*0.03, (NATIF_H/10) * cellulePosition + decallageY,NATIF_W*0.94 , NATIF_H * 0.09 };
	char classement[8];

	char _score[12];
	sprintf(_score,"%d",score);

	if(estRecherche == 1)
	{
		sprintf(classement,"-");

		SDL_SetRenderDrawColor(renderer, 255, 211, 0, 70);
	}
	else if (estRecherche == 2)
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 70);
		sprintf(classement,"%d",cellulePosition  );
	}
	else
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 70);
		sprintf(classement,"%d",cellulePosition + 1 );
	}

	SDL_RenderFillRect(renderer, &cellule);



	ecrireText(renderer,police, classement, noirCOLOR, NATIF_W*0.08, (NATIF_H/10) * cellulePosition + (cellule.h/2) + decallageY , 0.5 , 0);
	ecrireText(renderer,police, username, noirCOLOR,-1, (NATIF_H/10) * cellulePosition + (cellule.h/2) + decallageY , 0.5 , 0);
	ecrireText(renderer,police, _score, noirCOLOR, NATIF_W*0.9 , (NATIF_H/10) * cellulePosition + (cellule.h/2) + decallageY, 0.5 , 0);

}

void afficherCelluleScrollView(SDL_Renderer *renderer, int cellulePosition, char *text, TTF_Font *police,int decallageY)
{
	SDL_Rect cellule = { NATIF_W*0.05 , ( ( HUD_SIZE*0.6) * (cellulePosition + 1) ) + decallageY , NATIF_W*0.35 , HUD_SIZE*0.01 };
	cellule.y += HUD_SIZE*0.15 ;
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, &cellule);
	SDL_Color noir = {0,0,0};

	ecrireText(renderer,police, text, noirCOLOR, NATIF_W*0.05 + (cellule.w/2), cellule.y  + (HUD_SIZE*0.6)/2 + NATIF_H*0.01 , 1.0 , 1);
}


void afficherHUD(SDL_Renderer *renderer, TTF_Font *police , char *recherche, int _SELECTION, int decallageY,int selectionScrollingList)
{
	SDL_Rect hudBackground = {NATIF_W*0.0, NATIF_H * 0.0 ,NATIF_W*1.0 , HUD_SIZE };
	SDL_SetRenderDrawColor(renderer, BACKGROUND_C.r, BACKGROUND_C.g, BACKGROUND_C.b, 255);
	SDL_RenderFillRect(renderer, &hudBackground);

	SDL_Rect hud = {NATIF_W*0.0, NATIF_H * 0.0 ,NATIF_W*1.0 , HUD_SIZE - HUD_SIZE/10};
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 170);
	SDL_RenderFillRect(renderer, &hud);



	////////////////////////////////////////////
	// BOUTTON HUD

	////////////////////////////////////////////
	// RECHERCHE JOUEUR
	SDL_Rect rechercheJoueur = {NATIF_W*0.55, HUD_SIZE * 0.15, NATIF_W*0.35, HUD_SIZE*0.6};
	SDL_SetRenderDrawColor(renderer, 155, 155, 155, 255);
	SDL_RenderFillRect(renderer, &rechercheJoueur);
	////////////////////////////////////////////
	// LANCER RECHERCHE
	SDL_Rect lancerRecherche = {NATIF_W*0.90, HUD_SIZE * 0.15, HUD_SIZE*0.6, HUD_SIZE*0.6};
	SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
	SDL_RenderFillRect(renderer, &lancerRecherche);
	// ECRIRE RECHERCHE ACTUEL
	if(strlen(recherche) < 1)
		ecrireText(renderer,police, "Rechercher", noirC, rechercheJoueur.x + rechercheJoueur.w/2, rechercheJoueur.y + rechercheJoueur.h/2 + NATIF_H*0.01 , 1.0 , 0);
	else
		ecrireText(renderer,police, recherche, noirCOLOR, rechercheJoueur.x + rechercheJoueur.w/2, rechercheJoueur.y + rechercheJoueur.h/2 + NATIF_H*0.01 , 1.0 , 0);


	////////////////////////////////////////////
	// BOUTTON LIST
	// LIST DEROULANTE
	if(_SELECTION == LIST_SCROLL)
	{
		SDL_Rect listDeroulante = {NATIF_W*0.05 , HUD_SIZE * 0.15, NATIF_W*0.35, HUD_SIZE*4.2};
		SDL_SetRenderDrawColor(renderer, 155, 155, 155, 255);
		SDL_RenderFillRect(renderer, &listDeroulante);


		int sauterValeurActuellementAffichier = 0;
		for(int i = 0  ; i <= NOMBRE_CELLULE_LIST_SCROLL ; i ++)
		{
			if(i + (int)fabs(decallageY/HUD_SIZE*0.60) == selectionScrollingList)
				sauterValeurActuellementAffichier = 1;
			afficherCelluleScrollView(renderer,i + fabs(decallageY/HUD_SIZE*0.60),(char*)nomList[i + (int)fabs(decallageY/HUD_SIZE*0.60) + sauterValeurActuellementAffichier ],police , decallageY );
		}
	}
	// LISTE AFFICHEUR
	SDL_Rect listSelection = {NATIF_W*0.05 , HUD_SIZE * 0.15, NATIF_W*0.35, HUD_SIZE*0.6};
	SDL_SetRenderDrawColor(renderer, 155, 155, 155, 255);
	SDL_RenderFillRect(renderer, &listSelection);
	// AFFICHER LISTE
	SDL_Rect ouvrirListeSelection = {NATIF_W*0.05 + listSelection.w, HUD_SIZE * 0.15, HUD_SIZE*0.6, HUD_SIZE*0.6};
	SDL_SetRenderDrawColor(renderer, 195, 195, 255, 255);
	SDL_RenderFillRect(renderer, &ouvrirListeSelection);
	// ECRIRE SELECTION ACTUEL
	ecrireText(renderer,police, (char*)nomList[selectionScrollingList], noirCOLOR, listSelection.x + listSelection.w/2, listSelection.y + listSelection.h/2 + NATIF_H*0.01 , 1.0 , 0);

}



int interactionInterface(int x,int y, int _SELECTION, int scrollPositionList, int *selectionScrollingList, int *scrollPositionWindow, struct classement donner[]){

	SDL_Rect ouvrirListeSelection = {NATIF_W*0.05 + NATIF_W*0.35, HUD_SIZE * 0.15, HUD_SIZE*0.6, HUD_SIZE*0.6};
	SDL_Rect lancerRecherche = {NATIF_W*0.90, HUD_SIZE * 0.15, HUD_SIZE*0.6, HUD_SIZE*0.6};
	SDL_Rect rechercheJoueur = {NATIF_W*0.55, HUD_SIZE * 0.15, NATIF_W*0.35, HUD_SIZE*0.6};
	SDL_Rect listDeroulante = {NATIF_W*0.05 , HUD_SIZE * 0.15, NATIF_W*0.35, HUD_SIZE*4.0};

	if(x != -1 && y!= -1)
	{
		if( x > ouvrirListeSelection.x && x < ouvrirListeSelection.x + ouvrirListeSelection.w && y > ouvrirListeSelection.y && y < ouvrirListeSelection.h + ouvrirListeSelection.y)
		{
			printf("OUVRIR SELECTION\n" );
			return LIST_SCROLL;
		}

		else if( x > lancerRecherche.x && x < lancerRecherche.x + lancerRecherche.w && y > lancerRecherche.y && y < lancerRecherche.h + lancerRecherche.y)
		{
			printf("RECHERCHER JOUEUR\n" );
			*scrollPositionWindow = 0;
			return SEARCH_PLAYER;
		}

		else if( x > rechercheJoueur.x && x < rechercheJoueur.x + rechercheJoueur.w && y > rechercheJoueur.y && y < rechercheJoueur.h + rechercheJoueur.y)
		{
			printf("TEXTFILED RECHERCHER JOUEUR\n" );
			return TEXT_FIELD_USERNAME;
		}
		else if ( x > listDeroulante.x && x < listDeroulante.x + listDeroulante.w && y > listDeroulante.y && y < listDeroulante.h + listDeroulante.y && _SELECTION == LIST_SCROLL)
		{
			printf("CLIC DANS LA SELECTION\n" );

			SDL_Rect listSelection = {NATIF_W*0.05 , HUD_SIZE * 0.15, NATIF_W*0.35, HUD_SIZE*0.6};
			if(x > listSelection.x && x < listSelection.x + listSelection.w && y > listSelection.y && y < listSelection.h + listSelection.y)
			{
				printf("SELECTION ACTUEL\n" );
			}
			else
			{
				// HUD_SIZE*0.6 // TAILLE D'UNE CELLULE
				// HAUT CELLULE = listSelection.h + listSelection.y

				int ElementSelection = (int)( (y - (listSelection.h+listSelection.y + scrollPositionList) )/(HUD_SIZE*0.6) );
				if(*selectionScrollingList <= ElementSelection)
					*selectionScrollingList = ElementSelection + 1;
				else
					*selectionScrollingList = ElementSelection ;

					chargementDonner(*selectionScrollingList, "", 0,donner);

			}

			return LIST_SCOLL_ELEMENT;
		}
		else
		{
			printf("SCREEN CLICK\n" );
			return SCREEN;
		}

	}

	return _SELECTION;
}

int leaderboard(SDL_Renderer *renderer,int WinWeidth , int WinHeight)
{
	printf("JE SUIS LA\n" );
	float ratioWindowSize = (float)WinWeidth/(float)NATIF_W;
	inverseRatio = (float)NATIF_W/(float)WinWeidth;
	printf("RATIO UP = %f DOWN = %f\n",ratioWindowSize,inverseRatio );
	SDL_RenderSetScale(renderer, ratioWindowSize, ratioWindowSize);
	/////////////////////////////////////////////////
	// AJOUT ALPHA MODE
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	struct classement donner[32];

  chargementDonner(0, "", 0, donner);

  /////////////////////////////////////////////////
  // CHARGEMENT ELEMENT
  TTF_Font *police = TTF_OpenFont("../room/neon.ttf" , NATIF_W*0.05);
  if(!police)
	{
		printf("Erreur chargement police\n" );
		return EXIT_FAILURE;
	}



  /////////////////////////////////////////////////
  int halt = SDL_FALSE;
  float _IPS;

	int selectionScrollingList = 0;
  int scrollPositionWindow = 0;
	int scrollPositionList = 0;
	int _SELECTION = 0;

	char rechercher[32]="";


	////////////////////////////////////////////////
	// CHARGER GLOBAL


  while(!halt)
  {
    /////////////////////////////////////////////////
    // TIME LANCEMENT FRAME
    int lancementFrameTime = SDL_GetTicks();

    //////////////////////////////////////////
    // GESTION EVENEMENT
		int xMouse = -1, yMouse = -1;

		//////////////////////////////////////////
		// SAVOIR SI ON SCROLL SUR LIST OU VUE
		scrollEvent(&halt,&xMouse,&yMouse,&scrollPositionWindow,&scrollPositionList, &_SELECTION, rechercher, donner, selectionScrollingList );

		_SELECTION = interactionInterface(xMouse,yMouse,_SELECTION,scrollPositionList,&selectionScrollingList, &scrollPositionWindow, donner);


    //////////////////////////////////////////
    // CODE HERE
    SDL_RenderClear(renderer);

		if(_SELECTION == SEARCH_PLAYER)
		{
			afficherCellule(renderer,0 ,rechercher,1,police,scrollPositionWindow + HUD_SIZE,1);
			for(int i = 0;  i< PRELOAD_CELLULE_MAX * AFFICHER_PLUS ; i++)
				afficherCellule(renderer,i + 1,donner[i].username,donner[i].score,police,scrollPositionWindow + HUD_SIZE,2);
		}
		else
		{
			for(int i = 0;  i< PRELOAD_CELLULE_MAX * AFFICHER_PLUS ; i++)
				afficherCellule(renderer,i,donner[i].username,donner[i].score,police,scrollPositionWindow + HUD_SIZE,0);
		}




		afficherHUD(renderer,police,rechercher,_SELECTION,scrollPositionList,selectionScrollingList);

		SDL_Rect rechercheJoueur = {NATIF_W*0.55, HUD_SIZE * 0.15, NATIF_W*0.35, HUD_SIZE*0.6};



		SDL_SetRenderDrawColor(renderer, BACKGROUND_C.r, BACKGROUND_C.g, BACKGROUND_C.b, 255);
    SDL_RenderPresent(renderer);
    //////////////////////////////////////////
    //END CODE
    //////////////////////////////////////////

    /////////////////////////////////////////////////
    // LIMITER LE NOMBRE DE FRAME A X FPS
    _limiterFrame(lancementFrameTime,&_IPS);


  }

  /////////////////////////////////////////////////
  // DESTRUCTION
  TTF_CloseFont(police);
  /////////////////////////////////////////////////
  return 0;
}

/*
int main()
{
  TTF_Init();
  SDL_Window *window = SDL_CreateWindow("Nineteen | Launcher", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1920,1080, SDL_WINDOW_OPENGL  );
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC |SDL_RENDERER_TARGETTEXTURE);

  SDL_Event Event;
  while(SDL_PollEvent(&Event));
  leaderboard(renderer,1920,1080);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_Quit();

}*/