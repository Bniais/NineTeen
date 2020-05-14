/////////////////////////////////////////////////
///	\file leaderboard.c
///	\author Samy M.
///	\version 1.0
///	\date 31 janvier 2020
///	\brief permet d'affciher le score du joueur
/////////////////////////////////////////////////

#include <stdio.h>
#include <math.h>
FILE *EXT_FILE;


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <ctype.h>

const SDL_Rect EMPLACEMENT_HUD = {0,0,1716,158};
const SDL_Rect EMPLCEMENT_CELLULE = {0,162,1610,112};
const SDL_Rect EMPLCEMENT_CELLULE_FLASH = {0,1200,1610,112};
const SDL_Rect EMPLACEMENT_CHAMPS = {0,398,612,102};
const SDL_Rect EMPLACEMENT_RECHERCHE = {258,281,102,102};
const SDL_Rect EMPLACEMENT_LIST = {387,281,102,102};
const SDL_Rect EMPLACEMENT_BG_LIST = {0,520,604,620};
const SDL_Rect EMPLACEMENT_EASY = {0,280,102,102};
const SDL_Rect EMPLACEMENT_HARD = {130,280,102,102};


#include "../../include/libWeb.h"

#define LIMITE_FPS 60
const float FRAME_TIME_LEADERBOARD = 1000.0/LIMITE_FPS*1.f;

#define NATIF_W 1280
#define NATIF_H 720
float inverseRatio = 1.f;


#define NOMBRE_CELLULE_FENETRE 10
#define PRELOAD_CELLULE_MAX 15
#define NOMBRE_CELLULE_LIST_SCROLL 6

#define NOMBRE_JEUX 16
int DONNER_CHARGER[NOMBRE_JEUX]={0};

int NOMBRE_JOUEUR_MAX = 0;

int AFFICHER_PLUS = 0;

const int HUD_SIZE = NATIF_W * 0.085;



const SDL_Color BACKGROUND_C = {10, 24, 40};
const float VITESSE_SCROLL = 30.0;


#define FRAME_RECHERCHE 25
const int ALPHA_FLASH[FRAME_RECHERCHE] = {13, 17, 22, 27, 30, 33,   35, 38, 38, 38, 38, 38,38, 38, 38, 38, 38, 38, 35, 33,   30, 27, 22, 17, 13};
///////////////////////////////////////////////
// LES SELECTION
#define SCREEN 0
#define LIST_SCROLL 1
#define LIST_SCOLL_ELEMENT 2
#define TEXT_FIELD_USERNAME 3
#define SEARCH_PLAYER 4
#define CHANGER_DIFFICULTER 5
///////////////////////////////////////////////
SDL_Rect boutonHardEasy ;
SDL_Point boutonPosition_OFF ;
SDL_Point boutonPosition_ON ;



enum {EASY,HARD,OFF,OFF_EASY,OFF_HARD};

int statutBoutonEASY_HARD = OFF;

/**
*\struct classement
*\brief Contient les scores et nom des joueur charger
*/
struct classement
{
	int score; /*!< \Score */
	char username[32];/*!< \Nom d'utulisateur */
};

#define FRAME_SCROLL_SMOOTH 8

/**
*\struct Scroll
*\brief Contient les informations sur un scroll
*/
typedef struct
{
	float pos; /*!< \Position actuel */
	float dest; /*!< \Destination */
	int frame; /*!< \Frame */
}Scroll;


#define NOMBRE_ELEMENT_LIST 7
const char nomList[NOMBRE_ELEMENT_LIST][30]={"GLOBAL","FLAPPY BIRD","TETRIS","ASTEROID","PACMAN","SNAKE","DEMINEUR"};

SDL_Color noirCOLOR = {0,0,0};
SDL_Color noirC = {50,50,50};



/////////////////////////////////////////////////////
/// \fn void concatenationChaine(char dest[], char source[], int debut)
/// \brief permet de copier une chaine au +n element de la chaine source
///
/// \param char dest[] chaine destination
/// \param char source[] chaine source
/// \param char debut indice debut de la copie
///
/// \return void
/////////////////////////////////////////////////////
void concatenationChaine(char dest[], char source[], int debut)
{
	int i,j;
	/////////////////////////////////////////////////////
	// COPIE i dans j
	for(i=debut , j =0 ; i < strlen(source) + 1; i++, j++)
		dest[j] = source[i];

}


/////////////////////////////////////////////////////
/// \fn int chargementDonner(int gameID, char *username, int offset,int limite, struct classement donner[])
/// \brief permet de charger les donner dans la structure
///
/// \param int gameID
/// \param char *username
/// \param int offset
/// \param int limite
/// \param struct classement donner[]
///
/// \return int nombre de nouveau ellement charger
/////////////////////////////////////////////////////
int chargementDonner(int gameID, char *username, int offset,int limite, struct classement donner[])
{

	/////////////////////////////////////////////////////
	// FIXE UNE LIMITE DE CHARGEMENT DE DONNER
	int limiteMAX = 0;
	if(offset + limite > NOMBRE_JOUEUR_MAX)
	{
		limiteMAX = offset + limite - NOMBRE_JOUEUR_MAX;
	}


	//////////////////////////////////
	// CAST
	char _gameID[5];
	sprintf(_gameID,"%d",gameID);

	char _offset[5];
	sprintf(_offset,"%d",offset);

	char _limite[5];
	sprintf(_limite,"%d",limite - limiteMAX);


	/////////////////////////////////////////////////////
	// ALLOCATION DE MEMOIRE
	char *retour = malloc(sizeof(char) * 2048);
	if(!retour)
	{
			fprintf(EXT_FILE,"leaderboard.c : chargementDonner() : malloc() \n" );
	}


	/////////////////////////////////////////////////////
	// RECHERCHER SI ON DEPASSE PAS LA LIMITE POSSIBLE DE CHARGEMENT DE JOUEUR
	if(limite - limiteMAX > 0)
	{
		/////////////////////////////////////////////////////
		// RECUPERATION DES DONNER
		while ( getLeaderboard(_gameID,username,_offset,_limite,retour) != EXIT_SUCCESS );

		/////////////////////////////////////////////////////
		// TRAITEMENT DES DONNER
		for(int i = offset ; i < (offset + limite - limiteMAX) ; i++ )
		{
			sscanf(retour,"%s %d",donner[i].username,&donner[i].score);

			/////////////////////////////////////////////////////
			// CALCUL DE LA LONGUEUR DE LA CHAINE RECU
			char lenghtDelete[256];
			sprintf(lenghtDelete,"%s %d",donner[i].username,donner[i].score);
			/////////////////////////////////////////////////////
			// SUPPRESSION DE LA CHAINE DEJA ETUDIER
			concatenationChaine(retour,retour,strlen(lenghtDelete)+1);

		}
	}

	/////////////////////////////////////////////////////
	// NETTOYAGE DE L'ALLOCATION
	free(retour);


	return limite - limiteMAX;
}


