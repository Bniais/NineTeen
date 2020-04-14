#define VERSION_LOGICIEL "version=0.1.4b"


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

#include "./include/textField.h"
#include "./include/fullpath.h"
#include "./include/communFunctions.h"
#include "./include/libWeb.h"
#include "./room/room.h"

#include "define/define.h"

#define DIR_OBJ_LOAD "room/salle.obj"

#include "launcher/launcher.h"

#define TRUE 1
#define FALSE 0

#ifdef _WIN32
  #define DIR_TOKEN_FILE "C:\\Windows\\Temp\\.Nineteen"
#else
  #define DIR_TOKEN_FILE "/tmp/.Nineteen"
#endif

#define DIR_CONFIG_FILE "launcher/.config"
#define DIR_MUSIC_FILE "assets/background.wav"

#define DIR_IMG_BACKGROUND "assets/image/launcher_no_font.png"
#define DIR_ING_BACKGROUND_TXT "assets/image/launcher.png"


#define DIR_FONT_POLICE "assets/font/police.ttf"
#define DIR_FONT_PASSWORD "assets/font/password.ttf"

const SDL_Color rouge = {255,0,0};
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


int checkVersion(char version[])
{
  if( checkVersionOnline(version) )
  {
    #ifdef _WIN32
      system("mshta vbscript:Execute(\"msgbox \"\"Une nouvelle version est disponible\"\":close\")");
    #endif

    #ifdef __APPLE__
      system("osascript -e 'tell application (path to frontmost application as text) to display dialog \"Une nouvelle version est disponible\" buttons {\"QUITTER\"} with icon stop'");
    #endif

    #ifdef __linux
      system("zenity --warning --text \"Une nouvelle version est disponible\"");
    #endif

    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

void chargementConfig(int *delai, int *tentative)
{
	char *response;;
	envoyez_requet(&response, URL_CONFIG_FILE, "");
	sscanf(response, "%d %d", delai ,tentative);
	free(response);
}

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

int dejaConneceter(char *token)
{
	FILE * fp;
    fp = fopen (DIR_TOKEN_FILE, "r");
	if (!fp)
	{
		return 0;
	}
	else
	{
		fscanf(fp,"%s",token);
		fclose(fp);

		if ( !connectWithKey(token) ){
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

void printAll(SDL_Renderer *renderer, SDL_Texture* background,SDL_Texture* loading, TTF_Font *police,SDL_Rect targetId, SDL_Rect targetPwd, SDL_Rect targetConnect, SDL_Rect targetInscription, int frame_anims[NB_ANIM] )
{
//	SDL_Rect targetId = { LARGUEUR/5.5 , HAUTEUR/3, LARGUEUR/1.7 , HAUTEUR/14};
	SDL_Rect targetIdLabel = { LARGUEUR/6.5 , HAUTEUR/4 , LARGUEUR/1.7 , HAUTEUR/14};

//	SDL_Rect targetPwd = { LARGUEUR/5.5 , HAUTEUR/1.9 , LARGUEUR/1.7 , HAUTEUR/14 };
	SDL_Rect targetPwdLabel = { LARGUEUR/6.3 , HAUTEUR/2.3 , LARGUEUR/1.7 , HAUTEUR/14 };

//	SDL_Rect targetConnect = { LARGUEUR/1.87, HAUTEUR/1.5 , LARGUEUR/4  , HAUTEUR/14};
//	SDL_Rect targetInscription = {LARGUEUR/5, HAUTEUR/1.5, LARGUEUR/3.3 , HAUTEUR/14};

	SDL_Rect targetUIView = {LARGUEUR/6.5,HAUTEUR/4.8, HAUTEUR,HAUTEUR/2};

	SDL_RenderCopy(renderer, background, NULL, NULL);

	afficherLoading(renderer, loading, cyan, 0, 0 ,frame_anims[ANIM_LOADING], LARGUEUR, HAUTEUR, 2*LARGUEUR);

	SDL_SetRenderDrawColor(renderer, noir.r , noir.g, noir.b,200);
	SDL_RenderFillRect(renderer,&targetUIView);


	SDL_SetRenderDrawColor(renderer, blanc_foncer.r , blanc_foncer.g, blanc_foncer.b,255);
	SDL_RenderFillRect(renderer,&targetId);

	SDL_RenderFillRect(renderer,&targetPwd);

	renduTextField(renderer,"Identifiant",police,blanc_foncer,targetIdLabel);
	renduTextField(renderer,"Mot de passe",police,blanc_foncer,targetPwdLabel);



	//Change couleur boutons selon animation
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



	SDL_SetRenderDrawColor(renderer, vertDraw.r   , vertDraw.g , vertDraw.b ,255);
	SDL_RenderFillRect(renderer,&targetConnect);
	renduTextField(renderer,"Connexion",police,noir,targetConnect);

	SDL_SetRenderDrawColor(renderer, bleu_foncerDraw.r, bleu_foncerDraw.g, bleu_foncer.b,255);
	SDL_RenderFillRect(renderer,&targetInscription);
	renduTextField(renderer,"Inscription",police,noir,targetInscription);


}

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

void connexion(SDL_Renderer *renderer, char *token, char *tokenCpy,char path[])
{
	//////////////////////////////////////////
	// INIT CHAINE DE CONCATENATION DU PATH
	//////////////////////////////////////////
	// AFFICHAGE
	SDL_Texture* background = IMG_LoadTexture(renderer,DIR_IMG_BACKGROUND);
	if(!background)
		printf("Impossible de charger %s\n",DIR_IMG_BACKGROUND );

	SDL_Texture* loading = IMG_LoadTexture(renderer,DIR_LOADING);
	if(!loading)
		printf("Impossible de charger %s\n",DIR_LOADING );


	TTF_Font *police = police = TTF_OpenFont(DIR_FONT_POLICE,100);
	if(!police)
		printf("Impossible de charger %s\n",DIR_FONT_POLICE );

	TTF_Font *ttf_pwd = ttf_pwd = TTF_OpenFont(DIR_FONT_PASSWORD,100);
	if(!ttf_pwd)
		printf("Impossible de charger %s\n",DIR_FONT_PASSWORD );


	// permet de recuperer depuis le serveur l'information sur
	// le nombre de tentative de connexion
//	int delai = DELAY;
//	int tentative = TENTATIVE;
//	chargementConfig(&delai,&tentative);
	int retour = EXIT_FAILURE;
	char identifiant[24]="";
	char motDePasse[24]="";
	char identifiantCpy[24]="";
	char motDePasseCpy[24]="";

	//anims (connection, hover_co, hover_inscription)
	int frame_anims[NB_ANIM] = {-1,0,-1,0};
	SDL_Rect targetId = { LARGUEUR/5.5 , HAUTEUR/3, LARGUEUR/1.7 , HAUTEUR/14};
	SDL_Rect targetPwd = { LARGUEUR/5.5 , HAUTEUR/1.9 , LARGUEUR/1.7 , HAUTEUR/14 };
	SDL_Rect targetConnect = { LARGUEUR/1.87, HAUTEUR/1.5 , LARGUEUR/4  , HAUTEUR/14};
	SDL_Rect targetInscription = {LARGUEUR/5, HAUTEUR/1.5, LARGUEUR/3.3 , HAUTEUR/14};
	SDL_Rect curseur;
	int frame = 0;
	unsigned int lastTime = 0, currentTime;

	printAll(renderer,background, loading, police, targetId, targetPwd, targetConnect, targetInscription,frame_anims);
	SDL_RenderPresent(renderer);
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
		targetId = (SDL_Rect){ LARGUEUR/5.5 , HAUTEUR/3, LARGUEUR/1.7 , HAUTEUR/14};
		SDL_RenderCopy(renderer, background, NULL, NULL);
		SDL_GetMouseState(&mouse.x, &mouse.y);

		//anims hover
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


		//get text
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

		//printf("Etat ID = %d\nEtat MDP = %d\n",etatIdentifant,etatMotDePasse );

		// SI CLIC SOURIS //
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
			exit(0);
		}
		else if ( etatIdentifant == TF_RETURN || etatMotDePasse  == TF_RETURN )
		{
			if(thread == NULL){
				frame_anims[ANIM_LOADING] = 0;
				connectEnded = 0;
				frame_anims[ANIM_CONNECTION]=FRAME_ANIM_MAX-1;
				strcpy(tokenCpy, token);
				strcpy(identifiantCpy, identifiant);
				strcpy(motDePasseCpy, motDePasse);
				printf("CONNEXION...\n" );
				printf("%s\n", identifiantCpy);
				connectStruct = (ConnectStruct){tokenCpy,identifiantCpy,motDePasseCpy};
				thread = SDL_CreateThread(  (int(*)(void*))connectWithUsername, NULL, &connectStruct );
			}

			/*if ( !connectWithUsername(token,identifiant,motDePasse) )
			{
				pressConnexion = SDL_TRUE;
			}*/
		}
		else if(etatIdentifant == TF_MOUSE_OUT_CLICK || etatMotDePasse  == TF_MOUSE_OUT_CLICK)
		{
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
						frame_anims[ANIM_LOADING] = 0;
						connectEnded = 0;
						frame_anims[ANIM_CONNECTION]=FRAME_ANIM_MAX-1;
						printf("CONNEXION...\n" );
						strcpy(tokenCpy, token);
                        strcpy(identifiantCpy, identifiant);
                        strcpy(motDePasseCpy, motDePasse);
						connectStruct = (ConnectStruct){tokenCpy,identifiantCpy,motDePasseCpy};
						thread = SDL_CreateThread( (int(*)(void*))connectWithUsername, NULL, &connectStruct);
					}

					/*if ( !retour )
					{
						pressConnexion = SDL_TRUE;
						etatMotDePasse = RESPONDER_FALSE;
						etatIdentifant = RESPONDER_FALSE;
					}
					else if ( retour == -5 )
					{
					//	for (int i=0;i < tentative && retour == -5 ;i++) {
							printf("ECHEC : -5 DELAI DEPASSER (PB LINUX CONNU)\n");
					//		SDL_Delay(delai);
					//		retour = connectWithUsername(token,identifiant,motDePasse);
					//	}
						//if (!retour)
						//{
						//	pressConnexion = SDL_TRUE;
						//	etatMotDePasse = RESPONDER_FALSE;
						//	etatIdentifant = RESPONDER_FALSE;
						//}
					}*/

				}
				else if ( TF_ClickIn( targetInscription , mouse) )
				{
					printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n" );
					frame_anims[ANIM_INSCRIPTION]=FRAME_ANIM_MAX-1;
					ouvrirUrlRegistration();

				}

				mouse.x = 0;
				mouse.y = 0;
			}
		}





		printAll(renderer,background, loading, police, targetId, targetPwd, targetConnect, targetInscription, frame_anims);


		// permet de ne pas afficher une zone de text vide
		if( strlen(motDePasse) >= 1){
			renduTextField(renderer,motDePasse,ttf_pwd,noir,targetPwd);

		}

		if( strlen(identifiant) >= 1){
			renduTextField(renderer,identifiant,police,noir,targetId);

		}

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



		/*SDL_Rect curseur = { LARGUEUR/6.5 , HAUTEUR/4 , LARGUEUR/1.7 , HAUTEUR/14};
		curseur.x += curseur.h*0.5;
		curseur.w = curseur.h*0.5 * strlen(motDePasse);*/

		if(connectEnded && thread){
			printf("get thread value \n" );
			SDL_WaitThread(thread, &retour);
				printf("retour : %d \n", retour);
			//SDL_DetachThread(thread);
			thread = NULL;
			frame_anims[ANIM_LOADING] = 0;
		}
		else if(thread)
			frame_anims[ANIM_LOADING]++;

		frame ++;

		if(retour == EXIT_SUCCESS){
						strcpy(token,tokenCpy);
						pressConnexion = SDL_TRUE;
		}



		SDL_RenderPresent(renderer);
		SDL_RenderClear(renderer);

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


	//destruction des creations
	SDL_DestroyTexture(background);
	TTF_CloseFont(police);
	TTF_CloseFont(ttf_pwd);
}



int chargementFichier(SDL_Renderer *renderer,struct MeilleureScore_s meilleureScore[],char *token,const C_STRUCT aiScene** scene,char path[] )
{
	char concatenation[128]="";




	SDL_RenderClear(renderer);

	//FIX ANIMATION AFFICHAGE MAC
	SDL_Event fixMac;
	while ( SDL_PollEvent(&fixMac) );

	SDL_Texture* background = IMG_LoadTexture(renderer,DIR_ING_BACKGROUND_TXT);
	if(!background)
		printf("Fichier introuvable %s\n", DIR_ING_BACKGROUND_TXT);
	SDL_RenderCopy(renderer, background, NULL, NULL);


	//fond chargement
	SDL_Rect chargement = {LARGUEUR*0.05,HAUTEUR*0.85,LARGUEUR*0.90,HAUTEUR*0.08};
	SDL_SetRenderDrawColor(renderer, noir.r , noir.g, noir.b,200);
	SDL_RenderFillRect(renderer,&chargement);
	SDL_RenderPresent(renderer);


	SDL_Rect chargementAff = {LARGUEUR*0.05,HAUTEUR*0.85,0,HAUTEUR*0.08};
	int progression = LARGUEUR*0.90 / (NB_FILE+1);


	for(int i = 0 ; i < NB_FILE ; i++)
	{
		strcpy(concatenation,path);
		strcat(concatenation,verifierFichier[i]);
		FILE *fp = fopen(concatenation,"r");
		if(!fp)
		{
			printf("Fichier %s introuvable \n",concatenation );
			return SDL_FALSE;
		}
		else
		{
			fclose(fp);
		}


		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, background, NULL, NULL);

		SDL_SetRenderDrawColor(renderer, noir.r , noir.g, noir.b,200);
		SDL_RenderFillRect(renderer,&chargement);



		chargementAff.w += progression;
		SDL_SetRenderDrawColor(renderer, blanc.r , blanc.g, blanc.b,200);
		SDL_RenderFillRect(renderer,&chargementAff);


		SDL_RenderPresent(renderer);
	}


	InitMeilleureScore(meilleureScore);
	updateMeilleureScore(meilleureScore,token);


	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, background, NULL, NULL);
	SDL_SetRenderDrawColor(renderer, noir.r , noir.g, noir.b,200);
	SDL_RenderFillRect(renderer,&chargement);
	chargementAff.w += progression;
	SDL_SetRenderDrawColor(renderer, blanc.r , blanc.g, blanc.b,200);
	SDL_RenderFillRect(renderer,&chargementAff);
	SDL_RenderPresent(renderer);

	strcpy(concatenation,path);
	strcat(concatenation,DIR_OBJ_LOAD);
	aiImportModel(concatenation,scene);

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, background, NULL, NULL);
	SDL_SetRenderDrawColor(renderer, noir.r , noir.g, noir.b,200);
	SDL_RenderFillRect(renderer,&chargement);
	chargementAff.w = LARGUEUR*0.90;
	SDL_SetRenderDrawColor(renderer, blanc.r , blanc.g, blanc.b,200);
	SDL_RenderFillRect(renderer,&chargementAff);
	SDL_RenderPresent(renderer);


	SDL_DestroyTexture(background);


	return SDL_TRUE;
}









