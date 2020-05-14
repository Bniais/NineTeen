#include "../../include/communFunctions.h"
#include "../../define/define.h"
#include "../../include/hashage.h"
#include "../../include/libWeb.h"
#include "config.h"
#include <time.h>


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
//#include "../../include/SDL2/SDL2_gfxPrimitives.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


/*http://ressources.polytech-lille.fr/cours-algos-calcul-scientifique/progrcit.html*/
void it_coef_lagrange(int n,double x[NMAX],double f[NMAX],double d[NMAX])
{
 int i,j;
 for(i=0;i<=n;i++)
 {
  d[i]=f[i];
  for(j=0;j<=n;j++)
  if(j!=i) d[i]/=x[i]-x[j];
 }
}

double it_pol_lagrange(int n,double d[NMAX],double x[NMAX],double alpha)
{
 int i,j;
 double l,p;
 p=0;
 for(i=0;i<=n;i++)
 {
  l=d[i];
  for(j=0;j<=n;j++)
  if(j!=i) l*=alpha-x[j];
  p+=l;
 }
 return(p);
}

/* Ellipse-circle collision detection
 * http://yehar.com/blog/?p=2926
 * by Olli Niemitalo in 2012-08-06.
 * This work is placed in the public domain.
 */
//hitbox
#define maxIterations 5
double innerPolygonCoef[maxIterations+1];
double outerPolygonCoef[maxIterations+1];

int iterate(double x, double y, double c0x, double c0y, double c2x, double c2y, double rr, int collideRect) {
	if(collideRect)
		return SDL_TRUE;

    for (int t = 1; t <= maxIterations; t++) {
      double c1x = (c0x + c2x)*innerPolygonCoef[t];
      double c1y = (c0y + c2y)*innerPolygonCoef[t];
      double tx = x - c1x;
      double ty = y - c1y;
      if (tx*tx + ty*ty <= rr) {
	return SDL_TRUE;
      }
      double t2x = c2x - c1x;
      double t2y = c2y - c1y;
      if (tx*t2x + ty*t2y >= 0 && tx*t2x + ty*t2y <= t2x*t2x + t2y*t2y &&
	  (ty*t2x - tx*t2y >= 0 || rr*(t2x*t2x + t2y*t2y) >= (ty*t2x - tx*t2y)*(ty*t2x - tx*t2y))) {
	return SDL_TRUE;
      }
      double t0x = c0x - c1x;
      double t0y = c0y - c1y;
      if (tx*t0x + ty*t0y >= 0 && tx*t0x + ty*t0y <= t0x*t0x + t0y*t0y &&
	  (ty*t0x - tx*t0y <= 0 || rr*(t0x*t0x + t0y*t0y) >= (ty*t0x - tx*t0y)*(ty*t0x - tx*t0y))) {
	return SDL_TRUE;
      }
      double c3x = (c0x + c1x)*outerPolygonCoef[t];
      double c3y = (c0y + c1y)*outerPolygonCoef[t];
      if ((c3x-x)*(c3x-x) + (c3y-y)*(c3y-y) < rr) {
	c2x = c1x;
	c2y = c1y;
	continue;
      }
      double c4x = c1x - c3x + c1x;
      double c4y = c1y - c3y + c1y;
      if ((c4x-x)*(c4x-x) + (c4y-y)*(c4y-y) < rr) {
	c0x = c1x;
	c0y = c1y;
	continue;
      }
      double t3x = c3x - c1x;
      double t3y = c3y - c1y;
      if (ty*t3x - tx*t3y <= 0 || rr*(t3x*t3x + t3y*t3y) > (ty*t3x - tx*t3y)*(ty*t3x - tx*t3y)) {
	if (tx*t3x + ty*t3y > 0) {
	  if (fabs(tx*t3x + ty*t3y) <= t3x*t3x + t3y*t3y || (x-c3x)*(c0x-c3x) + (y-c3y)*(c0y-c3y) >= 0) {
	    c2x = c1x;
	    c2y = c1y;
	    continue;
	  }
	} else if (-(tx*t3x + ty*t3y) <= t3x*t3x + t3y*t3y || (x-c4x)*(c2x-c4x) + (y-c4y)*(c2y-c4y) >= 0) {
	  c0x = c1x;
	  c0y = c1y;
	  continue;
	}
      }
      return SDL_FALSE;
    }
    return SDL_FALSE; // Out of iterations so it is unsure if there was a collision. But have to return something.
}

// Test for collision between an ellipse of horizontal radius w0 and vertical radius h0 at (x0, y0) and
// an ellipse of horizontal radius w1 and vertical radius h1 at (x1, y1)
int collide(double x0, double y0, double w0, double h0, double x1, double y1, double w1, double h1, int collideRect) {

	double x = fabs(x1 - x0)*h1;
	double y = fabs(y1 - y0)*w1;
	w0 *= h1;
	h0 *= w1;
	double r = w1*h1;

	if (x*x + (h0 - y)*(h0 - y) <= r*r || (w0 - x)*(w0 - x) + y*y <= r*r || x*h0 + y*w0 <= w0*h0
			|| ((x*h0 + y*w0 - w0*h0)*(x*h0 + y*w0 - w0*h0) <= r*r*(w0*w0 + h0*h0) && x*w0 - y*h0 >= -h0*h0 && x*w0 - y*h0 <= w0*w0)) {
		return SDL_TRUE;
	} else {
		if ((x-w0)*(x-w0) + (y-h0)*(y-h0) <= r*r || (x <= w0 && y - r <= h0) || (y <= h0 && x - r <= w0)) {
			return iterate(x, y, w0, 0, 0, h0, r*r, collideRect);
		}
		return SDL_FALSE;
	}
}

// Test for collision between two ellipses, "0" and "1". Ellipse is at (x, y) with major or minor radius
  // vector (wx, wy) and the other major or minor radius perpendicular to that vector and hw times as long.