/////////////////////////////////////////////////////
/// \fn int _strcmpMin(char s1[], char s2[])
/// \brief permet de comparer de chaine caster en minature
///
/// \param char s1[]
/// \param char s2[]
///
/// \return 0 si egale
/////////////////////////////////////////////////////
int _strcmpMin(char s1[], char s2[])
{
	/////////////////////////////////////////
	// SI IL NE SONT PAS DE LA MEME LONGUEUR QUITTER
	if( strlen(s1) == strlen(s2) )
	{
		/////////////////////////////////////////
		// CASTER EN MINATURE LES DEUX CHAINES
		char tmp1[strlen(s1)];
		for(int i = 0; i < strlen(s1)+1 ; i++)
			tmp1[i] = tolower(s1[i]);

		char tmp2[strlen(s2)];
		for(int i = 0; i < strlen(s2)+1 ; i++)
			tmp2[i] = tolower(s2[i]);

		/////////////////////////////////////////
		// RETOURNER LA COMPARAISON DE CES 2 CHAINES
		return strcmp(tmp1,tmp2);
	}

	return -1;
}

/////////////////////////////////////////////////////
/// \fn int rechercherJoueur(char nomJoueur[], struct classement donner[], int selectionScrollingList)
/// \brief permet de rechercher un joueur dans une structure de donner de type classement
///
/// \param char nomJoueur[] nom du joueur rechercher
/// \param struct classement donner[] structure de donner
/// \param int selectionScrollingList liste sur la quelle on ce trouve
///
/// \return int indice de recherche diff -1 si trouver
/////////////////////////////////////////////////////
int rechercherJoueur(char nomJoueur[], struct classement donner[], int selectionScrollingList)
{
	/////////////////////////////////////////////////////
	// INITIALISATION DE L'INDICE
	int indice = -1;

	/////////////////////////////////////////////////////
	// RECHERCHE SUR TOUS LES JOUEUR
	for(int i = 0; i < NOMBRE_JOUEUR_MAX  ; i++)
	{

		/////////////////////////////////////////////////////
		// SI ON RECHERCHE DANS UN ESPACE NON CHARGER ON RECARGE 15 ELLEMENT
		if(i >= PRELOAD_CELLULE_MAX + AFFICHER_PLUS)
		{
			/////////////////////////////////////////////////////
			// INCREMENTATION DU NOMBRE D ELEMENT CHARGER
			AFFICHER_PLUS += chargementDonner(selectionScrollingList, "", PRELOAD_CELLULE_MAX+AFFICHER_PLUS,15,donner);
		}

		/////////////////////////////////////////////////////
		// RECHERCHE DES ELEMENTS
		if(!_strcmpMin(nomJoueur,donner[i].username))
		{
			/////////////////////////////////////////////////////
			// STOCK L INDICE SI TROUVER
			indice = i;
			/////////////////////////////////////////////////////
			// ARRET LA BOUCLE
			i = NOMBRE_JOUEUR_MAX+1;
		}

	}

	/////////////////////////////////////////////////////
	// RETOURNE RESULTAT
	return indice;
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

/////////////////////////////////////////////////////
/// \fn void gestionEvenement(int *halt, int *xMouse, int *yMouse, Scroll *scrollPositionWindow, Scroll *scrollPositionList, int *_SELECTION, char rechercher[], struct classement donner[], int selectionScrollingList)
/// \brief permet de gerer tous les evenements liee au jeu
///
/// \param int *halt stop le programme
/// \param int *xMouse coordonner souris x
/// \param int *yMouse coordonner souris y
/// \param Scroll *scrollPositionWindow position scroll sur la fenetre
/// \param Scroll *scrollPositionList position scroll dans la liste
/// \param int *_SELECTION connaitre l'evenement a gerer dans le programme principal
/// \param char rechercher[] chaine de text liee a la recherche
/// \param struct classement donner[] strucuteur de donner actuel
/// \param int selectionScrollingList element selectionner actuellement dans la liste
/// \param int *frameType les frames pour le curseur
/// \return void
/////////////////////////////////////////////////////
void gestionEvenement(int *halt, int *xMouse, int *yMouse, Scroll *scrollPositionWindow, Scroll *scrollPositionList, int *_SELECTION, char rechercher[], struct classement donner[], int selectionScrollingList, int *frameType)
{
	/////////////////////////////////////////////////////
	// INITIALISATION DE L EVENT
  SDL_Event event;
  while( SDL_PollEvent( &event ) )
  {
	    if(event.type == SDL_QUIT)
			*halt = SDL_TRUE;

		/////////////////////////////////////////////////////
		// SI APPUIE SUR UNE TOUCHE
		if(event.type == SDL_KEYDOWN)
		{
			/////////////////////////////////////////////////////
			// SI TOUCHE ESPACE
			if(event.key.keysym.sym == SDLK_ESCAPE)
		    {
				// QUITTER
		     	*halt = SDL_TRUE;

		    }

		}
		/////////////////////////////////////////////////////
		// SI EVENMENT DE ROULETTE SOURIS
    if(event.type == SDL_MOUSEWHEEL)
    {
			/////////////////////////////////////////
			// RECUPERER POSITION SOURIS POUR SAVOIR OU SCROLL SI LES DEUX VUE SCROLL SON OUVERTE
			int x,y;
			SDL_GetMouseState(&x,&y);
			/////////////////////////////////////////////////////
			// AJOUT DES RATIO
			x *= inverseRatio;
			y *= inverseRatio;
			/////////////////////////////////////////////////////
			// DIMENSION LISTE DEROULANTE
			SDL_Rect listDeroulante = {NATIF_W*0.05 , HUD_SIZE * 0.15, NATIF_W*0.35, HUD_SIZE*4.2};

			/////////////////////////////////////////////////////
			// CLIQUE PARTOUT SAUF DANS LISTE DEROULANTE
			if(*_SELECTION != LIST_SCROLL || !(x > listDeroulante.x && x < listDeroulante.x + listDeroulante.w && y > listDeroulante.y && y < listDeroulante.h + listDeroulante.y))
			{
				int scrollBefore = scrollPositionWindow->dest;
				/////////////////////////////////
				// UPDATE VIEW SCROLL
				scrollPositionWindow->dest += event.wheel.y * VITESSE_SCROLL;
				/////////////////////////////////
				// DEPASSEMENT HAUT
				if(scrollPositionWindow->dest > 0)
					scrollPositionWindow->dest = 0;

				/////////////////////////////////////////////////////
				// LIMITE DU SCROLL AU NOMBRE MAX D ELEMENT DANS LA LISTE
				else if (scrollPositionWindow->dest < -(int)(  (NATIF_H/10) * ( (PRELOAD_CELLULE_MAX + AFFICHER_PLUS) - NOMBRE_CELLULE_FENETRE  ) + HUD_SIZE ) )
				{
					scrollPositionWindow->dest = -(int)(  (NATIF_H/10) * ( (PRELOAD_CELLULE_MAX + AFFICHER_PLUS) - NOMBRE_CELLULE_FENETRE  ) + HUD_SIZE );

					////////////////////////////////////////////////////
					// ON RECHARGE DE NOUVEAU ELEMENT
					AFFICHER_PLUS += chargementDonner(selectionScrollingList, "", PRELOAD_CELLULE_MAX+AFFICHER_PLUS,15,donner);

				}

				if(scrollBefore != scrollPositionWindow->dest)
					scrollPositionWindow->frame = FRAME_SCROLL_SMOOTH;


			}
			/////////////////////////////////////////////////////
			// SCROLL SUR LA LISTE
			else
			{
				int scrollBefore = scrollPositionList->dest;
				/////////////////////////////////
				// UPDATE VIEW SCROLL
				scrollPositionList->dest += event.wheel.y;
				// DEPASSEMENT HAUT
				if(scrollPositionList->dest > 0)
					scrollPositionList->dest = 0;
				// DEPASSEMENT BAS
				else if (scrollPositionList->dest < -(int)( HUD_SIZE*0.60 * (NOMBRE_ELEMENT_LIST - NOMBRE_CELLULE_LIST_SCROLL - 1) ) ) // -1 POUR SELECTION ACTUEL
					scrollPositionList->dest = -(int)( HUD_SIZE*0.60 * (NOMBRE_ELEMENT_LIST - NOMBRE_CELLULE_LIST_SCROLL - 1) );

				if( scrollPositionList->dest != scrollBefore)
					scrollPositionList->frame = FRAME_SCROLL_SMOOTH;
			}

    }

		/////////////////////////////////////////////////////
		// CLIQUE AVEC LA SOURIS
		// RECUPERE LA POSITION DE LA SOURIE QUAND ON CLIQUE
		if(event.type == SDL_MOUSEBUTTONUP)
		{
			SDL_GetMouseState(xMouse,yMouse);
			*xMouse *= inverseRatio;
			*yMouse *= inverseRatio;
		}

		/////////////////////////////////////////////////////
		// SI SELECTION EST SUR CHAMPS DE TEXT
			switch (event.type)
			{
				/////////////////////////////////////////////////////
				// RECUPERER LES TOUCHES
				case SDL_TEXTINPUT:
					 strcat(rechercher, event.text.text);
					 *frameType = 0;
					 break;
				 /////////////////////////////////////////////////////
	 			// AUTRE TOUCHE NON TEXT
				case SDL_KEYDOWN:{
					/////////////////////////////////////////////////////
					// EFFACER
					if(event.key.keysym.sym == SDLK_BACKSPACE)
					{
						*frameType = 0;
						if(strlen(rechercher) > 0)
							rechercher[strlen(rechercher) - 1] = '\0';
					}
					/////////////////////////////////////////////////////
					// ENTRE
					if(event.key.keysym.sym == SDLK_RETURN)
					{
						*frameType = 0;
						if(strlen(rechercher) > 0)
						{
							*_SELECTION = SEARCH_PLAYER;
						}

					}

			}break;
			default:break;
		}




  }

}


/////////////////////////////////////////////////////
/// \fn void ecrireText(SDL_Renderer* renderer, TTF_Font* font, char * text, SDL_Color couleur, int xPosition, int yPosition, float scale, int estHUD, int frameType)
/// \brief permet d'ecrire du text a l'ecran
///
/// \param SDL_Renderer* renderer pointeur sur le rendu
/// \param TTF_Font* font pointeur sur la police
/// \param char * text pointeur sur le text
/// \param SDL_Color couleur couleur du text
/// \param int xPosition position -1 pour centrer
/// \param int yPosition position -1 pour centrer
/// \param float scale scale du text
/// \param int estHUD est pour l'HUD
/// \param int frameType frame pour le curseur
///
/// \return void
/////////////////////////////////////////////////////
void ecrireText(SDL_Renderer* renderer, TTF_Font* font, char * text, SDL_Color couleur, int xPosition, int yPosition, float scale, int estHUD, int frameType){

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




	if(frameType != -1 && frameType%(int)(LIMITE_FPS*1.4) < ((LIMITE_FPS*1.4)/2)){
		SDL_Rect curseur = dest;
		curseur.x += dest.w;
		curseur.w = 4;
		curseur.y += curseur.h/12;
		curseur.h -= curseur.h/3;
		curseur.h --;
		SDL_SetRenderDrawColor(renderer, 0,0,0,255);
		SDL_RenderFillRect(renderer, &curseur);
	}



	SDL_RenderCopy(renderer, Message, &src, &dest);
	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
}

Uint8 brillance(Uint8 color, int power)
{
    return (Uint8) (255 * SDL_pow((double) color / 255, power));
}

/////////////////////////////////////////////////////
/// \fn void afficherCellule(SDL_Renderer *renderer,SDL_Texture *texture, int cellulePosition, char *username , int score, TTF_Font * police, int decallageY)
/// \brief affiche une cellule
///
/// \param SDL_Renderer* renderer pointeur sur le rendu
/// \param SDL_Texture *texture
/// \param int cellulePosition
/// \param struct classement donner
/// \param TTF_Font * police
/// \param int decallageY
///
/// \return EXIT_SUCCESS/EXIT_FAILURE
/////////////////////////////////////////////////////
int afficherCellule(SDL_Renderer *renderer,SDL_Texture *texture, int cellulePosition, struct classement donner, TTF_Font * police, int decallageY, int frameRecherche)
{

	/////////////////////////////////////////////////////
	// CASTER LES DONNER EN CHAR
	char _cellulePosition[12];
	sprintf(_cellulePosition,"%d",cellulePosition + 1);
	char _score[12];
	sprintf(_score,"%d",donner.score);
	/////////////////////////////////////////////////////

	/////////////////////////////////////////////////////
	// DIMENSION CELLULE
	SDL_Rect cellule = {NATIF_W*0.03, (NATIF_H/10) * cellulePosition + decallageY,NATIF_W*0.94 , NATIF_H * 0.09 + NATIF_H * 0.025 };

	if( SDL_RenderCopy(renderer,texture,&EMPLCEMENT_CELLULE,&cellule) )
	{
			fprintf(EXT_FILE,"leaderboard : afficherCellule() :SDL_RenderCopy ERR %s\n",SDL_GetError() );

		return EXIT_FAILURE;
	}

	if(frameRecherche>=0 && frameRecherche<FRAME_RECHERCHE){
		SDL_SetTextureAlphaMod(texture, ALPHA_FLASH[frameRecherche]*4);
		if( SDL_RenderCopy(renderer,texture,&EMPLCEMENT_CELLULE_FLASH,&cellule) )
		{
			fprintf(EXT_FILE,"leaderboard : afficherCellule() :SDL_RenderCopy flash ERR %s\n",SDL_GetError() );
			return EXIT_FAILURE;
		}
		SDL_SetTextureAlphaMod(texture, 255);
	}

	/////////////////////////////////////////////////////
	// ECRIRE LE TEXT
	ecrireText(renderer,police, _cellulePosition, noirCOLOR, NATIF_W*0.08, (NATIF_H/10) * cellulePosition + (cellule.h/2) + decallageY , 0.5 , 0, -1);
	ecrireText(renderer,police, donner.username, noirCOLOR,-1, (NATIF_H/10) * cellulePosition + (cellule.h/2) + decallageY , 0.5 , 0, -1);
	ecrireText(renderer,police, _score, noirCOLOR, NATIF_W*0.9 , (NATIF_H/10) * cellulePosition + (cellule.h/2) + decallageY, 0.5 , 0, -1);

	return EXIT_SUCCESS;
}


/////////////////////////////////////////////////////
/// \fn void afficherCelluleScrollView(SDL_Renderer *renderer, int cellulePosition, char *text, TTF_Font *police,int decallageY)
/// \brief affiche une / separateur entre des listes
///
/// \param SDL_Renderer* renderer pointeur sur le rendu
/// \param int cellulePosition
/// \param  char *text
/// \param  TTF_Font *police
/// \param int decallageY
///
/// \return void
/////////////////////////////////////////////////////
void afficherCelluleScrollView(SDL_Renderer *renderer, int cellulePosition, char *text, TTF_Font *police,int decallageY)
{
	SDL_Rect cellule = { NATIF_W*0.05 , ( ( HUD_SIZE*0.6) * (cellulePosition + 1) ) + decallageY , NATIF_W*0.35 , HUD_SIZE*0.01 };
	cellule.y += HUD_SIZE*0.15 ;
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, &cellule);
	/////////////////////////////////////////////////////
	// ECRIRE LE TEXT
	ecrireText(renderer,police, text, noirCOLOR, NATIF_W*0.05 + (cellule.w/2), cellule.y  + (HUD_SIZE*0.6)/2 + NATIF_H*0.01 , 0.8 , 1, -1);
}

