#include "room.h"
#include "../define/define.h"

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
	//initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING );

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

	SDL_Rect char_rect;
	SDL_Rect target= {50,50,CHAR_DIM.x,CHAR_DIM.y};

	//Creation texture
	SDL_Texture* texture = IMG_LoadTexture(renderer, "link.jpg");
	if(!texture)
	{
		printf("Erreur lors de la creation d'une texture : %s",SDL_GetError());
		return EXIT_FAILURE;
	}

	//Dimensions texture ?
	int w,h;
	SDL_QueryTexture(texture, NULL, NULL, &w, &h);
	printf("%d /  %d \n", w, h);

	int frame_move=0;	//determine combien de frame on a passé à se déplacer, resmise à zéro à chaque fois que le personnage arrête de bouger
	int turn=NO_TURN; //La direction du personnage. Si on bouge, on incrémente frame_move à chaque frame
	int lastTurn=TURN_DOWN; //La dernière direction prise (pour avoir une direction à l'arrêt)
	const Uint8 *keystate = SDL_GetKeyboardState(NULL); //dans le main



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

		//Initialiser les résultats des inputs
		if(turn==NO_TURN)
			frame_move=0;
		else{
			lastTurn=turn;
			turn=NO_TURN;
			frame_move++;
		}

		//Keyboard inputs
    SDL_PumpEvents();
		//Ici, la priorité est à l'élément le plus en haut (si on appuie sur droite+gauche, ça bouge à gauche)
    if(keystate[SDL_SCANCODE_LEFT])
      turn=TURN_LEFT;
    else if(keystate[SDL_SCANCODE_RIGHT])
      turn=TURN_RIGHT;
    else if(keystate[SDL_SCANCODE_UP])
      turn=TURN_UP;
		else if(keystate[SDL_SCANCODE_DOWN])
      turn=TURN_DOWN;

		//Drawing

		//On met à jour quel rectangle de notre sprite on va dessiner, si on ne bouge plus on le met à la frame 0 (premiere animation de la ligne)
		if(turn!=NO_TURN)
			char_rect=select_anim(turn, frame_move);
		else
			char_rect=select_anim(lastTurn, 0);

		//On le dessine
		SDL_RenderCopy(renderer, texture,&char_rect, &target);

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
