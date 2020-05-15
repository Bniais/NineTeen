/**
*\file asteroid.c
*\brief Jeu asteroid avec bonus
*\author Rayyan LAJNEF
*\version 1.0
*\date 12/05/2020
*/


#include "config.h"
#include <time.h>



#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <stdlib.h>
#include <stdio.h>

#include <math.h>


/**
*\fn void asteroidInit()
*\brief Initialise l'environement SDL, TTF et rand
*/
void asteroidInit(){

	// SDL Init
	Mix_Volume(5, 40);//shoot
	Mix_Volume(6, 128);//bonus
	Mix_Volume(7, 128);//hit
	Mix_Volume(8, 128);//nuclear
	for(int i = 9; i < 9+NB_CHANNEL_EXPLOSION; i++)
		Mix_Volume(i, 50);//explo

	Mix_Volume(9+NB_CHANNEL_EXPLOSION, 0);//thrust principal
	Mix_Volume(9+NB_CHANNEL_EXPLOSION+1, 0);//thrust gauche
	Mix_Volume(9+NB_CHANNEL_EXPLOSION+2, 0);//thrust droite
	Mix_Volume(9+NB_CHANNEL_EXPLOSION+3, 128);//gameover
	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
	/*//audio

	if( Mix_OpenAudio( MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 1, 1024 ) == -1 )
		printf("Erreur init SDL_Mixer\n" );*/

	//rand
	srand(time(NULL));
}

//BONUS

/**
*\fn int useNuclear( Asteroid ** asteroides, int * nb_asteroid, Explosion ** explosions, int * nb_explosions, ScoreTotal * score, long keys[4], long long * score_hash, int done, Mix_Chunk * exploSound)
*\brief Utilisation de la bombe nucléaire
*\param asteroides la liste des asteroides
*\param nb_asteroid le nombre d'asteroide
*\param  explosions les explosions
*\param  nb_explosions le nombre d'explosion
*\param  score Le score total
*\param keys Les clés de hashage
*\param score_hash Le score hashé
*\param done indique si le jeu est fini
*\param exploSound Son d'explosion
*\return 0 si pas de problème sinon return HACKED
*/
int useNuclear( Asteroid ** asteroides, int * nb_asteroid, Explosion ** explosions, int * nb_explosions, ScoreTotal * score, long keys[4], long long * score_hash, int done, Mix_Chunk * exploSound){

	int skinAste;

	*nb_explosions += *nb_asteroid;

	(*explosions) = realloc((*explosions), *nb_explosions * sizeof(Explosion));

	for(int i=0; i<*nb_asteroid; i++){
		if(!done){
			skinAste = (int)(((*asteroides)[i].difficulte_pere-START_DIFFICULTE)/(MAX_DIFF/NB_ASTE_TEXTURES));
			if(skinAste < 0)
				skinAste = 0;
			if(skinAste > NB_ASTE_TEXTURES-1)
				skinAste = NB_ASTE_TEXTURES-1;

			if(!changeProtectedVar(score_hash, &(score->score), (score->score) + SCORE_ASTEROID[skinAste] / ((*asteroides)[i].difficulte_pere/(*asteroides)[i].difficulte), keys))
				return HACKED;
			score->frameToDest = FRAME_SCORE_ANIM;
		}

		int channel=Mix_GroupAvailable(1);
		if (channel==-1) {
			channel = Mix_GroupOldest(1);;
			if (channel==-1)
				channel = 8;
		}

		Mix_PlayChannel(channel, exploSound, 0);
		(*explosions)[*nb_explosions - 1 - i].id = (*asteroides)[i].frozen >= 2 ? EXPLO_GLACE : EXPLO_ASTE;
		(*explosions)[*nb_explosions - 1 - i].taille = 2*(*asteroides)[i].taille * RATIO_ASTEROID_EXPLO_SIZE;
		(*explosions)[*nb_explosions - 1 - i].x = (*asteroides)[i].x - (*explosions)[*nb_explosions - 1 - i].taille/2;
		(*explosions)[*nb_explosions - 1 - i].y = (*asteroides)[i].y - (*explosions)[*nb_explosions - 1 - i].taille/2;
		(*explosions)[*nb_explosions - 1 - i].frame = FRAME_EXPLOSIONS[(*explosions)[*nb_explosions - 1 - i].id];
	}


	(*nb_asteroid) = 0;
	(*asteroides) = realloc(*asteroides, sizeof(Asteroid));

    return 0;
}



/**
*\fn int recoit_bonus(int id_bonus, Vaiss * vaisseau, int *nbBombeNucleaire, ScoreTotal *score, float munitions[NB_MISSILES], long keys[4], long long * score_hash, Mix_Chunk * bonusSound){
*\brief Applique les changements en fonction des bonus
*\param id_bonus id du bonus
*\param vaisseau Le vaisseau
*\param  nbBombeNucleaire Nombre de bombe nucléaire en notre possession
*\param  score Le score total
*\param munitions[NB_MISSILES] munitions des missiles
*\param keys Les clés de hashage
*\param score_hash Le score hashé
*\param bonusSound Son des bonus
*\return 0 si pas de problème sinon return HACKED
*/

int recoit_bonus(int id_bonus, Vaiss * vaisseau, int *nbBombeNucleaire, ScoreTotal *score, float munitions[NB_MISSILES], long keys[4], long long * score_hash, Mix_Chunk * bonusSound){
	Mix_PlayChannel(6, bonusSound, 0);
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
				(vaisseau->degat_missile) += DEGAT_ADD;
			}
			break;

		case BOMBE_NUCLEAIRE:
			if(*nbBombeNucleaire < MAX_BOMBE_NUCLEAIRE)
				(*nbBombeNucleaire) ++;
	    	break;

		case POINT_PETIT:
			if(!changeProtectedVar(score_hash, &(score->score), (score->score) + BONUS_POINT[PETIT], keys))
				return HACKED;
			break;

		case POINT_MOYEN:
			if(!changeProtectedVar(score_hash, &(score->score), (score->score) + BONUS_POINT[MOYEN], keys))
				return HACKED;
			break;

		case POINT_GRAND:
			if(!changeProtectedVar(score_hash, &(score->score), (score->score) + BONUS_POINT[GRAND], keys))
				return HACKED;
			break;

		default:
			munitions[id_bonus - NB_BONUS +1] += AMMO_GRANT;
			break;

	}

	return 0;
}



// Collision Mur
/**
*\fn void collision_mur(float * xToMove, float * yToMove, float x, float y, float rayon, int hardcore){
*\brief collision avec les côtés de l'écran
*\param xToMove coord en x
*\param yToMove coord en y
*\param x coord en x
*\param y coord en y
*\param rayon
*\param hardcore mode hardcore activé
*/
void collision_mur(float * xToMove, float * yToMove, float x, float y, float rayon, int hardcore){


    int deltaGauche = 0 - (x + rayon);
    int deltaDroite = PLAYGROUND_SIZE_W - (x - rayon);
    int deltaHaut = 0 - (y + rayon);
    int deltaBas = PLAYGROUND_SIZE_H - (y - rayon);


		if(deltaDroite <= 0){
				*xToMove -= PLAYGROUND_SIZE_W + deltaDroite;
		}
		if(deltaGauche >= 0){
				*xToMove += PLAYGROUND_SIZE_W + deltaGauche;
		}

    if(deltaBas <= 0){
        *yToMove -= PLAYGROUND_SIZE_H + deltaBas;
    }
    if(deltaHaut >= 0){
        *yToMove += PLAYGROUND_SIZE_H + deltaHaut;
    }

}

/**
*\fn float dist_2f(float x1, float y1, float x2, float y2)
*\brief calcul la distance entre deux points
*\param x1 coord en x
*\param y1 coord en y
*\param x2 coord en x
*\param y2 coord en y
*\return la distance entre deux points
*/
float dist_2f(float x1, float y1, float x2, float y2){
	return sqrt(pow(x2-x1,2)+pow(y2-y1,2));
}

/**
*\fn int trop_pres(float x1, float y1, float x2, float y2, float dist, int cote_spawn)
*\brief vaisseau trop_pres ou non
*\param x1 coord en x
*\param y1 coord en y
*\param x2 coord en x
*\param y2 coord en y
*\param dist distance
*\param cote_spawn côté de spawn des asteroides
*\return si oui ou non les deux points sont trop près
*/
int trop_pres(float x1, float y1, float x2, float y2, float dist, int cote_spawn){
    if(x2-x1 > PLAYGROUND_SIZE_W/2 && cote_spawn != 0 && cote_spawn != 2)
        x2-= PLAYGROUND_SIZE_W;
    else if(x1-x2 > PLAYGROUND_SIZE_W/2 && cote_spawn != 2 && cote_spawn != 0)
        x1-= PLAYGROUND_SIZE_W;

    if(y2-y1 > PLAYGROUND_SIZE_H/2 && cote_spawn != 1 && cote_spawn != 3)
        y2-= PLAYGROUND_SIZE_H;
    else if(y1-y2 > PLAYGROUND_SIZE_H/2 && cote_spawn != 3 && cote_spawn != 1)
        y1-= PLAYGROUND_SIZE_H;
    return (dist_2f(x1,y1,x2,y2)<=dist) ;

}
/**
*\fn int sur_asteroid(Asteroid * asteroides, int nb_asteroid, int i_asteroid)
*\brief test si un asteroide est sur un autre asteroide
*\param asteroides liste des asteroides
*\param nb_asteroid nombre d'asteroide
*\param i_asteroid indice de l'asteroide
*\return return VRAI si sur un asteroid sinon FAUX
*/
int sur_asteroid(Asteroid * asteroides, int nb_asteroid, int i_asteroid){
	for(int i=0; i<nb_asteroid; i++){
		if(i!=i_asteroid && trop_pres(asteroides[i].x,asteroides[i].y,asteroides[i_asteroid].x,asteroides[i_asteroid].y,asteroides[i_asteroid].taille + asteroides[i].taille,asteroides[i_asteroid].cote_spawn)){
			return SDL_TRUE;
		}
	}
	return SDL_FALSE;
}

/**
*\fn int vaisseau_touche(Vaiss vaisseau, Asteroid * asteroides, int nb_asteroid)
*\brief test si le vaisseau a touché un asteroid
*\param vaisseau structure du vaisseau
*\param asteroides liste des asteroides
*\param nb_asteroid nombre d'asteroides
*\return return l'indice de l'asteroide sinon return -1
*/
int vaisseau_touche(Vaiss vaisseau, Asteroid * asteroides, int nb_asteroid){
	for(int i=0;i<nb_asteroid;i++){
		if(trop_pres(vaisseau.x,vaisseau.y,asteroides[i].x,asteroides[i].y,asteroides[i].taille+RAYON_VAISS,asteroides[i].cote_spawn)){
			return i;
		}
	}
	return -1;
}

/**
*\fn int asteroid_touche(Asteroid asteroid, Missile * missiles, int nb_missiles )
*\brief test si un asteroide est sur un autre asteroide
*\param asteroid un asteroide
*\param missiles liste des missiles
*\param nb_missiles nombre de missile
*\return return l'indice de l'asteroide sinon return -1
*/
int asteroid_touche(Asteroid asteroid, Missile * missiles, int nb_missiles ){

	for(int i=0;i<nb_missiles;i++){
		if(trop_pres(asteroid.x,asteroid.y,missiles[i].x,missiles[i].y,asteroid.taille+RAYON_MISSILES[missiles[i].id],-1)){
			return i;
		}
	}
	return -1;
}



/**
*\fn float det(float x1, float y1, float x2, float y2 )
*\brief calcul le determinant
*\param x1 coord en x
*\param y1 coord en y
*\param x2 coord en x
*\param y2 coord en y
*\return le determinant
*/
float det(float x1, float y1, float x2, float y2 ){
	return( (x1*y2) - (y1*x2));

}

/**
*\fn int ligneColli(SDL_Point lignes_laser[5][2], SDL_Point asteroid_diametre[2])
*\brief  détermine si une des lignes est en collision avec une autre ligne qui correspond au diamètre d'un asteroid perpendiculaire aux lignes du laser
*\param ligne_laser[5][2] tableau de ligne du laser
*\param asteroid_diametre[2] diametre de l'asteroide
*\return Vrai sinon Faux
*/
int ligneColli(SDL_Point lignes_laser[5][2], SDL_Point asteroid_diametre[2]){
	for(int i = 0; i< 5; i++){
		if(
			(det(lignes_laser[i][1].x - lignes_laser[i][0].x, lignes_laser[i][1].y - lignes_laser[i][0].y,
			    asteroid_diametre[0].x - lignes_laser[i][0].x, asteroid_diametre[0].y - lignes_laser[i][0].y)
		   *det(lignes_laser[i][1].x - lignes_laser[i][0].x, lignes_laser[i][1].y - lignes_laser[i][0].y,
   			    asteroid_diametre[1].x - lignes_laser[i][0].x, asteroid_diametre[1].y - lignes_laser[i][0].y) < 0)

		&&

			(det(asteroid_diametre[1].x -asteroid_diametre[0].x, asteroid_diametre[1].y - asteroid_diametre[0].y,
				lignes_laser[i][0].x - asteroid_diametre[0].x, lignes_laser[i][0].y - asteroid_diametre[0].y )
		   *det(asteroid_diametre[1].x -asteroid_diametre[0].x, asteroid_diametre[1].y - asteroid_diametre[0].y,
			   lignes_laser[i][1].x - asteroid_diametre[0].x, lignes_laser[i][1].y - asteroid_diametre[0].y ) < 0)
		)
			return SDL_TRUE;
	}
	return SDL_FALSE;
}