/////////////////////////////////////////////////////
/// \fn int afficherHUD(SDL_Renderer *renderer,SDL_Texture *texture, TTF_Font *police , char *recherche, int _SELECTION, int decallageY,int selectionScrollingList, int frameType)
/// \brief affiche l'HUD complet
///
/// \param SDL_Renderer* renderer pointeur sur le rendu
/// \param SDL_Texture *texture
/// \param TTF_Font *police
/// \param char *recherche
/// \param int _SELECTION
/// \param int decallageY
/// \param int selectionScrollingList
/// \param int frameType
/// \return int
/////////////////////////////////////////////////////
int afficherHUD(SDL_Renderer *renderer,SDL_Texture *texture, TTF_Font *police , char *recherche, int _SELECTION, int decallageY,int selectionScrollingList, int frameType)
{


	////////////////////////////////////////////
	// HUD BACKGROUND
	SDL_Rect hudBackground = {NATIF_W*0.0, NATIF_H * 0.0 ,NATIF_W*1.0 , HUD_SIZE + NATIF_H*0.025 };
	if ( SDL_RenderCopy(renderer, texture, &EMPLACEMENT_HUD, &hudBackground) )
	{
			fprintf(EXT_FILE,"leaderboard : afficherHUD() :SDL_RenderCopy ERR %s\n",SDL_GetError() );

		return EXIT_FAILURE;
	}



	////////////////////////////////////////////
	// CHAMPS RECHERCHER JOUEUR
	SDL_Rect rechercheJoueur = {NATIF_W*0.55, HUD_SIZE * 0.15, NATIF_W*0.35, HUD_SIZE*0.6 + NATIF_H * 0.015};
	if ( SDL_RenderCopy(renderer,texture,&EMPLACEMENT_CHAMPS,&rechercheJoueur) )
	{
			fprintf(EXT_FILE,"leaderboard : afficherHUD() :SDL_RenderCopy ERR %s\n",SDL_GetError() );

		return EXIT_FAILURE;
	}

	////////////////////////////////////////////
	// BOUTON RECHERCHER JOUEUR
	SDL_Rect lancerRecherche = {NATIF_W*0.897, HUD_SIZE * 0.15, HUD_SIZE*0.6, HUD_SIZE*0.6 + NATIF_H * 0.015};
	if ( SDL_RenderCopy(renderer,texture,&EMPLACEMENT_RECHERCHE,&lancerRecherche) )
	{
			fprintf(EXT_FILE,"leaderboard : afficherHUD() :SDL_RenderCopy ERR %s\n",SDL_GetError() );
		return EXIT_FAILURE;
	}

	// ECRIRE RECHERCHE ACTUEL
	if(strlen(recherche) < 1)
		ecrireText(renderer,police, " ", noirC, rechercheJoueur.x + rechercheJoueur.w/2, rechercheJoueur.y + rechercheJoueur.h/2 + NATIF_H*0.01 , 0.8 , 0, frameType);
	else
		ecrireText(renderer,police, recherche, noirCOLOR, rechercheJoueur.x + rechercheJoueur.w/2, rechercheJoueur.y + rechercheJoueur.h/2 + NATIF_H*0.01 , 0.8 , 0, frameType);


	////////////////////////////////////////////
	// AFFICHER BOUTON EASY/HARD
	if(statutBoutonEASY_HARD == EASY || statutBoutonEASY_HARD == OFF_EASY)
	{
		if( SDL_RenderCopy(renderer,texture,&EMPLACEMENT_EASY,&boutonHardEasy) )
		{
				fprintf(EXT_FILE,"leaderboard : afficherHUD() :SDL_RenderCopy ERR %s\n",SDL_GetError() );

			return EXIT_FAILURE;
		}
	}
	else if (statutBoutonEASY_HARD == HARD || statutBoutonEASY_HARD == OFF_HARD)
	{
		if ( SDL_RenderCopy(renderer,texture,&EMPLACEMENT_HARD,&boutonHardEasy) )
		{
				fprintf(EXT_FILE,"leaderboard : afficherHUD() :SDL_RenderCopy ERR %s\n",SDL_GetError() );

			return EXIT_FAILURE;
		}
	}



	////////////////////////////////////////////
	// LIST DEROULTANTE
	if(_SELECTION == LIST_SCROLL)
	{
		SDL_Rect listDeroulante = {NATIF_W*0.05 , HUD_SIZE * 0.15, NATIF_W*0.35, HUD_SIZE*4.2 + NATIF_H * 0.015};
		if ( SDL_RenderCopy(renderer,texture,&EMPLACEMENT_BG_LIST,&listDeroulante) )
		{
				fprintf(EXT_FILE,"leaderboard : afficherHUD() :SDL_RenderCopy ERR %s\n",SDL_GetError() );

			return EXIT_FAILURE;
		}

		////////////////////////////////////////
		// PERMET DE SAUTER l AFFICHAGE DE L ELEMENT ACTUEL
		int sauterValeurActuellementAffichier = 0;
		for(int i = 0  ; i < NOMBRE_CELLULE_LIST_SCROLL ; i ++)
		{
			if(i + (int)fabs(decallageY/HUD_SIZE*0.60) == selectionScrollingList)
				sauterValeurActuellementAffichier = 1;
			afficherCelluleScrollView(renderer,i + fabs(decallageY/HUD_SIZE*0.60),(char*)nomList[i + (int)fabs(decallageY/HUD_SIZE*0.60) + sauterValeurActuellementAffichier ],police , decallageY );
		}
	}


	// CHAMPS LISTE ACTUEL
	SDL_Rect listSelection = {NATIF_W*0.05 , HUD_SIZE * 0.15, NATIF_W*0.35, HUD_SIZE*0.6 + NATIF_H * 0.015};
	if ( SDL_RenderCopy(renderer,texture,&EMPLACEMENT_CHAMPS,&listSelection) )
	{
			fprintf(EXT_FILE,"leaderboard : afficherHUD() :SDL_RenderCopy ERR %s\n",SDL_GetError() );

		return EXIT_FAILURE;
	}

	// BOUTON CHAMPS LISTE
	SDL_Rect ouvrirListeSelection = {NATIF_W*0.047 + listSelection.w, HUD_SIZE * 0.15, HUD_SIZE*0.6, HUD_SIZE*0.6 + NATIF_H*0.015};
	if ( SDL_RenderCopy(renderer,texture,&EMPLACEMENT_LIST,&ouvrirListeSelection) )
	{
			fprintf(EXT_FILE,"leaderboard : afficherHUD() :SDL_RenderCopy ERR %s\n",SDL_GetError() );

		return EXIT_FAILURE;
	}

	// ECRIRE LE TEXT DES CHAMPS
	if(selectionScrollingList < 7)
		ecrireText(renderer,police, (char*)nomList[selectionScrollingList], noirCOLOR, listSelection.x + listSelection.w/2, listSelection.y + listSelection.h/2 + NATIF_H*0.01 , 0.8 , 0,-1);
	else
		ecrireText(renderer,police, (char*)nomList[(12-selectionScrollingList) + 1], noirCOLOR, listSelection.x + listSelection.w/2, listSelection.y + listSelection.h/2 + NATIF_H*0.01 , 0.8 , 0,-1);

	return EXIT_SUCCESS;
}


