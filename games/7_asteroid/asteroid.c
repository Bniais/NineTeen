#include "define.h"
#include "config.h"
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
//#include <SDL_ttf.h>
//#include <SDL_mixer.h>
//#include <SDL_image.h>

#include <stdlib.h>
#include <stdio.h>

#include <math.h>

void myInit(){

	// SDL Init
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();

	/*//audio
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	if( Mix_OpenAudio( MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 1024 ) == -1 )
		printf("Erreur init SDL_Mixer\n" );*/

	//rand
	srand(time(NULL));
}

SDL_Point maximizeWindow(SDL_Rect displayBounds, float* ratioWindowSize){
	SDL_Point maxW = {(PLAYGROUND_SIZE_W + 2 * HUD_W), (PLAYGROUND_SIZE_H + 2 * HUD_H)};
	if( maxW.x > displayBounds.w - ESPACE_DISPLAY_WINDOW.x  ||  maxW.y > (displayBounds.h - ESPACE_DISPLAY_WINDOW.y) - ESPACE_DISPLAY_WINDOW.y){
		if( (float)maxW.x/maxW.y > (float)(displayBounds.w - ESPACE_DISPLAY_WINDOW.x)/(displayBounds.h - ESPACE_DISPLAY_WINDOW.y) ){
			*ratioWindowSize = (float)maxW.x / (displayBounds.w - ESPACE_DISPLAY_WINDOW.x);
			maxW.y /= (float)maxW.x / (displayBounds.w - ESPACE_DISPLAY_WINDOW.x);
			maxW.x = displayBounds.w - ESPACE_DISPLAY_WINDOW.x;
		}
		else{
			*ratioWindowSize = (float)maxW.y / (displayBounds.h - ESPACE_DISPLAY_WINDOW.y);
			maxW.x /= (float)maxW.y / (displayBounds.h - ESPACE_DISPLAY_WINDOW.y);
			maxW.y = (displayBounds.h - ESPACE_DISPLAY_WINDOW.y);
		}
	}
	return maxW;
}

void afficherPauseMenu(SDL_Renderer *renderer, SDL_Point mouseCoor, SDL_Texture *voileTexture, SDL_Texture *pauseMenuTexture){
	//voile noir
	SDL_RenderCopy(renderer, voileTexture, NULL, NULL);
	SDL_Rect dest = BOUTON_PAUSE_BASE;
	SDL_Rect src = {0, 0, BOUTON_PAUSE_SIZE_W, BOUTON_PAUSE_SIZE_H};

	//boutons liste
	for(int i = 0; i < NB_BOUTON_PAUSE; i++){
		if( mouseCoor.x >= dest.x && mouseCoor.x <= dest.x + dest.w && mouseCoor.y >= dest.y && mouseCoor.y <= dest.y + dest.h)
			src.y = BOUTON_PAUSE_SIZE_H;
		else
			src.y = 0;

		SDL_RenderCopy(renderer, pauseMenuTexture, &src, &dest);
		src.x += BOUTON_PAUSE_SIZE_W;
		dest.y += BOUTON_PAUSE_SIZE_H + ESPACEMENT_BOUTON_PAUSE;
	}
}

//BONUS

void detruire_asteroid(Asteroid ** asteroides, int * nb_asteroid, int i_asteroid, Vaiss * vaisseau,int touche_bouclier, int *frame_2asteroid);

void recoit_bonus(int id_bonus, Vaiss * vaisseau, Asteroid ** asteroides, int * nb_asteroid, int * frame_2asteroid){

	switch (id_bonus) {
		case VITESSE_DE_TIR:
			vaisseau->temps_recharge/=2;
			break;

		case TIR_MULTIPLE:
			if(vaisseau->nb_tir<NB_TIR_MAX){
		 		(vaisseau->nb_tir)++;
			}
			break;

	  case BOUCLIER:
			vaisseau->bouclier=1;
			break;

	  case BONUS_VITESSE_MISSILE:
			vaisseau->vitesse_missile*=BONUS_ACCELERATION_MISSILE;
			if(	vaisseau->vitesse_missile>VITESSE_MISSILE_MAX){
					vaisseau->vitesse_missile=VITESSE_MISSILE_MAX;
			}
			break;

	  case DEGAT:
			if(vaisseau->degat_missile<DEGAT_MISSILE_MAX){
				(vaisseau->degat_missile)++;
			}
			break;

	  case BOMBE_NUCLEAIRE:
			for(int i=0;i<*nb_asteroid;i++){
				detruire_asteroid(asteroides, nb_asteroid, i, vaisseau, SDL_TRUE, frame_2asteroid);
			}
			*frame_2asteroid=FRAME_BOMBE_NUCLEAIRE;
			break;

	}
}



