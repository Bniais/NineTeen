//#include "room.h"
#include "../define/define.h"
#include <SDL.h>

#include <SDL_image.h>

#define NO_TURN -1
#define TURN_LEFT 0
#define TURN_RIGHT 1
#define TURN_UP 2
#define TURN_DOWN 3

//Dimensions du charactère dans l'image
const SDL_Point CHAR_DIM = {64,96};

#define NB_ANIM_CHAR 8.0
#define TIME_TOTAL_ANIM 1000
#define FPS 10
//On a 8 anim pour chaque dir, et ces 8 anims prennent 1sec et en 1sec il y a 60 frames donc 8 anim prendront 60 frames donc 1 anim prendra 7.5 frames

//(en réalité le sprite à plus que 5 anim mais on se limite à 5 ici)

SDL_Rect select_anim(int turn, long frame)
{
	SDL_Rect r;
	r.x=CHAR_DIM.x*(int)((frame%FPS)/(((TIME_TOTAL_ANIM/1000.0)*FPS)/NB_ANIM_CHAR));
	//printf("%ld / %f = %d\n",(frame%FPS),(((TIME_TOTAL_ANIM/1000.0)*FPS)/NB_ANIM_CHAR), (int)((frame%FPS)/(((TIME_TOTAL_ANIM/1000.0)*FPS)/NB_ANIM_CHAR)));
	//on retrouve nos 7.5frames, et ça retourne un nombre entre 0/7.5 et 59/7.5 càd entre 0 et 7
	//les 7.5 premieres frames (0-6) seront pour l'anim 1 puis 7-14 pour 2, ...
	//et on multiplie ce résultat par charDim.x pour décaler le rectangle pour obtenir que l'animation ciblée


	r.y=CHAR_DIM.y*turn;
	//La ligne représente une direction, donc turn vaudra 0 pour la direction de notre premiere ligne du sprite, 1 pour la deuxième, etc...

	r.w=CHAR_DIM.x;
	r.h=CHAR_DIM.y;

	return r;
}

typedef struct {float x; float y;}v2f;
v2f vaisseau_dest_f = {200,200};