int collideOblique(double x0, double y0, double wx0, double wy0, double hw0,
	double x1, double y1, double wx1, double wy1, double hw1, int collideRect) {
	float rr = hw1*hw1*(wx1*wx1 + wy1*wy1)*(wx1*wx1 + wy1*wy1)*(wx1*wx1 + wy1*wy1);
	float x = hw1*wx1*(wy1*(y1 - y0) + wx1*(x1 - x0)) - wy1*(wx1*(y1 - y0) - wy1*(x1 - x0));
	float y = hw1*wy1*(wy1*(y1 - y0) + wx1*(x1 - x0)) + wx1*(wx1*(y1 - y0) - wy1*(x1 - x0));
	float temp = wx0;
	wx0 = hw1*wx1*(wy1*wy0 + wx1*wx0) - wy1*(wx1*wy0 - wy1*wx0);
	float temp2 = wy0;
	wy0 = hw1*wy1*(wy1*wy0 + wx1*temp) + wx1*(wx1*wy0 - wy1*temp);
	float hx0 = hw1*wx1*(wy1*(temp*hw0)-wx1*temp2*hw0)-wy1*(wx1*(temp*hw0)+wy1*temp2*hw0);
	float hy0 = hw1*wy1*(wy1*(temp*hw0)-wx1*temp2*hw0)+wx1*(wx1*(temp*hw0)+wy1*temp2*hw0);

	if (wx0*y - wy0*x < 0) {
		x = -x;
		y = -y;
	}

	if ((wx0 - x)*(wx0 - x) + (wy0 - y)*(wy0 - y) <= rr) {
		return SDL_TRUE;
	} else if ((wx0 + x)*(wx0 + x) + (wy0 + y)*(wy0 + y) <= rr) {
		return SDL_TRUE;
	} else if ((hx0 - x)*(hx0 - x) + (hy0 - y)*(hy0 - y) <= rr) {
		return SDL_TRUE;
	} else if ((hx0 + x)*(hx0 + x) + (hy0 + y)*(hy0 + y) <= rr) {
		return SDL_TRUE;
	} else if (x*(hy0 - wy0) + y*(wx0 - hx0) <= hy0*wx0 - hx0*wy0 && y*(wx0 + hx0) - x*(wy0 + hy0) <= hy0*wx0 - hx0*wy0) {
		return SDL_TRUE;
	} else if (x*(wx0-hx0) - y*(hy0-wy0) > hx0*(wx0-hx0) - hy0*(hy0-wy0)
			   && x*(wx0-hx0) - y*(hy0-wy0) < wx0*(wx0-hx0) - wy0*(hy0-wy0)
			   && (x*(hy0-wy0) + y*(wx0-hx0) - hy0*wx0 + hx0*wy0)*(x*(hy0-wy0) + y*(wx0-hx0) - hy0*wx0 + hx0*wy0)
			   <= rr*((wx0-hx0)*(wx0-hx0) + (wy0-hy0)*(wy0-hy0))) {
		return SDL_TRUE;
	} else if (x*(wx0+hx0) + y*(wy0+hy0) > -wx0*(wx0+hx0) - wy0*(wy0+hy0)
			   && x*(wx0+hx0) + y*(wy0+hy0) < hx0*(wx0+hx0) + hy0*(wy0+hy0)
			   && (y*(wx0+hx0) - x*(wy0+hy0) - hy0*wx0 + hx0*wy0)*(y*(wx0+hx0) - x*(wy0+hy0) - hy0*wx0 + hx0*wy0)
			   <= rr*((wx0+hx0)*(wx0+hx0) + (wy0+hy0)*(wy0+hy0))) {
		return SDL_TRUE;
	} else {
		if ((hx0-wx0 - x)*(hx0-wx0 - x) + (hy0-wy0 - y)*(hy0-wy0 - y) <= rr) {
			return iterate(x, y, hx0, hy0, -wx0, -wy0, rr, collideRect);
		} else if ((hx0+wx0 - x)*(hx0+wx0 - x) + (hy0+wy0 - y)*(hy0+wy0 - y) <= rr) {
			return iterate(x, y, wx0, wy0, hx0, hy0, rr, collideRect);
		} else if ((wx0-hx0 - x)*(wx0-hx0 - x) + (wy0-hy0 - y)*(wy0-hy0 - y) <= rr) {
			return iterate(x, y, -hx0, -hy0, wx0, wy0, rr, collideRect);
		} else if ((-wx0-hx0 - x)*(-wx0-hx0 - x) + (-wy0-hy0 - y)*(-wy0-hy0 - y) <= rr) {
			return iterate(x, y, -wx0, -wy0, -hx0, -hy0, rr, collideRect);
		} else if (wx0*y - wy0*x < wx0*hy0 - wy0*hx0 && fabs(hx0*y - hy0*x) < hy0*wx0 - hx0*wy0) {
			if (hx0*y - hy0*x > 0) {
				return iterate(x, y, hx0, hy0, -wx0, -wy0, rr, collideRect);
			}
			return iterate(x, y, wx0, wy0, hx0, hy0, rr, collideRect);
		} else if (wx0*x + wy0*y > wx0*(hx0-wx0) + wy0*(hy0-wy0) && wx0*x + wy0*y < wx0*(hx0+wx0) + wy0*(hy0+wy0)
	 			&& (wx0*y - wy0*x - hy0*wx0 + hx0*wy0)*(wx0*y - wy0*x - hy0*wx0 + hx0*wy0) < rr*(wx0*wx0 + wy0*wy0)) {
			if (wx0*x + wy0*y > wx0*hx0 + wy0*hy0) {
				return iterate(x, y, wx0, wy0, hx0, hy0, rr, collideRect);
			}
			return iterate(x, y, hx0, hy0, -wx0, -wy0, rr, collideRect);
		}
		else {
			if (hx0*y - hy0*x < 0) {
				x = -x;
				y = -y;
			}
			if (hx0*x + hy0*y > -hx0*(wx0+hx0) - hy0*(wy0+hy0) && hx0*x + hy0*y < hx0*(hx0-wx0) + hy0*(hy0-wy0)
					&& (hx0*y - hy0*x - hy0*wx0 + hx0*wy0)*(hx0*y - hy0*x - hy0*wx0 + hx0*wy0) < rr*(hx0*hx0 + hy0*hy0)) {
				if (hx0*x + hy0*y > -hx0*wx0 - hy0*wy0) {
					return iterate(x, y, hx0, hy0, -wx0, -wy0, rr, collideRect);
				}
				return iterate(x, y, -wx0, -wy0, -hx0, -hy0, rr, collideRect);
			}
			return SDL_FALSE;
		}
	}
}