/////////////////////////////////////////////////////
/// \fn int interactionInterface(int x,int y, int _SELECTION, Scroll scrollPositionList, int *selectionScrollingList, Scroll *scrollPositionWindow, struct classement donner[NOMBRE_JEUX][NOMBRE_JOUEUR_MAX])
/// \brief permet d'interagire avec l'interface
///
/// \param int x
/// \param int y
/// \param int _SELECTION
/// \param Scroll scrollPositionList
/// \param int *selectionScrollingList
/// \param Scroll *scrollPositionWindow
/// \param struct classement donner[][]
///
/// \return int
/////////////////////////////////////////////////////
int interactionInterface(int x,int y, int _SELECTION, Scroll scrollPositionList, int *selectionScrollingList, Scroll *scrollPositionWindow, struct classement donner[NOMBRE_JEUX][NOMBRE_JOUEUR_MAX]){

	SDL_Rect ouvrirListeSelection = {NATIF_W*0.05 + NATIF_W*0.35, HUD_SIZE * 0.15, HUD_SIZE*0.6, HUD_SIZE*0.6};
	SDL_Rect lancerRecherche = {NATIF_W*0.90, HUD_SIZE * 0.15, HUD_SIZE*0.6, HUD_SIZE*0.6};
	SDL_Rect rechercheJoueur = {NATIF_W*0.55, HUD_SIZE * 0.15, NATIF_W*0.35, HUD_SIZE*0.6};
	SDL_Rect listDeroulante = {NATIF_W*0.05 , HUD_SIZE * 0.15, NATIF_W*0.35, HUD_SIZE*4.0};
	SDL_Rect listSelection = {NATIF_W*0.05 , HUD_SIZE * 0.15, NATIF_W*0.35, HUD_SIZE*0.6};

	/////////////////////////////////////////////////////
	// SI IL Y A EUX CLIQUE DIF de -1 -1
	if(x != -1 && y!= -1)
	{
		/////////////////////////////////////////////////////
		// CLIQUE POUR OUVRIR LA SELECTION
		if( (x > ouvrirListeSelection.x && x < ouvrirListeSelection.x + ouvrirListeSelection.w && y > ouvrirListeSelection.y && y < ouvrirListeSelection.h + ouvrirListeSelection.y)
				||
				(x > listSelection.x && x < listSelection.x + listSelection.w && y > listSelection.y && y < listSelection.h + listSelection.y && _SELECTION != LIST_SCROLL) 	)
		{
			if(_SELECTION == LIST_SCROLL)
				return LIST_SCOLL_ELEMENT;
			return LIST_SCROLL;
		}
		/////////////////////////////////////////////////////
		// CLIQUER SUR RECHERCHER UN JOUEUR
		else if( x > lancerRecherche.x && x < lancerRecherche.x + lancerRecherche.w && y > lancerRecherche.y && y < lancerRecherche.h + lancerRecherche.y)
		{
			scrollPositionWindow->dest = 0;
			scrollPositionWindow->frame = FRAME_SCROLL_SMOOTH;
			return SEARCH_PLAYER;
		}
		/////////////////////////////////////////////////////
		// CLIQUE SUR LE CHAMPS TEXT DU JOUEUR
		else if( x > rechercheJoueur.x && x < rechercheJoueur.x + rechercheJoueur.w && y > rechercheJoueur.y && y < rechercheJoueur.h + rechercheJoueur.y)
		{
			return TEXT_FIELD_USERNAME;
		}
		/////////////////////////////////////////////////////
		// CLIQUE DANS LA SELECTION SI CAS LIST SCROLL
		else if ( x > listDeroulante.x && x < listDeroulante.x + listDeroulante.w && y > listDeroulante.y && y < listDeroulante.h + listDeroulante.y && _SELECTION == LIST_SCROLL)
		{


			/////////////////////////////////////////////////////
			// CLIQUER SUR LA SELECTION ACTUEL
			if(x > listSelection.x && x < listSelection.x + listSelection.w && y > listSelection.y && y < listSelection.h + listSelection.y);

			else
			{
				// HUD_SIZE*0.6 // TAILLE D'UNE CELLULE
				// HAUT CELLULE = listSelection.h + listSelection.y

				/////////////////////////////////////////////////////
				// CALCUL DE LA POSTION DE L ELEMENT SELECTIONNER
				int ElementSelection = (int)( (y - (listSelection.h+listSelection.y + scrollPositionList.pos) )/(HUD_SIZE*0.6) );
				if(*selectionScrollingList <= ElementSelection)
					*selectionScrollingList = ElementSelection + 1;
				else
					*selectionScrollingList = ElementSelection ;


				////////////////////////////////////////////////////
				// ON RECHARGE DE NOUVEAU ELEMENT
				scrollPositionWindow->dest = 0;
				scrollPositionWindow->frame = FRAME_SCROLL_SMOOTH;
				AFFICHER_PLUS = 0;
				chargementDonner(*selectionScrollingList, "", 0,15,donner[*selectionScrollingList]);

				/////////////////////////////////////////////////////
				// DETERMINER SI CHARGEMENT AVEC MODE EASY OU HARD
				if(*selectionScrollingList)
				{
					if(statutBoutonEASY_HARD == OFF)
					{
						if(*selectionScrollingList <= 6)
							statutBoutonEASY_HARD = HARD;
						else
							statutBoutonEASY_HARD = EASY;
					}
				}
				else
				{
					if(statutBoutonEASY_HARD != OFF)
					{
						statutBoutonEASY_HARD = OFF;
					}

				}

			}

			return LIST_SCOLL_ELEMENT;
		}

		/////////////////////////////////////////////////////
		// APPUI SUR BOUTON DIFFICULTER
		else if (x > boutonHardEasy.x && x < boutonHardEasy.x + boutonHardEasy.w && y > boutonHardEasy.y && y < boutonHardEasy.h + boutonHardEasy.y)
		{
			if(statutBoutonEASY_HARD == EASY)
			{
				statutBoutonEASY_HARD = OFF_EASY;
			}

			else if(statutBoutonEASY_HARD == HARD)
			{
				statutBoutonEASY_HARD = OFF_HARD;
			}

			return CHANGER_DIFFICULTER;
		}

		/////////////////////////////////////////////////////
		// CLIQUE SUR L ECRAN AILLEUR DE TOUS CE QUI PERMET UNE INTERACTION
		else
		{
			return SCREEN;
		}

	}

	return _SELECTION;
}