v2f acce_dest = {0,0};
SDL_Rect thrust_src ={0,0,72,70};
SDL_Rect vaisseau_srcr = {0,0,72,70};
SDL_Rect vaisseau_srcl = {0,70,72,70};
SDL_Rect vaisseau_dest = {200,200,72,70};
SDL_Rect gem_src = {0,0,72,52};
//SDL_Point center = {72/2, 70/2};
int signt = -1;
int signl = -1;
int signr = -1;
int main(){
	//initialize SDL
	SDL_Init(SDL_INIT_EVERYTHING );

	//Creation fenêtre
	SDL_Window* myWindow=SDL_CreateWindow("Snake", 0, 0,	500, 500, WINDOW_FLAG);

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
	SDL_Texture* vaisseau = IMG_LoadTexture(renderer, "vaisseau.png");
	SDL_Texture* gem = IMG_LoadTexture(renderer, "gem.png");
	SDL_Texture* thrust = IMG_LoadTexture(renderer, "thrust.png");
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

	int angle = 0;
	int angleChange[5] = { 1, 2, 5 , 8, 13};
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
    if(keystate[SDL_SCANCODE_LEFT]){
		signl = 1;
	}
	else{
		signl = -1;
		if(vaisseau_srcl.x>5*vaisseau_srcl.w)
			vaisseau_srcl.x=5*vaisseau_srcl.w;
	}

    if(keystate[SDL_SCANCODE_RIGHT]){

		signr = 1;
	}
    else {

		signr = -1;
		if(vaisseau_srcr.x>5*vaisseau_srcr.w)
			vaisseau_srcr.x=5*vaisseau_srcr.w;
	}

	if(keystate[SDL_SCANCODE_UP]){

		signt = 1;
	}
    else {

		signt = -1;
		if(thrust_src.x>1*thrust_src.w)
			thrust_src.x=1*thrust_src.w;
	}

		//Drawing

		//On met à jour quel rectangle de notre sprite on va dessiner, si on ne bouge plus on le met à la frame 0 (premiere animation de la ligne)
	if(turn!=NO_TURN)
		char_rect=select_anim(turn, frame_move);
	else
		char_rect=select_anim(lastTurn, 0);


	printf("%d\n", vaisseau_srcr.x/vaisseau_srcr.w);
	printf("%d\n", vaisseau_srcl.x/vaisseau_srcl.w);

	vaisseau_srcl.x = vaisseau_srcl.x + signl * vaisseau_srcl.w;
	vaisseau_srcr.x = vaisseau_srcr.x + signr * vaisseau_srcr.w;
	thrust_src.x = thrust_src.x + signt * thrust_src.w;


	acce_dest.x/=1.05;
	acce_dest.y/=1.05;
	if(thrust_src.x>0){

		acce_dest.x += 3* sin(3.1415*angle/180);
		acce_dest.y -=  3*cos(3.1415*angle/180);
	}

	vaisseau_dest_f.x += acce_dest.x;
	vaisseau_dest_f.y += acce_dest.y;

	//left
	if(vaisseau_srcl.x > 8*vaisseau_srcl.w)
		vaisseau_srcl.x = 6*vaisseau_srcl.w;

	if(vaisseau_srcl.x < 0)
		vaisseau_srcl.x = 0;

	if(vaisseau_srcl.x/vaisseau_srcl.w<6 && vaisseau_srcl.x/vaisseau_srcl.w>0){
		acce_dest.x += (angleChange[vaisseau_srcl.x/vaisseau_srcl.w - 1])/20. * sin(3.1415*(angle-45)/180);
		acce_dest.y -= (angleChange[vaisseau_srcl.x/vaisseau_srcl.w - 1])/20. * cos(3.1415*(angle-45)/180);
		angle-=angleChange[vaisseau_srcl.x/vaisseau_srcl.w - 1];
	}

	else if(vaisseau_srcl.x/vaisseau_srcl.w>=5){
		acce_dest.x +=  sin(3.1415*(angle+45)/180);
		acce_dest.y -=  cos(3.1415*(angle+45)/180);
		angle-=20;
	}


	//right
	if(vaisseau_srcr.x > 8*vaisseau_srcr.w)
		vaisseau_srcr.x = 6*vaisseau_srcr.w;

	if(vaisseau_srcr.x < 0)
		vaisseau_srcr.x = 0;

	if(vaisseau_srcr.x/vaisseau_srcr.w<5 && vaisseau_srcr.x/vaisseau_srcr.w>0){
		acce_dest.x += (angleChange[vaisseau_srcr.x/vaisseau_srcr.w - 1])/20. * sin(3.1415*(angle+45)/180);
		acce_dest.y -= (angleChange[vaisseau_srcr.x/vaisseau_srcr.w - 1])/20. * cos(3.1415*(angle+45)/180);
		angle+=angleChange[vaisseau_srcr.x/vaisseau_srcr.w - 1];
	}
	else if(vaisseau_srcr.x/vaisseau_srcr.w>=5){
		acce_dest.x +=  sin(3.1415*(angle-45)/180);
		acce_dest.y -=  cos(3.1415*(angle-45)/180);
		angle+=20;
	}


	//thrust
	if(thrust_src.x > 4*thrust_src.w)
		thrust_src.x = 2*thrust_src.w;




	/*SDL_RenderCopy(renderer, vaisseau, &vaisseau_src, &vaisseau_dest0);
	SDL_RenderCopy(renderer, vaisseau, &vaisseau_src, &vaisseau_dest);
	SDL_RenderCopy(renderer, vaisseau, &vaisseau_src, &vaisseau_dest2);
	//SDL_SetTextureColorMod(gem, rand()%256,rand()%256,rand()%256);*/
	vaisseau_dest.x = vaisseau_dest_f.x;
	vaisseau_dest.y = vaisseau_dest_f.y;

	SDL_RenderCopyEx(renderer, vaisseau, &vaisseau_srcl, &vaisseau_dest,angle,NULL,SDL_FLIP_NONE);
	SDL_RenderCopyEx(renderer, vaisseau, &vaisseau_srcr, &vaisseau_dest,angle,NULL,SDL_FLIP_NONE);
	if(thrust_src.x < 0)
		thrust_src.x = 0;
	else{
		SDL_RenderCopyEx(renderer, thrust, &thrust_src, &vaisseau_dest,angle,NULL,SDL_FLIP_NONE);
	}
	SDL_SetTextureColorMod(gem, 255,0,0);
	/*SDL_RenderCopy(renderer, gem, &vaisseau_src, &vaisseau_dest0);
	SDL_RenderCopy(renderer, gem, &vaisseau_src, &vaisseau_dest);
	SDL_RenderCopy(renderer, gem, &vaisseau_src, &vaisseau_dest2);*/
	SDL_RenderCopyEx(renderer, gem, &gem_src, &vaisseau_dest,angle,NULL,SDL_FLIP_NONE);




	//On le dessine
	//SDL_RenderCopy(renderer, texture,&char_rect, &target);

	//On affiche
	SDL_RenderPresent(renderer);


    //Next frame (pour limiter à 60fps)
    currentTime = SDL_GetTicks();
    while (currentTime-lastTime < 1000/FPS){
      currentTime = SDL_GetTicks();
    }
    lastTime = currentTime;

	//On efface
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
	}
}
