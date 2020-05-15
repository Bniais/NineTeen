/////////////////////////////////////////////////
///	\file main.c
///	\author Samy M.
///	\version 1.0
///	\date 31 janvier 2020
///	\brief regroupe le launcher ainsi que le programme principal du jeu
/////////////////////////////////////////////////

#define VERSION_LOGICIEL "version=1.1.1"
#define VERSION "1.1.1"

#ifdef _WIN32
#define WESNOTH_EXPORT __declspec(dllexport)
WESNOTH_EXPORT unsigned long NvOptimusEnablement = 0x00000001;
WESNOTH_EXPORT int AmdPowerXpressRequestHighPerformance = 1;
#elif defined __APPLE__
#define WESNOTH_EXPORT __attribute__((visibility("default")))
WESNOTH_EXPORT unsigned long NvOptimusEnablement = 0x00000001;
WESNOTH_EXPORT int AmdPowerXpressRequestHighPerformance = 1;
#endif


#include <stdio.h>
#define MODE_DEV 0
extern FILE *EXT_FILE;
extern int MACOS_VER;


#include <stdio.h>
#include <stdlib.h>
#include "SDL2/SDL_thread.h"
#ifdef _WIN32
    #include<winsock2.h>
    #include<windows.h>
#endif


//HELLO

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include "include/textField.h"
#include "include/fullpath.h"
#include "include/communFunctions.h"
#include "include/libWeb.h"
#include "room/room.h"

#include "include/define.h"

#define DIR_OBJ_LOAD "../room/textures/salle.obj"

#include "main.h"

#define TRUE 1
#define FALSE 0

#ifdef _WIN32
	#define DIR_LOG "C:\\Windows\\Temp\\"
    #define DIR_TOKEN_FILE "C:\\Windows\\Temp\\.Nineteen"
#else
    #define DIR_TOKEN_FILE "/tmp/.Nineteen"
    #define DIR_LOG "/tmp/"
#endif

#define DIR_CONFIG_FILE "../launcher/.config"
#define DIR_MUSIC_FILE "../assets/background.wav"

#define DIR_IMG_BACKGROUND "../assets/image/launcher_no_font.png"
#define DIR_ING_BACKGROUND_TXT "../assets/image/launcher.png"


#define DIR_FONT_POLICE "../assets/font/police.ttf"
#define DIR_FONT_PASSWORD "../assets/font/password.ttf"

const SDL_Color rouge = {255,0,0};
const SDL_Color rougeFoncer = {209,52,52};
const	SDL_Color noir = {0,0,0};
const	SDL_Color blanc = {255,255,255};
const	SDL_Color blanc_foncer = {200,200,200};
const	SDL_Color vert = {76,196,66};
const SDL_Color bleu_foncer = {66,100,196};
const SDL_Color cyan = {0x38,0xf9,0xff};

// RESOLUTION
#define WIN_LARGUEUR 1440
#define WIN_HAUTEUR 960

int LARGUEUR = 0;
int HAUTEUR = 0;

#define SIZE_SESSION 256
#define URL_REGISTRATION "https://nineteen.recognizer.fr/inscription.php"
#define URL_CONFIG_FILE "https://nineteen.recognizer.fr/include/launcher/config.php"
const int DELAY = 200;
const int TENTATIVE = 8;

#define FPS 30

#define NB_ANIM 5
typedef enum{ANIM_CONNECTION, ANIM_HOVER_CONNECTION, ANIM_INSCRIPTION, ANIM_HOVER_INSCRIPTION, ANIM_LOADING} anims;
#define FRAME_ANIM_MAX 5
#define RATIO_ANIM 10
const int RATIO_CLICK[FRAME_ANIM_MAX] = {-5, -20, -50, -55, -30};




//Le thread qui sera utiliser
SDL_Thread *thread = NULL;