void drawShip(SDL_Renderer *renderer, Ship ship, SDL_Texture **textures){
	SDL_Rect srcShip = SHIP_SRC;
	srcShip.y += (ship.dir%3) * srcShip.h;
	SDL_Rect srcFlame = srcShip;
	srcShip.x += ship.form * srcShip.w;


	SDL_Rect dest = SHIP_SRC;
	dest.w/=RATIO_SIZE_SHIP;
	dest.h/=RATIO_SIZE_SHIP;
	dest.x+=ship.x;
	dest.y+=ship.y;


	for(int i=0; i<3; i++){
		if((i == 1 &&  ship.dir <6) || (i == 0 && ship.dir <6 && ship.dir%3 != 1 && ship.dir>=3) || (i == 2 && ship.dir <6 && ship.dir%3 != 2 && ship.dir>=3 ))
			SDL_RenderCopy(renderer, textures[SH_FLAME], &srcFlame, &dest);
		srcFlame.x+= srcFlame.w;
	}


	SDL_RenderCopy(renderer, textures[SH_SHIP], &srcShip, &dest);

	SDL_Rect destWeapon = WEAPON_DEST[0];
	for(int i=0; i<=ship.nbWeapon; i++){
		destWeapon = WEAPON_DEST[WEAPON_DISPOSITION[ship.nbWeapon][i]-1];
		destWeapon.x += DECALLAGE_TURN[ship.dir%3][WEAPON_DISPOSITION[ship.nbWeapon][i]-1];
		destWeapon.x /= RATIO_SIZE_SHIP;
		destWeapon.y /= RATIO_SIZE_SHIP;
		destWeapon.w /= RATIO_SIZE_SHIP;
		destWeapon.h /= RATIO_SIZE_SHIP;
		destWeapon.x += dest.x;
		destWeapon.y += dest.y;

		SDL_RenderCopy(renderer, textures[SH_WEAPON], &WEAPON_SRC, &destWeapon);
	}
}

void drawEnemy(SDL_Renderer *renderer, Enemy enemy, SDL_Texture **textures){
	SDL_Rect srcEnemy = ENEMY_SRC[enemy.id];
	SDL_Rect destEnemy = srcEnemy;
	destEnemy.w /= RATIO_SIZE_ENEMY[enemy.id];
	destEnemy.h /= RATIO_SIZE_ENEMY[enemy.id];
	destEnemy.x = enemy.x;
	destEnemy.y = enemy.y;

	if( enemy.id == BASE_ENEMY){
		SDL_RenderCopyEx(renderer, textures[SH_ENEMY_ROND + enemy.id], &srcEnemy, &destEnemy, enemy.rota, NULL, SDL_FLIP_NONE);
		if(enemy.frameHit){
			srcEnemy.y += srcEnemy.h;
			SDL_SetTextureAlphaMod(textures[SH_ENEMY_ROND + enemy.id], ALPHA_HIT);
			SDL_RenderCopyEx(renderer, textures[SH_ENEMY_ROND + enemy.id], &srcEnemy, &destEnemy, enemy.rota, NULL, SDL_FLIP_NONE);
			srcEnemy.y -= srcEnemy.h;
			SDL_SetTextureAlphaMod(textures[SH_ENEMY_ROND + enemy.id], 255);
		}

		srcEnemy.x += srcEnemy.w;
	}
	SDL_RenderCopy(renderer, textures[SH_ENEMY_ROND + enemy.id], &srcEnemy, &destEnemy );
	if(enemy.frameHit){
		SDL_SetTextureAlphaMod(textures[SH_ENEMY_ROND + enemy.id], ALPHA_HIT);
		srcEnemy.y += srcEnemy.h;
		SDL_RenderCopy(renderer, textures[SH_ENEMY_ROND + enemy.id], &srcEnemy, &destEnemy );
		SDL_SetTextureAlphaMod(textures[SH_ENEMY_ROND + enemy.id], 255);
	}
}


int shipHitEnemy(Ship ship, Enemy enemy){
	return (collideOblique(SHIP_HITBOX[ship.form].x + ship.x, SHIP_HITBOX[ship.form].y + ship.y, SHIP_HITBOX[ship.form].rx , 0, SHIP_HITBOX[ship.form].ry/SHIP_HITBOX[ship.form].rx,
			ENEMY_HITBOX[enemy.id].x + enemy.x, ENEMY_HITBOX[enemy.id].y + enemy.y, cos(enemy.rota) * ENEMY_HITBOX[enemy.id].rx, sin(enemy.rota) * ENEMY_HITBOX[enemy.id].rx, ENEMY_HITBOX[enemy.id].ry/ENEMY_HITBOX[enemy.id].rx, 0)
			||
			collideOblique(ENEMY_ADDITIONAL_HITBOX[enemy.id].x + enemy.x, ENEMY_ADDITIONAL_HITBOX[enemy.id].y + enemy.y, cos(enemy.rota) * ENEMY_ADDITIONAL_HITBOX[enemy.id].rx, sin(enemy.rota) * ENEMY_ADDITIONAL_HITBOX[enemy.id].rx, ENEMY_ADDITIONAL_HITBOX[enemy.id].ry/ENEMY_ADDITIONAL_HITBOX[enemy.id].rx,
				SHIP_HITBOX[ship.form].x + ship.x, SHIP_HITBOX[ship.form].y + ship.y,SHIP_HITBOX[ship.form].rx ,  0, SHIP_HITBOX[ship.form].ry/SHIP_HITBOX[ship.form].rx, 1)
			);

}