/**
*\fn int laser_touche(Asteroid asteroid, Vaiss vaisseau)
*\brief test si le laser touche un asteroide
*\param asteroid un asteroide
*\param vaisseau le vaisseau
*\return Vrai si le laser touche l'asteroid sinon Faux
*/
int laser_touche(Asteroid asteroid, Vaiss vaisseau){
	SDL_Point lignes_laser[5][2]=
	{
		{{vaisseau.x + MISSILE_SRC[SHOT_LASER].y/4. *2 * cos(vaisseau.angle + PI/5.5), vaisseau.y + MISSILE_SRC[SHOT_LASER].y/4. * 2 * sin(vaisseau.angle + PI/5.5)},{0, 0}},
		{{vaisseau.x + MISSILE_SRC[SHOT_LASER].y/4. * cos(vaisseau.angle + PI/5.5), vaisseau.y + MISSILE_SRC[SHOT_LASER].y/4. * sin(vaisseau.angle + PI/5.5)},{0, 0}},
		{{vaisseau.x, vaisseau.y},{0, 0}},
		{{vaisseau.x + MISSILE_SRC[SHOT_LASER].y/4. *2 * cos(vaisseau.angle - PI/5.5), vaisseau.y + MISSILE_SRC[SHOT_LASER].y/4. * 2 * sin(vaisseau.angle - PI/5.5)},{0, 0}},
		{{vaisseau.x + MISSILE_SRC[SHOT_LASER].y/4. * cos(vaisseau.angle - PI/5.5), vaisseau.y + MISSILE_SRC[SHOT_LASER].y/4. * sin(vaisseau.angle - PI/5.5)},{0, 0}}
	};

	for(int i = 0; i< 5; i++){
		lignes_laser[i][1].x = lignes_laser[i][0].x + 5000*cos(vaisseau.angle);
		lignes_laser[i][1].y = lignes_laser[i][0].y + 5000*sin(vaisseau.angle);
	}

	SDL_Point asteroid_diametreg[2]={
		{asteroid.x + asteroid.taille*cos(vaisseau.angle - PI/2), asteroid.y + asteroid.taille*sin(vaisseau.angle - PI/2)},
		{asteroid.x + asteroid.taille*cos(vaisseau.angle + PI/2), asteroid.y + asteroid.taille*sin(vaisseau.angle + PI/2)}
	};

	// normal
	if(ligneColli(lignes_laser, asteroid_diametreg))
		return SDL_TRUE;

	// right/left
	if(asteroid.x < asteroid.taille || asteroid.x > PLAYGROUND_SIZE_W-asteroid.taille){

		if(asteroid.x < asteroid.taille)
			asteroid.x += PLAYGROUND_SIZE_W;
		else
			asteroid.x -= PLAYGROUND_SIZE_W;

		SDL_Point asteroid_diametre[2]={
			{asteroid.x + asteroid.taille*cos(vaisseau.angle - PI/2), asteroid.y + asteroid.taille*sin(vaisseau.angle - PI/2)},
			{asteroid.x + asteroid.taille*cos(vaisseau.angle + PI/2), asteroid.y + asteroid.taille*sin(vaisseau.angle + PI/2)}
		};

		if(ligneColli(lignes_laser, asteroid_diametre))
			return SDL_TRUE;

		//revert
		if(asteroid.x < PLAYGROUND_SIZE_W/2)
			asteroid.x += PLAYGROUND_SIZE_W;
		else
			asteroid.x -= PLAYGROUND_SIZE_W;
	}

	// top/bot
	if(asteroid.y < asteroid.taille || asteroid.y > PLAYGROUND_SIZE_H-asteroid.taille){

		if(asteroid.y < asteroid.taille)
			asteroid.y += PLAYGROUND_SIZE_H;
		else
			asteroid.y -= PLAYGROUND_SIZE_H;

		SDL_Point asteroid_diametre[2]={
			{asteroid.x + asteroid.taille*cos(vaisseau.angle - PI/2), asteroid.y + asteroid.taille*sin(vaisseau.angle - PI/2)},
			{asteroid.x + asteroid.taille*cos(vaisseau.angle + PI/2), asteroid.y + asteroid.taille*sin(vaisseau.angle + PI/2)}
		};

		if(ligneColli(lignes_laser, asteroid_diametre))
			return SDL_TRUE;

		//revert
		if(asteroid.y < PLAYGROUND_SIZE_H/2)
			asteroid.y += PLAYGROUND_SIZE_H;
		else
			asteroid.y -= PLAYGROUND_SIZE_H;
	}

		//corne
	if((asteroid.y < asteroid.taille || asteroid.y > PLAYGROUND_SIZE_H-asteroid.taille) && (asteroid.x < asteroid.taille || asteroid.x > PLAYGROUND_SIZE_W-asteroid.taille)){

		if(asteroid.y < asteroid.taille)
			asteroid.y += PLAYGROUND_SIZE_H;
		else
			asteroid.y -= PLAYGROUND_SIZE_H;

		if(asteroid.x < asteroid.taille)
			asteroid.x += PLAYGROUND_SIZE_W;
		else
			asteroid.x -= PLAYGROUND_SIZE_W;

		SDL_Point asteroid_diametre[2]={
			{asteroid.x + asteroid.taille*cos(vaisseau.angle - PI/2), asteroid.y + asteroid.taille*sin(vaisseau.angle - PI/2)},
			{asteroid.x + asteroid.taille*cos(vaisseau.angle + PI/2), asteroid.y + asteroid.taille*sin(vaisseau.angle + PI/2)}
		};

		if(ligneColli(lignes_laser, asteroid_diametre))
			return SDL_TRUE;
	}

    return SDL_FALSE;
}


/**
*\fn void decaler_gauche_a(Asteroid * tab, int taille_tab, int i)
*\brief decale de 1 les valeurs d'un tableau d'asteroide
*\param tab tab d'asteroide
*\param taille_tab taille du tableau
*\param i indice de début
*/
void decaler_gauche_a(Asteroid * tab, int taille_tab, int i){

    for(;i<taille_tab-1;i++){
        tab[i]=tab[i+1];
    }
}

/**
*\fn void decaler_gauche_m(Missile * tab, int taille_tab, int i)
*\brief decale de 1 les valeurs d'un tableau de missile
*\param tab tab de missile
*\param taille_tab taille du tableau
*\param i indice de début
*/
void decaler_gauche_m(Missile * tab, int taille_tab, int i){

    for(;i<taille_tab-1;i++){
        tab[i]=tab[i+1];
    }
}
/**
*\fn void decaler_gauche_e(Explosion * tab, int taille_tab, int i)
*\brief decale de 1 les valeurs d'un tableau d'explosion
*\param tab tab d'explosion
*\param taille_tab taille du tableau
*\param i indice de début
*/
void decaler_gauche_e(Explosion * tab, int taille_tab, int i){

    for(;i<taille_tab-1;i++){
        tab[i]=tab[i+1];
    }
}


/**
*\fn void asteroid_cpy(Asteroid * asteroid_src, Asteroid * asteroid_dest)
*\brief copie de la source au destinataire
*\param asteroid_src à copier
*\param asteroid_dest reçoit les valeurs
*/
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
	asteroid_dest->frame_hit = asteroid_src->frame_hit;
	asteroid_dest->frozen = asteroid_src->frozen;
	asteroid_dest->frame_depart = asteroid_src->frame_depart;
	asteroid_dest->cote_spawn = asteroid_src->cote_spawn;
}

/**
*\fn int getFirstNullText(TextBonus textsBonus[MAX_TEXT_BONUS])
*\brief détermine à quel endroit le texte va s'afficher
*\param textsBonus[MAX_TEXT_BONUS]
*\return l'indice
*/
int getFirstNullText(TextBonus textsBonus[MAX_TEXT_BONUS]){
	for(int i=0; i<MAX_TEXT_BONUS; i++){
		if(!textsBonus[i].frame)
			return i;
	}

	int minI=0, minFrame = textsBonus[0].frame;
	for(int i=1; i<MAX_TEXT_BONUS; i++){
		if(textsBonus[i].frame < minFrame)
			minI = i;
	}

	return minI;
}


/**
*\fn int isObtainable(int id_bonus, Vaiss vaisseau, float munitions[NB_MISSILES], int nbBombeNucleaire)
*\brief test si un bonus peut être obtenu
*\param id_bonus indice du bonus
*\param vaisseau le vaisseau
*\param munitions[NB_MISSILES] munitions des missiles
*\param nbBombeNucleaire nombre de bombe nucleaire stocké
*\return Vrai si le bonus est possible à obtenir sinon FAUX
*/
int isObtainable(int id_bonus, Vaiss vaisseau, float munitions[NB_MISSILES], int nbBombeNucleaire){
	if(id_bonus >= NB_BONUS)
		return (munitions[id_bonus - NB_BONUS +1] <= MAX_RATIO_AMMO_OBTAINABLE);
	else
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

			case BOMBE_NUCLEAIRE:
				return (nbBombeNucleaire < MAX_BOMBE_NUCLEAIRE);

			default :
				return SDL_TRUE;
		}
}

/**
*\fn int get_bonus(Vaiss vaisseau, float munitions[NB_MISSILES], int nbBombeNucleaire)
*\brief attribue un bonus au hasard
*\param vaisseau le vaisseau
*\param munitions[NB_MISSILES] munitions des missiles
*\param nbBombeNucleaire nombre de bombe nucleaire stocké
*\return l'indice du bonus
*/
int get_bonus(Vaiss vaisseau, float munitions[NB_MISSILES], int nbBombeNucleaire){
	int chanceTotal = 0;
	int i;
	for(i=0; i<NB_BONUS + NB_MISSILES -1; i++)
		if(isObtainable(i, vaisseau, munitions, nbBombeNucleaire))
			chanceTotal += CHANCE_BONUS[i];

	int proba = rand()%chanceTotal + 1;

	for(i=0; i<NB_BONUS  + NB_MISSILES -1 && proba > 0; i++)
		if(isObtainable(i, vaisseau, munitions, nbBombeNucleaire))
			proba -= CHANCE_BONUS[i];

	return i-1;

}