/////////////////////////////////////////////////////
/// \fn int checkVersion(char version[])
/// \brief permet de verifier la version du VERSION_LOGICIEL
///
/// \param version La version du jeu
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int checkVersion(char version[])
{
  if( checkVersionOnline(version) )
  {
    afficherMessageSysteme("Une nouvelle version est disponible. Version actuel : " VERSION);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}


/////////////////////////////////////////////////////
/// \fn int dejaConneceter(char *token)
/// \brief verifie si l utilisateur est deja connecter
///
/// \param token La clé de connexion de l'utilisateur
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int dejaConneceter(char *token)
{
  // FONCTION FACILEMENT COMPRENSIBLE AVEC LES PRINTF

	FILE * fp;
    fp = fopen (DIR_TOKEN_FILE, "r");
	if (!fp)
	{
    fprintf(EXT_FILE,"main.c -> dejaConneceter() : aucun fichier token trouver\n" );
		return EXIT_FAILURE;
	}
	else
	{
    fprintf(EXT_FILE,"main.c -> dejaConneceter() : parsing fichier token\n" );
		fscanf(fp,"%s",token);
		fclose(fp);

		if ( !connectWithKey(token) ){
      fprintf(EXT_FILE,"main.c -> dejaConneceter() : Connexion automatique avec le token\n" );
			return EXIT_SUCCESS;
		}

    fprintf(EXT_FILE,"main.c -> dejaConneceter() : Le fichier token n'as pas permis de ce connecter\n" );
	}

  fprintf(EXT_FILE,"main.c -> dejaConneceter() : EXIT_FAILURE\n" );
	return EXIT_FAILURE;
}

/////////////////////////////////////////////////////
/// \fn int sauvegarderToken(char *token)
/// \brief permet de sauvegarder le token dans un fichier
///
/// \param token La clé de connexion de l'utilisateur
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int sauvegarderToken(char *token)
{
  // FONCTION FACILEMENT COMPRENSIBLE AVEC LES PRINTF

	FILE *fp;
	fp = fopen(DIR_TOKEN_FILE,"w");
	if (!fp)
	{
    fprintf(EXT_FILE,"main.c -> sauvegarderToken() : impossible d'ecrire/cree le fichier token\n" );
		return EXIT_FAILURE;
	}
	else
	{
		fprintf(fp, "%s\n",token );
		fclose(fp);
		return EXIT_SUCCESS;
	}

}


/////////////////////////////////////////////////////
/// \fn void printAll(SDL_Renderer *renderer, SDL_Texture* background,SDL_Texture* loading, TTF_Font *police,SDL_Rect targetId, SDL_Rect targetPwd, SDL_Rect targetConnect, SDL_Rect targetInscription, int frame_anims[NB_ANIM],int valeurRetour )
/// \brief cette fonction permet d'afficher tous les elements present dans le launcher.
///
/// \return void
/////////////////////////////////////////////////////
void printAll(SDL_Renderer *renderer, SDL_Texture* background,SDL_Texture* loading, TTF_Font *police,SDL_Rect targetId, SDL_Rect targetPwd, SDL_Rect targetConnect, SDL_Rect targetInscription, int frame_anims[NB_ANIM],int valeurRetour )
{
  // RECT LABEL IDENTIFIANT
	SDL_Rect targetIdLabel = { LARGUEUR/6.5 , HAUTEUR/4 , LARGUEUR/1.7 , HAUTEUR/14};

  // RECT LABEL MDP
	SDL_Rect targetPwdLabel = { LARGUEUR/6.3 , HAUTEUR/2.3 , LARGUEUR/1.7 , HAUTEUR/14 };

  // RECT FOND DE LA VUE DE CONNEXION
	SDL_Rect targetUIView = {LARGUEUR/6.5,HAUTEUR/4.8, HAUTEUR,HAUTEUR/2};

  // RECT EMPLACEMENT MESSAGE ALERTE ERREUR CONNEXION
  SDL_Rect targetMessage = { LARGUEUR*0.19 , HAUTEUR*0.61, LARGUEUR*0.05 , HAUTEUR*0.05};

  // AFFICHER IMG BACKGROUND
	SDL_RenderCopy(renderer, background, NULL, NULL);

  // AFFICHER CHARGEMENT REQUETE
	afficherLoading(renderer, loading, cyan, 0, 0 ,frame_anims[ANIM_LOADING], LARGUEUR, HAUTEUR, 2*LARGUEUR);

  // DESSINER LE FOND DE LA VUE DE CONNEXION
	SDL_SetRenderDrawColor(renderer, noir.r , noir.g, noir.b,200);
	SDL_RenderFillRect(renderer,&targetUIView);

  // DESSINER RECTANGLE IDENTIFIANT
	SDL_SetRenderDrawColor(renderer, blanc_foncer.r , blanc_foncer.g, blanc_foncer.b,255);
	SDL_RenderFillRect(renderer,&targetId);

  // DESSINER RECTANGLE MDP
	SDL_RenderFillRect(renderer,&targetPwd);

  // ECRIRE LES LABEL AU EMPLACEMENT EN DERNIER PARAMETRE
	renduTextField(renderer,"Identifiant",police,blanc_foncer,targetIdLabel);
	renduTextField(renderer,"Mot de passe",police,blanc_foncer,targetPwdLabel);

  // ECRIRE LES MESSAGES D ERREUR DE CONNEXION
  if(valeurRetour == -3 )
    renduTextField(renderer,"Identifant/Mot de passe incorrect",police,rougeFoncer,targetMessage);
  else if(valeurRetour == -5)
    renduTextField(renderer,"Echec de synchronisation, try later",police,rougeFoncer,targetMessage);


	//CHANGER COULEUR BOUTON SELON ANIMATION
	SDL_Color vertDraw = vert;
	vertDraw.r += RATIO_ANIM * frame_anims[ANIM_HOVER_CONNECTION];
	vertDraw.b += RATIO_ANIM * frame_anims[ANIM_HOVER_CONNECTION];
	vertDraw.g += RATIO_ANIM * frame_anims[ANIM_HOVER_CONNECTION];

	if(frame_anims[ANIM_CONNECTION]!=-1){
		vertDraw.r += RATIO_CLICK[frame_anims[ANIM_CONNECTION]];
		vertDraw.b += RATIO_CLICK[frame_anims[ANIM_CONNECTION]];
		vertDraw.g += RATIO_CLICK[frame_anims[ANIM_CONNECTION]];
	}

	SDL_Color bleu_foncerDraw = bleu_foncer;
	bleu_foncerDraw.r += RATIO_ANIM * frame_anims[ANIM_HOVER_INSCRIPTION];
	bleu_foncerDraw.b += RATIO_ANIM * frame_anims[ANIM_HOVER_INSCRIPTION];
	bleu_foncerDraw.g += RATIO_ANIM * frame_anims[ANIM_HOVER_INSCRIPTION];

	if(frame_anims[ANIM_INSCRIPTION]!=-1){
		bleu_foncerDraw.r += RATIO_CLICK[frame_anims[ANIM_INSCRIPTION]];
		bleu_foncerDraw.b += RATIO_CLICK[frame_anims[ANIM_INSCRIPTION]];
		bleu_foncerDraw.g += RATIO_CLICK[frame_anims[ANIM_INSCRIPTION]];
	}


  // DESSINER RECTANGLE BOUTON CONNEXION + TEXT
	SDL_SetRenderDrawColor(renderer, vertDraw.r   , vertDraw.g , vertDraw.b ,255);
	SDL_RenderFillRect(renderer,&targetConnect);
	renduTextField(renderer,"Connexion",police,noir,targetConnect);
  // DESSINER RECTANGLE BOUTON INSCRIPTION + TEXT
	SDL_SetRenderDrawColor(renderer, bleu_foncerDraw.r, bleu_foncerDraw.g, bleu_foncer.b,255);
	SDL_RenderFillRect(renderer,&targetInscription);
	renduTextField(renderer,"Inscription",police,noir,targetInscription);

}


/////////////////////////////////////////////////////
/// \fn void ouvrirUrlRegistration()
/// \brief permet d'ouvrir une page web sur les 3 OS
///
/// \return void
/////////////////////////////////////////////////////
void ouvrirUrlRegistration()
{
	#ifdef _WIN64
		system("start "URL_REGISTRATION);
	#elif __APPLE__
		system("open "URL_REGISTRATION);
	#elif __linux__
		system("xdg-open "URL_REGISTRATION);
	#endif
}


/////////////////////////////////////////////////////
/// \fn int connexion(SDL_Renderer *renderer, char *token, char *tokenCpy,char path[])
/// \brief la fonction gere l'interaction globale de la vue, elle envoie les requetes de connexion et traite les retour
///
/// \param renderer Le renderer où afficher
/// \param token  La clé de connexion de l'utilisateur
/// \param tokenCpy La copie de la clé de connexion de l'utilisateur
/// \param path Le chemin d'execution
///
/// \return EXIT_FAILURE/EXIT_SUCCESS
/////////////////////////////////////////////////////
int connexion(SDL_Renderer *renderer, char *token, char *tokenCpy,char path[])
{
	//////////////////////////////////////////
	// INIT CHAINE DE CONCATENATION DU PATH
  int initFailed = SDL_FALSE;
	//////////////////////////////////////////
	// AFFICHAGE
	SDL_Texture* background = IMG_LoadTexture(renderer,DIR_IMG_BACKGROUND);
	if(!background)
  {
    fprintf(EXT_FILE,"main.c -> connexion() : IMG_LoadTexture : %s\n",DIR_IMG_BACKGROUND );

    // SET initFailed
    initFailed = SDL_TRUE;
  }


	SDL_Texture* loading = IMG_LoadTexture(renderer,DIR_LOADING);
	if(!loading)
  {
    fprintf(EXT_FILE,"main.c -> connexion() : IMG_LoadTexture : %s\n",DIR_LOADING );

    // SET initFailed
    initFailed = SDL_TRUE;
  }


	TTF_Font *police = police = TTF_OpenFont(DIR_FONT_POLICE,100);
	if(!police)
  {
    fprintf(EXT_FILE,"main.c -> connexion() : TTF_OpenFont : %s\n",DIR_FONT_POLICE );

    // SET initFailed
    initFailed = SDL_TRUE;
  }

	TTF_Font *ttf_pwd = ttf_pwd = TTF_OpenFont(DIR_FONT_PASSWORD,100);
	if(!ttf_pwd)
  {
    fprintf(EXT_FILE,"main.c -> connexion() : TTF_OpenFont : %s\n",DIR_FONT_PASSWORD );

    // SET initFailed
    initFailed = SDL_TRUE;
  }

  ///////////////////////////////
  // INITALISATION DES VARIABLES
	int retour = EXIT_FAILURE;
  ///////////////////////////////
  ///////////////////////////////
  // ALLOCATION
	char *identifiant = NULL;
  if ( _malloc((void**)&identifiant,sizeof(char),24,EXT_FILE,SDL_MESSAGEBOX_ERROR,"allocation failed","main.c : connexion() : char*identifiant ",NULL) )
    // SET initFailed if failed
    initFailed = SDL_TRUE;
  else
    // SET VALUE TO ""
    strcpy(identifiant,"");
  ///////////////////////////////
  ///////////////////////////////
  // ALLOCATION
  char *motDePasse = NULL;
  if ( _malloc((void**)&motDePasse,sizeof(char),24,EXT_FILE,SDL_MESSAGEBOX_ERROR,"allocation failed","main.c : connexion() : char*motDePasse ",NULL) )
    // SET initFailed
    initFailed = SDL_TRUE;
  else
    // SET VALUE TO ""
    strcpy(motDePasse,"");
  ///////////////////////////////
  ///////////////////////////////
  // ALLOCATION
  char *identifiantCpy = NULL;
  if ( _malloc((void**)&identifiantCpy,sizeof(char),24,EXT_FILE,SDL_MESSAGEBOX_ERROR,"allocation failed","main.c : connexion() : char*identifiantCpy ",NULL) )
    // SET initFailed
    initFailed = SDL_TRUE;
  else
    // SET VALUE TO ""
    strcpy(identifiantCpy,"");
  ///////////////////////////////
  ///////////////////////////////
  // ALLOCATION
  char *motDePasseCpy = NULL;
  if ( _malloc((void**)&motDePasseCpy,sizeof(char),24,EXT_FILE,SDL_MESSAGEBOX_ERROR,"allocation failed","main.c : connexion() : char*motDePasseCpy ",NULL) )
    // SET initFailed
    initFailed = SDL_TRUE;
  else
    // SET VALUE TO ""
    strcpy(motDePasseCpy,"");



  if(initFailed == SDL_TRUE)
  {

    // NETTOYGAE MEMOIRE
    if(background)
    {
      SDL_DestroyTexture(background);
      background = NULL;
    }
    if(loading)
    {
      SDL_DestroyTexture(loading);
      loading = NULL;
    }
    if(police)
    {
      TTF_CloseFont(police);
      police = NULL;
    }
    if(ttf_pwd)
    {
      TTF_CloseFont(ttf_pwd);
      ttf_pwd = NULL;
    }
    if(identifiant)
    {
      free(identifiant);
      identifiant = NULL;
    }
    if(motDePasse)
    {
      free(motDePasse);
      motDePasse = NULL;
    }
    if(identifiantCpy)
    {
      free(identifiantCpy);
      identifiantCpy = NULL;
    }
    if(motDePasseCpy)
    {
      free(motDePasseCpy);
      motDePasseCpy = NULL;
    }

    fprintf(EXT_FILE,"main.c -> connexion() : Erreur lors d'une allocation EXIT_FAILURE \n");
    return EXIT_FAILURE;
  }

  ///////////////////////////////
  //  ALLOCATION MEMORY END
  ///////////////////////////////
  ///////////////////////////////

  ///////////////////////////////
  // INITALISATION SANS ALLOCATION
  // DE MEMOIRE DYNAMIQUE
  ///////////////////////////////


	//anims (connection, hover_co, hover_inscription)
	int frame_anims[NB_ANIM] = {-1,0,-1,0};
  ///////////////////////////////
  // PLACEMENT DES RECTS
	SDL_Rect targetId = { LARGUEUR/5.5 , HAUTEUR/3, LARGUEUR/1.7 , HAUTEUR/14};
	SDL_Rect targetPwd = { LARGUEUR/5.5 , HAUTEUR/1.9 , LARGUEUR/1.7 , HAUTEUR/14 };
	SDL_Rect targetConnect = { LARGUEUR/1.87, HAUTEUR/1.5 , LARGUEUR/4  , HAUTEUR/14};
	SDL_Rect targetInscription = {LARGUEUR/5, HAUTEUR/1.5, LARGUEUR/3.3 , HAUTEUR/14};
	SDL_Rect curseur;
	int frame = 0;
	unsigned int lastTime = 0, currentTime;

  ///////////////////////////////
  // ETAT DE CHAQUE BOUTON
  // etatIdentifant is FIRST RESPONDER
	int etatIdentifant = RESPONDER_TRUE;
	int etatMotDePasse = RESPONDER_FALSE;
	int pressConnexion = SDL_FALSE;


	SDL_Point mouse = {0,0};
	int pressMaj = SDL_FALSE;
	ConnectStruct connectStruct;
	extern int connectEnded;
	connectEnded = SDL_FALSE;

	do
	{
    ///////////////////////////////
    // DESSIN DU BACKGROUND
		SDL_RenderCopy(renderer, background, NULL, NULL);
		SDL_GetMouseState(&mouse.x, &mouse.y);

		///////////////////////////////
    // ANIMATION MOUSE OVER
		if ( TF_ClickIn( targetConnect , mouse) )
		{
			frame_anims[ANIM_HOVER_CONNECTION]++;
			if(frame_anims[ANIM_HOVER_CONNECTION] > FRAME_ANIM_MAX)
				frame_anims[ANIM_HOVER_CONNECTION] = FRAME_ANIM_MAX;
		}
		else{
			frame_anims[ANIM_HOVER_CONNECTION]--;
			if(frame_anims[ANIM_HOVER_CONNECTION] < 0)
				frame_anims[ANIM_HOVER_CONNECTION] = 0;
		}

		if ( TF_ClickIn( targetInscription , mouse) )
		{
			frame_anims[ANIM_HOVER_INSCRIPTION]++;
			if(frame_anims[ANIM_HOVER_INSCRIPTION] > FRAME_ANIM_MAX)
				frame_anims[ANIM_HOVER_INSCRIPTION] = FRAME_ANIM_MAX;
		}
		else{
			frame_anims[ANIM_HOVER_INSCRIPTION]--;
			if(frame_anims[ANIM_HOVER_INSCRIPTION] < 0 )
				frame_anims[ANIM_HOVER_INSCRIPTION] = 0;
		}


		///////////////////////////////
    // RECUPERATION DES ENTREE CLAVIER
		if(etatIdentifant != RESPONDER_FALSE){
			etatIdentifant = textField(renderer, police, blanc_foncer ,identifiant, strlen(identifiant) ,&targetId , &mouse,&pressMaj);
			if(etatIdentifant)
				frame = 0;
		}
		else if(etatMotDePasse != RESPONDER_FALSE){
			etatMotDePasse = textField(renderer, ttf_pwd, blanc_foncer ,motDePasse, strlen(motDePasse) ,&targetPwd , &mouse,&pressMaj);
			if(etatMotDePasse)
				frame = 0;
		}


    ///////////////////////////////
		// GESTION EVENEMNT CLIC SOURIS
    // GESTION EVENEMENT CLAVIER
		if (etatIdentifant == TF_TAB)
		{
			etatIdentifant = RESPONDER_FALSE;
			etatMotDePasse = RESPONDER_TRUE;
		}
		else if (etatMotDePasse == TF_TAB)
		{
			etatIdentifant = RESPONDER_TRUE;
			etatMotDePasse = RESPONDER_FALSE;
		}
		else if (etatIdentifant == TF_QUIT || etatMotDePasse  == TF_QUIT)
		{
			fprintf(EXT_FILE,"main.c : connexion() : APPUIE SUR LA CROIX QUIITER : EXIT_FAILURE NORMALY\n");

      // NETTOYAGE DE LA MEMOIRE ALLOUER
      SDL_DestroyTexture(background);
      background = NULL;

      SDL_DestroyTexture(loading);
      loading = NULL;

      TTF_CloseFont(police);
      police = NULL;

      TTF_CloseFont(ttf_pwd);
      ttf_pwd = NULL;

      free(identifiant);
      identifiant = NULL;

      free(motDePasse);
      motDePasse = NULL;

      free(identifiantCpy);
      identifiantCpy = NULL;

      free(motDePasseCpy);
      motDePasseCpy = NULL;


      return EXIT_FAILURE;
		}

		else if ( etatIdentifant == TF_RETURN || etatMotDePasse  == TF_RETURN )
		{
			if(thread == NULL){
        fprintf(EXT_FILE,"main.c : connexion() : lancement d'un thread de connexion\n");

        // VARIABLES ANIMATION CONNEXION
				frame_anims[ANIM_LOADING] = 0;
				connectEnded = 0;
				frame_anims[ANIM_CONNECTION]=FRAME_ANIM_MAX-1;
        ////////////////////////////////
        // VARIABLE CONNEXION SAV
				strcpy(tokenCpy, token);
				strcpy(identifiantCpy, identifiant);
				strcpy(motDePasseCpy, motDePasse);

        fprintf(EXT_FILE,"main.c : connexion() : tentative de connexion...\n");
				connectStruct = (ConnectStruct){tokenCpy,identifiantCpy,motDePasseCpy};
				thread = SDL_CreateThread(  (int(*)(void*))connectWithUsername, NULL, &connectStruct );
			}
		}
		else if(etatIdentifant == TF_MOUSE_OUT_CLICK || etatMotDePasse  == TF_MOUSE_OUT_CLICK)
		{
      // PERTE DU FOCUS DES TEXT_FIELD
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
					if(thread == NULL){
            fprintf(EXT_FILE,"main.c : connexion() : lancement d'un thread de connexion\n");

            // VARIABLES ANIMATION CONNEXION
						frame_anims[ANIM_LOADING] = 0;
						connectEnded = 0;
						frame_anims[ANIM_CONNECTION]=FRAME_ANIM_MAX-1;

            ////////////////////////////////
            // VARIABLE CONNEXION SAV
						strcpy(tokenCpy, token);
            strcpy(identifiantCpy, identifiant);
            strcpy(motDePasseCpy, motDePasse);

            fprintf(EXT_FILE,"main.c : connexion() : tentative de connexion...\n");
						connectStruct = (ConnectStruct){tokenCpy,identifiantCpy,motDePasseCpy};
						thread = SDL_CreateThread( (int(*)(void*))connectWithUsername, NULL, &connectStruct);
					}

				}
				else if ( TF_ClickIn( targetInscription , mouse) )
				{
					////////////////////////////////
          // ANIMATION
					frame_anims[ANIM_INSCRIPTION]=FRAME_ANIM_MAX-1;

          fprintf(EXT_FILE,"main.c : connexion() : redirection vers page inscription\n");
					ouvrirUrlRegistration();

				}

        // RESET MOUSE
				mouse.x = 0;
				mouse.y = 0;
			}
		}

    // AFFICHE TOUS
		printAll(renderer,background, loading, police, targetId, targetPwd, targetConnect, targetInscription, frame_anims,retour);


		// EMPECHE L'AFFICHE D UNE ZONE DE TEXT VIDE
		if( strlen(motDePasse) >= 1)
			renduTextField(renderer,motDePasse,ttf_pwd,noir,targetPwd);
    // EMPECHE L'AFFICHE D UNE ZONE DE TEXT VIDE
		if( strlen(identifiant) >= 1)
			renduTextField(renderer,identifiant,police,noir,targetId);

		if(etatMotDePasse == RESPONDER_TRUE){
			curseur=targetPwd;
			curseur.x += targetPwd.h*0.5 * (strlen(motDePasse)+1);
			curseur.w = 4;
			curseur.y += curseur.h/6;
			curseur.h -= curseur.h/3;
		}

		if(etatIdentifant == RESPONDER_TRUE){
			curseur=targetId;
			curseur.x += targetId.h*0.5 * (strlen(identifiant)+1);
			curseur.w = 4;
			curseur.y += curseur.h/6;
			curseur.h -= curseur.h/3;
		}

		if(frame%(int)(FPS*1.4) < ((FPS*1.4)/2)){

			SDL_SetRenderDrawColor(renderer, 0,0,0,255);
			SDL_RenderFillRect(renderer, &curseur);
		}


		if(connectEnded && thread){
			fprintf(EXT_FILE,"main.c : connexion() : recuperation valeur thread\n");
			SDL_WaitThread(thread, &retour);
				fprintf(EXT_FILE,"main.c : connexion() : le thread a retourner une valeur\n");

			thread = NULL;
      // STOPER l'ANIM
			frame_anims[ANIM_LOADING] = 0;
		}
		else if(thread)
      // FAIRE CONTINUER L ANIM
			frame_anims[ANIM_LOADING]++;

		frame ++;

    // FIN DE CONNEXION
		if(retour == EXIT_SUCCESS){
						strcpy(token,tokenCpy);
						pressConnexion = SDL_TRUE;
		}


    // AFFICHAGE ET NETTOYAGE DE LA FENETRE
		SDL_RenderPresent(renderer);
		SDL_RenderClear(renderer);

    // CALCUL DU TEMPS DE LA FRAME POUR LIMITATION
		currentTime = SDL_GetTicks();
		while( currentTime - lastTime < 1000/FPS )
			currentTime = SDL_GetTicks();

		lastTime = currentTime;


		//Decremente anim click
		frame_anims[ANIM_INSCRIPTION]--;
		if(frame_anims[ANIM_INSCRIPTION]<-1)
			frame_anims[ANIM_INSCRIPTION]=-1;
		frame_anims[ANIM_CONNECTION]--;
		if(frame_anims[ANIM_CONNECTION]<-1)
			frame_anims[ANIM_CONNECTION]=-1;


	} while( !pressConnexion ) ;

  // NETTOYAGE DE LA MEMOIRE ALLOUER
  SDL_DestroyTexture(background);
  background = NULL;

  SDL_DestroyTexture(loading);
  loading = NULL;

  TTF_CloseFont(police);
  police = NULL;

  TTF_CloseFont(ttf_pwd);
  ttf_pwd = NULL;

  free(identifiant);
  identifiant = NULL;

  free(motDePasse);
  motDePasse = NULL;

  free(identifiantCpy);
  identifiantCpy = NULL;

  free(motDePasseCpy);
  motDePasseCpy = NULL;

  return EXIT_SUCCESS;
}