// Collision Mur

void collision_mur(float * x, float * y, float rayon){

	int deltaGauche = 0 - (*x + rayon);
	int deltaDroite = PLAYGROUND_SIZE_W - (*x - rayon);
	int deltaHaut = 0 - (*y + rayon);
	int deltaBas = PLAYGROUND_SIZE_H - (*y - rayon);

	if(deltaDroite <= 0){
		*x= rayon - deltaDroite;
	}
	if(deltaGauche >= 0){
		*x= PLAYGROUND_SIZE_W - rayon - deltaGauche;
	}

	if(deltaBas <= 0){
		*y= rayon - deltaBas;
	}
	if(deltaHaut >= 0){
		*y= PLAYGROUND_SIZE_H - rayon - deltaHaut;
	}

}

float dist_2f(float x1, float y1, float x2, float y2){
	return sqrt(pow(x2-x1,2)+pow(y2-y1,2));
}

int trop_pres(float x1, float y1, float x2, float y2, float dist){
    if(x2-x1 > PLAYGROUND_SIZE_W/2)
        x2-= PLAYGROUND_SIZE_W;
    else if(x1-x2 > PLAYGROUND_SIZE_W/2)
        x1-= PLAYGROUND_SIZE_W;

    if(y2-y1 > PLAYGROUND_SIZE_H/2)
        y2-= PLAYGROUND_SIZE_H;
    else if(y1-y2 > PLAYGROUND_SIZE_H/2)
        y1-= PLAYGROUND_SIZE_H;

    return (dist_2f(x1,y1,x2,y2)<=dist) ;

}


int vaisseau_touche(Vaiss vaisseau, Asteroid * asteroides, int nb_asteroid){
	for(int i=0;i<nb_asteroid;i++){
		if(trop_pres(vaisseau.x,vaisseau.y,asteroides[i].x,asteroides[i].y,asteroides[i].taille+RAYON_VAISS)){
			return i;
		}
	}
	return -1;
}

int asteroid_touche(Asteroid asteroid, Missile * missiles, int nb_missiles ){

	for(int i=0;i<nb_missiles;i++){
		if(trop_pres(asteroid.x,asteroid.y,missiles[i].x,missiles[i].y,asteroid.taille+RAYON_MISSILE)){
			return i;
		}
	}
	return -1;
}


void decaler_gauche_a(Asteroid * tab, int taille_tab, int i){

    for(;i<taille_tab-1;i++){
        tab[i]=tab[i+1];
    }
}
void decaler_gauche_m(Missile * tab, int taille_tab, int i){

    for(;i<taille_tab-1;i++){
        tab[i]=tab[i+1];
    }
}
int est_taille(Asteroid asteroid){
	if(asteroid.taille==TAILLE_ASTEROID[PETIT]){
		return PETIT;
	}
	else if(asteroid.taille==TAILLE_ASTEROID[MOYEN]){
		return MOYEN;
	}
	else return GRAND;
}

