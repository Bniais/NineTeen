#include "config.h"
#include <stdlib.h>
#include <time.h>

#define NO_TURN -1
#define TURN_LEFT 0
#define TURN_RIGHT 1
#define TURN_UP 2
#define TURN_DOWN 3

//Dimensions du charactère dans l'image
const SDL_Point CHAR_DIM = {64,96};

#define NB_ANIM_CHAR 8.0
#define TIME_TOTAL_ANIM 1000
#define FPS 60
//On a 8 anim pour chaque dir, et ces 8 anims prennent 1sec et en 1sec il y a 60 frames donc 8 anim prendront 60 frames donc 1 anim prendra 7.5 frames

//(en réalité le sprite à plus que 5 anim mais on se limite à 5 ici)

SDL_Rect select_anim(int turn, long frame)
{
	SDL_Rect r;
	r.x=CHAR_DIM.x*(int)((frame%FPS)/(((TIME_TOTAL_ANIM/1000.0)*FPS)/NB_ANIM_CHAR));
	printf("%ld / %f = %d\n",(frame%FPS),(((TIME_TOTAL_ANIM/1000.0)*FPS)/NB_ANIM_CHAR), (int)((frame%FPS)/(((TIME_TOTAL_ANIM/1000.0)*FPS)/NB_ANIM_CHAR)));
	//on retrouve nos 7.5frames, et ça retourne un nombre entre 0/7.5 et 59/7.5 càd entre 0 et 7
	//les 7.5 premieres frames (0-6) seront pour l'anim 1 puis 7-14 pour 2, ...
	//et on multiplie ce résultat par charDim.x pour décaler le rectangle pour obtenir que l'animation ciblée


	r.y=CHAR_DIM.y*turn;
	//La ligne représente une direction, donc turn vaudra 0 pour la direction de notre premiere ligne du sprite, 1 pour la deuxième, etc...

	r.w=CHAR_DIM.x;
	r.h=CHAR_DIM.y;

	return r;
}


int main(){
	srand(time(NULL));
	//initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING);
  TTF_Init();
	char str[21];
  //Creation fenêtre
	SDL_Window* myWindow=SDL_CreateWindow("Snake", WINDOW_X, WINDOW_Y,	WINDOW_W, WINDOW_H, WINDOW_FLAG);

	//Creation renderer
	SDL_Renderer* renderer = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); // Création du renderer
	if(renderer == NULL) // Gestion des erreurs
	{
		printf("Erreur lors de la creation d'un renderer : %s",SDL_GetError());
		return EXIT_FAILURE;
	}

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	long currentTime, lastTime=0;

  TTF_Font* Sans = TTF_OpenFont("./flappy.ttf", 100); //this opens a font style and sets a size

  if(Sans==NULL)
    printf("TTF_OpenFont() Failed: %s\n", TTF_GetError());


  SDL_Color Black = {0, 0, 0};  // this is the color in rgb format, maxing out all would give you the color white, and it will be your text's color

  SDL_Surface* surfaceMessage = NULL; // as TTF_RenderText_Solid could only be used on SDL_Surface then you have to create the surface first

  SDL_Texture* Message = NULL; //now you can convert it into a texture

  SDL_Rect Message_rect; //create a rect


  //Mind you that (0,0) is on the top left of the window/screen, think a rect as the text's box, that way it would be very simple to understance

  //Now since it's a texture, you have to put RenderCopy in your game loop area, the area where the whole code executes






  //TTF
  if(TTF_Init() == -1)
  {
    fprintf(stderr, "Erreur d'initialisation de TTF_Init : %s\n", TTF_GetError());
    exit(EXIT_FAILURE);
  }


	int quit=0;
	while(!quit){
		SDL_Event event;
		//Events
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
				//fermer
					return 0;
					break;
			}
		}
		//Drawing

    //TTF

		//On le dessine
    Message_rect.x = 0;  //controls the rect's x coordinate
    Message_rect.y = 30; // controls the rect's y coordinte
    SDL_QueryTexture(Message,NULL,SDL_TEXTUREACCESS_STATIC,&(Message_rect.w), &(Message_rect.h) );

    for(int i=0; i<5;i++){
			currentTime = SDL_GetTicks();
			sprintf(str, "%ld", currentTime);
			SDL_FreeSurface(surfaceMessage);
			surfaceMessage = TTF_RenderText_Blended(Sans, str, Black);
			SDL_DestroyTexture(Message);
			Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
      SDL_RenderCopy(renderer, Message, NULL, &Message_rect); //you put the renderer's name first, the Message, the crop size(you can ignore this if you don't want to dabble with cropping), and the rect which is the size and coordinate of your texture
	  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	  SDL_RenderDrawRect(renderer, &Message_rect);
	  Message_rect.y += Message_rect.h+10;
      (Message_rect.w)/=1.5;
      (Message_rect.h)/=1.5;
      Message_rect.y += Message_rect.h; // controls the rect's y coordinte
    }
    //Don't forget too free your surface and texture
		//On affiche
		SDL_RenderPresent(renderer);


    //Next frame (pour limiter à 60fps)
    currentTime = SDL_GetTicks();
    while (currentTime-lastTime < 1000/FPS){
      currentTime = SDL_GetTicks();
    }
    lastTime = currentTime;

		//On efface
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
	}
}