/////////////////////////////////////////////////////
/// \fn int chargementFichier(SDL_Renderer *renderer,struct MeilleureScore_s meilleureScore[],char *token,const C_STRUCT aiScene** scene, char *path )
/// \brief verifie la presence de tous les fichiers du jeu et charge la scene 3D ainsi que la table des scores
///
/// \param renderer Le renderer où afficher
/// \param meilleureScore Le tableau de records
/// \param token La clé de connexion de l'utilisateur
/// \param scene La scène opengl
/// \param path Le chemin d'execution
///
/// \return EXIT_FAILURE/EXIT_SUCCESS
/////////////////////////////////////////////////////
int chargementFichier(SDL_Renderer *renderer,struct MeilleureScore_s meilleureScore[],char *token,const C_STRUCT aiScene** scene, char *path )
{

  // NB ALLOC 2 //

  //FIX ANIMATION AFFICHAGE MAC
  // QUI NECESSITE UN EVENT
  SDL_Event fixMac;
  while ( SDL_PollEvent(&fixMac) );

  //////////////////////////////////////
  // NETTOYAGE DE LA FENETRE
  SDL_RenderClear(renderer);


  //////////////////////////////////////
  // Initalisation des variables
	char *concatenation = NULL;
	if ( _malloc((void**)&concatenation,sizeof(char),512,EXT_FILE,SDL_MESSAGEBOX_ERROR,"allocation failed","main.c : chargementFichier() : char*concatenation ",NULL) )
		return EXIT_FAILURE;
  //////////////////////////////////////
  // TEXTURE BACKGROUND
	SDL_Texture* background = IMG_LoadTexture(renderer,DIR_ING_BACKGROUND_TXT);
	if(!background)
	{
		fprintf(EXT_FILE,"main.c -> chargementFichier() : IMG_LoadTexture : %s\n",DIR_ING_BACKGROUND_TXT );

		// NETTOYAGE MEMOIRE
		free(concatenation);
		concatenation=NULL;
		return EXIT_FAILURE;
	}
	//////////////////////////////////////

  //////////////////////////////////////
  // ERREUR RENDER COPY
	if ( SDL_RenderCopy(renderer, background, NULL, NULL) )
	{
	    fprintf(EXT_FILE,"main.c : chargementFichier() :SDL_RenderCopy ERR %s\n",SDL_GetError() );

	    //////////////////////////////////////
	    // NETTOYAGE MEMOIRE
	    SDL_DestroyTexture(background);
	    free(concatenation);
	    concatenation=NULL;
	    return EXIT_FAILURE;
	}

	//AFFICHER FOND BAR DE CHARGEMENT
	SDL_Rect chargement = {LARGUEUR*0.05,HAUTEUR*0.85,LARGUEUR*0.90,HAUTEUR*0.08};
	SDL_SetRenderDrawColor(renderer, noir.r , noir.g, noir.b,200);
	SDL_RenderFillRect(renderer,&chargement);
	SDL_RenderPresent(renderer);

  //INIT BAR DE CHARGEMENT
	SDL_Rect chargementAff = {LARGUEUR*0.05,HAUTEUR*0.85,0,HAUTEUR*0.08};
	int progression = LARGUEUR*0.90 / (NB_FILE+1);


	for(int i = 0 ; i < NB_FILE ; i++)
	{
		strcpy(concatenation,path);
		strcat(concatenation,verifierFichier[i]);
		FILE *fp = fopen(concatenation,"r");
		if(!fp)
		{
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,concatenation,"Fichier introuvable, Merci de réinstaller le programme.",NULL);
			fprintf(EXT_FILE,"main.c : chargementFichier() : %s introuvable \n",concatenation );

      //////////////////////////////////////
      // NETTOYAGE DE LA MEMOIRE
      SDL_DestroyTexture(background);
      free(concatenation);
      concatenation=NULL;
			return EXIT_FAILURE;
		}
		else
		{
			fclose(fp);
		}

    //////////////////////////////////////
    // MISE A JOUR DE L'AFFICHAGE //
		SDL_RenderClear(renderer);
		if ( SDL_RenderCopy(renderer, background, NULL, NULL) )
	    {
	      fprintf(EXT_FILE,"main.c : chargementFichier() :SDL_RenderCopy ERR %s\n",SDL_GetError() );

	      //////////////////////////////////////
	      // NETTOYAGE MEMOIRE
	      SDL_DestroyTexture(background);
	      free(concatenation);
	      concatenation=NULL;
	      return EXIT_FAILURE;
	    }
    // BAR FOND CHARGEMENT
		SDL_SetRenderDrawColor(renderer, noir.r , noir.g, noir.b,200);
		SDL_RenderFillRect(renderer,&chargement);
    // BAR DE CHARGEMENT
		chargementAff.w += progression;
		SDL_SetRenderDrawColor(renderer, blanc.r , blanc.g, blanc.b,200);
		SDL_RenderFillRect(renderer,&chargementAff);

    // AFFICHER LE TOUT
		SDL_RenderPresent(renderer);
	}

  //////////////////////////////////////
  // MSG LOG
  fprintf(EXT_FILE,"main.c : chargementFichier() : tout les fichiers sont present\n");


	InitMeilleureScore(meilleureScore);
	if( updateMeilleureScore(meilleureScore,token) )
  {
    fprintf(EXT_FILE,"main.c : chargementFichier() : updateMeilleureScore() : EXIT_FAILURE \n");
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"SERVEUR","Une erreur est survenu lors de la communication avec le serveur",NULL);
    //////////////////////////////////////
    // NETTOYAGE MEMOIRE
    SDL_DestroyTexture(background);
    free(concatenation);
    concatenation=NULL;
    return EXIT_FAILURE;
  }

  //////////////////////////////////////
  // MSG LOG
  fprintf(EXT_FILE,"main.c : chargementFichier() : chargement structure score = OK\n");

  //////////////////////////////////////
  // NETTOYAGE FENETRE
	SDL_RenderClear(renderer);
  // BACKGROUND
	if ( SDL_RenderCopy(renderer, background, NULL, NULL) )
  {
    fprintf(EXT_FILE,"main.c : chargementFichier() :SDL_RenderCopy ERR %s\n",SDL_GetError() );

    //////////////////////////////////////
    // NETTOYAGE MEMOIRE
    SDL_DestroyTexture(background);
    free(concatenation);
    concatenation=NULL;
    return EXIT_FAILURE;
  }
  // BAR CHARGEMENT
	SDL_SetRenderDrawColor(renderer, noir.r , noir.g, noir.b,200);
	SDL_RenderFillRect(renderer,&chargement);
	chargementAff.w += progression;
	SDL_SetRenderDrawColor(renderer, blanc.r , blanc.g, blanc.b,200);
	SDL_RenderFillRect(renderer,&chargementAff);

  // AFFICHER LE TOUS
	SDL_RenderPresent(renderer);

  //////////////////////////////////////
  // IMPORTER OBJ
	strcpy(concatenation,path);
	strcat(concatenation,DIR_OBJ_LOAD);

	if ( aiImportModel(concatenation,scene) )
  {
    fprintf(EXT_FILE,"main.c : chargementFichier() : impossible de charger %s\n",concatenation );
    //////////////////////////////////////
    // NETTOYAGE MEMOIRE
    SDL_DestroyTexture(background);
    free(concatenation);
    concatenation=NULL;

    return EXIT_FAILURE;
  }

  fprintf(EXT_FILE,"main.c : chargementFichier() : scene charger avec success : %s : nbTextures %d\n",concatenation,(*scene)->mNumMaterials );


  //////////////////////////////////////
  // MISE A JOUR DE L'AFFICHAGE
  // AVEC DERNIER FICHIER A CHARGER
	SDL_RenderClear(renderer);
  if ( SDL_RenderCopy(renderer, background, NULL, NULL) )
  {
    fprintf(EXT_FILE,"main.c : chargementFichier() :SDL_RenderCopy ERR %s\n",SDL_GetError() );

    //////////////////////////////////////
    // NETTOYAGE MEMOIRE
    SDL_DestroyTexture(background);
    free(concatenation);
    concatenation=NULL;
    return EXIT_FAILURE;
  }

  // BAR CHARGEMENT
	SDL_SetRenderDrawColor(renderer, noir.r , noir.g, noir.b,200);
	SDL_RenderFillRect(renderer,&chargement);
	chargementAff.w = LARGUEUR*0.90;
	SDL_SetRenderDrawColor(renderer, blanc.r , blanc.g, blanc.b,200);
	SDL_RenderFillRect(renderer,&chargementAff);

  // AFFICHER LE TOUS
	SDL_RenderPresent(renderer);



  // LIBERATION DE MEMOIRE
  SDL_DestroyTexture(background);
  background = NULL;
  free(concatenation);
  concatenation = NULL;

	return EXIT_SUCCESS;
}