void detruire_asteroid(Asteroid ** asteroides, int * nb_asteroid, int i_asteroid,Vaiss * vaisseau,int touche_bouclier,int *frame_2asteroid){

    if((*asteroides)[i_asteroid].taille>=TAILLE_ASTEROID[MOYEN] && !touche_bouclier && (*asteroides)[i_asteroid].bonus != BOMBE_NUCLEAIRE){
			if(est_taille((*asteroides)[i_asteroid])==GRAND){
				(*asteroides)[i_asteroid].pv=PV[MOYEN];
			}
			else {
				(*asteroides)[i_asteroid].pv=PV[PETIT];
			}
      (*asteroides)[i_asteroid].taille=TAILLE_ASTEROID[est_taille((*asteroides)[i_asteroid])-1];
      (*asteroides)[i_asteroid].angle+=PI/2;
			(*nb_asteroid)++;
      (*asteroides)=realloc(*asteroides,sizeof(Asteroid)*(*nb_asteroid));
			(*asteroides)[*nb_asteroid-1].x = (*asteroides)[i_asteroid].x;
	    (*asteroides)[*nb_asteroid-1].y = (*asteroides)[i_asteroid].y;
	    (*asteroides)[*nb_asteroid-1].taille = (*asteroides)[i_asteroid].taille;
	    (*asteroides)[*nb_asteroid-1].angle = (*asteroides)[i_asteroid].angle;
      (*asteroides)[*nb_asteroid-1].angle += PI;
			(*asteroides)[*nb_asteroid-1].bonus = NO_BONUS;
			(*asteroides)[*nb_asteroid-1].vitesse=(*asteroides)[i_asteroid].vitesse;
			(*asteroides)[*nb_asteroid-1].pv=(*asteroides)[i_asteroid].pv;

    }
    else{

        decaler_gauche_a(*asteroides, *nb_asteroid, i_asteroid);
        (*nb_asteroid)--;

        if(*nb_asteroid !=0){
            (*asteroides)=realloc(*asteroides,sizeof(Asteroid)*(*nb_asteroid));
        }

    }

		if((*asteroides)[i_asteroid].bonus != NO_BONUS){
			if((*asteroides)[i_asteroid].bonus!=BOMBE_NUCLEAIRE){
				recoit_bonus((*asteroides)[i_asteroid].bonus,vaisseau,asteroides,nb_asteroid,frame_2asteroid);
				(*asteroides)[i_asteroid].bonus=NO_BONUS;
			}
			else{
				recoit_bonus((*asteroides)[i_asteroid].bonus,vaisseau,asteroides,nb_asteroid,frame_2asteroid);
			}

		}
}
// Vaisseau
void turnLeft(Vaiss * vaisseau){
	vaisseau->angle -= TURN_AMMOUNT;

	if(vaisseau->angle < 0){
		vaisseau->angle += 2 * PI;
	}
}

void turnRight(Vaiss * vaisseau){
	vaisseau->angle += TURN_AMMOUNT;
}

void acceleration( Vector2f *accelerate, Vaiss vaisseau){
	accelerate->x+=ACCEL*cos(vaisseau.angle);
	accelerate->y+=ACCEL*sin(vaisseau.angle);
}

void move( Vaiss *vaisseau,Vector2f accelerate){
	vaisseau->x+=accelerate.x;
	vaisseau->y+=accelerate.y;

}

void afficher_vaisseau( Vaiss vaisseau, SDL_Renderer *renderer){

	SDL_Rect vaisseauRect={(int)vaisseau.x-RAYON_VAISS, (int)vaisseau.y-RAYON_VAISS,RAYON_VAISS*2,RAYON_VAISS*2};
	if(vaisseau.bouclier){
			SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
	}
	else
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer,&vaisseauRect);
	SDL_RenderDrawLine(renderer, vaisseauRect.x+RAYON_VAISS , vaisseauRect.y+RAYON_VAISS, vaisseauRect.x+RAYON_VAISS+DISTANCE_CANON*cos(vaisseau.angle) , vaisseauRect.y+RAYON_VAISS+DISTANCE_CANON*sin(vaisseau.angle));

	if(vaisseauRect.x<0){
		vaisseauRect.x+=PLAYGROUND_SIZE_W;
	}
	else if(vaisseauRect.x+RAYON_VAISS*2>PLAYGROUND_SIZE_W){
		vaisseauRect.x-=PLAYGROUND_SIZE_W;
	}
	SDL_RenderFillRect(renderer,&vaisseauRect);
	SDL_RenderDrawLine(renderer, vaisseauRect.x+RAYON_VAISS , vaisseauRect.y+RAYON_VAISS, vaisseauRect.x+RAYON_VAISS+DISTANCE_CANON*cos(vaisseau.angle) , vaisseauRect.y+RAYON_VAISS+DISTANCE_CANON*sin(vaisseau.angle));

	if(vaisseauRect.y<0){
		vaisseauRect.y+=PLAYGROUND_SIZE_H;
	}
	else if(vaisseauRect.y+RAYON_VAISS*2>PLAYGROUND_SIZE_H){
		vaisseauRect.y-=PLAYGROUND_SIZE_H;
	}

	SDL_RenderFillRect(renderer,&vaisseauRect);
	SDL_RenderDrawLine(renderer, vaisseauRect.x+RAYON_VAISS , vaisseauRect.y+RAYON_VAISS, vaisseauRect.x+RAYON_VAISS+DISTANCE_CANON*cos(vaisseau.angle) , vaisseauRect.y+RAYON_VAISS+DISTANCE_CANON*sin(vaisseau.angle));

	if(vaisseauRect.x<0){
		vaisseauRect.x+=PLAYGROUND_SIZE_W;
	}
	else if(vaisseauRect.x+RAYON_VAISS*2>PLAYGROUND_SIZE_W){
		vaisseauRect.x-=PLAYGROUND_SIZE_W;
	}
	SDL_RenderFillRect(renderer,&vaisseauRect);
	SDL_RenderDrawLine(renderer, vaisseauRect.x+RAYON_VAISS , vaisseauRect.y+RAYON_VAISS, vaisseauRect.x+RAYON_VAISS+DISTANCE_CANON*cos(vaisseau.angle) , vaisseauRect.y+RAYON_VAISS+DISTANCE_CANON*sin(vaisseau.angle));


	SDL_SetRenderDrawColor(renderer,255,0,0,255);
	SDL_RenderDrawPoint(renderer, vaisseauRect.x+RAYON_VAISS ,vaisseauRect.y+RAYON_VAISS);

}


