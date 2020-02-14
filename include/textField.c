#include <stdio.h>

// SDL
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

///////////////////////////////////////////////////////////////
/// \file textField.c
/// \author Mahi S.
/// \brief Permet l'interaction avec une zone de saisi
/// \version 1.0
/// \date 19 Janvier 2020
///////////////////////////////////////////////////////////////

#define MAX_SIZE 22

#define TF_MOUSE_OUT_CLICK 10
#define TF_RETURN 11
#define TF_MAX_SIZE 12
#define TF_TEXT_TYPED 13
#define TF_TAB 14
#define TF_QUIT 15


///////////////////////////////////////////////////////////////
/// \fn char SDL_Event_SecondeTouch_Convertion_USA_Keyboard (int sdlk_code)
/// \brief Converti la valeur d'une touche en touche secondaire.
///
/// \param SDLK_code code de touche
/// \return char correspondant au nouveau code de touche
///////////////////////////////////////////////////////////////
char SDL_Event_SecondeTouch_Convertion_USA_Keyboard(int sdlk_code)
{
	switch (sdlk_code) {
		// premier ligne
		case '`' : return '~';break;
		case '1' : return '!';break;
		case '2' : return '@';break;
		case '3' : return '#';break;
		case '4' : return '$';break;
		case '5' : return '%';break;
		case '6' : return '^';break;
		case '7' : return '&';break;
		case '8' : return '*';break;
		case '9' : return '(';break;
		case '0' : return ')';break;
		case '-' : return '_';break;
		case '=' : return '+';break;

		//deuxieme ligne
		case '[' : return '{';break;
		case ']' : return '}';break;
		case '\\' : return '|';break;

		//troiseme ligne
		case ';' : return ':';break;
		case '\'' : return '"';break;

		//quatrieme ligne
		case ',' : return '<';break;
		case '.' : return '>';break;
		case '/' : return '?';break;

	}
	return 0;
}

///////////////////////////////////////////////////////////////
/// \fn char SDL_Event_Charcode (int sdlk_code, int pressMaj)
/// \brief Converti les caracteres MAJ/MIN
///
/// \param int sdlk_code SDLK scancode
/// \param int pressMaj 0/1 permet de faire la bonne conversion
/// \return char code de la touche en char
///////////////////////////////////////////////////////////////
char SDL_Event_Charcode(int sdlk_code, int pressMaj)
{
	if(pressMaj)
	{
		if( ( sdlk_code >= SDLK_a && sdlk_code <= SDLK_z ) )
			return sdlk_code - ('a' - 'A');
		else
			return SDL_Event_SecondeTouch_Convertion_USA_Keyboard(sdlk_code);
	}
	else
	{
		return sdlk_code;
	}

	return 0;
}

///////////////////////////////////////////////////////////////
/// \fn void renduTextField (SDL_Renderer *renderer,char *chaine ,TTF_Font *font , SDL_Color color, SDL_Rect cible)
/// \brief sous fonction de textField()
///
/// Aucun parametre n'est a saisir, elle est appeler par textField().
/// Elle permet de copier la texture dans la zone de rendu.
///
///////////////////////////////////////////////////////////////
void renduTextField(SDL_Renderer *renderer,char *chaine ,TTF_Font *font , SDL_Color color, SDL_Rect cible)
{

	SDL_Surface *textZone = TTF_RenderText_Blended(font, chaine, color);
	if( !textZone )
		printf("Error Surface\n");
	SDL_Texture *message = SDL_CreateTextureFromSurface(renderer,textZone);
	if( !message )
		printf("Error Texture : %s\n",SDL_GetError());
	cible.x += cible.h*0.5;
	cible.w = cible.h*0.5 * strlen(chaine);
	SDL_RenderCopy(renderer, message, NULL, &cible);

	SDL_DestroyTexture(message);
	SDL_FreeSurface(textZone);
}