/////////////////////////////////////////////////////
/// \fn int launcher(SDL_Renderer* renderer, char *token, char *tokenCpy,struct MeilleureScore_s meilleureScore[],const C_STRUCT aiScene** scene, char path[])
/// \brief regroupe les fonctions permettant de faire fonctionner le launcher avec l appel de connexion/chargement fichier
///
/// \param renderer Le renderer où afficher
/// \param token La clé de connexion de l'utilisateur
/// \param tokenCpy La copie clé de connexion de l'utilisateur
/// \param meilleureScore Le tableaud de records
/// \param scene La scène opengl
/// \param path Le chemin d'execution
///
/// \return EXIT_FAILURE/EXIT_SUCCESS
/////////////////////////////////////////////////////
int launcher(SDL_Renderer* renderer, char *token, char *tokenCpy,struct MeilleureScore_s meilleureScore[],const C_STRUCT aiScene** scene, char path[])
{
	Mix_Music *musique = Mix_LoadMUS(DIR_MUSIC_FILE);
	if (!musique )
  {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Fichier introuvable",DIR_MUSIC_FILE,NULL);
    fprintf(EXT_FILE,"main.c : launcher() : Mix_LoadMUS %s DIR : %s\n",SDL_GetError(),DIR_MUSIC_FILE);
    return EXIT_FAILURE;
  }
	Mix_Volume(0,MIX_MAX_VOLUME/2);
	Mix_PlayMusic(musique, -1);


	if( SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND) )
  {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"SDL_SetRenderDrawBlendMode",SDL_GetError(),NULL);
    fprintf(EXT_FILE,"main.c : launcher() : SDL_SetRenderDrawBlendMode %s\n",SDL_GetError());
  }


	if ( dejaConneceter(token) )
	{
    fprintf(EXT_FILE,"main.c : launcher() : dejaConnecter : FALSE\n");
		if( connexion(renderer,token, tokenCpy, path) )
    {
      fprintf(EXT_FILE,"main.c : launcher() : connexion : EXIT_FAILURE\n");
      ///////////////////////////////////////////
      // COUPER MUSIQUE
      Mix_HaltMusic();
      ///////////////////////////////////////////
      // LIBERATION DE LA MEMOIRE
      Mix_FreeMusic(musique);
      return EXIT_FAILURE;
    }

		if ( sauvegarderToken(token) )
    {
      fprintf(EXT_FILE,"main.c : launcher() : sauvegarderToken : EXIT_FAILURE\n");
      ///////////////////////////////////////////
      // COUPER MUSIQUE
      Mix_HaltMusic();
      ///////////////////////////////////////////
      // LIBERATION DE LA MEMOIRE
      Mix_FreeMusic(musique);
      return EXIT_FAILURE;
    }
  }
  else
    fprintf(EXT_FILE,"main.c : launcher() : dejaConnecter : TRUE\n");


	if( chargementFichier(renderer,meilleureScore,token,scene,path) )
  {
    fprintf(EXT_FILE,"main.c : launcher() : chargementFichier : EXIT FAILURE\n");
    ///////////////////////////////////////////
    // COUPER MUSIQUE
    Mix_HaltMusic();
    ///////////////////////////////////////////
    // LIBERATION DE LA MEMOIRE
    Mix_FreeMusic(musique);
    return EXIT_FAILURE;
  }


  ///////////////////////////////////////////
  // COUPER MUSIQUE
  Mix_HaltMusic();
  ///////////////////////////////////////////
  // LIBERATION DE LA MEMOIRE
  Mix_FreeMusic(musique);

  return EXIT_SUCCESS;
}