//MISSILE
 void tirer(Vaiss * vaisseau, Missile ** missiles, int * nb_missiles){

	 *nb_missiles+=vaisseau->nb_tir;
	 (*missiles) = realloc(*missiles,*nb_missiles * sizeof(Missile));

	 for(int i=0;i<vaisseau->nb_tir;i++){
		 (*missiles)[*nb_missiles-1-i].x=vaisseau->x+DISTANCE_CANON*cos(vaisseau->angle);
		 (*missiles)[*nb_missiles-1-i].y=vaisseau->y+DISTANCE_CANON*sin(vaisseau->angle);
		 (*missiles)[*nb_missiles-1-i].angle=vaisseau->angle+angle_tir_multiple[vaisseau->nb_tir-1][i];
		 (*missiles)[*nb_missiles-1-i].frame=DUREE_MISSILE;
		 (*missiles)[*nb_missiles-1-i].vitesse=vaisseau->vitesse_missile;
		 (*missiles)[*nb_missiles-1-i].degat=vaisseau->degat_missile;
	 }

	 vaisseau->frame_recharge=vaisseau->temps_recharge;

}

void mouvement_tir(Missile * shot){

	shot->x+=shot->vitesse*cos(shot->angle);
	shot->y+=shot->vitesse*sin(shot->angle);

}

void update_frame(Missile ** missiles, int * nb_missiles, Vaiss * vaisseau, long int * frame, int * frame_apparition_asteroid, float * vitesse_spawn, int * frame_2asteroid){
	for(int i=0;i<*nb_missiles;i++){
		((*missiles)[i].frame)--;
			if((*missiles)[i].frame == 0){

				decaler_gauche_m(*missiles, *nb_missiles, i);
				(*nb_missiles)--;
				if(*nb_missiles != 0){
							(*missiles)=realloc(*missiles,sizeof(Missile)* (*nb_missiles));
				}

			}
	}
	if(vaisseau->frame_recharge){
		(vaisseau->frame_recharge)--;
	}
	(*frame)++;
	(*frame_apparition_asteroid)--;
	if(*vitesse_spawn > VITESSE_SPAWN_MIN){
		(*vitesse_spawn)-= ACCELERATION_SPAWN;
	}
	if(*frame_2asteroid){
		(*frame_2asteroid)--;
	}
}

void afficher_tir( SDL_Renderer * renderer, Missile shot){

	SDL_Rect missileRect={(int)shot.x-RAYON_MISSILE, (int)shot.y-RAYON_MISSILE,RAYON_MISSILE*2,RAYON_MISSILE*2};
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer,&missileRect);
}

//ASTEROID