int missileHitEnemy(Missile missile, Enemy enemy){
	return (collideOblique(missile.x, missile.y, cos(missile.rota) * MISSILE_HITBOX[missile.id].rx , sin(missile.rota) * MISSILE_HITBOX[missile.id].rx,MISSILE_HITBOX[missile.id].ry/MISSILE_HITBOX[missile.id].rx,
			ENEMY_HITBOX[enemy.id].x + enemy.x, ENEMY_HITBOX[enemy.id].y + enemy.y, cos(enemy.rota * PI / 180) * ENEMY_HITBOX[enemy.id].rx, sin(enemy.rota * PI / 180) * ENEMY_HITBOX[enemy.id].rx, ENEMY_HITBOX[enemy.id].ry/ENEMY_HITBOX[enemy.id].rx, 0)
			||
			collideOblique(ENEMY_ADDITIONAL_HITBOX[enemy.id].x + enemy.x, ENEMY_ADDITIONAL_HITBOX[enemy.id].y + enemy.y, cos(enemy.rota) * ENEMY_ADDITIONAL_HITBOX[enemy.id].rx, sin(enemy.rota) * ENEMY_ADDITIONAL_HITBOX[enemy.id].rx, ENEMY_ADDITIONAL_HITBOX[enemy.id].ry/ENEMY_ADDITIONAL_HITBOX[enemy.id].rx,
			missile.x, missile.y, cos(missile.rota) * MISSILE_HITBOX[missile.id].rx , sin(missile.rota) * MISSILE_HITBOX[missile.id].rx, MISSILE_HITBOX[missile.id].ry/MISSILE_HITBOX[missile.id].rx, 1)
			);

}

int missileHitShip(Missile missile, Ship ship){
	return (collideOblique(missile.x, missile.y, cos(missile.rota) * MISSILE_HITBOX[missile.id].rx , sin(missile.rota) * MISSILE_HITBOX[missile.id].rx,MISSILE_HITBOX[missile.id].ry/MISSILE_HITBOX[missile.id].rx,
			SHIP_HITBOX[ship.form].x + ship.x, SHIP_HITBOX[ship.form].y + ship.y,SHIP_HITBOX[ship.form].rx ,  0, SHIP_HITBOX[ship.form].ry/SHIP_HITBOX[ship.form].rx, 0));
}

void fire(Ship *ship, Missile **allyMissiles, int *nbAllyMissiles){

	for(int i = 0; i <= ship->nbWeapon; i++){
		if(ship->weapons[i].frame_reload == 0){
			*allyMissiles = realloc(*allyMissiles, ++(*nbAllyMissiles)  *  sizeof(Missile));
			(*allyMissiles)[*nbAllyMissiles - 1].x = ship->x + (WEAPON_DEST[WEAPON_DISPOSITION[ship->nbWeapon][i]-1].x + WEAPON_DEST[WEAPON_DISPOSITION[ship->nbWeapon][i]-1].w/2. +DECALLAGE_TURN[ship->dir%3][WEAPON_DISPOSITION[ship->nbWeapon][i]-1])/RATIO_SIZE_SHIP;
			(*allyMissiles)[*nbAllyMissiles - 1].y = ship->y + (WEAPON_DEST[WEAPON_DISPOSITION[ship->nbWeapon][i]-1].y)/RATIO_SIZE_SHIP;
			(*allyMissiles)[*nbAllyMissiles - 1].id = ship->weapons[i].id;
			(*allyMissiles)[*nbAllyMissiles - 1].rota = 0;
			(*allyMissiles)[*nbAllyMissiles - 1].frame = 0;
			(*allyMissiles)[*nbAllyMissiles - 1].damage = 1;
			(*allyMissiles)[*nbAllyMissiles - 1].speed = MISSILE_SPEED[ship->weapons[i].id];

			ship->weapons[i].frame_reload = RELOAD_FRAME[ship->weapons[i].id];
		}
	}
}

void enemyFire(Enemy *enemy, Missile **enemyMissiles, int *nbEnemyMissiles){
	for(int i = 0; i <= enemy->nbWeapon; i++){
		if(enemy->weapons[i].frame_reload == 0){
			*enemyMissiles = realloc(*enemyMissiles, ++(*nbEnemyMissiles)  *  sizeof(Missile));
			(*enemyMissiles)[*nbEnemyMissiles - 1].x = enemy->x + WEAPON_DEST_ENEMY[enemy->id][i].x;
			(*enemyMissiles)[*nbEnemyMissiles - 1].y = enemy->y + WEAPON_DEST_ENEMY[enemy->id][i].y;
			(*enemyMissiles)[*nbEnemyMissiles - 1].id = enemy->weapons[i].id;
			(*enemyMissiles)[*nbEnemyMissiles - 1].rota = PI;
			if( ANGLE_ENEMY_FIRE[enemy->id][i])
				(*enemyMissiles)[*nbEnemyMissiles - 1].rota += ANGLE_ENEMY_FIRE[enemy->id][i] ? (randSign() * ((rand()%(int)(PRECISION_RAND_FLOAT*ANGLE_ENEMY_FIRE[enemy->id][i])) / PRECISION_RAND_FLOAT)) : 0;

			(*enemyMissiles)[*nbEnemyMissiles - 1].damage = 1;
			(*enemyMissiles)[*nbEnemyMissiles - 1].frame = 0;
			(*enemyMissiles)[*nbEnemyMissiles - 1].speed = MISSILE_SPEED_ENEMY[enemy->id][i];

			enemy->weapons[i].frame_reload = RELOAD_FRAME_ENEMY[enemy->id][i];
		}
	}
}

void drawMissile(SDL_Renderer *renderer, Missile missile, SDL_Texture *textureMissile){
	SDL_Rect dest = MISSILE_SRC[missile.id];
	dest.w *= RATIO_SIZE_MISSILE[missile.id];
	dest.h *= RATIO_SIZE_MISSILE[missile.id];
	dest.x = missile.x - MISSILE_CENTER[missile.id].x ;
	dest.y = missile.y - MISSILE_CENTER[missile.id].y ;

	SDL_Rect src = MISSILE_SRC[missile.id];
	if(missile.id == W_BASE_ENEMY && missile.frame%4 < 2){
		src.y += src.w;
	}


	SDL_RenderCopyEx(renderer, textureMissile, &src, &dest, missile.rota * 180/PI, &MISSILE_CENTER[missile.id], SDL_FLIP_NONE);

	SDL_SetRenderDrawColor(renderer, 0,255,0,255);
	SDL_RenderDrawPoint(renderer, missile.x, missile.y);
}