int launcher(SDL_Renderer* renderer, char *token, char *tokenCpy,struct MeilleureScore_s meilleureScore[],const C_STRUCT aiScene** scene, char path[])
{
	Mix_Music *musique = Mix_LoadMUS(DIR_MUSIC_FILE);
	if (musique == NULL)
		printf("Impossible de charger musique dans %s\n", DIR_MUSIC_FILE );

	Mix_Volume(0,MIX_MAX_VOLUME/2);
	Mix_PlayMusic(musique, -1);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	if ( !dejaConneceter(token) )
	{
		connexion(renderer,token, tokenCpy, path);
		sauvegarderToken(token);
  	}




	if( !chargementFichier(renderer,meilleureScore,token,scene,path) )
	{
		Mix_HaltMusic();
		Mix_FreeMusic(musique);
		return EXIT_FAILURE;
	}
	else
	{
		Mix_HaltMusic();
		Mix_FreeMusic(musique);
		return EXIT_SUCCESS;
	}
	printf("end room\n" );
}




int main(int argc, char *argv[])
{
    #ifdef _WIN32
       HWND hWnd = GetConsoleWindow();
       ShowWindow( hWnd, SW_HIDE );
    #endif // _WIN32

  if ( checkVersion(VERSION_LOGICIEL) == EXIT_SUCCESS)
  {
    printf("REPERTOIRE D'EXECUTION %s\n",argv[0] );
    char *addPath = fullPath(argv[0]);
    printf("AJOUTER : %s\n",addPath);
    /////////////////////////////////////////////////////////////////
    // INIT SDL // TTF // MIXER
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    Mix_Init(MIX_INIT_MP3);
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1) //Initialisation de l'API Mixer
      printf("%s", Mix_GetError());

    /////////////////////////////////////////////////////////////////
    // RECUPERER TAILLE ECRAN
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    // APPLIQUER UN RATIO
    LARGUEUR = DM.w/1.78;
    HAUTEUR = LARGUEUR*0.66667;

    /////////////////////////////////////////////////////////////////
    // INIT VARIABLE TOKEN
    char *token = malloc(sizeof(char) * SIZE_SESSION + 1);
    char *tokenCpy = malloc(sizeof(char) * SIZE_SESSION + 1);
    tokenCpy=strcpy(tokenCpy, token);
    /////////////////////////////////////////////////////////////////
    // INIT VARIABLE QUI POINTE SUR LA SCENE
    const C_STRUCT aiScene* scene = NULL;

    /////////////////////////////////////////////////////////////////
    // INIT FENETRE WINDOWS ET RENDERER
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Surface* favicon;
    /////////////////////////////////////////////////////////////////
    // CREATION WINDOWS ET RENDERER ET FAVICON
    window = SDL_CreateWindow("Nineteen | Launcher", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,LARGUEUR,HAUTEUR, 0  );
    /////////////////////////////////////////////////////////////////
    // CHARGEMENT ICON
    // NOM PROBLEMATIQUE SI NON CHARGER ON APPLIQUE PAS SI NON CHARGER
    if( ( favicon=IMG_Load("favicon.png") ) )
      SDL_SetWindowIcon(window, favicon);
    else
      printf("Erreur chargement favicon\n");
    SDL_FreeSurface(favicon);
    /////////////////////////////////////////////////////////////////
    // CREATION DU RENDU
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);


    /////////////////////////////////////////////////////////////////
    // INIT STRUCTURE MEILLEURE_SCORE
    struct MeilleureScore_s meilleureScore[16];
    /////////////////////////////////////////////////////////////////
    // APPEL DU LAUNCHER
    if( launcher(renderer,token,tokenCpy,meilleureScore,&scene,addPath) == EXIT_SUCCESS)
    {
      printf("lancement room\n" );
      SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      /////////////////////////////////////////////////////////////////
      // APPEL DE LA ROOM
      printf("ROOM : %d\n",room(token,meilleureScore,window,scene) );
    }


    /////////////////////////////////////////////////////////////////
    // LIBERER MEMOIRE
    /////////////////////////////////////////////////////////////////
    // VIDER LA MEMOIRE DE LA SCENE
    aiReleaseImport(scene);
    // VIDER MEMOIRE TOKEN
    free(token);
    free(tokenCpy);
    /////////////////////////////////////////////////////////////////
    // QUITTER TTF
    TTF_Quit();
    /////////////////////////////////////////////////////////////////
    // FERMER AUDIO
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
  }

	return 0;
}