void spawn_asteroid(Vaiss vaisseau, Asteroid ** asteroides, int * nb_asteroid){
	int proche;
	(*nb_asteroid)++;
	(*asteroides)=realloc((*asteroides),sizeof(Asteroid)*(*nb_asteroid));
	(*asteroides)[*nb_asteroid-1].taille=TAILLE_ASTEROID[rand()%NB_TAILLE];
	(*asteroides)[*nb_asteroid-1].angle=(rand()%628)/100;
	(*asteroides)[*nb_asteroid-1].pv=PV[est_taille((*asteroides)[*nb_asteroid-1])];
	(*asteroides)[*nb_asteroid-1].vitesse=rand()%10+1;
	if(rand()%PROBA_BONUS==0){
		(*asteroides)[*nb_asteroid-1].bonus=rand()%NB_BONUS ;
		printf("BONUS : %d \n",(*asteroides)[*nb_asteroid-1].bonus);
	}
	else (*asteroides)[*nb_asteroid-1].bonus=NO_BONUS;

	do{
		proche=0;

		(*asteroides)[*nb_asteroid-1].x= rand()%PLAYGROUND_SIZE_W;
		(*asteroides)[*nb_asteroid-1].y= rand()%PLAYGROUND_SIZE_H;

		for(int i=0;i<(*nb_asteroid)-1;i++){

			if(trop_pres(	(*asteroides)[i].x,	(*asteroides)[i].y,	(*asteroides)[*nb_asteroid-1].x,(*asteroides)[*nb_asteroid-1].y,DIST_2ASTEROID+(*asteroides)[i].taille+(*asteroides)[*nb_asteroid-1].taille)){
				proche=1;
				break;
			}
		}

	}	while(trop_pres(vaisseau.x,vaisseau.y,(*asteroides)[*nb_asteroid-1].x,(*asteroides)[*nb_asteroid-1].y,DIST_2ASTEROID+RAYON_VAISS+(*asteroides)[*nb_asteroid-1].taille)|| proche);

}
void mouvement_asteroid(Asteroid* asteroid){

	asteroid->x+=asteroid->vitesse*cos(asteroid->angle);
	asteroid->y+=asteroid->vitesse*sin(asteroid->angle);

}


void afficher_asteroid(Asteroid asteroid, SDL_Renderer * renderer){



	SDL_Rect asteroidRect={(int)asteroid.x-asteroid.taille, (int)asteroid.y-asteroid.taille,asteroid.taille*2,asteroid.taille*2};
	if(asteroid.bonus != -1){
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	}
	else SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

	SDL_RenderFillRect(renderer,&asteroidRect);

	if(asteroidRect.x<0){
		asteroidRect.x+=PLAYGROUND_SIZE_W;
	}
	else if(asteroidRect.x+asteroid.taille*2>PLAYGROUND_SIZE_W){
		asteroidRect.x-=PLAYGROUND_SIZE_W;
	}

	SDL_RenderFillRect(renderer,&asteroidRect);

	if(asteroidRect.y<0){
		asteroidRect.y+=PLAYGROUND_SIZE_H;
	}
	else if(asteroidRect.y+asteroid.taille*2>PLAYGROUND_SIZE_H){
		asteroidRect.y-=PLAYGROUND_SIZE_H;
	}

	SDL_RenderFillRect(renderer,&asteroidRect);

	if(asteroidRect.x<0){
		asteroidRect.x+=PLAYGROUND_SIZE_W;
	}
	else if(asteroidRect.x+asteroid.taille*2>PLAYGROUND_SIZE_W){
		asteroidRect.x-=PLAYGROUND_SIZE_W;
	}

	SDL_RenderFillRect(renderer,&asteroidRect);

}