void drawBackground(SDL_Renderer *renderer, SDL_Texture *textureBackground, SDL_Rect *backgroundSrc, SDL_Rect *backgroundDest){
	for(int layer=0; layer<NB_BACKGROUND_LAYER; layer++){
		if(backgroundSrc[layer].y<0){
			int tmp = backgroundSrc[layer].y;
			backgroundSrc[layer].y = 0;
			backgroundDest[layer].y = -tmp;
			backgroundDest[layer].h = BACKGROUND_DEST.h + tmp;
			backgroundSrc[layer].h = BACKGROUND_SRC.h +tmp;

			SDL_RenderCopy(renderer, textureBackground, &backgroundSrc[layer], &backgroundDest[layer]);

			backgroundDest[layer].y = 0;
			backgroundDest[layer].h = BACKGROUND_DEST.h - backgroundDest[layer].h;

			backgroundSrc[layer].h = BACKGROUND_SRC.h - backgroundSrc[layer].h;
			backgroundSrc[layer].y = BACKGROUND_GLOBAL_SRC.h - backgroundSrc[layer].h;

			backgroundSrc[layer].x += backgroundSrc[layer].w;
			if(backgroundSrc[layer].x == (NB_BACKGROUND_LAYER * (layer+1) )*backgroundSrc[layer].w)
				backgroundSrc[layer].x =  (NB_BACKGROUND_LAYER * layer ) *backgroundSrc[layer].w;
			SDL_RenderCopy(renderer, textureBackground, &backgroundSrc[layer], &backgroundDest[layer]);

			backgroundSrc[layer].x -= backgroundSrc[layer].w;
			if(backgroundSrc[layer].x == (NB_BACKGROUND_LAYER*layer-1) * backgroundSrc[layer].w)
				backgroundSrc[layer].x = (NB_BACKGROUND_LAYER*(layer+1) -1) * backgroundSrc[layer].w;

			backgroundSrc[layer].y = tmp;
			backgroundDest[layer].h = BACKGROUND_DEST.h;
			backgroundSrc[layer].h = BACKGROUND_SRC.h;
		}
		else{
			backgroundDest[layer].y = 0;
			backgroundDest[layer].h = BACKGROUND_DEST.h;
			backgroundSrc[layer].h = BACKGROUND_SRC.h;
			SDL_RenderCopy(renderer, textureBackground, &backgroundSrc[layer], &backgroundDest[layer]);
		}
	}
}

void shiftEnemy(Enemy * enemies, int nbEnemy, int iEnemy){
	for(;iEnemy<nbEnemy-1;iEnemy++){
        enemies[iEnemy]=enemies[iEnemy+1];
    }
}

void shiftMissile(Missile * missiles, int nbMissile, int i){
	for(;i<nbMissile-1;i++){
        missiles[i]=missiles[i+1];
    }
}

void shiftExplosion(Explosion * explosions, int nbExplosions, int i){
	for(;i<nbExplosions-1;i++){
        explosions[i]=explosions[i+1];
    }
}

void drawExplosion(SDL_Renderer * renderer,Explosion explosion, SDL_Texture * explo_texture){
	SDL_Rect src = EXPLO_SRCS[explosion.id];
	src.x += (int)((FRAME_EXPLOSIONS[explosion.id] - explosion.frame) / ((float)FRAME_EXPLOSIONS[explosion.id] /NB_ANIM_EXPLOSIONS[explosion.id])) * src.w;
	SDL_Rect dest = {explosion.x, explosion.y, explosion.taille, explosion.taille};

	SDL_RenderCopy(renderer, explo_texture, &src, &dest);
}

void moveMissile(Missile *missile){
	missile->x += missile->speed * cos(missile->rota - PI/2);
	missile->y += missile->speed * sin(missile->rota - PI/2);
	(missile->frame)++;
}

void moveEnemy(Enemy **enemies, int * nbEnemy, int iEnemy){
	if((*enemies)[iEnemy].id == BASE_ENEMY && (*enemies)[iEnemy].frameWait == 0){
		float tmpX, tmpY, tmpD;
		while((*enemies)[iEnemy].dist < SPEED_ENEMY[(*enemies)[iEnemy].id]){
			tmpX = (*enemies)[iEnemy].x;
			tmpY = (*enemies)[iEnemy].y;
			(*enemies)[iEnemy].x += (*enemies)[iEnemy].dir * SPEED_DECOMPOSITION;
			(*enemies)[iEnemy].y = it_pol_lagrange(NMAX-1,(*enemies)[iEnemy].coefs,(*enemies)[iEnemy].abscisses,(*enemies)[iEnemy].x);

			tmpD =  sqrt(pow(tmpX - (*enemies)[iEnemy].x, 2) + pow(tmpY - (*enemies)[iEnemy].y, 2));
			(*enemies)[iEnemy].dist += tmpD;
		}
		(*enemies)[iEnemy].x -= (*enemies)[iEnemy].dir * (SPEED_DECOMPOSITION * ((*enemies)[iEnemy].dist - SPEED_ENEMY[(*enemies)[iEnemy].id])) / tmpD;
		(*enemies)[iEnemy].y = it_pol_lagrange(3,(*enemies)[iEnemy].coefs,(*enemies)[iEnemy].abscisses,(*enemies)[iEnemy].x);
		(*enemies)[iEnemy].dist = 0 ;


	}
	else if((*enemies)[iEnemy].frameWait == 0){
		(*enemies)[iEnemy].x += SPEED_ENEMY[(*enemies)[iEnemy].id];
	}

	if((*enemies)[iEnemy].x < BACKGROUND_DEST.x - ENEMY_SRC[(*enemies)[iEnemy].id].w / RATIO_SIZE_ENEMY[(*enemies)[iEnemy].id]  || (*enemies)[iEnemy].x > BACKGROUND_DEST.x + BACKGROUND_DEST.w){
		shiftEnemy(*enemies, *nbEnemy, iEnemy);
		(*nbEnemy)--;
		if(*nbEnemy !=0)
			*enemies=realloc(*enemies, sizeof(Enemy)* *nbEnemy );
	}
}