/**
*\fn int detruire_asteroid(Asteroid ** asteroides, int * nb_asteroid, int i_asteroid,Vaiss * vaisseau,int touche_bouclier, ScoreTotal * score, int* nbBombeNucleaire, float angleMissile, TextBonus bonusTexts[MAX_TEXT_BONUS], float munitions[NB_MISSILES], long keys[4], long long *score_hash, Mix_Chunk * bonusSound, Mix_Chunk * exploSound, int done)
*\brief détruit un asteroid
*\param asteroides liste d'asteroides
*\param nb_asteroid nombre d'asteroides
*\param i_asteroid indice de l'asteroide
*\param vaisseau le vaisseau
*\param touche_bouclier indique si il y a un bouclier
*\param score score total
*\param nbBombeNucleaire nombre de Bombe Nucléaire
*\param angleMissile angle du missile
*\param bonusTexts[MAX_TEXT_BONUS] texte du bonus affiché à l'écran
*\param munitions[NB_MISSILES] liste de munitions
*\param keys[4] clé du hashage
*\param score_hash score hashé
*\param bonusSound son du bonus
*\param exploSound son de l'explosion
*\param done indique si le jeu est fini
*\return 0 si il n'y a pas de problème sinon return HACKED
*/
int detruire_asteroid(Asteroid ** asteroides, int * nb_asteroid, int i_asteroid,Vaiss * vaisseau,int touche_bouclier, ScoreTotal * score, int* nbBombeNucleaire, float angleMissile, TextBonus bonusTexts[MAX_TEXT_BONUS], float munitions[NB_MISSILES], long keys[4], long long *score_hash, Mix_Chunk * bonusSound, Mix_Chunk * exploSound, int done){
	int channel=Mix_GroupAvailable(1);
	if (channel==-1) {
		channel = Mix_GroupOldest(1);;
		if (channel==-1)
			channel = 8;
	}

	Mix_PlayChannel(channel, exploSound, 0);
    int bonus = (*asteroides)[i_asteroid].bonus;
    (*asteroides)[i_asteroid].bonus = SDL_FALSE;

	if(!done){
		int skinAste = (int)(((*asteroides)[i_asteroid].difficulte_pere-START_DIFFICULTE)/(MAX_DIFF/NB_ASTE_TEXTURES));
		if(skinAste < 0)
			skinAste = 0;
		if(skinAste > NB_ASTE_TEXTURES-1)
			skinAste = NB_ASTE_TEXTURES-1;

		if(!changeProtectedVar(score_hash, &(score->score), (score->score) + SCORE_ASTEROID[skinAste] / ((*asteroides)[i_asteroid].difficulte_pere/(*asteroides)[i_asteroid].difficulte), keys))
			return HACKED;
		score->frameToDest = FRAME_SCORE_ANIM;
	}


    if((*asteroides)[i_asteroid].taille>TAILLE_MIN_SPLIT && (*asteroides)[i_asteroid].difficulte * pow((*asteroides)[i_asteroid].taille,2)  > DIFFICULTE_MIN_SPLIT * pow(MAX_ASTEROID_SIZE,2) && !touche_bouclier && bonus != BOMBE_NUCLEAIRE ){

		(*asteroides)[i_asteroid].frame_hit = 0;
        (*asteroides)[i_asteroid].taille/=2;
        (*asteroides)[i_asteroid].angle= angleMissile + PI/2;
		(*asteroides)[i_asteroid].pv_max /= 2;
		(*asteroides)[i_asteroid].difficulte /= 2;
		(*asteroides)[i_asteroid].pv = (*asteroides)[i_asteroid].pv_max;
		(*asteroides)[i_asteroid].angle_rota=rand()%(int)(2*PI*100);
		(*asteroides)[i_asteroid].vitesse_rota = randSign() * ((1+ MAX_ASTEROID_SIZE - (*asteroides)[i_asteroid].taille) / (float)(MAX_ASTEROID_SIZE - TAILLE_MIN_ASTEROID)) * ((rand()%(int)(MAX_VITESSE_ROTA*PRECISION_RAND_FLOAT))/PRECISION_RAND_FLOAT) ;
		(*asteroides)[i_asteroid].frozen= 0;

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

    if(bonus && !done){
		int id_bonus = get_bonus(*vaisseau, munitions, *nbBombeNucleaire);
		if( recoit_bonus(id_bonus,vaisseau,nbBombeNucleaire,score, munitions, keys, score_hash, bonusSound) == HACKED )
			return HACKED;

		int iText= getFirstNullText(bonusTexts);
		bonusTexts[iText].id = id_bonus;
		bonusTexts[iText].frame = FRAME_SHOW_BONUS_TEXT;
	}

    return 0;

}


// Vaisseau

/**
*\fn void turn_vaiss(Vaiss * vaisseau, int turn, Vector2f * accelerate)
*\brief permet au vaisseau de tourner
*\param vaisseau le vaisseau
*\param turn indique si l'on doit tourner à gauche ou à droite
*\param accelerate acceleration du vaisseau
*/
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

/**
*\fn void acceleration( Vector2f *accelerate, Vaiss vaisseau)
*\brief augmente la vitesse de déplacement du vaisseau
*\param accelerate acceleration du vaisseau
*\param vaisseau le vaisseau
*/
void acceleration( Vector2f *accelerate, Vaiss vaisseau){

	accelerate->x += RATIO_ACCEL[vaisseau.frame_thrust+1] * ACCEL * cos(vaisseau.angle);
	accelerate->y += RATIO_ACCEL[vaisseau.frame_thrust+1] * ACCEL * sin(vaisseau.angle);

}

/**
*\fn void move( Vaiss *vaisseau,Vector2f accelerate)
*\brief permet au vaisseau d'avancer
*\param vaisseau le vaisseau
*\param accelerate acceleration du vaisseau
*/
void move( Vaiss *vaisseau,Vector2f accelerate){
	vaisseau->x+=accelerate.x;
	vaisseau->y+=accelerate.y;

}

/**
*\fn void afficher_texture_vaisseau(Vaiss vaisseau, SDL_Renderer * renderer, SDL_Texture * vaisseau_texture, SDL_Texture * gem_texture, SDL_Texture * thrust_texture)
*\brief affichage de la texture du vaisseau
*\param vaisseau le vaisseau
*\param renderer Le renderer où dessiner
*\param vaisseau_texture texture du vaisseau
*\param gem_texture texture de la gemme sur le vaisseau
*\param thrust_texture texture des flammes du vaisseau
*/
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

/**
*\fn void afficher_vaisseau( Vaiss vaisseau, SDL_Renderer *renderer, SDL_Texture * vaisseau_texture, SDL_Texture * gem_texture, SDL_Texture * thrust_texture)
*\brief affichage du vaisseau
*\param vaisseau le vaisseau
*\param renderer Le renderer où dessiner
*\param vaisseau_texture texture du vaisseau
*\param gem_texture texture de la gemme sur le vaisseau
*\param thrust_texture texture des flammes du vaisseau
*/
void afficher_vaisseau( Vaiss vaisseau, SDL_Renderer *renderer, SDL_Texture * vaisseau_texture, SDL_Texture * gem_texture, SDL_Texture * thrust_texture){

    SDL_Rect vaisseauRect = {vaisseau.x-RAYON_VAISS,vaisseau.y-RAYON_VAISS,RAYON_VAISS*2,RAYON_VAISS*2};

    afficher_texture_vaisseau(vaisseau,renderer,vaisseau_texture,gem_texture,thrust_texture);

    if(vaisseauRect.x<0){
        vaisseauRect.x+=PLAYGROUND_SIZE_W;
        vaisseau.x+=PLAYGROUND_SIZE_W;
        afficher_texture_vaisseau(vaisseau,renderer,vaisseau_texture,gem_texture,thrust_texture);
    }
    else if(vaisseauRect.x+RAYON_VAISS*2>PLAYGROUND_SIZE_W){
        vaisseauRect.x-=PLAYGROUND_SIZE_W;
        vaisseau.x-=PLAYGROUND_SIZE_W;
        afficher_texture_vaisseau(vaisseau,renderer,vaisseau_texture,gem_texture,thrust_texture);
    }


    if(vaisseauRect.y<0 || vaisseauRect.y+RAYON_VAISS*2>PLAYGROUND_SIZE_H){
        if(vaisseauRect.y<0){
            vaisseau.y+=PLAYGROUND_SIZE_H;
            afficher_texture_vaisseau(vaisseau,renderer,vaisseau_texture,gem_texture,thrust_texture);
        }
        else {
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
}

/**
*\fn void afficher_explosion(SDL_Renderer * renderer,Explosion explosion, SDL_Texture * explo_texture)
*\brief affichage de d'explosion
*\param renderer Le renderer où dessiner
*\param explosion explosion de la bombe
*\param explo_texture texture de l'explosion
*/
void afficher_explosion(SDL_Renderer * renderer,Explosion explosion, SDL_Texture * explo_texture){
	SDL_Rect src = EXPLO_SRCS[explosion.id];
	src.x += (int)((FRAME_EXPLOSIONS[explosion.id] - explosion.frame) / ((float)FRAME_EXPLOSIONS[explosion.id] /NB_ANIM_EXPLOSIONS[explosion.id])) * src.w;
	SDL_Rect dest = {explosion.x, explosion.y, explosion.taille, explosion.taille};

	SDL_RenderCopy(renderer, explo_texture, &src, &dest);

	if( dest.x < 0 || dest.x + dest.w > PLAYGROUND_SIZE_W ){
		if(dest.x < 0 )
			dest.x += PLAYGROUND_SIZE_W;
		else
			dest.x -= PLAYGROUND_SIZE_W;

		SDL_RenderCopy(renderer, explo_texture, &src, &dest);
	}

	if( dest.y < 0 || dest.y + dest.w > PLAYGROUND_SIZE_H ){
		if(dest.y < 0 )
			dest.y += PLAYGROUND_SIZE_H;
		else
			dest.y -= PLAYGROUND_SIZE_H;

		SDL_RenderCopy(renderer, explo_texture, &src, &dest);

		if( dest.x < 0 || dest.x + dest.w > PLAYGROUND_SIZE_W ){
			if(dest.x < 0 )
				dest.x += PLAYGROUND_SIZE_W;
			else
				dest.x -= PLAYGROUND_SIZE_W;

			SDL_RenderCopy(renderer, explo_texture, &src, &dest);
		}
	}
}

//LASER


/**
*\fn void afficher_laser(SDL_Renderer * renderer, SDL_Texture * laser_texture, Vaiss vaisseau, int frame, Asteroid asteroid)
*\brief affichage du laser, fait par Hugo
*\param renderer Le renderer où dessiner
*\param laser_texture texture du laser
*\param vaisseau le vaisseau
*\param frame
*\param asteroid un asteroide
*/
void afficher_laser(SDL_Renderer * renderer, SDL_Texture * laser_texture, Vaiss vaisseau, int frame, Asteroid asteroid){



	SDL_Rect laser_src={0,MISSILE_SRC[SHOT_LASER].y,MISSILE_SRC[SHOT_LASER].x,MISSILE_SRC[SHOT_LASER].y};

	if(frame != 0)
		laser_src.y= (frame%(NB_LASER_BEAM-1)+1) * laser_src.h;

	SDL_Point spawn_laser= {0, RAYON_VAISS};

	float xM = spawn_laser.x;
    float yM = spawn_laser.y;
    spawn_laser.x = xM*cos(vaisseau.angle-PI/2) - yM * sin(vaisseau.angle-PI/2) ;
    spawn_laser.y = xM * sin(vaisseau.angle-PI/2) + yM * cos(vaisseau.angle-PI/2) ;


	SDL_Rect laser_dest = {vaisseau.x + spawn_laser.x - MISSILE_CENTRES[SHOT_LASER].x,vaisseau.y + spawn_laser.y - MISSILE_CENTRES[SHOT_LASER].y, laser_src.w, laser_src.h};

	SDL_RenderCopyEx(renderer,laser_texture, &laser_src, &laser_dest,vaisseau.angle*180/PI,&MISSILE_CENTRES[SHOT_LASER],SDL_FLIP_NONE);
}

/**
*\fn void afficher_text_bonus(SDL_Renderer * renderer, TextBonus textBonus[MAX_TEXT_BONUS], TTF_Font * font)
*\brief affichage du texte des bonus, fait par Hugo
*\param renderer Le renderer où dessiner
*\param textBonus[MAX_TEXT_BONUS]
*\param font Les polices
*/
void afficher_text_bonus(SDL_Renderer * renderer, TextBonus textBonus[MAX_TEXT_BONUS], TTF_Font * font){
	int nbText=0;
	for(int i=0; i<MAX_TEXT_BONUS; i++)
		if(textBonus[i].frame)
			nbText++;

	SDL_Rect dest;
	dest.y = PLAYGROUND_SIZE_H/2 - 0.5*ESPACEMENT_SHOW_TEXT*(nbText-1);

	int firstDraw= SDL_TRUE;
	for(int i=0; i<MAX_TEXT_BONUS; i++){
		if(textBonus[i].frame){
			SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, TEXT_BONUS[textBonus[i].id], BONUS_TEXT_COLOR);
			SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
			SDL_SetTextureAlphaMod(Message, ALPHA_BONUS[textBonus[i].frame]);
			SDL_QueryTexture(Message,NULL,(int*)SDL_TEXTUREACCESS_STATIC,&(dest.w), &(dest.h) );
			dest.w /= (OPEN_FONT_SIZE / SIZE_BONUS_TEXT);
			dest.h /= (OPEN_FONT_SIZE / SIZE_BONUS_TEXT);
			dest.x = PLAYGROUND_SIZE_W/2 - dest.w/2;
			if(firstDraw){
				dest.y -= dest.h/2;
				firstDraw = SDL_FALSE;
			}
			SDL_RenderCopy(renderer, Message, NULL, &dest);
			SDL_FreeSurface(surfaceMessage);
			SDL_DestroyTexture(Message);
			dest.y += ESPACEMENT_SHOW_TEXT;
		}
	}

}



//MISSILE

/**
*\fn void tirer(Vaiss * vaisseau, Missile ** missiles, int * nb_missiles, Mix_Chunk * shootSound)
*\brief permet de tirer
*\param vaisseau le vaisseau
*\param missile liste de missiles
*\param nb_missiles nombre de missiles
*\param shootSound son du tir
*/
 void tirer(Vaiss * vaisseau, Missile ** missiles, int * nb_missiles, Mix_Chunk * shootSound){

	Mix_PlayChannel( 5, shootSound,0);

	*nb_missiles+=vaisseau->nb_tir;
	(*missiles) = realloc(*missiles,*nb_missiles * sizeof(Missile));

	for(int i=0;i<vaisseau->nb_tir;i++){
		(*missiles)[*nb_missiles-1-i].id = vaisseau->missile_id;
		(*missiles)[*nb_missiles-1-i].x=vaisseau->x+DISTANCE_CANON*cos(vaisseau->angle);
		(*missiles)[*nb_missiles-1-i].y=vaisseau->y+DISTANCE_CANON*sin(vaisseau->angle);
		(*missiles)[*nb_missiles-1-i].angle=vaisseau->angle+angle_tir_multiple[vaisseau->nb_tir-1][i];
		(*missiles)[*nb_missiles-1-i].target_angle=(*missiles)[*nb_missiles-1-i].angle;
		(*missiles)[*nb_missiles-1-i].frame= DUREE_MISSILE_BASE * DUREE_MISSILES[vaisseau->missile_id];
		(*missiles)[*nb_missiles-1-i].vitesse=vaisseau->vitesse_missile * VITESSE_MISSILES[vaisseau->missile_id];
		(*missiles)[*nb_missiles-1-i].degat=vaisseau->degat_missile  * DEGAT_MISSILES[vaisseau->missile_id];
	}

	vaisseau->frame_recharge = vaisseau->temps_recharge * FREQUENCE_MISSILES[vaisseau->missile_id];

}


/**
*\fn float calculer_angle(float x1, float y1, float x2, float y2)
*\brief calcul de l'angle
*\param x1 coord en x
*\param y1 coord en y
*\param x2 coord en x
*\param y2 coord en y
*\return le resultat du calcul
*/
float calculer_angle(float x1, float y1, float x2, float y2){
	return atan2((y2-y1) , (x2-x1));
}

/**
*\fn float basePi(float angle)
*\brief converti un angle en base Pi
*\param angle l'angle à convertir
*/
float basePi(float angle){
	while(angle > PI)
		angle -= 2 * PI;
	while(angle < - PI)
		angle += 2*PI;

	return angle;
}
/**
*\fn float base360(float angle)
*\brief converti un angle en base 360
*\param angle l'angle à convertir
*/
float base360(float angle){
	return (angle * 360 / (2*PI));
}

/**
*\fn int asteroid_plus_proche(SDL_Renderer * renderer, Asteroid * asteroides, int nb_asteroid, Missile missile, float * best_angle_between)
*\brief cherche l'asteroid le plus proche
*\param renderer Le renderer où dessiner
*\param asteroides liste d'asteroides
*\param nb_asteroid nombre d'asteroides
*\param best_angle_between meilleur_angle_possible
*\return imin_asteroid indice de l'asteroide
*/
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

/**
*\fn int signOf( float f)
*\brief Détermine le signe d'un float
*\param f Le float
*\return Le signe de ce float
*/
int signOf( float f){
	return (f > 0) ? 1 : ((f < 0 )? -1 : 0);
}

/**
*\fnvoid mouvement_tir(SDL_Renderer *renderer,Missile * shot, Asteroid * asteroides, int nb_asteroid)
*\brief mouvement du missile en fonction de l'arme, fait par Hugo
*\param renderer Le renderer où dessiner
*\param shot un tir de missile
*\param asteroides liste d'asteroides
*\param nb_asteroid nombre d'asteroides
*/
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


/**
*\fn void rotateAsteroides(Asteroid * asteroides, int nb_asteroid)
*\brief permet la rotation de l'asteroide sur lui même
*\param asteroides liste d'asteroides
*\param nb_asteroid nombre d'asteroides
*/
void rotateAsteroides(Asteroid * asteroides, int nb_asteroid){
	for(int i=0; i<nb_asteroid; i++)
		asteroides[i].angle_rota += asteroides[i].vitesse_rota * (1 - 0.4*asteroides[i].frozen);
}

/**
*\fn void update_frame(Missile ** missiles, int * nb_missiles, Vaiss * vaisseau, long int * frame, int * frame_apparition_asteroid, float * vitesse_spawn, int * frame_2asteroid)
*\brief mise à jour à chaque frame
*\param missiles liste de missiles
*\param nb_missiles nombre de missiles
*\param vaisseau le vaisseau
*\param frame
*\param frame_apparition_asteroid
*\param vitesse_spawn vitesse à laquelle spawn les astéroides
*\param frame_2asteroid
*/
void update_frame(Missile ** missiles, int * nb_missiles, Vaiss * vaisseau, long int * frame, int * frame_apparition_asteroid, float * vitesse_spawn, int * frame_2asteroid){
	for(int i=0;i<*nb_missiles;i++){
		((*missiles)[i].frame)--;
			if((*missiles)[i].frame == 0){

				decaler_gauche_m(*missiles, *nb_missiles, i);
				(*nb_missiles)--;
				if(*nb_missiles != 0){
					(*missiles)=realloc(*missiles,sizeof(Missile)* (*nb_missiles));
				}
				i--;
			}
	}
	if(vaisseau->frame_recharge){
		(vaisseau->frame_recharge)--;
	}
	(*frame)++;
	(*frame_apparition_asteroid)--;
	if(*vitesse_spawn > VITESSE_SPAWN_MIN){
		(*vitesse_spawn)-= ACCELERATION_SPAWN ;
	}
	if(*frame_2asteroid){
		(*frame_2asteroid)--;
	}
}

/**
*\fn void afficher_tir( SDL_Renderer * renderer, Missile shot, SDL_Texture * missileTexture)
*\brief affichage du tir
*\param renderer Le renderer où dessiner
*\param shot un tir
*\param missileTexture texture du missile
*/
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


//ASTEROID

/**
*\fn void spawn_asteroid(Vaiss vaisseau, Asteroid ** asteroides, int * nb_asteroid, float difficulte, float munitions[NB_MISSILES], int hardcore)
*\brief apparation d'un asteroide
*\param vaisseau le vaisseau
*\param asteroides liste d'asteroides
*\param nb_asteroid nombre d'asteroid
*\param difficulte difficulte du jeu
*\param munitions[NB_MISSILES] liste des munitions
*\param hardcore mode de jeu hardcore ou non
*/
void spawn_asteroid(Vaiss vaisseau, Asteroid ** asteroides, int * nb_asteroid, float difficulte, float munitions[NB_MISSILES], int hardcore){
	difficulte *= 1 + randSign() * (rand()%(int)(PRECISION_RAND_FLOAT *INTERVALE_RAND_DIFFICULTE))/(float)PRECISION_RAND_FLOAT;
	int id_coord;
	float ratio_pv;
	(*nb_asteroid)++;
	(*asteroides)=realloc((*asteroides),sizeof(Asteroid)*(*nb_asteroid));
	if(!hardcore){
		(*asteroides)[*nb_asteroid-1].angle=(rand()%(int)PRECISION_RAND_FLOAT*2*PI)/PRECISION_RAND_FLOAT;
		(*asteroides)[*nb_asteroid-1].pv=PV_BASE;
		(*asteroides)[*nb_asteroid-1].vitesse=VITESSE_BASE;
		(*asteroides)[*nb_asteroid-1].difficulte=difficulte;
		(*asteroides)[*nb_asteroid-1].difficulte_pere = (*asteroides)[*nb_asteroid-1].difficulte;
		(*asteroides)[*nb_asteroid-1].angle_rota=rand()%(int)(2*PI*100);
		(*asteroides)[*nb_asteroid-1].frame_hit = 0;
		(*asteroides)[*nb_asteroid-1].frozen= 0;
		(*asteroides)[*nb_asteroid-1].frame_depart=FRAME_DEPART;
		(*asteroides)[*nb_asteroid-1].cote_spawn=-1;

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

		(*asteroides)[*nb_asteroid-1].vitesse_rota= randSign() * ((1+ MAX_ASTEROID_SIZE - (*asteroides)[*nb_asteroid-1].taille) / (float)(MAX_ASTEROID_SIZE - TAILLE_MIN_ASTEROID)) * ((rand()%(int)(MAX_VITESSE_ROTA*PRECISION_RAND_FLOAT))/PRECISION_RAND_FLOAT) ;


		(*asteroides)[*nb_asteroid-1].vitesse*=difficulte/ratio_pv;
		if((*asteroides)[*nb_asteroid-1].vitesse > VITESSE_MAX_ASTEROID){
			(*asteroides)[*nb_asteroid-1].vitesse= VITESSE_MAX_ASTEROID;
		}

		if(rand()%PROBA_BONUS==0){
			(*asteroides)[*nb_asteroid-1].bonus=SDL_TRUE;
		}
		else{
			(*asteroides)[*nb_asteroid-1].bonus=SDL_FALSE;
		}
		do{
			id_coord=rand()%4;
			if(id_coord<2){
				id_coord=1;
			}
			(*asteroides)[*nb_asteroid-1].x= coord_spawn[id_coord-1].x;
			(*asteroides)[*nb_asteroid-1].y= coord_spawn[id_coord-1].y;
		}	while(trop_pres(vaisseau.x,vaisseau.y,(*asteroides)[*nb_asteroid-1].x,(*asteroides)[*nb_asteroid-1].y,DIST_VAISSEAU_ASTEROID+RAYON_VAISS+(*asteroides)[*nb_asteroid-1].taille,-1));
	}
	else{
		(*asteroides)[*nb_asteroid-1].vitesse=VITESSE_BASE;
		(*asteroides)[*nb_asteroid-1].difficulte=difficulte;
		(*asteroides)[*nb_asteroid-1].difficulte_pere = (*asteroides)[*nb_asteroid-1].difficulte;
		(*asteroides)[*nb_asteroid-1].angle_rota=rand()%(int)(2*PI*100);
		(*asteroides)[*nb_asteroid-1].pv=PV_BASE;
		(*asteroides)[*nb_asteroid-1].pv_max=PV_BASE;
		(*asteroides)[*nb_asteroid-1].frame_hit = 0;
		(*asteroides)[*nb_asteroid-1].frozen= 0;
		(*asteroides)[*nb_asteroid-1].taille=rand()%MAX_ASTEROID_SIZE;
		(*asteroides)[*nb_asteroid-1].bonus=SDL_FALSE;
		(*asteroides)[*nb_asteroid-1].vitesse_rota= randSign() * ((1+ MAX_ASTEROID_SIZE - (*asteroides)[*nb_asteroid-1].taille) / (float)(MAX_ASTEROID_SIZE - TAILLE_MIN_ASTEROID)) * ((rand()%(int)(MAX_VITESSE_ROTA*PRECISION_RAND_FLOAT))/PRECISION_RAND_FLOAT) ;
		(*asteroides)[*nb_asteroid-1].frame_depart=FRAME_DEPART;

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

		(*asteroides)[*nb_asteroid-1].vitesse_rota= randSign() * ((1+ MAX_ASTEROID_SIZE - (*asteroides)[*nb_asteroid-1].taille) / (float)(MAX_ASTEROID_SIZE - TAILLE_MIN_ASTEROID)) * ((rand()%(int)(MAX_VITESSE_ROTA*PRECISION_RAND_FLOAT))/PRECISION_RAND_FLOAT) ;


		(*asteroides)[*nb_asteroid-1].vitesse*=difficulte/ratio_pv;
		(*asteroides)[*nb_asteroid-1].vitesse*=SPEED_HARDCORE;
		if((*asteroides)[*nb_asteroid-1].vitesse > VITESSE_MAX_ASTEROID_HARDCORE){
			(*asteroides)[*nb_asteroid-1].vitesse= VITESSE_MAX_ASTEROID_HARDCORE;
		}
		else if((*asteroides)[*nb_asteroid-1].vitesse < VITESSE_MIN_ASTEROID){
			(*asteroides)[*nb_asteroid-1].vitesse= VITESSE_MIN_ASTEROID;
		}
		do{
			(*asteroides)[*nb_asteroid-1].cote_spawn=rand()%4;
			if((*asteroides)[*nb_asteroid-1].cote_spawn%2){
				(*asteroides)[*nb_asteroid-1].y= PLAYGROUND_SIZE_H*((*asteroides)[*nb_asteroid-1].cote_spawn==1?0:1);
				(*asteroides)[*nb_asteroid-1].x= rand()%PLAYGROUND_SIZE_W;
			}
			else{
				(*asteroides)[*nb_asteroid-1].x= PLAYGROUND_SIZE_W*((*asteroides)[*nb_asteroid-1].cote_spawn?1:0);
				(*asteroides)[*nb_asteroid-1].y= rand()%PLAYGROUND_SIZE_H;
			}
		}	while(sur_asteroid(*asteroides,*nb_asteroid,*nb_asteroid-1));
		(*asteroides)[*nb_asteroid-1].angle=PI/2*(*asteroides)[*nb_asteroid-1].cote_spawn;
	}

}

/**
*\fn void mouvement_asteroid(Asteroid* asteroid, int hardcore)
*\brief mouvement d'un asteroide
*\param asteroid un asteroide
*\param hardcore mode de jeu hardcore ou non
*/
void mouvement_asteroid(Asteroid* asteroid, int hardcore){
	if(!asteroid->frame_depart || !hardcore){
		asteroid->x+=asteroid->vitesse*cos(asteroid->angle) * (1 - 0.4*asteroid->frozen); // 0 1   /  1 0.5   2  0
		asteroid->y+=asteroid->vitesse*sin(asteroid->angle) * (1 - 0.4*asteroid->frozen); // 0 1   /  1 0.5   2  0
	}
}

/**
*\fn void afficher_texture_asteroid(SDL_Renderer* renderer, SDL_Texture * textureAsteroid, SDL_Texture* textureFissure, SDL_Texture* textureBonus, SDL_Texture* textureGlace, SDL_Rect src, SDL_Rect dest, SDL_Rect srcFissure, SDL_Rect srcGlace, Asteroid asteroid)
*\brief affichage de la texture de l'asteroide
*\param renderer Le renderer où dessiner
*\param textureAsteroid texture de l'asteroid
*\param textureFissure texture de la fissure
*\param textureBonus texture du bonus
*\param textureGlace texture de la glace
*\param src Rect source
*\param dest Rect destinataire
*\param srcFissure Fissure source
*\param srcGlace Glace source
*\param asteroid un asteroid
*/
void afficher_texture_asteroid(SDL_Renderer* renderer, SDL_Texture * textureAsteroid, SDL_Texture* textureFissure, SDL_Texture* textureBonus, SDL_Texture* textureGlace, SDL_Rect src, SDL_Rect dest, SDL_Rect srcFissure, SDL_Rect srcGlace, Asteroid asteroid){

	SDL_RenderCopyEx(renderer, textureAsteroid, &src, &dest, asteroid.angle_rota, NULL, SDL_FLIP_NONE);
	if(srcGlace.y>=0){
		SDL_RenderCopyEx(renderer, textureGlace, &srcGlace, &dest, asteroid.angle_rota, NULL, SDL_FLIP_NONE);
	}


	if(asteroid.bonus){
		SDL_RenderCopyEx(renderer, textureBonus, &ASTE_SRC, &dest, asteroid.angle_rota, NULL, SDL_FLIP_NONE);
	}
	if( srcFissure.y>=0){
		SDL_RenderCopyEx(renderer, textureFissure, &srcFissure, &dest, asteroid.angle_rota, NULL, SDL_FLIP_NONE);
	}


}
/**
*\fn void afficher_asteroid(Asteroid asteroid, SDL_Renderer * renderer, SDL_Texture* textureAsteroid, SDL_Texture* textureFissure, SDL_Texture* textureBonus, SDL_Texture* textureGlace, int hardcore)
*\brief affichage de l'asteroide
*\param asteroid un asteroid
*\param renderer Le renderer où dessiner
*\param textureAsteroid texture de l'asteroid
*\param textureFissure texture de la fissure
*\param textureBonus texture du bonus
*\param textureGlace texture de la glace
*\param hardcore mode de jeu hardcore ou non
*/
void afficher_asteroid(Asteroid asteroid, SDL_Renderer * renderer, SDL_Texture* textureAsteroid, SDL_Texture* textureFissure, SDL_Texture* textureBonus, SDL_Texture* textureGlace, int hardcore){
	SDL_SetTextureAlphaMod(textureAsteroid, 255 - 150 * asteroid.frame_depart/FRAME_DEPART);
	SDL_SetTextureAlphaMod(textureFissure, 255 - 150 * asteroid.frame_depart/FRAME_DEPART);
	SDL_SetTextureAlphaMod(textureBonus, 255 - 150 * asteroid.frame_depart/FRAME_DEPART);
	SDL_SetTextureAlphaMod(textureGlace, 255 - 150 * asteroid.frame_depart/FRAME_DEPART);

	SDL_Rect src = ASTE_SRC;

	SDL_Rect asteroidRect={(int)asteroid.x-asteroid.taille, (int)asteroid.y-asteroid.taille,asteroid.taille*2,asteroid.taille*2};

	int sizeAste = (roundf(asteroid.taille/(MAX_ASTEROID_SIZE/NB_TAILLE_ASTE)) - 1);
	if(sizeAste < 0)
		sizeAste = 0;

	src.w = DIAMETRE_ASTE[sizeAste];
	src.h = DIAMETRE_ASTE[sizeAste];

	src.x = ASTE_SRC.w * sizeAste;

	SDL_Rect srcGlace = src;
	srcGlace.y += (asteroid.frozen - 1) * ASTE_SRC.h;

	SDL_Rect srcFissure = src;
	int fissure = (int)(((float)(asteroid.pv_max - asteroid.pv) / asteroid.pv_max) * (NB_FISSURES+1) ) - 1;
	srcFissure.y = fissure * ASTE_SRC.h;

	if(asteroid.frame_hit){
		src.x += NB_TAILLE_ASTE*ASTE_SRC.w;
	}
	else {
		SDL_SetTextureColorMod(textureFissure, 0, 0, 0);
	}

	if(srcGlace.y >= 0)
		SDL_SetTextureColorMod(textureFissure, FISSURE_GLACE_COLOR.r, FISSURE_GLACE_COLOR.g, FISSURE_GLACE_COLOR.b);



	int skinAste = (int)((asteroid.difficulte_pere-START_DIFFICULTE)/(MAX_DIFF/NB_ASTE_TEXTURES));
	if(skinAste < 0)
		skinAste = 0;
	if(skinAste > NB_ASTE_TEXTURES-1)
		skinAste = NB_ASTE_TEXTURES-1;

	src.y = ASTE_SRC.h * skinAste;




	afficher_texture_asteroid(renderer,textureAsteroid, textureFissure, textureBonus, textureGlace, src, asteroidRect, srcFissure, srcGlace, asteroid);

	//Dessiner bonus est diff�rent quand bord de l'�cran car on ne peut pas d�ssiner en desous de 0 -> � faire
	if(!hardcore || asteroid.cote_spawn == 1 || asteroid.cote_spawn == 3){
		if(asteroidRect.x+asteroid.taille*2>PLAYGROUND_SIZE_W || asteroidRect.x<0){
			if(asteroidRect.x<0){
				asteroidRect.x+=PLAYGROUND_SIZE_W;
			}
			else{
				asteroidRect.x-=PLAYGROUND_SIZE_W;
			}
			afficher_texture_asteroid(renderer,textureAsteroid, textureFissure, textureBonus, textureGlace, src, asteroidRect, srcFissure, srcGlace, asteroid);
		}
	}
	if(!hardcore || asteroid.cote_spawn == 0 || asteroid.cote_spawn == 2){
		if(asteroidRect.y+asteroid.taille*2>PLAYGROUND_SIZE_H || asteroidRect.y<0){
			if(asteroidRect.y<0){
				asteroidRect.y+=PLAYGROUND_SIZE_H;
			}
			else {
				asteroidRect.y-=PLAYGROUND_SIZE_H;
			}

			afficher_texture_asteroid(renderer,textureAsteroid, textureFissure, textureBonus, textureGlace, src, asteroidRect, srcFissure, srcGlace, asteroid);

			if(!hardcore || asteroid.cote_spawn == 1 || asteroid.cote_spawn == 3){
				if(asteroidRect.x+asteroid.taille*2>PLAYGROUND_SIZE_W || asteroidRect.x<0){
					if(asteroidRect.x<0){
						asteroidRect.x+=PLAYGROUND_SIZE_W;
					}
					else{
						asteroidRect.x-=PLAYGROUND_SIZE_W;
					}
					afficher_texture_asteroid(renderer,textureAsteroid, textureFissure, textureBonus, textureGlace, src, asteroidRect, srcFissure, srcGlace, asteroid);
				}
			}
		}
	}
}

/**
*\fn void initText(TextBonus textsBonus[MAX_TEXT_BONUS])
*\brief initialiser du texte
*\param textsBonus
*/
void initText(TextBonus textsBonus[MAX_TEXT_BONUS]){
	for(int i=0; i<MAX_TEXT_BONUS; i++){
		textsBonus[i].frame = 0;
	}
}

/**
*\fn void afficherJauge(SDL_Renderer * renderer, SDL_Texture * textureJauge, float ratioWindowSize, Jauge jauge)
*\brief afficher la jauge de munitions
*\param renderer Le renderer où dessiner
*\param textureJauge texture de la jauge
*\param ratioWindowSize  Le ratio de la taille de fenetre par rapport à la taille max
*\param jauge la jauge à dessiner
*/
void afficherJauge(SDL_Renderer * renderer, SDL_Texture * textureJauge, float ratioWindowSize, Jauge jauge){
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

	SDL_Rect src = JAUGE_DIM;
	SDL_Rect dest = JAUGE_DIM;
	dest.w *= ratioWindowSize;
	dest.h *= ratioWindowSize;

	dest.x = (HUD_W/2)*ratioWindowSize - dest.w/2;
	dest.y = BASE_WINDOW_H*ratioWindowSize/2 - dest.h/2;


	SDL_Rect jaugeRect = dest;
	jaugeRect.x+= 0.05*jaugeRect.w;
	jaugeRect.y+= 0.01*jaugeRect.h;
	jaugeRect.w -= 0.1*jaugeRect.w;
	jaugeRect.h -= 0.085 * jaugeRect.h;





	SDL_SetRenderDrawColor(renderer, jauge.color.r * MIN_RATIO_COLOR, jauge.color.g * MIN_RATIO_COLOR, jauge.color.b * MIN_RATIO_COLOR, 255);

	SDL_RenderFillRect(renderer, &jaugeRect);

	jaugeRect.y += (jaugeRect.h*(1-jauge.ammo));
	jaugeRect.h *= jauge.ammo;
	SDL_SetRenderDrawColor(renderer, jauge.color.r, jauge.color.g, jauge.color.b, 255);
	SDL_RenderFillRect(renderer, &jaugeRect);

	src.x += src.w;
	SDL_SetTextureColorMod(textureJauge, HUD_COLOR.r, HUD_COLOR.g, HUD_COLOR.b);
	SDL_RenderCopy(renderer,textureJauge, &src, &dest);

	src.x -= src.w;
	//SDL_SetTextureAlphaMod(textureJauge,150);
	SDL_SetTextureColorMod(textureJauge, jauge.color.r * RATIO_COLOR_JAUGE, jauge.color.g * RATIO_COLOR_JAUGE, jauge.color.b * RATIO_COLOR_JAUGE);
	SDL_RenderCopy(renderer,textureJauge, &src, &dest);


}
/**
*\fn void afficherRoue(SDL_Renderer * renderer, SDL_Texture * textureRoue, float ratioWindowSize, float munitions[NB_MISSILES], Roue roue, int currentId, SDL_Color centerColor)
*\brief afficher de la roue de munitions
*\param renderer Le renderer où dessiner
*\param textureRoue texture de la roue
*\param ratioWindowSize  Le ratio de la taille de fenetre par rapport à la taille max
*\param munitions[NB_MISSILES] liste des munitions
*\param roue la roue à dessiner
*\param currentId
*\param centerColor couleur du centre
*/
void afficherRoue(SDL_Renderer * renderer, SDL_Texture * textureRoue, float ratioWindowSize, float munitions[NB_MISSILES], Roue roue, int currentId, SDL_Color centerColor){
	SDL_Rect dest = ROUE_DIM;
	dest.w *= ratioWindowSize;
	dest.h *= ratioWindowSize;

	dest.x = (HUD_W/2)*ratioWindowSize - dest.w/2;
	dest.y = BASE_WINDOW_H*ratioWindowSize/2 - dest.h/2 + JAUGE_DIM.h* ratioWindowSize/2;


	SDL_Rect src = SRC_ROUE;
	src.x += src.w;

	//central gem
	SDL_SetTextureColorMod(textureRoue, centerColor.r, centerColor.g, centerColor.b);
	SDL_RenderCopyEx(renderer,textureRoue, &src, &dest, roue.rota, NULL, SDL_FLIP_NONE);

	//arround jauges

	src.y = src.w;
	for(int i=0; i< NB_ROUE_EMPLACEMENTS; i++){
		src.x = 0;
		float ammo = (i>=NB_MISSILES? 0 : munitions[i]) * NB_ARROUND_JAUGES;
		float ratioColor;
		for(int j=0; j<NB_ARROUND_JAUGES; j++){
			if(ammo>=1)
				ratioColor = 1;
			else if(ammo == 0)
				ratioColor = MIN_RATIO_COLOR;
			else
				ratioColor = MIN_RATIO_COLOR + (1-MIN_RATIO_COLOR) * ammo;

			SDL_Color currentColor = i>=NB_MISSILES? (SDL_Color){0,0,0} : GEM_COLORS[i];
			SDL_SetTextureColorMod(textureRoue, currentColor.r * ratioColor, currentColor.g * ratioColor, currentColor.b * ratioColor);
			SDL_RenderCopyEx(renderer,textureRoue, &src, &dest, roue.rota + (360./NB_ROUE_EMPLACEMENTS) * i, NULL, SDL_FLIP_NONE);
			src.x += src.w;

			ammo --;
			if(ammo<0)
				ammo = 0;
		}
	}

	//roue
	src.x = 0;
	src.y = 0;
	SDL_SetTextureColorMod(textureRoue,ROUE_COLOR.r,ROUE_COLOR.g,ROUE_COLOR.b);
	SDL_RenderCopyEx(renderer,textureRoue, &src, &dest, roue.rota, NULL, SDL_FLIP_NONE);

}

/**
*\fn static void afficherScoreTotal(SDL_Renderer *renderer, TTF_Font *font, int scoreShow, float ratioWindowSize)
*\brief affichage du score total
*\param renderer Le renderer où dessiner
*\param font Les polices
*\param scoreShow
*\param ratioWindowSize  Le ratio de la taille de fenetre par rapport à la taille max
*/
static void afficherScoreTotal(SDL_Renderer *renderer, TTF_Font *font, int scoreShow, float ratioWindowSize){
	char msgScore[20];
	sprintf(msgScore, "%d", scoreShow);
	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, msgScore, SCORE_COLOR);
	SDL_Rect dest ;

	SDL_Texture* Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
	SDL_SetTextureAlphaMod(Message, 200);
	SDL_QueryTexture(Message,NULL,(int*)SDL_TEXTUREACCESS_STATIC,&(dest.w), &(dest.h) );
	dest.w *= (0.8*HUD_H*ratioWindowSize)/dest.h;
	dest.h *= (0.8*HUD_H*ratioWindowSize)/dest.h;

	dest.x = BASE_WINDOW_W*ratioWindowSize/2 - dest.w/2;
	dest.y = HUD_H*ratioWindowSize/2 - dest.h/2;

	SDL_RenderCopy(renderer, Message, NULL, &dest);

	SDL_FreeSurface(surfaceMessage);
	SDL_DestroyTexture(Message);
}

/**
*\fn void afficherBombIcon(SDL_Renderer * renderer, SDL_Texture * textureBombIcon, int nbBombeNucleaire, float ratioWindowSize)
*\brief afficher de l'icone de la bombe
*\param renderer Le renderer où dessiner
*\param textureBombIcon texture de l'icone de bombe
*\param nbBombeNucleaire nombre de bombe nucleaire
*\param ratioWindowSize  Le ratio de la taille de fenetre par rapport à la taille max
*/
void afficherBombIcon(SDL_Renderer * renderer, SDL_Texture * textureBombIcon, int nbBombeNucleaire, float ratioWindowSize){
	if(nbBombeNucleaire)
		SDL_SetTextureAlphaMod(textureBombIcon, 255);
	else
		SDL_SetTextureAlphaMod(textureBombIcon, 50);

	SDL_Rect dest = ROUE_DIM;
	dest.w *= ratioWindowSize;
	dest.h *= ratioWindowSize;

	dest.x = (HUD_W/2)*ratioWindowSize - dest.w/2;
	dest.y = BASE_WINDOW_H*ratioWindowSize/2 + dest.h + JAUGE_DIM.h* ratioWindowSize/2;

	SDL_RenderCopy(renderer, textureBombIcon, NULL, &dest);
}
/**
*\fn static void myFrees(Missile ** missiles, Asteroid ** asteroides, Explosion ** explosions, TTF_Font ** fonts, SDL_Thread ** thread, Mix_Chunk ** sounds){
*\brief Libère la mémoire
*\param missiles liste de missiles
*\param asteroides liste d'asteroides
*\param explosions liste d'explosions
*\param fonts Les polices
*\param thread Le thread d'envoi de score
*\param sounds liste des sons
*/
static void myFrees(Missile ** missiles, Asteroid ** asteroides, Explosion ** explosions, TTF_Font ** fonts, SDL_Thread ** thread, Mix_Chunk ** sounds){

	if(*missiles)
		free(*missiles);

	if(*asteroides)
		free(*asteroides);

	if(*explosions)
		free(*explosions);

	for(int i=0; i<NB_ASTEROID_FONTS; i++)
		if(fonts[i]){
			TTF_CloseFont(fonts[i]);
			fonts[i] = NULL;
		}

	for(int i=0; i<NB_ASTEROID_SOUNDS; i++)
		if(sounds[i]){
			Mix_FreeChunk(sounds[i]);
			sounds[i] = NULL;
		}

	if(*thread){
		SDL_WaitThread(*thread, NULL);
		*thread = NULL;
	}
}

/**
*\fn static void drawHelpText(SDL_Renderer *renderer, SDL_Texture *flecheTexture,int hardcore)
*\brief Affiche les commandes et explications des bonus
*\param renderer Le renderer où afficher
*\param flecheTexture La texture des flèches directionelles
*\param hardcore mode de jeu hardcore ou non
*/
static void drawHelpText(SDL_Renderer *renderer, SDL_Texture *flecheTexture,int hardcore){

	//direction
	SDL_SetTextureColorMod(flecheTexture, SCORE_COLOR.r, SCORE_COLOR.g-20, SCORE_COLOR.b-20);
	SDL_RenderCopyEx(renderer, flecheTexture, &FLECHE_SRC, &FLECHE_DEST[0], 90, NULL, SDL_FLIP_NONE);
	SDL_RenderCopyEx(renderer, flecheTexture, &FLECHE_SRC, &FLECHE_DEST[1], 0, NULL, SDL_FLIP_NONE);
	SDL_RenderCopyEx(renderer, flecheTexture, &FLECHE_SRC, &FLECHE_DEST[2], 0, NULL, SDL_FLIP_HORIZONTAL);

	if(!hardcore){
		//tab
		SDL_SetTextureColorMod(flecheTexture, GEM_COLORS[0].r, GEM_COLORS[0].g, GEM_COLORS[0].b);
		SDL_RenderCopyEx(renderer, flecheTexture, &FLECHE_SRC, &FLECHE_DEST[3], 0, NULL, SDL_FLIP_NONE);
		SDL_RenderCopyEx(renderer, flecheTexture, &FLECHE_SRC, &FLECHE_DEST[4], 0, NULL, SDL_FLIP_HORIZONTAL);

		SDL_SetTextureColorMod(flecheTexture, SCORE_COLOR.r, SCORE_COLOR.g, SCORE_COLOR.b);
		//bomb
		SDL_RenderCopyEx(renderer, flecheTexture, &FLECHE_SRC, &FLECHE_DEST[5], -90, NULL, SDL_FLIP_NONE);

	}

}

extern int updateEnded;
/**
*\fn int asteroid( SDL_Renderer *renderer ,int highscore, int WinWidth, int WinHeight, char *token, int hardcore, SDL_Texture ** textures, int fullscreen)
*\brief La fonction principale : Lance et fait tourner le jeu asteroid
*\param renderer Le renderer où afficher
*\param highscore Le meilleur score fait par le joueur
*\param WinWidth Largeur de la fenêtre
*\param WinHeight Hauteur de la fenêtre
*\param token Le token du joueur pour les requêtes
*\param hardcore Le niveau de difficulté du jeu
*\param textures tableau de textures
*\param fullscreen pleine écran
*\return 0 en cas de retour normal
*/
int asteroid(SDL_Renderer * renderer, int highscore, int WinWidth, int WinHeight, char *token, int hardcore, SDL_Texture ** textures, int fullscreen){

/////////////////////
/// MISE EN PLACE ///``
/////////////////////
	asteroidInit();

	float ratioWindowSize = (float)WinWidth/BASE_WINDOW_W;

	SDL_Rect playgroundView = {HUD_W*ratioWindowSize, HUD_H*ratioWindowSize, PLAYGROUND_SIZE_W*ratioWindowSize, PLAYGROUND_SIZE_H*ratioWindowSize};
	SDL_RenderSetViewport(renderer, &playgroundView);
	SDL_RenderSetScale(renderer, ratioWindowSize, ratioWindowSize);
	SDL_SetRenderDrawColor(renderer, 3, 22, 34, 255);
	SDL_RenderClear(renderer);
	////////////
	/// Vars ///`
	////////////

	float xM,yM;
	SDL_Point spawn_laser;

	int w, h;
	SDL_GetRendererOutputSize(renderer, &w, &h);
	//Le thread qui sera utiliser
	SDL_Thread *thread = NULL;
	char buffer[10];
	EnvoiScore envoiScore;
	updateEnded = SDL_FALSE;
	int retour = EXIT_FAILURE;
	int frameRetour = 0;
	int frame_anim_loading = 0;

	//Textures
	SDL_SetTextureColorMod(textures[A_HUD], HUD_COLOR.r, HUD_COLOR.g, HUD_COLOR.b);


	//fonts
	TTF_Font* fonts[NB_ASTEROID_FONTS];

	for(int i=0; i< NB_ASTEROID_FONTS; i++){
		 fonts[i] = TTF_OpenFont(DIR_FONTS_ASTEROID[i], OPEN_FONT_SIZE);
		 if( fonts[i] == NULL ){
			printf("Erreur lors de la creation de font %s %s",DIR_FONTS_ASTEROID[i], TTF_GetError());
			return EXIT_FAILURE;
		}
	}

	//Sounds
	Mix_Chunk* sounds[NB_ASTEROID_SOUNDS];

	for(int i=0; i< NB_ASTEROID_SOUNDS; i++){
		 sounds[i] = Mix_LoadWAV(DIR_SOUNDS_ASTEROID[i]);
		 if( sounds[i] == NULL ){
			printf("Erreur lors de la creation de son %s %s", DIR_SOUNDS_ASTEROID[i], Mix_GetError());
			return EXIT_FAILURE;
		}
	}

	for(int i=0; i<3; i++)
		Mix_PlayChannel(9+NB_CHANNEL_EXPLOSION + i, sounds[SOUND_THRUST], -1);

	if(Mix_GroupChannels(9,9+NB_CHANNEL_EXPLOSION-1,1) != NB_CHANNEL_EXPLOSION){
		printf("couldnt group channels \n" );
	}


	//Keyboard
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);

	//malloc
	Missile * missiles=malloc(sizeof(Missile));
	Asteroid * asteroides=malloc(sizeof(Asteroid));
	Explosion * explosions = malloc(sizeof(Explosion));

	int firstframe = SDL_TRUE;
	while(1){
		////////////
		/// Vars ///`
		////////////
		//Score
		ScoreTotal score = {0, 0, 0};
		long keys[4];
		initialisationConstantHashage(keys);
		long long score_hash = hashage(score.score, keys);

		//Time
		unsigned int lastTime = 0, currentTime;
		long int frame=0;
		int frame_apparition_asteroid= 0;
		float vitesse_spawn = VITESSE_SPAWN_INIT;
		int frame_2asteroid= FRAME_INIT_SPAWN;
		int frame_vague=FRAME_VAGUE_INIT/2;
		int frame_entre_vague=FRAME_VAGUE_INIT;

		//VAISSEAU
	 	Vaiss vaisseau=
		{
			PLAYGROUND_SIZE_W/2, //coord x
			PLAYGROUND_SIZE_H/2, //coord y
			BASE_ANGLE,
			0, // frame recharge
			FREQUENCE_BASE, // temps recharge
			1, // nb tir
			0, //bouclier
			0, // id missile
			BASE_VITESSE_MISSILE,
			BASE_DEGAT_MISSILE,
			0, //frame turn left
			0, //frame turn right
			0, //frame_thrust
			0  //frame_explo
		};
		Vector2f accelerate={0,0};
		SDL_SetTextureAlphaMod(textures[A_VAISS], 255);
		SDL_SetTextureAlphaMod(textures[A_GEM], 255);
		SDL_SetTextureAlphaMod(textures[A_THRUST], 255);
		//MISSILES

		//sound
		for(int i=0; i<NB_ASTEROID_SOUNDS;i++)
			Mix_VolumeChunk(sounds[i], SOUND_VOLUMES[i]);

		int nb_missiles=0;
		float munitions[NB_MISSILES] = {1,0,0,0,0};
		//float munitions[NB_MISSILES] = {1,1,1,1,1};

		//ASTEROID
		int nb_asteroid=0;

		//BONUS
		int nbBombeNucleaire = 1;
		int rdyToBomb = SDL_TRUE;
		int rdyToReloadBomb = SDL_TRUE;
		int rdyToTab = SDL_TRUE;
		int rdyToSpace = SDL_TRUE;
		int rdyToUp = SDL_FALSE;
		int rdyToLeft = SDL_FALSE;
		int rdyToRight = SDL_FALSE;

		//DIFFICULTE
		float difficulte =START_DIFFICULTE;

		//mouse
		SDL_Point mouseCoor;

        int laserSoundOn = SDL_FALSE;
		int nb_explosions = 0;


		//anims
		int frameLaser = 0;
		TextBonus textsBonus[MAX_TEXT_BONUS];
		Roue roue ={0,0,0};
		Jauge jauge = {0, GEM_COLORS[0], munitions[0], 0};
		int frameAnimBomb = 0;

		//Vaisseau
		int turn;

	//	SDL_Rect vaisseau = {500,500,20,20};
		///////////////////////
		/// Initialize vars ///`
		///////////////////////
		initText(textsBonus);

		if(fullscreen)
        	SDL_RenderSetViewport(renderer, &playgroundView);
	/////////////////////
	/// BOUCLE DU JEU ///``
	/////////////////////
		int done = 0;
		while( 1 ){
			if(firstframe){
				done = SDL_TRUE;
				firstframe = SDL_FALSE;
				vaisseau.frame_explo = FRAME_SHOW_HELP;
			}
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
				}
			}


		////////////////////////////
		// Handle Keyboard inputs //`
		////////////////////////////
			SDL_PumpEvents();
			if(keystate[SDL_SCANCODE_ESCAPE]){
				myFrees(&missiles, &asteroides, &explosions, fonts, &thread, sounds);
				SDL_RenderSetViewport(renderer, NULL);
				return 0;
			}
			else if(done && ( ( rdyToSpace && keystate[SDL_SCANCODE_SPACE] ) || ( vaisseau.frame_explo == FRAME_SHOW_HELP && ((rdyToUp && keystate[SDL_SCANCODE_UP]) || ( rdyToLeft && keystate[SDL_SCANCODE_LEFT] ) || ( rdyToRight && keystate[SDL_SCANCODE_RIGHT] ) ) ) ) ){
				break;
			}

			if(!keystate[SDL_SCANCODE_SPACE])
				rdyToSpace = SDL_TRUE;

			if(!keystate[SDL_SCANCODE_UP])
				rdyToUp = SDL_TRUE;
			else
				rdyToUp = SDL_FALSE;

			if(!keystate[SDL_SCANCODE_LEFT])
				rdyToLeft = SDL_TRUE;
			else
				rdyToLeft = SDL_FALSE;

			if(!keystate[SDL_SCANCODE_RIGHT])
				rdyToRight = SDL_TRUE;
			else
				rdyToRight = SDL_FALSE;

			if(!done){
				if(keystate[SDL_SCANCODE_TAB] && rdyToTab){
					int nb_tab = 0;
					int ancienMissile_id = vaisseau.missile_id;
					do{
						vaisseau.missile_id++;
						if(vaisseau.missile_id >= NB_MISSILES){
							vaisseau.missile_id = 0;
							nb_tab += NB_ROUE_EMPLACEMENTS - NB_MISSILES;
						}

						nb_tab++;
					}while(!munitions[vaisseau.missile_id]);

					rdyToTab = SDL_FALSE;

					if(ancienMissile_id != vaisseau.missile_id){
						roue.rota_dest -= nb_tab * 360./NB_ROUE_EMPLACEMENTS;
						roue.frame = FRAME_ROTA_ROUE;
						jauge.frame = FRAME_ROTA_ROUE;
						jauge.frameAmmo = FRAME_AMMO;
					}
					else if(!roue.frame){
						roue.rota_dest = BLOCKING_ANIM;
						roue.frame = FRAME_ROTA_ROUE;
						jauge.frame = FRAME_ROTA_ROUE;
						jauge.frameAmmo = FRAME_AMMO;
					}

				}
				else if (!keystate[SDL_SCANCODE_TAB]){
					rdyToTab = SDL_TRUE;
				}


				if (keystate[SDL_SCANCODE_LEFT] && keystate[SDL_SCANCODE_RIGHT])
					turn = BOTH;
				else if( keystate[SDL_SCANCODE_LEFT])
					turn = LEFT;
				else if( keystate[SDL_SCANCODE_RIGHT] )
					turn = RIGHT;


				if(keystate[SDL_SCANCODE_DOWN] && !hardcore){
					if( nbBombeNucleaire && rdyToBomb){
						frame_2asteroid = FRAME_BOMBE_NUCLEAIRE;
						nbBombeNucleaire--;
						frameAnimBomb = FRAME_ANIM_BOMB;

						rdyToReloadBomb = SDL_FALSE;
						rdyToBomb = SDL_FALSE;

						Mix_PlayChannel(8, sounds[BIG_EXPLO], 0);
					}
				}
				else if( rdyToReloadBomb ) {
					rdyToBomb = SDL_TRUE;
				}

				//sound thrust
				Mix_Volume(9+NB_CHANNEL_EXPLOSION, 7* RATIO_ACCEL[vaisseau.frame_thrust]);
				Mix_Volume(9+NB_CHANNEL_EXPLOSION + 1, 5 * RATIO_TURN[vaisseau.frame_turn_left]);
				Mix_Volume(9+NB_CHANNEL_EXPLOSION + 2, 5 * RATIO_TURN[vaisseau.frame_turn_right]);

				if(!laserSoundOn && keystate[SDL_SCANCODE_SPACE] && vaisseau.missile_id == SHOT_LASER){
					laserSoundOn = SDL_TRUE;
					Mix_Volume(5,120);
					Mix_PlayChannel(5, sounds[SOUND_LASER_BEAM],-1);//, TIME_FADE_IN);
				}
				if(laserSoundOn && (!keystate[SDL_SCANCODE_SPACE] || vaisseau.missile_id != SHOT_LASER)){
					laserSoundOn = SDL_FALSE;
					Mix_Volume(5,40);
					Mix_HaltChannel(5);//, TIME_FADE_OUT);
				}
			}
			else {
				Mix_HaltChannel(5);
				Mix_Volume(9+NB_CHANNEL_EXPLOSION, 0);
				Mix_Volume(9+NB_CHANNEL_EXPLOSION + 1, 0);
				Mix_Volume(9+NB_CHANNEL_EXPLOSION + 2, 0);
			}






		//////////////
		// Gameplay //`
		//////////////
		//hardcore
		if(hardcore){
			for(int i=0; i<nb_asteroid;i++){
					if((asteroides[i].x < -asteroides[i].taille && asteroides[i].cote_spawn == 2) || (asteroides[i].y < -asteroides[i].taille && asteroides[i].cote_spawn == 3) || (asteroides[i].x - asteroides[i].taille > PLAYGROUND_SIZE_W && asteroides[i].cote_spawn == 0) || (asteroides[i].y - asteroides[i].taille > PLAYGROUND_SIZE_H && asteroides[i].cote_spawn == 1) ){

						detruire_asteroid(&asteroides,&nb_asteroid,i,&vaisseau,SDL_TRUE, &score, &nbBombeNucleaire, 0, textsBonus, munitions, keys, &score_hash, sounds[SOUND_BONUS], NULL, done);
					}
			}
		}

			accelerate.x/=DECELERATION + BONUS_DEAD_DECELERATION*done;
		    accelerate.y/=DECELERATION + BONUS_DEAD_DECELERATION*done;

			if(!done){
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
			}

			move(&vaisseau,accelerate);

			if(keystate[SDL_SCANCODE_SPACE] && !hardcore){

				if(!done && vaisseau.frame_recharge == 0 && munitions[vaisseau.missile_id]){
					if(vaisseau.missile_id != SHOT_LASER){
						tirer(&vaisseau,&missiles,&nb_missiles, sounds[SOUND_SHOOT + vaisseau.missile_id]);
					}
					munitions[vaisseau.missile_id] -= MUNITIONS_USAGE[vaisseau.missile_id] * MUNITIONS_USAGE_MULTIPLE[vaisseau.nb_tir-1];
					if(munitions[vaisseau.missile_id] <= 0){
						munitions[vaisseau.missile_id] = 0;

						roue.rota_dest -= (NB_MISSILES-vaisseau.missile_id + (NB_ROUE_EMPLACEMENTS-NB_MISSILES)) * 360./NB_ROUE_EMPLACEMENTS;
						roue.frame = FRAME_ROTA_ROUE;
						jauge.frame = FRAME_ROTA_ROUE;

						vaisseau.missile_id = SHOT_NORMAL;
					}

					jauge.frameAmmo = FRAME_AMMO;
				}
				rdyToSpace=SDL_FALSE;
			}




			for(int i=0;i<nb_missiles; i++){
				mouvement_tir(renderer, &missiles[i], asteroides, nb_asteroid);
			}
			for(int i=0;i<nb_asteroid; i++){
				mouvement_asteroid(&asteroides[i], hardcore);
			}
		///////////////////
		// Check hitboxs //`
		///////////////////

		spawn_laser=(SDL_Point){0, RAYON_VAISS};
    xM = spawn_laser.x;
    yM = spawn_laser.y;
    spawn_laser.x = xM*cos(vaisseau.angle-PI/2) - yM * sin(vaisseau.angle-PI/2) ;
    spawn_laser.y = xM * sin(vaisseau.angle-PI/2) + yM * cos(vaisseau.angle-PI/2) ;
    collision_mur(&vaisseau.x,&vaisseau.y, vaisseau.x + spawn_laser.x, vaisseau.y + spawn_laser.y, 0,hardcore);


			for(int i=0;i<nb_asteroid;i++){
				float angle_touche = vaisseau.angle;
				if(!hardcore){
					collision_mur(&asteroides[i].x,&asteroides[i].y, asteroides[i].x, asteroides[i].y, asteroides[i].taille,hardcore);
				}


				//hitbox laser
				if(!done){
					if(vaisseau.missile_id == SHOT_LASER && keystate[SDL_SCANCODE_SPACE] && munitions[vaisseau.missile_id]){
						if( laser_touche(asteroides[i], vaisseau) ){
							Mix_PlayChannel(7, sounds[SOUND_HIT], 0);
							asteroides[i].pv -= DEGAT_MISSILES[SHOT_LASER] * (vaisseau.degat_missile + RATIO_DMG_UP_LASER*((vaisseau.nb_tir/(float)BASE_DEGAT_MISSILE - 1) + (vaisseau.vitesse_missile/(float)BASE_VITESSE_MISSILE - 1) + (vaisseau.temps_recharge/(float)FREQUENCE_BASE -1)));
							asteroides[i].frame_hit = FRAME_HIT_ANIM;

							if(asteroides[i].frozen >=2)
								asteroides[i].pv = DEAD_FROZEN;
						}
					}
				}

				if(asteroides[i].pv > 0){
					int i_missile=asteroid_touche(asteroides[i],missiles,nb_missiles);
					if(i_missile!=-1){
						Mix_PlayChannel(7, sounds[SOUND_HIT],0);
						asteroides[i].pv-=missiles[i_missile].degat;
						asteroides[i].frame_hit = FRAME_HIT_ANIM;


						if(asteroides[i].frozen >=2)
							asteroides[i].pv = DEAD_FROZEN;

						if(missiles[i_missile].id == SHOT_GLACE)
							asteroides[i].frozen++;


						explosions = realloc(explosions, ++nb_explosions * sizeof(Explosion));
						explosions[nb_explosions-1].id = EXPLO_MISSILE;
						explosions[nb_explosions-1].x = missiles[i_missile].x - TAILLE_EXPLOSIONS[missiles[i_missile].id]/2;
						explosions[nb_explosions-1].y = missiles[i_missile].y - TAILLE_EXPLOSIONS[missiles[i_missile].id]/2;
						explosions[nb_explosions-1].frame = FRAME_EXPLOSIONS[explosions[nb_explosions-1].id];
						explosions[nb_explosions-1].taille = BASE_TAILLE_EXPLOSION * TAILLE_EXPLOSIONS[missiles[i_missile].id];

						angle_touche = missiles[i_missile].angle;
						decaler_gauche_m(missiles, nb_missiles, i_missile);
						(nb_missiles)--;
						if(nb_missiles !=0){
								missiles=realloc(missiles,sizeof(Missile)*(nb_missiles));
						}

					}
				}


				if(asteroides[i].pv<=0){

					explosions = realloc(explosions, ++nb_explosions * sizeof(Explosion));
					explosions[nb_explosions-1].id = asteroides[i].pv == DEAD_FROZEN ? EXPLO_GLACE : EXPLO_ASTE;
					explosions[nb_explosions-1].taille = 2*asteroides[i].taille * RATIO_ASTEROID_EXPLO_SIZE;
					explosions[nb_explosions-1].x = asteroides[i].x - explosions[nb_explosions-1].taille/2;
					explosions[nb_explosions-1].y = asteroides[i].y - explosions[nb_explosions-1].taille/2;
					explosions[nb_explosions-1].frame = FRAME_EXPLOSIONS[explosions[nb_explosions-1].id];

					int retour=0;
					if(asteroides[i].pv == DEAD_FROZEN)
						retour = detruire_asteroid(&asteroides,&nb_asteroid,i,&vaisseau,SDL_TRUE, &score, &nbBombeNucleaire, angle_touche, textsBonus, munitions, keys, &score_hash, sounds[SOUND_BONUS], sounds[SOUND_ICE_EXPLO], done);
					else
						retour = detruire_asteroid(&asteroides,&nb_asteroid,i,&vaisseau,SDL_FALSE, &score, &nbBombeNucleaire, angle_touche, textsBonus, munitions, keys, &score_hash, sounds[SOUND_BONUS], sounds[SOUND_EXPLO], done);

					if(retour == HACKED){
						myFrees(&missiles, &asteroides, &explosions, fonts, &thread, sounds);
						SDL_RenderSetViewport(renderer, NULL);
						return HACKED;
					}
				}

			}

			int i_touche=vaisseau_touche(vaisseau,asteroides,nb_asteroid);
			if(!done && i_touche != -1){
				if(vaisseau.bouclier){
					vaisseau.bouclier=0;

					explosions = realloc(explosions, ++nb_explosions * sizeof(Explosion));
					explosions[nb_explosions-1].id = asteroides[i_touche].frozen >= 2 ? EXPLO_GLACE : EXPLO_ASTE;
					explosions[nb_explosions-1].taille = 2*asteroides[i_touche].taille * RATIO_ASTEROID_EXPLO_SIZE;
					explosions[nb_explosions-1].x = asteroides[i_touche].x - explosions[nb_explosions-1].taille/2;
					explosions[nb_explosions-1].y = asteroides[i_touche].y - explosions[nb_explosions-1].taille/2;
					explosions[nb_explosions-1].frame = FRAME_EXPLOSIONS[explosions[nb_explosions-1].id];

					if( detruire_asteroid(&asteroides,&nb_asteroid,i_touche, &vaisseau,SDL_TRUE,&score, &nbBombeNucleaire, vaisseau.angle, textsBonus, munitions, keys, &score_hash, sounds[SOUND_BONUS],asteroides[i_touche].frozen >= 2 ? sounds[SOUND_ICE_EXPLO] : sounds[SOUND_EXPLO], done) == HACKED){
						myFrees(&missiles, &asteroides, &explosions, fonts, &thread, sounds);
						SDL_RenderSetViewport(renderer, NULL);
						return HACKED;
					}
				}
				else{
					int channel=Mix_GroupAvailable(1);
					if (channel==-1) {
						channel = Mix_GroupOldest(1);;
						if (channel==-1)
							channel = 8;
					}

					Mix_PlayChannel(channel, sounds[SOUND_EXPLO], 0);
					if(score_hash == hashage(score.score, keys)){

						frame_anim_loading = 0;
						//On atttend si jamais un envoi est en cours
						if(thread){
	                        SDL_WaitThread(thread, &retour);
	                        if( retour == EXIT_FAILURE){
	                            frameRetour = -2*FRAME_ANIM_RETOUR;
	                        }
	                        else{
	                             frameRetour = FRAME_ANIM_RETOUR;
	                        }
	                        thread = NULL;
						}


						// CONVERTIR SCORE EN TEXT

						sprintf(buffer,"%d",score.score);

						if(hardcore)
							envoiScore = (EnvoiScore){"3", buffer, token};
						//updateScore("5",buffer,token);
						else
							envoiScore = (EnvoiScore){"10", buffer, token};
						//updateScore("8",buffer,token);

						printf("CONNEXION...\n" );

						thread = SDL_CreateThread(  (int(*)(void*))updateScore, NULL, &envoiScore );
						done = 1;
						vaisseau.frame_explo = FRAME_EXPLOSIONS[EXPLO_ASTE];
						Mix_PlayChannel(9+NB_CHANNEL_EXPLOSION+3, sounds[SOUND_GAMEOVER],0);
					}
					else{
						myFrees(&missiles, &asteroides, &explosions, fonts, &thread, sounds);
						SDL_RenderSetViewport(renderer, NULL);
						return HACKED;
					}

				}
			}

			for(int i=0;i<nb_missiles;i++){
					collision_mur(&missiles[i].x,&missiles[i].y, missiles[i].x, missiles[i].y, RAYON_MISSILES[missiles[i].id],hardcore);
			}


		//////////
		// Draw //`
		//////////

			SDL_RenderCopy(renderer, textures[A_BACKGROUND], NULL, NULL);



			for(int i=0; i<nb_missiles; i++){
				afficher_tir(renderer, missiles[i], textures[A_BULLET]);
			}



			afficher_vaisseau(vaisseau,renderer,textures[A_VAISS],textures[A_GEM],textures[A_THRUST]);




			if(!done && vaisseau.missile_id == SHOT_LASER && keystate[SDL_SCANCODE_SPACE] && munitions[vaisseau.missile_id]){
				afficher_laser(renderer,textures[A_LASER], vaisseau, frameLaser, asteroides[0]);
				frameLaser++;
				accelerate.x -= LASER_ACCEL * cos(vaisseau.angle);
				accelerate.y -= LASER_ACCEL * sin(vaisseau.angle);
			}
			else{
				frameLaser = 0;
			}
			if(!hardcore){
				if(!done && (frame_apparition_asteroid==0 || nb_asteroid <= (frame/FRAME_APPARITION_ASTEROID))&& frame_2asteroid == 0 ){
					spawn_asteroid(vaisseau,&asteroides,&nb_asteroid,difficulte, munitions, hardcore);
					frame_2asteroid=FRAME_2ASTEROID;
					frame_apparition_asteroid=vitesse_spawn;
					rdyToReloadBomb = SDL_TRUE;
				}
			}
			else if( !done && frame_vague == 0){
				frame_vague=frame_entre_vague;
				for(int i=0;i<(int)roundf((difficulte+20)/4.57);i++){
					spawn_asteroid(vaisseau,&asteroides,&nb_asteroid,difficulte, munitions, hardcore);
				}
			}
			//int a = asteroid_plus_proche(renderer, asteroides, nb_asteroid, missiles[0], NULL);
			for(int j=0;j<nb_asteroid;j++){
				afficher_asteroid(asteroides[j],renderer, textures[A_ASTEROID], textures[A_FISSURE], textures[A_BONUS], textures[A_GLACE],hardcore);
			}

			for(int i=0; i<nb_explosions; i++){
				afficher_explosion(renderer, explosions[i], textures[A_EXPLO_MISSILE+explosions[i].id]);
			}


	        afficher_text_bonus(renderer, textsBonus, fonts[FONT_BONUS]);

			if(frameAnimBomb){
				if( FRAME_ANIM_BOMB - frameAnimBomb == FRAME_KILL_ASTERO_BOMB)
					if(useNuclear(&asteroides, &nb_asteroid, &explosions, &nb_explosions, &score, keys, &score_hash, done, sounds[SOUND_EXPLO]) == HACKED){
						myFrees(&missiles, &asteroides, &explosions, fonts, &thread, sounds);
						SDL_RenderSetViewport(renderer, NULL);
						return HACKED;
					}

				SDL_Rect srcBomb = {SRC_BOMB.w * ((int)( (FRAME_ANIM_BOMB - frameAnimBomb)/((float)FRAME_ANIM_BOMB/NB_ANIM_BOMB) )%NB_COL_BOMB),
									SRC_BOMB.h * ((int)( (FRAME_ANIM_BOMB - frameAnimBomb)/((float)FRAME_ANIM_BOMB/NB_ANIM_BOMB) )/NB_COL_BOMB),
									SRC_BOMB.w,
									SRC_BOMB.h};

				SDL_RenderCopy(renderer, textures[A_BOMB], &srcBomb, NULL);

				frameAnimBomb --;
			}

			if(vaisseau.frame_explo > 0){
				int alpha = 500 * (float)vaisseau.frame_explo / FRAME_EXPLOSIONS[EXPLO_ASTE] - 245;
				if(alpha<0)
					alpha = 0;
				SDL_SetTextureAlphaMod(textures[A_VAISS], alpha);
				SDL_SetTextureAlphaMod(textures[A_GEM], alpha);
				SDL_SetTextureAlphaMod(textures[A_THRUST], alpha);
				afficher_vaisseau(vaisseau,renderer,textures[A_VAISS],textures[A_GEM],textures[A_THRUST]);
				afficher_explosion(renderer, (Explosion){vaisseau.x-1.5*RAYON_VAISS, vaisseau.y-1.5*RAYON_VAISS, vaisseau.frame_explo, 3*RAYON_VAISS, EXPLO_ASTE}, textures[A_EXPLO_ASTEROID]);
			}

			if(vaisseau.frame_explo == FRAME_DESTROY_ASTE){
				useNuclear(&asteroides, &nb_asteroid, &explosions, &nb_explosions, &score, keys, &score_hash, done, sounds[SOUND_EXPLO]);
			}

			if(done && vaisseau.frame_explo-- == FRAME_SHOW_HELP ){
				SDL_SetTextureAlphaMod(textures[A_VAISS], 255);
				SDL_SetTextureAlphaMod(textures[A_GEM], 255);
				SDL_SetTextureAlphaMod(textures[A_THRUST], 255);

				vaisseau= (Vaiss)
				{
					PLAYGROUND_SIZE_W/2, //coord x
					PLAYGROUND_SIZE_H/2, //coord y
					BASE_ANGLE,
					0, // frame recharge
					FREQUENCE_BASE, // temps recharge
					1, // nb tir
					0, //bouclier
					0, // id missile
					BASE_VITESSE_MISSILE,
					BASE_DEGAT_MISSILE,
					0, //frame turn left
					0, //frame turn right
					0, // frame_thrust
					FRAME_SHOW_HELP//frame_explo
				};
				drawReplay(renderer, fonts[FONT_BONUS],0.7, PLAYGROUND_SIZE_W,PLAYGROUND_SIZE_H);

				drawHelpText(renderer, textures[A_ARROW],hardcore);
			}


			//hud
			SDL_RenderSetScale(renderer, 1, 1);
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderSetViewport(renderer, NULL);
			SDL_RenderCopy(renderer, textures[A_HUD], NULL, NULL);
			if(!hardcore){

				afficherJauge(renderer, textures[A_JAUGE], ratioWindowSize, jauge);
				afficherRoue(renderer,textures[A_ROUE], ratioWindowSize, munitions, roue, vaisseau.missile_id, jauge.color );
				afficherBombIcon(renderer,textures[A_BOMB_ICON], nbBombeNucleaire, ratioWindowSize);

			}



			afficherScoreTotal(renderer, fonts[FONT_SCORE], score.scoreShow, ratioWindowSize);

			if(thread && updateEnded){
				SDL_WaitThread(thread, &retour);
				thread = NULL;
				if(retour == EXIT_FAILURE){
					frameRetour = -2*FRAME_ANIM_RETOUR;
				}
				else{
					 frameRetour = FRAME_ANIM_RETOUR;
				}
			}
			else if(thread){
				afficherLoading(renderer, textures[A_LOADING], SCORE_COLOR, 0, 0, frame_anim_loading++, w , h, BASE_WINDOW_W*ratioWindowSize);
			}

			if(frameRetour){

				afficherRetour(renderer, textures[A_LOADING],fonts[FONT_SCORE], SCORE_COLOR, 0, 0, frameRetour, w, h, BASE_WINDOW_W*ratioWindowSize);
				if(frameRetour >0)
					frameRetour--;
				else
					frameRetour++;
			}


			drawQuit(renderer, fonts[FONT_BONUS],ratioWindowSize, SCORE_COLOR);

			//afficher
			SDL_RenderPresent(renderer);

			SDL_RenderSetViewport(renderer, &playgroundView);
			SDL_RenderSetScale(renderer, ratioWindowSize, ratioWindowSize);
		////////////////
		// Next frame //`
		////////////////
			if(frame_vague){
				frame_vague--;
			}
			if(score.frameToDest)
				score.scoreShow += (float)(score.score - score.scoreShow ) / ( score.frameToDest-- );

			for(int i=0; i<nb_asteroid; i++){
				if(asteroides[i].frame_hit)
					asteroides[i].frame_hit--;

				if(asteroides[i].frame_depart)
					asteroides[i].frame_depart--;
			}

			for(int i=0; i<nb_explosions; i++){
				explosions[i].frame--;

				if(!explosions[i].frame){
					decaler_gauche_e(explosions, nb_explosions, i);
					nb_explosions--;
					if(nb_explosions != 0){
						explosions=realloc(explosions,sizeof(Explosion)* nb_explosions);
					}
				}
			}

			for(int i=0; i<MAX_TEXT_BONUS; i++){
				if(textsBonus[i].frame)
					textsBonus[i].frame--;
			}

			if(roue.frame){
				if(roue.rota_dest != BLOCKING_ANIM)
					roue.rota += (roue.rota_dest - roue.rota) / roue.frame;
				else
					roue.rota += BLOCKING_ANIM_ANGLE[roue.frame-1];

				roue.frame--;
			}

			if(jauge.frame){
				jauge.color.r += (GEM_COLORS[vaisseau.missile_id].r - jauge.color.r) / jauge.frame;
				jauge.color.g += (GEM_COLORS[vaisseau.missile_id].g - jauge.color.g) / jauge.frame;
				jauge.color.b += (GEM_COLORS[vaisseau.missile_id].b - jauge.color.b) / jauge.frame;
				jauge.frame--;
			}

			if(jauge.frameAmmo){
				jauge.ammo += (munitions[vaisseau.missile_id] - jauge.ammo) / jauge.frameAmmo;
			}



			rotateAsteroides(asteroides, nb_asteroid);
			update_frame(&missiles,&nb_missiles,&vaisseau,&frame,&frame_apparition_asteroid,&vitesse_spawn,&frame_2asteroid);
			difficulte+=RATIO_DIFFICULTE_AUGMENT + difficulte * RATIO_DIFFICULTE_AUGMENT_MULTI;
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
	}


	//printf("Waw t'es nul, %d\n", score);
	return 0;
}