// int launchSnake(SDL_Window *myWindow, SDL_Renderer* renderer, char *identifiant, char *token){
int main(){
/////////////////////
/// MISE EN PLACE ///``
/////////////////////
	myInit();

	////////////
	/// Vars ///`
	////////////

	//Keyboard
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);

	//Time
	unsigned int lastTime = 0, currentTime;
	long int frame=0;
	int frame_apparition_asteroid= 0;
	float vitesse_spawn = VITESSE_SPAWN_INIT;
	int frame_2asteroid= 0;


	//Fonts
	/*TTF_Font* scoreFont = TTF_OpenFont("./fonts/flappy.ttf", OPEN_FONT_SIZE);
	if( scoreFont == NULL ){
		printf("TTF_OpenFont() Failed: %s\n", TTF_GetError());
		return EXIT_FAILURE;
	}*/

	//audio
	/*Mix_Chunk *flap_wav = Mix_LoadWAV( "../3_flappy_bird/Sounds/flap.wav" );
	if( !flap_wav)
		printf("Erreur chargement des sons\n");*/

	//Window and renderer

	//VAISSEAU
 	Vaiss vaisseau={500,500,BASE_ANGLE,0,RECHARGE_TIR,1,0,VITESSE_MISSILE,DEGAT_MISSILE};
	Vector2f accelerate={0,0};

	//MISSILES
	Missile * missiles=malloc(sizeof(Missile));
	int nb_missiles=0;

	//ASTEROID
	Asteroid * asteroides=malloc(sizeof(Asteroid));
	int nb_asteroid=0;

	float ratioWindowSize = 1;
	SDL_Rect displayBounds;
	SDL_GetDisplayBounds(0, &displayBounds);
	SDL_Point maxWindowSize = maximizeWindow(displayBounds, &ratioWindowSize);



	SDL_Window *myWindow = SDL_CreateWindow("Asteroïde", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, maxWindowSize.x ,maxWindowSize.y, WINDOW_FLAG);
	if( myWindow == NULL ){
		printf("Erreur lors de la creation de la fenêtre : %s", SDL_GetError());
		return EXIT_FAILURE;
	}
	printf("%d, %d\n", maxWindowSize.x ,maxWindowSize.y );

	SDL_Renderer *renderer = SDL_CreateRenderer(myWindow, -1, SDL_RENDERER_ACCELERATED);
	if( renderer == NULL ){
		printf("Erreur lors de la creation d'un renderer : %s", SDL_GetError());
		return EXIT_FAILURE;
	}
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	//Views
	SDL_Rect playgroundView = {HUD_W/ratioWindowSize, HUD_H/ratioWindowSize, PLAYGROUND_SIZE_W, PLAYGROUND_SIZE_H};
	SDL_Rect hudView = {0, 0, (PLAYGROUND_SIZE_W + 2 * HUD_W), (PLAYGROUND_SIZE_H + 2 * HUD_H)};
	SDL_Rect hudDraw = {0, 0, (PLAYGROUND_SIZE_W + 2 * HUD_W)/ratioWindowSize, (PLAYGROUND_SIZE_H + 2 * HUD_H)/ratioWindowSize};

	//hud and menus
	int paused = 0;
	int rdyToPause = 1;

	//mouse
	SDL_Point mouseCoor;

	//Textures
	SDL_Texture *hudTexture = IMG_LoadTexture(renderer, DIR_HUD);
	if( hudTexture == NULL ){
		printf("Erreur lors de la creation de texture");
		return EXIT_FAILURE;
	}

	//Vaisseau
	int turn;

//	SDL_Rect vaisseau = {500,500,20,20};
	///////////////////////
	/// Initialize vars ///`
	///////////////////////