void spawnEnemy(Enemy ** enemies, int *nbEnemy, int id, int nbSpawn){
	return;
	*nbEnemy += nbSpawn;
	*enemies = realloc( *enemies, *nbEnemy * sizeof(Enemy));

	//Polynomial interpolation
	double x[NMAX];
	double f[NMAX];
	double d[NMAX];
	if(id == BASE_ENEMY){
		x[0] = BACKGROUND_DEST.x - ENEMY_SRC[BASE_ENEMY].w/RATIO_SIZE_ENEMY[BASE_ENEMY];
		f[0] = rand()%300+100;
		for(int i=1; i<NMAX; i++){
			x[i] = x[i-1] + (BACKGROUND_DEST.w + ENEMY_SRC[BASE_ENEMY].w/RATIO_SIZE_ENEMY[BASE_ENEMY])/3;
			f[i] = rand()%250+100;
		}
		//x[NMAX-1] = BACKGROUND_DEST.x + BACKGROUND_DEST.w + ENEMY_SRC[BASE_ENEMY].w/RATIO_SIZE_ENEMY[BASE_ENEMY];

		it_coef_lagrange(3,x,f,d);
	}

	int dir = randSign();

	for(int i = *nbEnemy-1; i> *nbEnemy - 1 - nbSpawn; i--){
		(*enemies)[i] = (Enemy){
			id, //id
			BACKGROUND_DEST.x - ENEMY_SRC[id].w/RATIO_SIZE_ENEMY[id], //x
			200, //y
			0, //rota
			ENEMY_HP[id], //hp
			0, // framehit
			0, //dist
			FRAME_MULTI_SPAWN[id] * (*nbEnemy-1 -i),//frameWait
			dir, //dir
			NB_WEAPON_ENEMY[id] //nbWeapon
		};
		printf("dir : %d\n", (*enemies)[i].dir);
		for(int j=0; j<= (*enemies)[i].nbWeapon; j++)
            (*enemies)[i].weapons[j] = WEAPONS_ENEMY[id][j];

		if(id == BASE_ENEMY){
			for(int j=0; j<NMAX; j++){
				(*enemies)[i].abscisses[j] = x[j];
				(*enemies)[i].coefs[j] = d[j];
			}
			(*enemies)[i].x = x[(*enemies)[i].dir == 1 ? 0: NMAX-1];
			(*enemies)[i].y = f[(*enemies)[i].dir == 1 ? 0: NMAX-1];
		}
	}
}