/////////////////////////////////////////////////////
// MAIN
int leaderboard(SDL_Renderer *renderer,int WinWeidth , int WinHeight, int _MAX_JOUEUR)
{
	AFFICHER_PLUS = 0;
	NOMBRE_JOUEUR_MAX = _MAX_JOUEUR;
	boutonHardEasy.x = NATIF_W*0.398;
	boutonHardEasy.y = HUD_SIZE * 0.15;
	boutonHardEasy.w =  HUD_SIZE*0.6;
	boutonHardEasy.h =  HUD_SIZE*0.6 + NATIF_H * 0.015;

	boutonPosition_OFF.x = NATIF_W*0.398;
	boutonPosition_OFF.y = HUD_SIZE * 0.15;

	boutonPosition_ON.x =  NATIF_W*0.44;
	boutonPosition_ON.y = HUD_SIZE * 0.15;


	/////////////////////////////////////////////////
	// EFFACER EVENEMENT VENANT DE LA ROOM
	SDL_Event Ev;
	while (SDL_PollEvent(&Ev));
	/////////////////////////////////////////////////


	/////////////////////////////////////////////////
	// CHARGEMENT ELEMENT
	SDL_Texture *texture = IMG_LoadTexture(renderer,"../room/leaderboard/tilset.png");
	if(!texture)
	{

			fprintf(EXT_FILE,"leaderboard.c -> leaderboard() : IMG_LoadTexture : ../room/leaderboard/tilset.png\n" );

		return EXIT_FAILURE;
	}
  TTF_Font *police = TTF_OpenFont("../room/fonts/neon.ttf" , NATIF_W*0.05);
  if(!police)
	{
			fprintf(EXT_FILE,"leaderboard.c -> leaderboard() : TTF_OpenFont : ../room/fonts/neon.ttf\n" );

		return EXIT_FAILURE;
	}
	/////////////////////////////////////////////////

	/////////////////////////////////////////////////
	// REDIMENSION DE LA FENETRE
	float ratioWindowSize = (float)WinWeidth/(float)NATIF_W;
	inverseRatio = (float)NATIF_W/(float)WinWeidth;
	if ( SDL_RenderSetScale(renderer, ratioWindowSize, ratioWindowSize) )
	{
			fprintf(EXT_FILE,"leaderboard.c -> leaderboard() : SDL_RenderSetScale : %s\n",SDL_GetError() );

		return EXIT_FAILURE;
	}

	/////////////////////////////////////////////////
	// AJOUT ALPHA MODE
	if ( SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND) )
	{
			fprintf(EXT_FILE,"leaderboard.c -> leaderboard() : SDL_SetRenderDrawBlendMode : %s\n",SDL_GetError() );

		return EXIT_FAILURE;
	}

	/////////////////////////////////////////////////
	// INIT
	struct classement donner[NOMBRE_JEUX][NOMBRE_JOUEUR_MAX];
	/////////////////////////////////////////////////
	// CHARGEMENT DES DONNERS
	chargementDonner(0, "", 0,PRELOAD_CELLULE_MAX, donner[0] );
  /////////////////////////////////////////////////
  int halt = SDL_FALSE;
  float _IPS;
	/////////////////////////////////////////////////
	int selectionScrollingList = 0;
    Scroll scrollPositionWindow = {0,0,0};
	Scroll scrollPositionList = {0,0,0};
	int _SELECTION = 0;
	int indice = -1;

	int frameRecherche = 0;
	long int frameType = 0;

	/////////////////////////////////////////////////
	char rechercher[32]="";
	////////////////////////////////////////////////



  while(!halt)
  {
    /////////////////////////////////////////////////
    // TIME LANCEMENT FRAME
    int lancementFrameTime = SDL_GetTicks();

		//////////////////////////////////////////
		// CODE HERE

    //////////////////////////////////////////
    // GESTION EVENEMENT
		int xMouse = -1, yMouse = -1;

		//////////////////////////////////////////
		// RECUPERER TOUS LES EVENTS POSSIBLE
		gestionEvenement(&halt,&xMouse,&yMouse,&scrollPositionWindow,&scrollPositionList, &_SELECTION, rechercher, donner[selectionScrollingList], selectionScrollingList, &frameType );
		/////////////////////////////////////////////////
		// RECUPERER LA SELECTION
		_SELECTION = interactionInterface(xMouse,yMouse,_SELECTION,scrollPositionList,&selectionScrollingList, &scrollPositionWindow, donner);

    if( SDL_RenderClear(renderer) )
		{
				fprintf(EXT_FILE,"leaderboard.c -> leaderboard() : SDL_RenderClear : %s\n",SDL_GetError() );

			return EXIT_FAILURE;
		}


		for(int i = 0;  i< PRELOAD_CELLULE_MAX + AFFICHER_PLUS ; i++)
		{
			if( afficherCellule(renderer,texture,i,donner[selectionScrollingList][i],police,roundf(scrollPositionWindow.pos) + HUD_SIZE, i == indice ? frameRecherche-1 : -1) != EXIT_SUCCESS)
			{
					fprintf(EXT_FILE,"leaderboard.c -> leaderboard() : afficherCellule()" );

				return EXIT_FAILURE;
			}
		}


		if ( afficherHUD(renderer,texture,police,rechercher,_SELECTION,roundf(scrollPositionList.pos),selectionScrollingList, frameType) != EXIT_SUCCESS)
		{
				fprintf(EXT_FILE,"leaderboard.c -> leaderboard() : afficherHUD()" );

			return EXIT_FAILURE;
		}



		if ( SDL_SetRenderDrawColor(renderer, BACKGROUND_C.r, BACKGROUND_C.g, BACKGROUND_C.b, 255) )
		{
				fprintf(EXT_FILE,"leaderboard.c -> leaderboard() : afficherHUD()" );

			return EXIT_FAILURE;
		}

    SDL_RenderPresent(renderer);

		frameType ++;

		//anim recherche
		if(frameRecherche)
			frameRecherche--;

		//Scroll smooth
		if(scrollPositionList.frame){
			scrollPositionList.pos += (scrollPositionList.dest - scrollPositionList.pos) / scrollPositionList.frame;
		}

		if(scrollPositionWindow.frame){
			scrollPositionWindow.pos += (scrollPositionWindow.dest - scrollPositionWindow.pos) / scrollPositionWindow.frame;
		}

		// CALCUL NOUVELLE POSITION BOUTON EASY/HARD
		switch (statutBoutonEASY_HARD) {
			// ANIMATION DES BOUTON HARD / EASY
			case OFF:{
				if(boutonHardEasy.x > boutonPosition_OFF.x)
					boutonHardEasy.x -= ( (boutonPosition_ON.x - boutonPosition_OFF.x)/10 ) * (60/_IPS); // RETIRER UNE VALEUR D ANIMATION
				else if (boutonHardEasy.x != boutonPosition_OFF.x)
					boutonHardEasy.x = boutonPosition_OFF.x;
			};break;




			case OFF_EASY:{
				if(boutonHardEasy.x > boutonPosition_OFF.x)
					boutonHardEasy.x -= ( (boutonPosition_ON.x - boutonPosition_OFF.x)/10 ) * (60/_IPS); // RETIRER UNE VALEUR D ANIMATION
				else if (boutonHardEasy.x != boutonPosition_OFF.x)
					boutonHardEasy.x = boutonPosition_OFF.x;
				else
				{
					selectionScrollingList = 13 - selectionScrollingList;
					////////////////////////////////////////////////////
					// ON RECHARGE DE NOUVEAU ELEMENT
					AFFICHER_PLUS = 0;
					chargementDonner(selectionScrollingList, "", 0,15,donner[selectionScrollingList]);
					statutBoutonEASY_HARD = HARD;
				}
			};break;

			case EASY:{
				if(boutonHardEasy.x < boutonPosition_ON.x)
					boutonHardEasy.x += ( (boutonPosition_ON.x - boutonPosition_OFF.x)/10 ) * (60/_IPS); // RETIRER UNE VALEUR D ANIMATION
				else if (boutonHardEasy.x != boutonPosition_OFF.x)
					boutonHardEasy.x = boutonPosition_ON.x;
			};break;



			case OFF_HARD:{
				if(boutonHardEasy.x > boutonPosition_OFF.x)
					boutonHardEasy.x -= ( (boutonPosition_ON.x - boutonPosition_OFF.x)/10 ) * (60/_IPS); // RETIRER UNE VALEUR D ANIMATION
				else if (boutonHardEasy.x != boutonPosition_OFF.x)
					boutonHardEasy.x = boutonPosition_OFF.x;
				else
				{
					selectionScrollingList =  (12-selectionScrollingList) + 1;
					////////////////////////////////////////////////////
					// ON RECHARGE DE NOUVEAU ELEMENT
					AFFICHER_PLUS = 0;
					chargementDonner(selectionScrollingList, "", 0,15,donner[selectionScrollingList]);
					statutBoutonEASY_HARD = EASY;
				}
			};break;

			case HARD:{
				if(boutonHardEasy.x < boutonPosition_ON.x)
					boutonHardEasy.x += ( (boutonPosition_ON.x - boutonPosition_OFF.x)/10 ) * (60/_IPS); // RETIRER UNE VALEUR D ANIMATION
				else if (boutonHardEasy.x != boutonPosition_OFF.x)
					boutonHardEasy.x = boutonPosition_ON.x;
			};break;

		}



		/////////////////////////////////////////
		// RECHERCHE DU JOUEUR SI NECESSAIRE
		if(_SELECTION == SEARCH_PLAYER)
		{
			indice = rechercherJoueur(rechercher, donner[selectionScrollingList],selectionScrollingList);
			if( indice != -1)
			{
				frameRecherche = FRAME_RECHERCHE ;
				int scrollBefore = scrollPositionWindow.dest;
				//////////////////////////////////////////////////////
				// CIBLER SUR scrollPositionWindow
				scrollPositionWindow.dest = -(  (NATIF_H/10) * indice   );

				//////////////////////////////////////////////////////
				// VERIFIER QUE CA DEPASSE PAS LA TAILLE MAX
				if (scrollPositionWindow.dest < -(int)(  (NATIF_H/10) * ( (PRELOAD_CELLULE_MAX + AFFICHER_PLUS) - NOMBRE_CELLULE_FENETRE  ) + HUD_SIZE ) )
				{
					scrollPositionWindow.dest = -(int)(  (NATIF_H/10) * ( (PRELOAD_CELLULE_MAX + AFFICHER_PLUS) - NOMBRE_CELLULE_FENETRE  ) + HUD_SIZE );

					////////////////////////////////////////////////////
					// ON RECHARGE DE NOUVEAU ELEMENT
					AFFICHER_PLUS += chargementDonner(selectionScrollingList, "", PRELOAD_CELLULE_MAX+AFFICHER_PLUS,15,donner[selectionScrollingList]);

					printf("ELEMENT CHARGER = %d\n",AFFICHER_PLUS + PRELOAD_CELLULE_MAX );
				}
				if(scrollPositionWindow.dest != scrollBefore){
					frameRecherche += FRAME_SCROLL_SMOOTH/1.8;
					scrollPositionWindow.frame = FRAME_SCROLL_SMOOTH/2.2;
				}


			}
			_SELECTION = SCREEN;
		}


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
	police = NULL;
	SDL_DestroyTexture(texture);
	texture = NULL;
  /////////////////////////////////////////////////
  return 0;
}


/*
int main()
{
  TTF_Init();
  SDL_Window *window = SDL_CreateWindow("Nineteen | Launcher", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,1280,720, SDL_WINDOW_OPENGL  );
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC |SDL_RENDERER_TARGETTEXTURE);

  SDL_Event Event;
  while(SDL_PollEvent(&Event));
  leaderboard(renderer,1280,720,41);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_Quit();

}*/