///////////////////////////////////////////////////////////////
/// \fn int textField (SDL_Renderer* renderer, TTF_Font *police, SDL_Color color, char *chaine, int startWhere, SDL_Rect cible)
/// \brief Recuperer une chaine de caractere saisi au clavier.
///
/// Copier la nouvelle texture dans SDL_Renderer, il est necessaire de realiser un RenderPresent().
///
/// \param SDL_Renderer *renderer Pointeur sur le rendu SDL.
/// \param TTF_Font *police Pointeur sur la police SDL.
/// \param SDL_Color color Couleur du texte a affichager.
/// \param char *chaine ecrit la chaine de caracatere dans la variable en entre.
/// \param int startWhere mettre strlen(chaine) pour reprendre a la fin de la chaine la saisi.
/// \param SDL_Rect cible indiquer la cible ou afficher le resultat.
/// \return retourne la raison de sorti (TF_MAX_SIZE, TF_TEXT_TYPED..).
///////////////////////////////////////////////////////////////
int textField(SDL_Renderer* renderer, TTF_Font *police, SDL_Color color, char *chaine, int startWhere, SDL_Rect *cible, SDL_Point *mouse,int *pressMaj)
{
	int currentSize = startWhere;
	int pressReturn = SDL_FALSE;
	int clickOut = SDL_FALSE;
	int touchPressed = SDL_FALSE;

	int retour = 0;

	do
	{
		SDL_Event evenement;
		while ( SDL_PollEvent(&evenement) )
		{

			// keyboard
			if(evenement.type == SDL_KEYDOWN)
			{
				if(evenement.key.keysym.sym == SDLK_RETURN) // Fin de la saisi
				{
					retour = TF_RETURN;
					//return pressReturn;
				}
				else if(evenement.key.keysym.sym == SDLK_TAB )
				{
					printf("Tab clic\n");
					retour = TF_TAB;
				}
				else if(evenement.key.keysym.sym == SDLK_LSHIFT || evenement.key.keysym.sym == SDLK_RSHIFT)
				{
					*pressMaj = SDL_TRUE;
				}
				else if (evenement.key.keysym.sym == SDLK_BACKSPACE)
				{
					if(currentSize)
					{
						currentSize--;
						*(chaine + currentSize) = '\0';
						//renduTextField(renderer, chaine, police, color, *cible);
						retour = TF_TEXT_TYPED;
						//return touchPressed;
					}

				}
				else if (currentSize < MAX_SIZE) {
					chaine[currentSize] = SDL_Event_Charcode(evenement.key.keysym.sym, *pressMaj);
					currentSize++;
					chaine[currentSize] = '\0';
					//renduTextField(renderer, chaine, police, color, *cible);
					retour = TF_TEXT_TYPED;
					//return touchPressed;
				}

			}
			else if(evenement.type == SDL_KEYUP)
			{
				if(evenement.key.keysym.sym == SDLK_LSHIFT || evenement.key.keysym.sym == SDLK_RSHIFT)
				{
						*pressMaj = SDL_FALSE;
				}

			}
			else if ( evenement.type == SDL_QUIT)
			{
				retour = TF_QUIT;
			}

			// mouse
			if (SDL_GetMouseState(&mouse->x, &mouse->y) && SDL_BUTTON(SDL_BUTTON_LEFT)) {
				if( ! ( mouse->x >= cible->x && mouse->x <= ( cible->x + cible->w ) && mouse->y >= cible->y && mouse->y <= (cible->y + cible->h)) )
				{

					retour = TF_MOUSE_OUT_CLICK;
					//return TF_MOUSE_OUT_CLICK;
				}

			}

		}

	} while (!retour);

	return retour;
}

int TF_ClickIn(SDL_Rect rect, SDL_Point mouse)
{
	return ( ( mouse.x >= rect.x && mouse.x <= ( rect.x + rect.w ) && mouse.y >= rect.y && mouse.y <= (rect.y + rect.h)) );
}

/*
int main(int argc, char *argv[]) {
	SDL_Init(SDL_INIT_EVERYTHING );
	TTF_Init();

	TTF_Font *police = TTF_OpenFont("police.ttf",100);

	SDL_Window* window = SDL_CreateWindow("Nineteen | Piano Tils", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 800, SDL_WINDOW_SHOWN  );
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );


	char chaine[MAX_SIZE];
	SDL_Rect cible = {0,0,400,50};
	SDL_Color rouge = {255,255,255};
	SDL_RenderClear(renderer);

	int statut = 0;
	while ( statut != TF_RETURN && statut != TF_MOUSE_OUT_CLICK )
	{
		statut = textField(renderer, police, rouge ,chaine, strlen(chaine) ,cible);
		SDL_RenderPresent(renderer);
		SDL_RenderClear(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
*/
