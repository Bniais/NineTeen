
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

void asteroidInit(){

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

//BONUS

void useNuclear(int * nbBombeNucleaire, Asteroid ** asteroides, int * nb_asteroid, int * frame_2asteroid){
	(*nb_asteroid) = 0;
	(*asteroides) = realloc(*asteroides, sizeof(Asteroid));
	*frame_2asteroid = FRAME_BOMBE_NUCLEAIRE;
	(*nbBombeNucleaire)--;
}


void recoit_bonus(int id_bonus, Vaiss * vaisseau, int *nbBombeNucleaire, int *point){
	switch (id_bonus) {
		case VITESSE_DE_TIR:
			vaisseau->temps_recharge /= BONUS_FREQUENCE_MISSILE;
			if(vaisseau->temps_recharge < FREQUENCE_MISSILE_MIN)
				vaisseau->temps_recharge = FREQUENCE_MISSILE_MIN;
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
			(*nbBombeNucleaire) ++;
	    	break;

		case POINT_PETIT:
			(*point) += BONUS_POINT[PETIT];
			break;

		case POINT_MOYEN:
			(*point) += BONUS_POINT[MOYEN];
			break;

		case POINT_GRAND:
			(*point) += BONUS_POINT[GRAND];
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
		if(trop_pres(asteroid.x,asteroid.y,missiles[i].x,missiles[i].y,asteroid.taille+RAYON_MISSILES[missiles[i].id])){
			return i;
		}
	}
	return -1;
}

int laser_touche(Asteroid asteroid, Vaiss vaisseau){
    return 1;
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


void asteroid_cpy(Asteroid * asteroid_src, Asteroid * asteroid_dest){
    asteroid_dest->x = asteroid_src->x;
    asteroid_dest->y = asteroid_src->y;
    asteroid_dest->taille = asteroid_src->taille;
    asteroid_dest->angle = asteroid_src->angle;
	asteroid_dest->vitesse = asteroid_src->vitesse;
	asteroid_dest->pv = asteroid_src->pv;
	asteroid_dest->difficulte = asteroid_src->difficulte;
	asteroid_dest->difficulte_pere = asteroid_src->difficulte_pere;
	asteroid_dest->pv_max = asteroid_src->pv_max;
	asteroid_dest->bonus = asteroid_src->bonus;
}

int randSign(){
	return (rand()%2 ? 1 : -1);
}

void detruire_asteroid(Asteroid ** asteroides, int * nb_asteroid, int i_asteroid,Vaiss * vaisseau,int touche_bouclier, int * point, int* nbBombeNucleaire, float angleMissile){

    int bonus = (*asteroides)[i_asteroid].bonus;
    (*asteroides)[i_asteroid].bonus = NO_BONUS;


    if((*asteroides)[i_asteroid].taille>TAILLE_MIN_SPLIT && !touche_bouclier && bonus != BOMBE_NUCLEAIRE){

        (*asteroides)[i_asteroid].taille/=2;
        (*asteroides)[i_asteroid].angle= angleMissile + PI/2;
		(*asteroides)[i_asteroid].pv_max /= 2;
		(*asteroides)[i_asteroid].difficulte /= 2;
		(*asteroides)[i_asteroid].pv = (*asteroides)[i_asteroid].pv_max;
		(*asteroides)[i_asteroid].angle_rota=rand()%(int)(2*PI*100);
		(*asteroides)[i_asteroid].vitesse_rota = randSign() * ((1+ MAX_ASTEROID_SIZE - (*asteroides)[i_asteroid].taille) / (float)(MAX_ASTEROID_SIZE - TAILLE_MIN_ASTEROID)) * (rand()%(int)(MAX_VITESSE_ROTA*PRECISION_RAND_FLOAT)/PRECISION_RAND_FLOAT) ;


        (*nb_asteroid)++;

        (*asteroides)=realloc(*asteroides,sizeof(Asteroid)*(*nb_asteroid));
        asteroid_cpy(&(*asteroides)[i_asteroid], &(*asteroides)[*nb_asteroid-1]);


        (*asteroides)[*nb_asteroid-1].angle += PI;
		(*asteroides)[*nb_asteroid-1].angle_rota=rand()%(int)(2*PI*100);
		(*asteroides)[*nb_asteroid-1].vitesse_rota = -(*asteroides)[i_asteroid].vitesse_rota;

    }
    else{

        decaler_gauche_a(*asteroides, *nb_asteroid, i_asteroid);
        (*nb_asteroid)--;

        if(*nb_asteroid !=0){
            (*asteroides)=realloc(*asteroides,sizeof(Asteroid)*(*nb_asteroid));
        }
    }

    if(bonus != NO_BONUS)
        recoit_bonus(bonus,vaisseau,nbBombeNucleaire,point);

}


// Vaisseau
void turn_vaiss(Vaiss * vaisseau, int turn, Vector2f * accelerate){
	if(turn == LEFT || turn == BOTH){
		accelerate->x +=  RATIO_TURN[vaisseau->frame_turn_left] * RATIO_ACCEL_TURN * cos(vaisseau->angle-PI/4);
		accelerate->y +=  RATIO_TURN[vaisseau->frame_turn_left] * RATIO_ACCEL_TURN * sin(vaisseau->angle-PI/4);

		vaisseau->frame_turn_left ++;

		if(vaisseau->angle < 0){
			vaisseau->angle += 2 * PI;
		}
		if(vaisseau->frame_turn_left > NB_FRAME_TURN-1)
			vaisseau->frame_turn_left = RESET_TURN ;

	}
	else{
		vaisseau->frame_turn_left --;
		if(vaisseau->frame_turn_left>RESET_TURN-1)
			vaisseau->frame_turn_left=RESET_TURN-1;

		if(vaisseau->frame_turn_left < 0)
			vaisseau->frame_turn_left = 0;
	}
	vaisseau->angle -= RATIO_TURN[vaisseau->frame_turn_left] * TURN_AMMOUNT;

	if(turn == RIGHT || turn == BOTH){
		accelerate->x +=  RATIO_TURN[vaisseau->frame_turn_right] * RATIO_ACCEL_TURN * cos(vaisseau->angle+PI/4);
		accelerate->y +=  RATIO_TURN[vaisseau->frame_turn_right] * RATIO_ACCEL_TURN * sin(vaisseau->angle+PI/4);

		vaisseau->frame_turn_right ++;

		if(vaisseau->angle < 0){
			vaisseau->angle += 2 * PI;
		}
		if(vaisseau->frame_turn_right > NB_FRAME_TURN-1)
			vaisseau->frame_turn_right = RESET_TURN;

	}
	else{
		vaisseau->frame_turn_right --;
		if(vaisseau->frame_turn_right>RESET_TURN-1)
			vaisseau->frame_turn_right=RESET_TURN-1;

		if(vaisseau->frame_turn_right < 0)
			vaisseau->frame_turn_right = 0;
	}
	vaisseau->angle += RATIO_TURN[vaisseau->frame_turn_right] * TURN_AMMOUNT;
}



void acceleration( Vector2f *accelerate, Vaiss vaisseau){

	accelerate->x += RATIO_ACCEL[vaisseau.frame_thrust+1] * ACCEL * cos(vaisseau.angle);
	accelerate->y += RATIO_ACCEL[vaisseau.frame_thrust+1] * ACCEL * sin(vaisseau.angle);

}

void move( Vaiss *vaisseau,Vector2f accelerate){
	vaisseau->x+=accelerate.x;
	vaisseau->y+=accelerate.y;

}
void afficher_texture_vaisseau(Vaiss vaisseau, SDL_Renderer * renderer, SDL_Texture * vaisseau_texture, SDL_Texture * gem_texture, SDL_Texture * thrust_texture){

	if(vaisseau.bouclier)
		SDL_SetTextureColorMod(vaisseau_texture, 246,200,50);
	else
		SDL_SetTextureColorMod(vaisseau_texture, 255, 255, 255);

	SDL_Rect vaisseau_src = VAISSEAU_SRC;
	SDL_Rect vaisseau_dest= VAISSEAU_DEST;
	SDL_Rect gem_src = GEM_SRC;

	vaisseau_dest.x=vaisseau.x-CENTRE_VAISS.x;
	vaisseau_dest.y=vaisseau.y-CENTRE_VAISS.y;

	vaisseau_src.x=vaisseau.frame_turn_right*vaisseau_src.w;

	SDL_RenderCopyEx(renderer, vaisseau_texture, &vaisseau_src, &vaisseau_dest,vaisseau.angle*180/PI+90,&CENTRE_VAISS,SDL_FLIP_NONE);
	vaisseau_src.y+=vaisseau_src.h;
	vaisseau_src.x=vaisseau.frame_turn_left*vaisseau_src.w;
	SDL_RenderCopyEx(renderer, vaisseau_texture, &vaisseau_src, &vaisseau_dest,vaisseau.angle*180/PI+90,&CENTRE_VAISS,SDL_FLIP_NONE);

	SDL_SetTextureColorMod(gem_texture, GEM_COLORS[vaisseau.missile_id].r, GEM_COLORS[vaisseau.missile_id].g, GEM_COLORS[vaisseau.missile_id].b);
	SDL_RenderCopyEx(renderer, gem_texture, &gem_src, &vaisseau_dest,vaisseau.angle*180/PI+90,&CENTRE_VAISS,SDL_FLIP_NONE);

	if(vaisseau.frame_thrust != -1){
		vaisseau_src.y=0;
		vaisseau_src.x=vaisseau.frame_thrust*vaisseau_src.w;
		SDL_SetTextureColorMod(thrust_texture, 255, 255, 255);
		SDL_RenderCopyEx(renderer,thrust_texture, &vaisseau_src, &vaisseau_dest,vaisseau.angle*180/PI+90,&CENTRE_VAISS,SDL_FLIP_NONE);
	}

}

void afficher_vaisseau( Vaiss vaisseau, SDL_Renderer *renderer, SDL_Texture * vaisseau_texture, SDL_Texture * gem_texture, SDL_Texture * thrust_texture ){

	SDL_Rect vaisseauRect = {vaisseau.x-RAYON_VAISS,vaisseau.y-RAYON_VAISS,RAYON_VAISS*2,RAYON_VAISS*2};

	afficher_texture_vaisseau(vaisseau,renderer,vaisseau_texture,gem_texture,thrust_texture);

	if(vaisseauRect.x<0){
		vaisseau.x+=PLAYGROUND_SIZE_W;
		afficher_texture_vaisseau(vaisseau,renderer,vaisseau_texture,gem_texture,thrust_texture);
	}
	else if(vaisseauRect.x+RAYON_VAISS*2>PLAYGROUND_SIZE_W){
		vaisseau.x-=PLAYGROUND_SIZE_W;
		afficher_texture_vaisseau(vaisseau,renderer,vaisseau_texture,gem_texture,thrust_texture);
	}

	if(vaisseauRect.y<0){
		vaisseau.y+=PLAYGROUND_SIZE_H;
		afficher_texture_vaisseau(vaisseau,renderer,vaisseau_texture,gem_texture,thrust_texture);
	}
	else if(vaisseauRect.y+RAYON_VAISS*2>PLAYGROUND_SIZE_H){
		vaisseau.y-=PLAYGROUND_SIZE_H;
		afficher_texture_vaisseau(vaisseau,renderer,vaisseau_texture,gem_texture,thrust_texture);
	}

	if(vaisseauRect.x<0){
		vaisseau.x+=PLAYGROUND_SIZE_W;
		afficher_texture_vaisseau(vaisseau,renderer,vaisseau_texture,gem_texture,thrust_texture);
	}
	else if(vaisseauRect.x+RAYON_VAISS*2>PLAYGROUND_SIZE_W){
		vaisseau.x-=PLAYGROUND_SIZE_W;
		afficher_texture_vaisseau(vaisseau,renderer,vaisseau_texture,gem_texture,thrust_texture);
	}

}

//LASER

void afficher_laser(SDL_Renderer * renderer, SDL_Texture * laser_texture, Vaiss vaisseau, int frame){

	SDL_Rect laser_src={0,MISSILE_SRC[SHOT_LASER].y,MISSILE_SRC[SHOT_LASER].x,MISSILE_SRC[SHOT_LASER].y};

	if(frame != 0)
		laser_src.y= (frame%(NB_LASER_BEAM-1)+1) * laser_src.h;

	SDL_Point spawn_laser= {0, RAYON_VAISS};
	printf("%f\n",vaisseau.angle );
	/*spawn_laser.y+=(CENTRE_VAISS.x)*cos(vaisseau.angle-PI/2) + CENTRE_VAISS.x;
	spawn_laser.x-=(CENTRE_VAISS.y)*sin(vaisseau.angle-PI/2);*/

	float xM = spawn_laser.x;
    float yM = spawn_laser.y;
    spawn_laser.x = xM*cos(vaisseau.angle-PI/2) - yM * sin(vaisseau.angle-PI/2) ;
    spawn_laser.y = xM * sin(vaisseau.angle-PI/2) + yM * cos(vaisseau.angle-PI/2) ;

	SDL_Rect laser_dest = {vaisseau.x + spawn_laser.x - MISSILE_CENTRES[SHOT_LASER].x,vaisseau.y + spawn_laser.y - MISSILE_CENTRES[SHOT_LASER].y, laser_src.w, laser_src.h};

	SDL_RenderCopyEx(renderer,laser_texture, &laser_src, &laser_dest,vaisseau.angle*180/PI,&MISSILE_CENTRES[SHOT_LASER],SDL_FLIP_NONE);
}
//MISSILE
 void tirer(Vaiss * vaisseau, Missile ** missiles, int * nb_missiles){

	*nb_missiles+=vaisseau->nb_tir;
	(*missiles) = realloc(*missiles,*nb_missiles * sizeof(Missile));

	for(int i=0;i<vaisseau->nb_tir;i++){
		(*missiles)[*nb_missiles-1-i].id = vaisseau->missile_id;
		(*missiles)[*nb_missiles-1-i].x=vaisseau->x+DISTANCE_CANON*cos(vaisseau->angle);
		(*missiles)[*nb_missiles-1-i].y=vaisseau->y+DISTANCE_CANON*sin(vaisseau->angle);
		(*missiles)[*nb_missiles-1-i].angle=vaisseau->angle+angle_tir_multiple[vaisseau->nb_tir-1][i];
		(*missiles)[*nb_missiles-1-i].target_angle=(*missiles)[*nb_missiles-1-i].angle;
		(*missiles)[*nb_missiles-1-i].frame= DUREE_MISSILE_BASE * DUREE_MISSILES[vaisseau->missile_id];
		printf("%d \n", (*missiles)[*nb_missiles-1-i].frame );
		(*missiles)[*nb_missiles-1-i].vitesse=vaisseau->vitesse_missile * VITESSE_MISSILES[vaisseau->missile_id];
		(*missiles)[*nb_missiles-1-i].degat=vaisseau->degat_missile  * DEGAT_MISSILES[vaisseau->missile_id];
	}

	vaisseau->frame_recharge = vaisseau->temps_recharge * FREQUENCE_MISSILES[vaisseau->missile_id];

}
float calculer_angle(float x1, float y1, float x2, float y2){
	//printf("angle : %f \n", atan((y1-y2)/(x2-x1)));
	return atan2((y2-y1) , (x2-x1));

	//cah toa

}


float basePi(float angle){
	while(angle > PI)
		angle -= 2 * PI;
	while(angle < - PI)
		angle += 2*PI;

	return angle;
}

float base360(float angle){
	return (angle * 360 / (2*PI));
}

int asteroid_plus_proche(SDL_Renderer * renderer, Asteroid * asteroides, int nb_asteroid, Missile missile, float * best_angle_between){

	int i;
	float dist,min_dist=9999;
	float angle_min=9999;
	int imin_asteroid = -1;
	float angle_between;


	//printf("angle between : %f \n", basePi(missile.angle - calculer_angle(asteroides[0].x,asteroides[0].y,missile.x, missile.y)));

	for(int shiftH = -1;shiftH <=1; shiftH++){
		for(int shiftW = -1; shiftW <=1; shiftW++){

			for (i=0;i<nb_asteroid;i++){
				angle_between = basePi(calculer_angle(missile.x+ shiftW * PLAYGROUND_SIZE_W, missile.y+ shiftH * PLAYGROUND_SIZE_H, asteroides[i].x,asteroides[i].y) - missile.angle);
				if(abs(angle_between) < CHAMP_VISION_TELEGUIDE ){
					dist = dist_2f(asteroides[i].x,asteroides[i].y,missile.x+ shiftW * PLAYGROUND_SIZE_W, missile.y+ shiftH * PLAYGROUND_SIZE_H);
					if(dist < min_dist){
						min_dist= dist;
						imin_asteroid=i;
						*best_angle_between = angle_between;
					}
				}
			}

			if(imin_asteroid==-1){
				for(i=0;i<nb_asteroid;i++){
					angle_between = basePi(calculer_angle(missile.x+ shiftW * PLAYGROUND_SIZE_W, missile.y+ shiftH * PLAYGROUND_SIZE_H, asteroides[i].x,asteroides[i].y) - missile.angle);
					if(abs(angle_between) < abs(angle_min)){
						angle_min=angle_between;
						imin_asteroid=i;
						*best_angle_between = angle_between;
					}
				}
			}

		}
	}




	return imin_asteroid;
}

int signOf( float f){
	return (f > 0) ? 1 : ((f < 0 )? -1 : 0);
}

void mouvement_tir(SDL_Renderer *renderer,Missile * shot, Asteroid * asteroides, int nb_asteroid){
	int i;
	float angle_between;

	if(shot->id == SHOT_ZIGZAG){
		shot->target_angle += randSign() * ((rand()%(int)(PRECISION_RAND_FLOAT*INTERVALLE_ZIGZAG_ANGLE)) / PRECISION_RAND_FLOAT + BASE_ZIGZAG_ANGLE);
		shot->angle += (shot->target_angle - shot->angle )/FRAME_TO_REACH_ANGLE;
	}
	else if(shot->id == SHOT_TELEGUIDE && nb_asteroid){
		i=asteroid_plus_proche(renderer, asteroides,nb_asteroid,*shot, &angle_between);
		if(i!=-1)
			shot->angle += fabs(angle_between)>ANGLE_TELEGUIDE ? signOf(angle_between) * ANGLE_TELEGUIDE : angle_between;
	}



	shot->x+= shot->vitesse*cos(shot->angle);
	shot->y+= shot->vitesse*sin(shot->angle);

}



void rotateAsteroides(Asteroid * asteroides, int nb_asteroid){
	for(int i=0; i<nb_asteroid; i++)
		asteroides[i].angle_rota += asteroides[i].vitesse_rota;
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

void afficher_tir( SDL_Renderer * renderer, Missile shot, SDL_Texture * missileTexture){
	SDL_Rect src = { MISSILE_CUT * shot.id, 0, MISSILE_SRC[shot.id].x, MISSILE_SRC[shot.id].y};
	float ratioSize = (float)RAYON_MISSILES[shot.id]/MISSILES_SRC_RAYON[shot.id];
	SDL_Point centre = MISSILE_CENTRES[shot.id];
	centre.x *= ratioSize;
	centre.y *= ratioSize;

	SDL_Rect missileRect={(int)shot.x-centre.x, (int)shot.y-centre.y, MISSILE_SRC[shot.id].x * ratioSize,MISSILE_SRC[shot.id].y * ratioSize};

	if(shot.frame <= FRAME_MISSILE_DEATH)
		SDL_SetTextureAlphaMod(missileTexture, ALPHA_MISSILE[shot.frame-1]);
	else
		SDL_SetTextureAlphaMod(missileTexture, 255);

	SDL_RenderCopyEx(renderer, missileTexture, &src, &missileRect, (shot.angle+PI/2) * 360 /  (2*PI), &centre, SDL_FLIP_NONE);

	/*SDL_SetRenderDrawColor(renderer, 255, 50, 50, 255);
	SDL_RenderFillRect(renderer,&missileRect);*/
}

int isObtainable(int id_bonus, Vaiss vaisseau){
	switch(id_bonus){
		case TIR_MULTIPLE :
			return (vaisseau.nb_tir < NB_TIR_MAX);

	    case BOUCLIER :
			return (!vaisseau.bouclier);

	    case VITESSE_DE_TIR:
			return (vaisseau.temps_recharge > FREQUENCE_MISSILE_MIN);

	    case BONUS_VITESSE_MISSILE :
			return (vaisseau.vitesse_missile<VITESSE_MISSILE_MAX);

		case DEGAT:
			return (vaisseau.degat_missile<DEGAT_MISSILE_MAX);

		default :
			return SDL_TRUE;
	}
}

//ASTEROID
int get_bonus(Vaiss vaisseau){
	int chanceTotal = 0;
	int i;
	for(i=0; i<NB_BONUS; i++)
		if(isObtainable(i, vaisseau))
			chanceTotal += CHANCE_BONUS[i];

	int proba = rand()%chanceTotal + 1;

	for(i=0; i<NB_BONUS && proba > 0; i++)
		if(isObtainable(i, vaisseau))
			proba -= CHANCE_BONUS[i];

	return i-1;

}


void spawn_asteroid(Vaiss vaisseau, Asteroid ** asteroides, int * nb_asteroid, float difficulte){
	difficulte *= 1 + randSign() * rand()%(int)(PRECISION_RAND_FLOAT *INTERVALE_RAND_DIFFICULTE)/(float)PRECISION_RAND_FLOAT;
	int id_coord;
	float ratio_pv;
	(*nb_asteroid)++;
	(*asteroides)=realloc((*asteroides),sizeof(Asteroid)*(*nb_asteroid));
	(*asteroides)[*nb_asteroid-1].angle=(rand()%(int)PRECISION_RAND_FLOAT*2*PI)/PRECISION_RAND_FLOAT;
	(*asteroides)[*nb_asteroid-1].pv=PV_BASE;
	(*asteroides)[*nb_asteroid-1].vitesse=VITESSE_BASE;
	(*asteroides)[*nb_asteroid-1].difficulte=difficulte;
	(*asteroides)[*nb_asteroid-1].difficulte_pere = (*asteroides)[*nb_asteroid-1].difficulte;
	(*asteroides)[*nb_asteroid-1].angle_rota=rand()%(int)(2*PI*100);

	if(difficulte >= 1+1/PRECISION_RAND_FLOAT){
			ratio_pv=1+(rand()%(int)((difficulte-1)*PRECISION_RAND_FLOAT))/PRECISION_RAND_FLOAT;
	}
	else ratio_pv = difficulte/2;

	(*asteroides)[*nb_asteroid-1].pv*=ratio_pv;
	(*asteroides)[*nb_asteroid-1].pv_max = (*asteroides)[*nb_asteroid-1].pv;

	(*asteroides)[*nb_asteroid-1].taille=(ratio_pv/difficulte)*MAX_ASTEROID_SIZE;
	if((*asteroides)[*nb_asteroid-1].taille < TAILLE_MIN_ASTEROID){
		(*asteroides)[*nb_asteroid-1].taille=TAILLE_MIN_ASTEROID;
	}
	(*asteroides)[*nb_asteroid-1].vitesse_rota= randSign() * ((1+ MAX_ASTEROID_SIZE - (*asteroides)[*nb_asteroid-1].taille) / (float)(MAX_ASTEROID_SIZE - TAILLE_MIN_ASTEROID)) * (rand()%(int)(MAX_VITESSE_ROTA*PRECISION_RAND_FLOAT)/PRECISION_RAND_FLOAT) ;


	(*asteroides)[*nb_asteroid-1].vitesse*=difficulte/ratio_pv;
	if((*asteroides)[*nb_asteroid-1].vitesse > VITESSE_MAX_ASTEROID){
		(*asteroides)[*nb_asteroid-1].vitesse= VITESSE_MAX_ASTEROID;
	}

	if(rand()%PROBA_BONUS==0){
		(*asteroides)[*nb_asteroid-1].bonus=get_bonus(vaisseau);
	}
	else (*asteroides)[*nb_asteroid-1].bonus=NO_BONUS;

	do{

		id_coord=rand()%4;
		if(id_coord<2){
			id_coord=1;
		}

		(*asteroides)[*nb_asteroid-1].x= coord_spawn[id_coord-1].x;
		(*asteroides)[*nb_asteroid-1].y= coord_spawn[id_coord-1].y;


	}	while(trop_pres(vaisseau.x,vaisseau.y,(*asteroides)[*nb_asteroid-1].x,(*asteroides)[*nb_asteroid-1].y,DIST_VAISSEAU_ASTEROID+RAYON_VAISS+(*asteroides)[*nb_asteroid-1].taille));

}
void mouvement_asteroid(Asteroid* asteroid){

	asteroid->x+=asteroid->vitesse*cos(asteroid->angle);
	asteroid->y+=asteroid->vitesse*sin(asteroid->angle);

}

void afficher_texture_asteroid(SDL_Renderer* renderer, SDL_Texture * textureAsteroid, SDL_Texture* textureFissure, SDL_Rect src, SDL_Rect dest, SDL_Rect bonusRect, SDL_Rect srcFissure, Asteroid asteroid){
	SDL_RenderCopyEx(renderer, textureAsteroid, &src, &dest, asteroid.angle_rota, NULL, SDL_FLIP_NONE);
	if(asteroid.bonus != NO_BONUS){
		SDL_RenderFillRect(renderer, &bonusRect);
	}
	if(srcFissure.y>=0){

		SDL_RenderCopyEx(renderer, textureFissure, &srcFissure, &dest, asteroid.angle_rota, NULL, SDL_FLIP_NONE);
	}
}

void afficher_asteroid(Asteroid asteroid, SDL_Renderer * renderer, SDL_Texture* textureAsteroid, SDL_Texture* textureFissure, int aim){
	SDL_Rect src = ASTE_SRC;

	if(aim)
		SDL_SetTextureColorMod(textureAsteroid, 255,0,0);
	else
		SDL_SetTextureColorMod(textureAsteroid, 255,255,255);
	SDL_Rect asteroidRect={(int)asteroid.x-asteroid.taille, (int)asteroid.y-asteroid.taille,asteroid.taille*2,asteroid.taille*2};

	int sizeAste = (roundf(asteroid.taille/(MAX_ASTEROID_SIZE/NB_TAILLE_ASTE)) - 1);
	if(sizeAste < 0)
		sizeAste = 0;

	src.w = DIAMETRE_ASTE[sizeAste];
	src.h = DIAMETRE_ASTE[sizeAste];

	src.x = ASTE_SRC.w * sizeAste;

	SDL_Rect srcFissure = src;
	int fissure = (int)(((float)(asteroid.pv_max - asteroid.pv) / asteroid.pv_max) * (NB_FISSURES+1) ) - 1;
	if(sizeAste < 0)
		sizeAste = 0;
	srcFissure.y = fissure * ASTE_SRC.h;

	int skinAste = (int)((asteroid.difficulte_pere-START_DIFFICULTE)/(MAX_DIFF/NB_ASTE_TEXTURES));
	if(skinAste < 0)
		skinAste = 0;
	if(skinAste > NB_ASTE_TEXTURES-1)
		skinAste = NB_ASTE_TEXTURES-1;

	src.y = ASTE_SRC.h * skinAste;


	SDL_SetRenderDrawColor(renderer, 255, 150, 80, 255);
	SDL_Rect bonusRect = asteroidRect;
	bonusRect.x += bonusRect.w/2 - bonusRect.w/4;
	bonusRect.y += bonusRect.h/2- bonusRect.h/4;
	bonusRect.w /=2;
	bonusRect.h /=2;

	afficher_texture_asteroid(renderer,textureAsteroid, textureFissure, src, asteroidRect, bonusRect, srcFissure, asteroid);

	//Dessiner bonus est différent quand bord de l'écran car on ne peut pas déssiner en desous de 0 -> à faire
	if(asteroidRect.x+asteroid.taille*2>PLAYGROUND_SIZE_W || asteroidRect.x<0){
		if(asteroidRect.x<0){
			asteroidRect.x+=PLAYGROUND_SIZE_W;
			bonusRect.x +=PLAYGROUND_SIZE_W;
		}
		else{
			asteroidRect.x-=PLAYGROUND_SIZE_W;
			bonusRect.x-=PLAYGROUND_SIZE_W;
		}
		afficher_texture_asteroid(renderer,textureAsteroid, textureFissure, src, asteroidRect, bonusRect, srcFissure, asteroid);
	}


	if(asteroidRect.y+asteroid.taille*2>PLAYGROUND_SIZE_H || asteroidRect.y<0){
		if(asteroidRect.y<0){
			asteroidRect.y+=PLAYGROUND_SIZE_H;
			bonusRect.y+=PLAYGROUND_SIZE_H;
		}
		else {
			asteroidRect.y-=PLAYGROUND_SIZE_H;
			bonusRect.y-=PLAYGROUND_SIZE_H;
		}

		afficher_texture_asteroid(renderer,textureAsteroid, textureFissure, src, asteroidRect, bonusRect, srcFissure, asteroid);
	}

	if(asteroidRect.x+asteroid.taille*2>PLAYGROUND_SIZE_W || asteroidRect.x<0){
		if(asteroidRect.x<0){
			asteroidRect.x+=PLAYGROUND_SIZE_W;
			bonusRect.x+=PLAYGROUND_SIZE_W;
		}
		else{
			asteroidRect.x-=PLAYGROUND_SIZE_W;
			bonusRect.x+=PLAYGROUND_SIZE_W;
		}
		afficher_texture_asteroid(renderer,textureAsteroid, textureFissure, src, asteroidRect, bonusRect, srcFissure, asteroid);
	}

}

int asteroid(SDL_Renderer * renderer, int highscore, float ratioWindowSize, char *token, int hardcore){

/////////////////////
/// MISE EN PLACE ///``
/////////////////////
	asteroidInit();

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
	int point=0;


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
 	Vaiss vaisseau=
	{
		500, //coord x
		500, //coord y
		BASE_ANGLE,
		0, // frame recharge
		FREQUENCE_BASE, // temps recharge
		1, // nb tir
		0, //bouclier
		0, // id missile
		BASE_VITESSE_MISSILE,
		BASE_DEGAT_MISSILE,
		0, //frame turn left
		0  //frame turn right
	};
	Vector2f accelerate={0,0};

	//MISSILES
	Missile * missiles=malloc(sizeof(Missile));
	int nb_missiles=0;

	//ASTEROID
	Asteroid * asteroides=malloc(sizeof(Asteroid));
	int nb_asteroid=0;

	//BONUS
	int nbBombeNucleaire = 2;
	int rdyToBomb = SDL_TRUE;
	int rdyToReloadBomb = SDL_TRUE;
	int rdyToTab = SDL_TRUE;

	//DIFFICULTE
	float difficulte =START_DIFFICULTE;

	//Views
	SDL_Rect playgroundView = {HUD_W/ratioWindowSize, HUD_H/ratioWindowSize, PLAYGROUND_SIZE_W/ratioWindowSize, PLAYGROUND_SIZE_H/ratioWindowSize};

	//mouse
	SDL_Point mouseCoor;

	//Textures
	SDL_Texture* textures[NB_ASTEROID_TEXTURES];
	//Textures
	for(int i=0; i< NB_ASTEROID_TEXTURES; i++){
		 textures[i] = IMG_LoadTexture(renderer, DIR_TEXTURES_ASTEROID[i]);
		 if( textures[i] == NULL ){
			printf("Erreur lors de la creation de texture %s", SDL_GetError());
			return EXIT_FAILURE;
		}
	}

	//anims
	int frameLaser = 0;


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

			}
		}


	////////////////////////////
	// Handle Keyboard inputs //`
	////////////////////////////
		SDL_PumpEvents();

		if(keystate[SDL_SCANCODE_ESCAPE])
			return 0;

		if(keystate[SDL_SCANCODE_TAB] && rdyToTab){
			vaisseau.missile_id ++;
			if(vaisseau.missile_id >= NB_MISSILES)
				vaisseau.missile_id = 0;
			rdyToTab = SDL_FALSE;
		}
		else if (!keystate[SDL_SCANCODE_TAB]){
			rdyToTab = SDL_TRUE;
		}


		if (keystate[SDL_SCANCODE_LEFT] && keystate[SDL_SCANCODE_RIGHT]){
			turn = BOTH;
		}
		else if( keystate[SDL_SCANCODE_LEFT] )
			turn = LEFT;
		else if( keystate[SDL_SCANCODE_RIGHT] )
			turn = RIGHT;

		if(keystate[SDL_SCANCODE_DOWN]){
			if(  nbBombeNucleaire && rdyToBomb){
				useNuclear(&nbBombeNucleaire, &asteroides, &nb_asteroid, &frame_2asteroid);
				rdyToReloadBomb = SDL_FALSE;
				rdyToBomb = SDL_FALSE;
			}
		}
		else if( rdyToReloadBomb ) {
			rdyToBomb = SDL_TRUE;
		}





	//////////////
	// Gameplay //`
	//////////////

	accelerate.x/=DECELERATION;
    accelerate.y/=DECELERATION;

	turn_vaiss(&vaisseau,turn,&accelerate);

	if( keystate[SDL_SCANCODE_UP] ){
		vaisseau.frame_thrust++;
		if(vaisseau.frame_thrust > NB_FRAME_THRUST-1)
			vaisseau.frame_thrust = RESET_THRUST;
	}
	else {
		vaisseau.frame_thrust--;
		if(vaisseau.frame_thrust>RESET_THRUST-1){
			vaisseau.frame_thrust=RESET_THRUST-1;
		}
		if(vaisseau.frame_thrust<-1){
			vaisseau.frame_thrust=-1;
		}
	}
	acceleration(&accelerate,vaisseau);


	move(&vaisseau,accelerate);
	if( keystate[SDL_SCANCODE_SPACE] && vaisseau.frame_recharge == 0){
		if(vaisseau.missile_id != SHOT_LASER){
			tirer(&vaisseau,&missiles,&nb_missiles);
		}

	}

	for(int i=0;i<nb_missiles; i++){
		mouvement_tir(renderer, &missiles[i], asteroides, nb_asteroid);

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
				asteroides[i].pv-=missiles[i_missile].degat;

				decaler_gauche_m(missiles, nb_missiles, i_missile);
				(nb_missiles)--;
				if(nb_missiles !=0){
						missiles=realloc(missiles,sizeof(Missile)*(nb_missiles));
				}

			}

		//hitbox laser
		if(vaisseau.missile_id == SHOT_LASER){
			laser_touche(asteroides[i], vaisseau);
		}


		if(asteroides[i].pv<=0){
				detruire_asteroid(&asteroides,&nb_asteroid,i,&vaisseau,SDL_FALSE, &point, &nbBombeNucleaire, missiles[i_missile].angle);
		}
	}

	int i_touche=vaisseau_touche(vaisseau,asteroides,nb_asteroid);
	if(i_touche != -1){
		if(vaisseau.bouclier){
			vaisseau.bouclier=0;
			detruire_asteroid(&asteroides,&nb_asteroid,i_touche, &vaisseau,SDL_TRUE,&point, &nbBombeNucleaire, 0);
		}
		else break;
	}
	for(int i=0;i<nb_missiles;i++){
			collision_mur(&missiles[i].x,&missiles[i].y,RAYON_MISSILES[missiles[i].id]);

	}



	//////////
	// Draw //`
	//////////

	SDL_RenderCopy(renderer, textures[T_BACKGROUND], NULL, NULL);



	for(int i=0; i<nb_missiles; i++){
		afficher_tir(renderer, missiles[i], textures[T_BULLET]);
	}

	if(vaisseau.missile_id == SHOT_LASER && keystate[SDL_SCANCODE_SPACE]){
		afficher_laser(renderer,textures[T_LASER], vaisseau, frameLaser);
		frameLaser++;
		accelerate.x -= LASER_ACCEL * cos(vaisseau.angle);
		accelerate.y -= LASER_ACCEL * sin(vaisseau.angle);
	}
	else{
		frameLaser = 0;
	}

	afficher_vaisseau(vaisseau,renderer,textures[T_VAISS],textures[T_GEM],textures[T_THRUST]);


	if((frame_apparition_asteroid==0 || nb_asteroid <= (frame/FRAME_APPARITION_ASTEROID))&& frame_2asteroid == 0 ){
		spawn_asteroid(vaisseau,&asteroides,&nb_asteroid,difficulte);
		frame_2asteroid=FRAME_2ASTEROID;
		frame_apparition_asteroid=vitesse_spawn;
		rdyToReloadBomb = SDL_TRUE;
	}

	//int a = asteroid_plus_proche(renderer, asteroides, nb_asteroid, missiles[0], NULL);
	for(int j=0;j<nb_asteroid;j++){
		/*if( j == a)
			afficher_asteroid(asteroides[j],renderer, textures[T_ASTEROID], textures[T_FISSURE], 1);
		else*/
			afficher_asteroid(asteroides[j],renderer, textures[T_ASTEROID], textures[T_FISSURE], 0);
	}
		//hud
		SDL_RenderSetScale(renderer, 1, 1);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderSetViewport(renderer, NULL);
		SDL_RenderCopy(renderer, textures[T_HUD], NULL, NULL);



		//afficher
		SDL_RenderPresent(renderer);

		SDL_RenderSetViewport(renderer, &playgroundView);
		SDL_RenderSetScale(renderer, 1. / ratioWindowSize, 1. / ratioWindowSize);
	////////////////
	// Next frame //`
	////////////////
		rotateAsteroides(asteroides, nb_asteroid);
		update_frame(&missiles,&nb_missiles,&vaisseau,&frame,&frame_apparition_asteroid,&vitesse_spawn,&frame_2asteroid);
		difficulte+=RATIO_DIFFICULTE_AUGMENT;
		//regulateFPS
		currentTime = SDL_GetTicks();
		while( currentTime - lastTime < FRAME_TIME )
			currentTime = SDL_GetTicks();

	//	if( currentTime - lastTime > FRAME_TIME )
		//	printf(" TIME FRAME : %d\n", currentTime - lastTime);

		lastTime = currentTime;

		// On efface
		SDL_SetRenderDrawColor(renderer, 3, 22, 34, 255);
		SDL_RenderClear(renderer);
	}

	//printf("Waw t'es nul, %d\n", score);
	return 0;
}