int main(int argc, char *argv[])
{
    #ifdef _WIN32
       HWND hWnd = GetConsoleWindow();
       ShowWindow( hWnd, SW_HIDE );
    #endif // _WIN32
	EXT_FILE = NULL;
  ////////////////////////////////////////////////////
  // CHOIX DU MODE DE SORTIE POUR LES CODES
  // D'ERREUR
  if(MODE_DEV)
  {
    // FILE POINTE SUR LA SORTIE STANDARS
    EXT_FILE = stderr;
    fprintf(EXT_FILE, "Version du logiciel %s\n",VERSION );
  }
  else
  {

    ////////////////////////////////////
    // CREATION D'UN FICHIER AVEC DATE DU JOUR ET HEURE
    char *nomFichier = NULL;
    ////////////////////////////////////
    // RECUPRATION DATE ET HEURE DEPUIS LE SERVEUR
    char *t_server;
    if ( envoyez_requet(&t_server, "https://nineteen.recognizer.fr/include/timestamp.php", "") == EXIT_SUCCESS )
    {
      ////////////////////////////////////
      // STOCKAGE DANS LES VARIABLES
      int year,mon,day,hour,min,sec;
      sscanf(t_server, "%d %d %d %d %d %d",&year , &mon , &day, &hour , &min, &sec);

      // LIBERATION MEMOIRE
      free(t_server);
      t_server = NULL;


      // ALLOCATION
      if ( _malloc((void**)&nomFichier,sizeof(char),128,EXT_FILE,SDL_MESSAGEBOX_ERROR,"allocation failed","main.c : main() : char*nomFichier ",NULL) )
        return EXIT_FAILURE;
      sprintf(nomFichier,"%sNineteenLog_%d-%02d-%02d_%02d-%02d-%d.log", DIR_LOG, year  , mon , day, hour , min, sec);
      EXT_FILE = fopen(nomFichier,"w");
      // VERIFIER CREATION DU FICHIER

      if(!EXT_FILE)
      {
        // REMISE SUR SORTI TERMINAL
        EXT_FILE = stderr;
        fprintf(EXT_FILE, "main.c : main() : Impossible de cree le fichier de log passage sur log terminal %s \n",nomFichier );
      }

      fprintf(EXT_FILE, "Nom du fichier %s\nVersion du logiciel %s\n",nomFichier,VERSION );
      // LIBERATION MEMOIRE
      free(nomFichier);
      nomFichier = NULL;
    }
    else
    {
      if ( _malloc((void**)&nomFichier,sizeof(char),18,EXT_FILE,SDL_MESSAGEBOX_ERROR,"allocation failed","main.c : main() : char*nomFichier ",NULL) )
        return EXIT_FAILURE;
      strcpy(nomFichier,"/tmp/NineteenLog_.log");
      EXT_FILE = fopen(nomFichier,"w");

      // VERIFIER CREATION DU FICHIER
      if(!EXT_FILE)
      {
        // REMISE SUR SORTI TERMINAL
        EXT_FILE = stderr;
        fprintf(EXT_FILE, "main.c : main() : Impossible de cree le fichier de log passage sur log terminal %s \n",nomFichier );
      }

      fprintf(EXT_FILE, "Nom du fichier %s\nVersion du logiciel %s",nomFichier,VERSION );
      free(nomFichier);
      nomFichier = NULL;

    }

  }

  fprintf(EXT_FILE,"INFORMATION GENERAL :\n");
  informationPreciseCPUGPU();
  fprintf(EXT_FILE," - RAM : %d,%d Go\n",SDL_GetSystemRAM()/1000, SDL_GetSystemRAM()%1000 );


  /////////////////////////////////////////////////////////////////
  // LANCER LE JEU SI ON A LA BONNE VERSION UNIQUEMENT
  if ( checkVersion(VERSION_LOGICIEL) == EXIT_SUCCESS)
  {
    fprintf(EXT_FILE,"main.c : main() : REPERTOIRE D'EXECUTION %s\n",argv[0] );
    char *addPath = fullPath(argv[0]);
    fprintf(EXT_FILE,"main.c : main() : AJOUTER : %s\n",addPath);
    /////////////////////////////////////////////////////////////////
    // INIT SDL // TTF // MIXER
    SDL_Init(SDL_INIT_VIDEO);
    /////////////////////////////////////////////////////////////////
    // RECUPERER INFO ORDINATEUR

    /////////////////////////////////////////////////////////////////
    TTF_Init();
    Mix_Init(MIX_INIT_MP3);
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) //Initialisation de l'API Mixer
      fprintf(EXT_FILE,"main.c : main() : Mix FAILED INIT %s", Mix_GetError());


	  /////////////////////////////////////////////////////////////////
    // ALLOCATION CHANNEL
	  Mix_AllocateChannels(16);
    /////////////////////////////////////////////////////////////////
    // RECUPERER TAILLE ECRAN
    SDL_DisplayMode DM;
    if ( SDL_GetCurrentDisplayMode(0, &DM) )
    {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Erreur Fatal.","GetCurrentDisplayMode",NULL);
      fprintf(EXT_FILE,"main.c : main() : GetCurrentDisplayMode %s\n",SDL_GetError() );
      return EXIT_FAILURE;
    }

    // APPLIQUER UN RATIO
    LARGUEUR = DM.w/1.78;
    HAUTEUR = LARGUEUR*0.66667;

    /////////////////////////////////////////////////////////////////
    // INIT VARIABLE TOKEN
    char *token=NULL,*tokenCpy=NULL;
    if ( _malloc((void**)&token,sizeof(char),SIZE_SESSION + 1,EXT_FILE,SDL_MESSAGEBOX_ERROR,"allocation failed","main.c : main() : allocation char*token ",NULL) )
      return EXIT_FAILURE;
    if ( _malloc((void**)&tokenCpy,sizeof(char),SIZE_SESSION + 1,EXT_FILE,SDL_MESSAGEBOX_ERROR,"allocation failed","main.c : main() : allocation char*tokenCpy ",NULL) )
      return EXIT_FAILURE;
    // COPIE DE TOKEN DANS TOKENCOPY
    tokenCpy=strcpy(tokenCpy, token);



    /////////////////////////////////////////////////////////////////
    // INIT FENETRE WINDOWS ET RENDERER ET SURFACE
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Surface* favicon = NULL;


    /////////////////////////////////////////////////////////////////
    // CREATION WINDOWS ET RENDERER ET FAVICON
    window = SDL_CreateWindow("Nineteen | Launcher", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,LARGUEUR,HAUTEUR, 0  );
    if(!window)
    {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Erreur Fatal.",SDL_GetError(),window);
      fprintf(EXT_FILE,"main.c : main() : SDL_CreateWindow %s\n",SDL_GetError());
      return EXIT_FAILURE;
    }
    /////////////////////////////////////////////////////////////////
    // CHARGEMENT ICON
    // NOM PROBLEMATIQUE SI NON CHARGER ON APPLIQUE PAS SI NON CHARGER
    if( ( favicon=IMG_Load("../assets/image/favicon.png") ) )
    {
      SDL_SetWindowIcon(window, favicon);
      SDL_FreeSurface(favicon);
    }
    else
    {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING,"../assets/image/favicon.png","Fichier introuvable",window);
      fprintf(EXT_FILE,"main.c : main() : Erreur chargement favicon ../assets/image/favicon.png\n");
    }
    /////////////////////////////////////////////////////////////////
    // CREATION DU RENDU
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if(!renderer)
    {
      SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"Erreur Fatal.",SDL_GetError(),window);
      fprintf(EXT_FILE,"main.c : main() : SDL_CreateRenderer %s\n",SDL_GetError());
      return EXIT_FAILURE;
    }

    /////////////////////////////////////////////////////////////////
    // INIT STRUCTURE MEILLEURE_SCORE
    struct MeilleureScore_s meilleureScore[16];
    /////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////
    // INIT VARIABLE QUI POINTE SUR LA SCENE
    const C_STRUCT aiScene* scene = NULL;
    // APPEL DU LAUNCHER
    /////////////////////////////////////////////////////////////////
    if( launcher(renderer,token,tokenCpy,meilleureScore,&scene,addPath) == EXIT_SUCCESS)
    {

      /////////////////////////////////////////////////////////////////
      // RECUPERER LA TAILLE DE L'ECRAN
	    SDL_Rect borderSize;
	    if ( SDL_GetWindowBordersSize(window,&borderSize.x,&borderSize.y,&borderSize.w,&borderSize.h) )
      {
        fprintf(EXT_FILE,"main.c : main() : SDL_GetWindowBordersSize %s\n",SDL_GetError());
      }
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      /////////////////////////////////////////////////////////////////
      // APPEL DE LA ROOM
      int fullscreen = 0;
      room(token,meilleureScore,window,scene, fullscreen, borderSize);
    }


    /////////////////////////////////////////////////////////////////
    // LIBERER MEMOIRE
    fprintf(EXT_FILE, "main.c : main() : liberation des allocation \n");
    /////////////////////////////////////////////////////////////////
    // VIDER LA MEMOIRE DE LA SCENE
    aiReleaseImport(scene);
    // VIDER MEMOIRE addPath
    free(addPath);
    addPath = NULL;
    // VIDER MEMOIRE TOKEN
    free(token);
    token = NULL;
    free(tokenCpy);
    tokenCpy = NULL;
    fprintf(EXT_FILE, "main.c : main() : allocation liberer\n");
    /////////////////////////////////////////////////////////////////
    // QUITTER TTF
    fprintf(EXT_FILE, "main.c : main() : quitter les API \n");
    TTF_Quit();
    /////////////////////////////////////////////////////////////////
    // FERMER AUDIO
    Mix_ChannelFinished(0);
    Mix_CloseAudio();
    /////////////////////////////////////////////////////////////////
    // QUITTER MIXER
    Mix_Quit();
    /////////////////////////////////////////////////////////////////
    // DETRUIRE LA FENETRE
    SDL_DestroyWindow(window);
    /////////////////////////////////////////////////////////////////
    // QUITTER TOUS LES SYSTEME SDL INITALISER
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    /////////////////////////////////////////////////////////////////
    // QUITTER SDL
    SDL_Quit();

    fprintf(EXT_FILE, "main.c : main() : API quitter \n");
  }

  // FERMETURE DU FICHIER DE LOG
  fprintf(EXT_FILE, "main.c : main() : fermeture du programme \n");
  fclose(EXT_FILE);

	return 0;
}