/////////////////////
/// BOUCLE DU JEU ///``
/////////////////////

	while( 1 ){

		// Init input
		SDL_GetMouseState(&(mouseCoor.x), &(mouseCoor.y));
		turn=NO_TURN;
	////////////
	// Events //`
	////////////
		SDL_Event event;
		while( SDL_PollEvent(&event) ){
			switch( event.type ){
				case SDL_QUIT:
					// fermer
					return 0;
					break;

				/*case SDL_MOUSEBUTTONDOWN:
					// gestion souris
					if( event.button.button == SDL_BUTTON_LEFT ){ // test
						if( (SDL_GetWindowFlags(myWindow) | SDL_WINDOW_FULLSCREEN) != SDL_GetWindowFlags(myWindow)){
							SDL_SetWindowFullscreen( myWindow , SDL_TRUE);
						}
						else{
							SDL_SetWindowFullscreen( myWindow , SDL_FALSE);
						}
					}
					break;
*/
				case SDL_KEYUP:
					if ( event.key.keysym.sym == SDLK_ESCAPE )
						rdyToPause = 1;
					break;

				case SDL_KEYDOWN:
					if ( event.key.keysym.sym == SDLK_ESCAPE && rdyToPause ){
						paused = !paused;
						rdyToPause = 0;
					}
					break;
			}
		}


	////////////////////////////
	// Handle Keyboard inputs //`
	////////////////////////////
		SDL_PumpEvents();


		if( keystate[SDL_SCANCODE_LEFT] )
			turn = LEFT;
		else if( keystate[SDL_SCANCODE_RIGHT] )
			turn = RIGHT;


	//////////////
	// Gameplay //`
	//////////////

	if( turn == LEFT )
		turnLeft(&vaisseau);
	if( turn == RIGHT )
		turnRight(&vaisseau);


	if( keystate[SDL_SCANCODE_UP] ){
        acceleration(&accelerate,vaisseau);

    }

  accelerate.x/=DECELERATION;
  accelerate.y/=DECELERATION;

	//printf("%d",frame_apparition_asteroid);
	//printf("\n");
	move(&vaisseau,accelerate);
	if( keystate[SDL_SCANCODE_DOWN] && vaisseau.frame_recharge == 0){
 		tirer(&vaisseau,&missiles,&nb_missiles);
	}

	for(int i=0;i<nb_missiles; i++){
		mouvement_tir(&missiles[i]);

	}

	for(int i=0;i<nb_asteroid; i++){
		mouvement_asteroid(&asteroides[i]);
	}
	///////////////////
	// Check hitboxs //`
	///////////////////

	collision_mur(&vaisseau.x,&vaisseau.y, RAYON_VAISS);
	for(int i=0;i<nb_asteroid;i++){
			collision_mur(&asteroides[i].x,&asteroides[i].y,asteroides[i].taille);

			int i_missile=asteroid_touche(asteroides[i],missiles,nb_missiles);
			if(i_missile!=-1){
				printf("POINT DE VIE AVANT : %d \n",	asteroides[i].pv);
				asteroides[i].pv-=missiles[i_missile].degat;
				decaler_gauche_m(missiles, nb_missiles, i_missile);
				(nb_missiles)--;
				if(nb_missiles !=0){
						missiles=realloc(missiles,sizeof(Missile)*(nb_missiles));
				}
				printf("POINT DE VIE : %d \n",	asteroides[i].pv);
				printf("DEGAT DU MISSILE : %f \n",missiles[i_missile].degat);
				if(asteroides[i].pv<=0){
						detruire_asteroid(&asteroides,&nb_asteroid,i,&vaisseau,SDL_FALSE, &frame_2asteroid);
				}

				//si--;
			}
	}
	int i_touche=vaisseau_touche(vaisseau,asteroides,nb_asteroid);
	if(i_touche != -1){
		if(vaisseau.bouclier){
			vaisseau.bouclier=0;
			detruire_asteroid(&asteroides,&nb_asteroid,i_touche, &vaisseau,SDL_TRUE,&frame_2asteroid);
		}
		else break;
	}
	for(int i=0;i<nb_missiles;i++){
			collision_mur(&missiles[i].x,&missiles[i].y,RAYON_MISSILE);

	}



	//////////
	// Draw //`
	//////////


	afficher_vaisseau(vaisseau,renderer);

	for(int i=0; i<nb_missiles; i++){
		afficher_tir(renderer, missiles[i]);
	}

	if((frame_apparition_asteroid==0 || nb_asteroid <= (frame/FRAME_APPARITION_ASTEROID))&& frame_2asteroid == 0 ){
		spawn_asteroid(vaisseau,&asteroides,&nb_asteroid);
		frame_2asteroid=FRAME_2ASTEROID;
		frame_apparition_asteroid=vitesse_spawn;

	}

	for(int j=0;j<nb_asteroid;j++){
		afficher_asteroid(asteroides[j],renderer);
	}
		//hud
		SDL_RenderSetScale(renderer, 1, 1);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderSetViewport(renderer, &hudView);
		SDL_RenderCopy(renderer, hudTexture, &hudView, &hudDraw);
		SDL_RenderSetViewport(renderer, &playgroundView);

		//afficher
		SDL_RenderSetScale(renderer, 1. / ratioWindowSize, 1. / ratioWindowSize);
		SDL_RenderPresent(renderer);


	////////////////
	// Next frame //`
	////////////////

		update_frame(&missiles,&nb_missiles,&vaisseau,&frame,&frame_apparition_asteroid,&vitesse_spawn,&frame_2asteroid);
		//regulateFPS
		currentTime = SDL_GetTicks();
		while( currentTime - lastTime < FRAME_TIME )
			currentTime = SDL_GetTicks();

	//	if( currentTime - lastTime > FRAME_TIME )
		//	printf(" TIME FRAME : %d\n", currentTime - lastTime);

		lastTime = currentTime;

		// On efface
		SDL_SetRenderDrawColor(renderer, 0, 40, 200, 255);
		SDL_RenderClear(renderer);
	}

	//printf("Waw t'es nul, %d\n", score);
	return 0;
}