extern int updateEnded;
int shooter( SDL_Renderer *renderer ,int highscore, int WinWidth, int WinHeight, char *token, int hardcore, SDL_Texture ** textures){

// // // // // // //
// MISE EN PLACE   //``
// // // // // // //
	//myInit();
	for (int t = 0; t <= maxIterations; t++) {
		int numNodes = 4 << t;
		innerPolygonCoef[t] = 0.5/cos(4*acos(0.0)/numNodes);
		outerPolygonCoef[t] = 0.5/(cos(2*acos(0.0)/numNodes)*cos(2*acos(0.0)/numNodes));
	}

	float ratioWindowSize = (float)WinWidth/BASE_WINDOW_W;
	SDL_RenderSetScale(renderer, ratioWindowSize, ratioWindowSize);
	SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b, 255);
	SDL_RenderClear(renderer);

	SDL_Thread *thread = NULL;
	char buffer[10];
	EnvoiScore envoiScore;
	updateEnded = SDL_FALSE;
	int retour = EXIT_FAILURE;
	int frameRetour = 0;
	int frame_anim_loading = 0;

	SDL_Rect backgroundSrc[3] = {BACKGROUND_SRC, BACKGROUND_SRC, BACKGROUND_SRC};
	backgroundSrc[1].x += 3* backgroundSrc[1].w;
	backgroundSrc[2].x += 6* backgroundSrc[2].w;

	SDL_Rect backgroundDest[3] = {BACKGROUND_DEST, BACKGROUND_DEST, BACKGROUND_DEST};


	Missile * allyMissiles = malloc(sizeof(Missile));
	Missile * enemyMissiles = malloc(sizeof(Missile));

	Enemy *enemies = malloc(sizeof(Enemy));
	Explosion *explosions = malloc(sizeof(Explosion));
	//Fonts
	TTF_Font * font = TTF_OpenFont("../games/2_snake/Fonts/zorque.ttf", OPEN_FONT_SIZE);
	//audio



	SDL_SetRenderDrawColor(renderer, 0,0,255,255);

	int quit = SDL_FALSE;
	while(!quit){
   		 // // // //
		// Vars   //`
		 // // // //

		int gameOver = SDL_FALSE;

		//score
		ScoreTotal score = {0,0,0};

		//hash setup
		long keys[4];
		initialisationConstantHashage(keys);
		long long score_hash = hashage(score.score, keys);

		//Keyboard
		const Uint8 *keystate = SDL_GetKeyboardState(NULL);

		//sound

		//Time
		unsigned int lastTime = 0, currentTime;

		//mouse
		SDL_Point mouseCoor;

		//vaiss
		Ship ship = {
			BASE_WINDOW_W/2 - SHIP_HITBOX[0].x, //x
			BASE_WINDOW_H/2 - SHIP_HITBOX[0].y, //y
			0, //dir
			0, //form
			0, //nbWeapon
			{{W_BASE, 0},{W_BASE, 0},{W_BASE, 0},{W_BASE, 0},{W_BASE, 0}} //weapons
		};

		int nbEnemy = 0;
		 spawnEnemy(&enemies, &nbEnemy, 0, 5);
		spawnEnemy(&enemies, &nbEnemy, 1, 1);
		spawnEnemy(&enemies, &nbEnemy, 2, 1);

		/*enemies[0] = (Enemy){
			BASE_ENEMY, //id
			BACKGROUND_DEST.x, //abcise
			x[0], //x
			200, //y
			0, //rota
			10, //hp
			0, //nbWeapon
			{{W_BASE_ENEMY, 0},{NO_WEAPON, -1},{NO_WEAPON, -1},{NO_WEAPON, -1},{NO_WEAPON, -1},{NO_WEAPON, -1},{NO_WEAPON, -1}},
			0, // framehit
			//abcises
            //coefs
		};*/

		/*enemies[1] = (Enemy){
			LASER_ENEMY, //id
			BACKGROUND_DEST.x, //abcise
			BASE_WINDOW_W/2, //x
			400, //y
			0, //rota
			3, //hp
			3, //nbWeapon
			{{W_BASE_ENEMY, 0},{W_LASER_ENEMY, 0},{W_LASER_ENEMY, 0},{W_BASE_ENEMY, 0},{NO_WEAPON, -1},{NO_WEAPON, -1},{NO_WEAPON, -1}},
			0, // framehit
			//abcises
			//coefs
		};*/

		/*enemies[2] = (Enemy){
			BOSS_ENEMY, //id
			BACKGROUND_DEST.x, //abcise
			BASE_WINDOW_W/2, //x
			700, //y
			0, //rota
			5, //hp
			6, //nbWeapon
			{{W_BASE_ENEMY, 0},{W_BASE_ENEMY, 0},{W_BASE_ENEMY, 0},{W_LASER_ENEMY, 0},{W_BASE_ENEMY, 0},{W_BASE_ENEMY, 0},{W_BASE_ENEMY, 0}},
			0, // framehit
			//abcises
			//coefs
		};*/

		int nbAllyMissiles = 0;
		int nbEnemyMissiles = 0;
		int nbExplosions = 0;

		 // // // // // // // //
		//   Initialize vars   //
		 // // // // // // // //

	 // // // // // // //
	//   BOUCLE DU JEU  //``
	 // // // // // // //
	 	int frame =0;
		while( 1 ){
		// // // //
		// Events //`
		// // // //
			SDL_Event event;
			while( SDL_PollEvent(&event) ){
				switch( event.type ){
					case SDL_QUIT:
						// fermer
						return 0;
						break;
				}
			}


		 // // // // // // // // //
		// Handle Keyboard inputs //`
		 // // // // // // // // //
			SDL_PumpEvents();


			//move
			if( keystate[SDL_SCANCODE_ESCAPE] ){
				return 0;
			}

			ship.dir = 0;
			if( keystate[SDL_SCANCODE_LEFT] ){
				ship.x-=SHIP_SPEED;
				ship.dir = 1;

				if(ship.x + DIST_BORD_VAISS.x < BACKGROUND_DEST.x)
					ship.x = BACKGROUND_DEST.x - DIST_BORD_VAISS.x;
			}
			else if( keystate[SDL_SCANCODE_RIGHT] ){
				ship.x+=SHIP_SPEED;
				ship.dir = 2;

				if(ship.x + DIST_BORD_VAISS.w > BACKGROUND_DEST.x + BACKGROUND_DEST.w)
					ship.x = BACKGROUND_DEST.x + BACKGROUND_DEST.w - DIST_BORD_VAISS.w;
			}

			if( keystate[SDL_SCANCODE_UP] ){
				ship.y-=SHIP_SPEED;
				ship.dir += 3;

				if(ship.y + DIST_BORD_VAISS.y < BACKGROUND_DEST.y)
					ship.y = BACKGROUND_DEST.y - DIST_BORD_VAISS.y;
			}
			else if( keystate[SDL_SCANCODE_DOWN] ){
				ship.y += RATIO_SPEED_DOWN * SHIP_SPEED;
				ship.dir += 6;

				if(ship.y + DIST_BORD_VAISS.h > BACKGROUND_DEST.y + BACKGROUND_DEST.h)
					ship.y = BACKGROUND_DEST.y + BACKGROUND_DEST.h - DIST_BORD_VAISS.h;
			}

			if( keystate[SDL_SCANCODE_SPACE] ){
				fire(&ship, &allyMissiles, &nbAllyMissiles);
			}


			if( keystate[SDL_SCANCODE_TAB] ){
				ship.nbWeapon++;
				if(ship.nbWeapon >= NB_MAX_WEAPON)
					ship.nbWeapon = 0;
				switch (ship.nbWeapon) {
					case 0:
						ship.form = 0;
						break;
					case 1:
					case 2:
						ship.form = 1;
						break;
					default:
						ship.form = 2;
				}
			}

		// // // // //
		// Gameplay //`
		// // // // //
		if(nbEnemy == 0){
			int r = rand()%9;
			if(r<5)
				spawnEnemy(&enemies, &nbEnemy, 0, 4 + rand()%3);
			else if (r<7)
				spawnEnemy(&enemies, &nbEnemy, 1, 1);
			else
				spawnEnemy(&enemies, &nbEnemy, 2, 1);
		}


		for(int i=0; i<nbEnemy; i++)
			enemyFire(&(enemies[i]), &enemyMissiles, &nbEnemyMissiles);

		//move missiles
		for(int i=0; i<nbAllyMissiles; i++)
			moveMissile(&(allyMissiles[i]));

		for(int i=0; i<nbEnemyMissiles; i++)
			moveMissile(&(enemyMissiles[i]));

		for(int i=0; i<nbEnemy; i++)
			moveEnemy(&enemies, &nbEnemy, i);


		// // // // //
		// Hitbox //`
		// // // // //

			//ship-enemy
			SDL_SetTextureColorMod(textures[SH_SHIP],255,255,255);
			for(int i=0; i<nbEnemy; i++)
				if( shipHitEnemy(ship, enemies[i]) )
					SDL_SetTextureColorMod(textures[SH_SHIP],255,0,0);


			//ship-missiles
			for(int i=0; i< nbEnemyMissiles; i++){
				if(missileHitShip(enemyMissiles[i], ship))
					SDL_SetTextureColorMod(textures[SH_SHIP],255,150,0);
			}

			//enemy-missiles
			for(int i=0; i< nbAllyMissiles; i++){
				for(int iEnemy = 0; iEnemy < nbEnemy; iEnemy++){
					//enemy hitted
					if(missileHitEnemy(allyMissiles[i], enemies[iEnemy])){
						enemies[iEnemy].hp -= allyMissiles[i].damage;
						enemies[iEnemy].frameHit = FRAME_HIT_ANIM;
						if(enemies[iEnemy].hp <= 0){
							explosions = realloc(explosions, ++nbExplosions * sizeof(Explosion));
							explosions[nbExplosions-1].id = EXPLO_SHIP;
							explosions[nbExplosions-1].taille = 2 * ENEMY_HITBOX[enemies[iEnemy].id].rx * RATIO_SHIP_EXPLO_SIZE;
							explosions[nbExplosions-1].x = enemies[iEnemy].x + ENEMY_HITBOX[enemies[iEnemy].id].x - explosions[nbExplosions-1].taille/2;
							explosions[nbExplosions-1].y = enemies[iEnemy].y + ENEMY_HITBOX[enemies[iEnemy].id].y - explosions[nbExplosions-1].taille/2;
							explosions[nbExplosions-1].frame = FRAME_EXPLOSIONS[explosions[nbExplosions-1].id];

							shiftEnemy(enemies, nbEnemy, iEnemy);
							nbEnemy--;
							if(nbEnemy !=0)
								enemies=realloc(enemies, sizeof(Enemy)* nbEnemy );

						}
						explosions = realloc(explosions, ++nbExplosions * sizeof(Explosion));
						explosions[nbExplosions-1].id = EXPLO_MISSILE;
						explosions[nbExplosions-1].x = allyMissiles[i].x - TAILLE_EXPLOSIONS[allyMissiles[i].id]/2 - MISSILE_CENTER[allyMissiles[i].id].x;
						explosions[nbExplosions-1].y = allyMissiles[i].y - TAILLE_EXPLOSIONS[allyMissiles[i].id]/2 - MISSILE_HITBOX[allyMissiles[i].id].ry;
						explosions[nbExplosions-1].frame = FRAME_EXPLOSIONS[explosions[nbExplosions-1].id];
						explosions[nbExplosions-1].taille = BASE_TAILLE_EXPLOSION * TAILLE_EXPLOSIONS[allyMissiles[i].id];

						shiftMissile(allyMissiles, nbAllyMissiles, i);
						nbAllyMissiles--;
						if(nbAllyMissiles !=0)
							allyMissiles=realloc(allyMissiles, sizeof(Missile)* nbAllyMissiles );
						break;
					}
				}
			}

		 // // //
		// Draw //`
		 // // //
			drawBackground(renderer, textures[SH_BACKGROUND], backgroundSrc, backgroundDest);

			if(nbEnemy && enemies[0].id == BASE_ENEMY){
				for(int i=enemies[0].abscisses[0]; i<=BACKGROUND_DEST.x + BACKGROUND_DEST.w ; i++){
					SDL_SetRenderDrawColor(renderer, 0,255,0,255);
					if(i == roundf(enemies[0].abscisses[0]) || i == roundf(enemies[0].abscisses[1]) || i == roundf(enemies[0].abscisses[2]) || i == roundf(enemies[0].abscisses[3]))
						SDL_SetRenderDrawColor(renderer, 255,0,0,255);
					SDL_RenderDrawPoint(renderer, i + (ENEMY_SRC[BASE_ENEMY].w/RATIO_SIZE_ENEMY[BASE_ENEMY])/2, it_pol_lagrange(3,enemies[0].coefs,enemies[0].abscisses,i)+ (ENEMY_SRC[BASE_ENEMY].h/RATIO_SIZE_ENEMY[BASE_ENEMY])/2);
				}

				/*SDL_SetRenderDrawColor(renderer, 255,255,255,255);
				for(int i=0; i<NMAX; i++)
					SDL_RenderDrawPoint(renderer, x[i], f[i]);*/
			}


			for(int i=0; i<nbAllyMissiles; i++)
				drawMissile(renderer, allyMissiles[i], textures[SH_MISSILE]);

			for(int i=0; i<nbEnemyMissiles; i++)
				drawMissile(renderer, enemyMissiles[i], textures[SH_MISSILE]);

			for(int i=0; i<nbEnemy; i++)
                drawEnemy(renderer, enemies[i] , textures);

			drawShip(renderer, ship, textures);


			for(int i=0; i<nbExplosions; i++)
				drawExplosion(renderer, explosions[i], textures[SH_EXPLO_MISSILE+explosions[i].id]);

			drawBeta(renderer, font ,ratioWindowSize, (SDL_Color){0xFF,0x00,0x00},28);

			SDL_RenderPresent(renderer);



		 // // // // //
		// Next frame //`
		 // // // // //
			//rotate enemy base
			for(int i=0; i<nbEnemy; i++){
				if( enemies[i].frameHit )
					enemies[i].frameHit--;

				if( enemies[i].frameWait )
					enemies[i].frameWait--;

				if(enemies[i].id == BASE_ENEMY)
					enemies[i].rota += 6;
			}

			//reload
			for(int i=0; i<=ship.nbWeapon; i++){
				if(ship.weapons[i].frame_reload)
					ship.weapons[i].frame_reload--;
			}

			for(int i=0; i < nbEnemy; i++){
				for(int j=0; j<=enemies[i].nbWeapon; j++){
					if(enemies[i].weapons[j].frame_reload)
						enemies[i].weapons[j].frame_reload--;
				}
			}

			//background
			for(int layer=0; layer<3; layer++){
				backgroundSrc[layer].y -= SCROLL_SPEED_BACKGROUND[layer];
				if(backgroundSrc[layer].y < -backgroundSrc[layer].h){
					backgroundSrc[layer].y = BACKGROUND_GLOBAL_SRC.h - BACKGROUND_SRC.h - (-backgroundSrc[layer].h -backgroundSrc[layer].y);
					backgroundSrc[layer].x += backgroundSrc[layer].w;
					if(backgroundSrc[layer].x == NB_BACKGROUND_LAYER*(layer+1)*backgroundSrc[layer].w)
						backgroundSrc[layer].x = NB_BACKGROUND_LAYER*layer*backgroundSrc[layer].w;
				}
			}

		 	frame++;

			//explo
			for(int i=0; i<nbExplosions; i++){
				explosions[i].frame--;

				if(!explosions[i].frame){
					shiftExplosion(explosions, nbExplosions, i);
					nbExplosions--;
					if(nbExplosions != 0){
						explosions=realloc(explosions,sizeof(Explosion) * nbExplosions);
					}
				}
			}

			//regulateFPS
			currentTime = SDL_GetTicks();
			while( currentTime - lastTime < 1000./FRAMES_PER_SECOND )
				currentTime = SDL_GetTicks();

			lastTime = currentTime;


			// On efface
			SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b, 255);
			SDL_RenderClear(renderer);

		}

	}
	return 0;
}
