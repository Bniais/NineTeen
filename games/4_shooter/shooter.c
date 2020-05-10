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
	/*if(collideRect)
		return SDL_TRUE;*/

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

int getDirForTexture(int dir){
	if(dir == -1)
		return 1;
	else if(dir == 1)
		return 2;
	return 0;
}


void drawShip(SDL_Renderer *renderer, Ship ship, SDL_Texture **textures){
	SDL_Rect srcShip = SHIP_SRC;
	srcShip.y += ship.dir * srcShip.h;
	SDL_Rect srcFlame = srcShip;
	srcShip.x += ship.form * srcShip.w;


	SDL_Rect dest = SHIP_SRC;
	dest.w/=RATIO_SIZE_SHIP;
	dest.h/=RATIO_SIZE_SHIP;
	dest.x+=ship.x;
	dest.y+=ship.y;


	for(int i=0; i<3; i++){
		SDL_RenderCopy(renderer, textures[SH_FLAME], &srcFlame, &dest);
		srcFlame.x+= srcFlame.w;
	}

	//SDL_RenderCopy(renderer, textures[SH_SHIP], &srcShip, &dest);
    SDL_Point pt = {SHIP_HITBOX[ship.form].x, SHIP_HITBOX[ship.form].y};
	SDL_RenderCopy(renderer, textures[SH_SHIP], &srcShip, &dest);

	SDL_Rect destWeapon = WEAPON_DEST[0];
	for(int i=0; i<=ship.nbWeapon; i++){
		destWeapon = WEAPON_DEST[WEAPON_DISPOSITION[ship.nbWeapon][i]-1];
		destWeapon.x += DECALLAGE_TURN[ship.dir][WEAPON_DISPOSITION[ship.nbWeapon][i]-1];
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

	if( enemy.id == 0){
		SDL_RenderCopyEx(renderer, textures[SH_ENEMY_ROND + enemy.id], &srcEnemy, &destEnemy, enemy.rota, NULL, SDL_FLIP_NONE);
		srcEnemy.x += srcEnemy.w;
	}
	SDL_RenderCopy(renderer, textures[SH_ENEMY_ROND + enemy.id], &srcEnemy, &destEnemy );
}


int shipHitEnemy(Ship ship, Enemy enemy){
	return (collideOblique(SHIP_HITBOX[ship.form].x + ship.x, SHIP_HITBOX[ship.form].y + ship.y, SHIP_HITBOX[ship.form].rx , 0, SHIP_HITBOX[ship.form].ry/SHIP_HITBOX[ship.form].rx,
			ENEMY_HITBOX[enemy.id].x + enemy.x, ENEMY_HITBOX[enemy.id].y + enemy.y, cos(enemy.rota) * ENEMY_HITBOX[enemy.id].rx, sin(enemy.rota) * ENEMY_HITBOX[enemy.id].rx, ENEMY_HITBOX[enemy.id].ry/ENEMY_HITBOX[enemy.id].rx, 0)
			||
			collideOblique(ENEMY_ADDITIONAL_HITBOX[enemy.id].x + enemy.x, ENEMY_ADDITIONAL_HITBOX[enemy.id].y + enemy.y, cos(enemy.rota) * ENEMY_ADDITIONAL_HITBOX[enemy.id].rx, sin(enemy.rota) * ENEMY_ADDITIONAL_HITBOX[enemy.id].rx, ENEMY_ADDITIONAL_HITBOX[enemy.id].ry/ENEMY_ADDITIONAL_HITBOX[enemy.id].rx,
				SHIP_HITBOX[ship.form].x + ship.x, SHIP_HITBOX[ship.form].y + ship.y,SHIP_HITBOX[ship.form].rx ,  0, SHIP_HITBOX[ship.form].ry/SHIP_HITBOX[ship.form].rx, 1)
			);

	/*return (collide( SHIP_HITBOX[ship.form].x + ship.x, SHIP_HITBOX[ship.form].y + ship.y, SHIP_HITBOX[ship.form].rx ,SHIP_HITBOX[ship.form].ry,
			ENEMY_HITBOX[enemy.id].x + enemy.x, ENEMY_HITBOX[enemy.id].y + enemy.y, ENEMY_HITBOX[enemy.id].rx, ENEMY_HITBOX[enemy.id].ry, SDL_FALSE)
			||
			collide( ENEMY_ADDITIONAL_HITBOX[enemy.id].x + enemy.x, ENEMY_ADDITIONAL_HITBOX[enemy.id].y + enemy.y,ENEMY_ADDITIONAL_HITBOX[enemy.id].rx, ENEMY_ADDITIONAL_HITBOX[enemy.id].ry,
				SHIP_HITBOX[ship.form].x + ship.x, SHIP_HITBOX[ship.form].y + ship.y, SHIP_HITBOX[ship.form].rx ,SHIP_HITBOX[ship.form].ry, SDL_TRUE)
			);*/
}

int missileHitEnemy(Missile missile, Enemy enemy){
	//return(collideOblique())
	return (collideOblique(missile.x, missile.y, cos(missile.rota) * MISSILE_HITBOX[missile.id].rx , sin(missile.rota) * MISSILE_HITBOX[missile.id].rx,MISSILE_HITBOX[missile.id].ry/MISSILE_HITBOX[missile.id].rx,
			ENEMY_HITBOX[enemy.id].x + enemy.x, ENEMY_HITBOX[enemy.id].y + enemy.y, cos(enemy.rota) * ENEMY_HITBOX[enemy.id].rx, sin(enemy.rota) * ENEMY_HITBOX[enemy.id].rx, ENEMY_HITBOX[enemy.id].ry/ENEMY_HITBOX[enemy.id].rx, 0)
			||
			collideOblique(ENEMY_ADDITIONAL_HITBOX[enemy.id].x + enemy.x, ENEMY_ADDITIONAL_HITBOX[enemy.id].y + enemy.y, cos(enemy.rota) * ENEMY_ADDITIONAL_HITBOX[enemy.id].rx, sin(enemy.rota) * ENEMY_ADDITIONAL_HITBOX[enemy.id].rx, ENEMY_ADDITIONAL_HITBOX[enemy.id].ry/ENEMY_ADDITIONAL_HITBOX[enemy.id].rx,
			missile.x, missile.y, cos(missile.rota) * MISSILE_HITBOX[missile.id].rx , sin(missile.rota) * MISSILE_HITBOX[missile.id].rx, MISSILE_HITBOX[missile.id].ry/MISSILE_HITBOX[missile.id].rx, 1)
			);

	/*return (collide( missile.x, missile.y, MISSILE_HITBOX[missile.id].rx ,MISSILE_HITBOX[missile.id].ry,
			ENEMY_HITBOX[enemy.id].x + enemy.x, ENEMY_HITBOX[enemy.id].y + enemy.y,ENEMY_HITBOX[enemy.id].rx, ENEMY_HITBOX[enemy.id].ry, SDL_FALSE)
			||
			collide(ENEMY_ADDITIONAL_HITBOX[enemy.id].x + enemy.x, ENEMY_ADDITIONAL_HITBOX[enemy.id].y + enemy.y,ENEMY_ADDITIONAL_HITBOX[enemy.id].rx, ENEMY_ADDITIONAL_HITBOX[enemy.id].ry,
				 missile.x, missile.y, MISSILE_HITBOX[missile.id].rx ,MISSILE_HITBOX[missile.id].ry, SDL_TRUE)
			);*/
}

void fire(Ship *ship, Missile **allyMissiles, int *nbAllyMissiles){

	for(int i = 0; i <= ship->nbWeapon; i++){
		if(ship->weapons[i].frame_reload == 0){
			*allyMissiles = realloc(*allyMissiles, ++(*nbAllyMissiles)  *  sizeof(Missile));
			(*allyMissiles)[*nbAllyMissiles - 1].x = ship->x + (WEAPON_DEST[WEAPON_DISPOSITION[ship->nbWeapon][i]-1].x + WEAPON_DEST[WEAPON_DISPOSITION[ship->nbWeapon][i]-1].w/2. +DECALLAGE_TURN[ship->dir][WEAPON_DISPOSITION[ship->nbWeapon][i]-1])/RATIO_SIZE_SHIP;
			(*allyMissiles)[*nbAllyMissiles - 1].y = ship->y + (WEAPON_DEST[WEAPON_DISPOSITION[ship->nbWeapon][i]-1].y)/RATIO_SIZE_SHIP;
			(*allyMissiles)[*nbAllyMissiles - 1].id = ship->weapons[i].id;
			(*allyMissiles)[*nbAllyMissiles - 1].rota = 0;

			ship->weapons[i].frame_reload = RELOAD_FRAME;
		}
	}
}

void drawMissile(SDL_Renderer *renderer, Missile missile, SDL_Texture *textureMissile){
	SDL_Rect dest = MISSILE_SRC[missile.id];
	dest.x = missile.x - MISSILE_CENTER[missile.id].x;
	dest.y = missile.y - MISSILE_CENTER[missile.id].y;

	SDL_RenderCopyEx(renderer, textureMissile, &MISSILE_SRC[missile.id], &dest, missile.rota * 180/PI, &MISSILE_CENTER[missile.id], SDL_FLIP_NONE);

	SDL_SetRenderDrawColor(renderer, 0,255,255,255);
	SDL_RenderDrawPoint(renderer, missile.x, missile.y);
}

drawBackground(SDL_Renderer *renderer, SDL_Texture *textureBackground, SDL_Rect *backgroundSrc, SDL_Rect *backgroundDest){
	if(backgroundSrc[0].y<0){
		int tmp = backgroundSrc[0].y;
		backgroundSrc[0].y = 0;
		backgroundDest[0].y = -tmp;
		backgroundDest[0].h = BACKGROUND_DEST.h + tmp;
		backgroundSrc[0].h = BACKGROUND_SRC.h +tmp;

		SDL_RenderCopy(renderer, textureBackground, &backgroundSrc[0], &backgroundDest[0]);

		backgroundDest[0].y = 0;
		backgroundDest[0].h = BACKGROUND_DEST.h - backgroundDest[0].h;

		backgroundSrc[0].h = BACKGROUND_SRC.h - backgroundSrc[0].h;
		backgroundSrc[0].y = BACKGROUND_GLOBAL_SRC.h - backgroundSrc[0].h;

		backgroundSrc[0].x += backgroundSrc[0].w;
		if(backgroundSrc[0].x == 3*backgroundSrc[0].w)
			backgroundSrc[0].x = 0;
		SDL_RenderCopy(renderer, textureBackground, &backgroundSrc[0], &backgroundDest[0]);

		backgroundSrc[0].x -= backgroundSrc[0].w;
		if(backgroundSrc[0].x == -backgroundSrc[0].w)
			backgroundSrc[0].x = 2*backgroundSrc[0].w;

		backgroundSrc[0].y = tmp;
		backgroundDest[0].h = BACKGROUND_DEST.h;
		backgroundSrc[0].h = BACKGROUND_SRC.h;
	}
	else{
		backgroundDest[0].y = 0;
		backgroundDest[0].h = BACKGROUND_DEST.h;
		backgroundSrc[0].h = BACKGROUND_SRC.h;
		SDL_RenderCopy(renderer, textureBackground, &backgroundSrc[0], &backgroundDest[0]);
	}

	if(backgroundSrc[1].y<0){
		int tmp = backgroundSrc[1].y;
		backgroundSrc[1].y = 0;
		backgroundDest[1].y = -tmp;
		backgroundDest[1].h = BACKGROUND_DEST.h + tmp;
		backgroundSrc[1].h = BACKGROUND_SRC.h +tmp;

		SDL_RenderCopy(renderer, textureBackground, &backgroundSrc[1], &backgroundDest[1]);

		backgroundDest[1].y = 0;
		backgroundDest[1].h = BACKGROUND_DEST.h - backgroundDest[1].h;

		backgroundSrc[1].h = BACKGROUND_SRC.h - backgroundSrc[1].h;
		backgroundSrc[1].y = BACKGROUND_GLOBAL_SRC.h - backgroundSrc[1].h;

		backgroundSrc[1].x += backgroundSrc[1].w;
		if(backgroundSrc[1].x == 6*backgroundSrc[1].w)
			backgroundSrc[1].x = 3*backgroundSrc[1].w;
		SDL_RenderCopy(renderer, textureBackground, &backgroundSrc[1], &backgroundDest[1]);

		backgroundSrc[1].x -= backgroundSrc[1].w;
		if(backgroundSrc[1].x == 2*backgroundSrc[1].w){
			printf("reset\n");
			backgroundSrc[1].x = 5*backgroundSrc[1].w;
		}


		backgroundSrc[1].y = tmp;
		backgroundDest[1].h = BACKGROUND_DEST.h;
		backgroundSrc[1].h = BACKGROUND_SRC.h;
	}
	else{
		backgroundDest[1].y = 0;
		backgroundDest[1].h = BACKGROUND_DEST.h;
		backgroundSrc[1].h = BACKGROUND_SRC.h;
		SDL_RenderCopy(renderer, textureBackground, &backgroundSrc[1], &backgroundDest[1]);
	}

	if(backgroundSrc[2].y<0){
		int tmp = backgroundSrc[2].y;
		backgroundSrc[2].y = 0;
		backgroundDest[2].y = -tmp;
		backgroundDest[2].h = BACKGROUND_DEST.h + tmp;
		backgroundSrc[2].h = BACKGROUND_SRC.h +tmp;

		SDL_RenderCopy(renderer, textureBackground, &backgroundSrc[2], &backgroundDest[2]);

		backgroundDest[2].y = 0;
		backgroundDest[2].h = BACKGROUND_DEST.h - backgroundDest[2].h;

		backgroundSrc[2].h = BACKGROUND_SRC.h - backgroundSrc[2].h;
		backgroundSrc[2].y = BACKGROUND_GLOBAL_SRC.h - backgroundSrc[2].h;

		backgroundSrc[2].x += backgroundSrc[2].w;
		if(backgroundSrc[2].x == 9*backgroundSrc[2].w)
			backgroundSrc[2].x = 6*backgroundSrc[2].w;
		SDL_RenderCopy(renderer, textureBackground, &backgroundSrc[2], &backgroundDest[2]);

		backgroundSrc[2].x -= backgroundSrc[2].w;
		if(backgroundSrc[2].x == 5*backgroundSrc[2].w){
			printf("reset\n");
			backgroundSrc[2].x = 8*backgroundSrc[2].w;
		}


		backgroundSrc[2].y = tmp;
		backgroundDest[2].h = BACKGROUND_DEST.h;
		backgroundSrc[2].h = BACKGROUND_SRC.h;
	}
	else{
		backgroundDest[2].y = 0;
		backgroundDest[2].h = BACKGROUND_DEST.h;
		backgroundSrc[2].h = BACKGROUND_SRC.h;
		SDL_RenderCopy(renderer, textureBackground, &backgroundSrc[2], &backgroundDest[2]);
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
	//Fonts

	//audio

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
			{{0, 0},{0, 0},{0, 0},{0, 0},{0, 0}} //weapons
		};

		Enemy enemy1 = {
			0, //id
			100, //x
			200, //y
			0, //r
			0, //rota
			1 //hp
		};

		Enemy enemy2 = {
			1, //id
			100, //x
			400, //y
			0, //r
			0, //rota
			3 //hp
		};

		Enemy enemy3 = {
			2, //id
			100, //x
			700, //y
			0, //r
			0, //rota
			5 //hp
		};

		int nbAllyMissiles = 0;

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
			int spd = 8;
			if( keystate[SDL_SCANCODE_UP] ){
				ship.y-=spd;

				if(ship.y + DIST_BORD_VAISS.y < BACKGROUND_DEST.y)
					ship.y = BACKGROUND_DEST.y - DIST_BORD_VAISS.y;
			}
			else if( keystate[SDL_SCANCODE_DOWN] ){
				ship.y+=spd;

				if(ship.y + DIST_BORD_VAISS.h > BACKGROUND_DEST.y + BACKGROUND_DEST.h)
					ship.y = BACKGROUND_DEST.y + BACKGROUND_DEST.h - DIST_BORD_VAISS.h;
			}

			ship.dir = 0;
			if( keystate[SDL_SCANCODE_LEFT] ){
				ship.x-=spd;
				ship.dir = 1;

				if(ship.x + DIST_BORD_VAISS.x < BACKGROUND_DEST.x)
					ship.x = BACKGROUND_DEST.x - DIST_BORD_VAISS.x;
			}
			else if( keystate[SDL_SCANCODE_RIGHT] ){
				ship.x+=spd;
				ship.dir = 2;

				if(ship.x + DIST_BORD_VAISS.w > BACKGROUND_DEST.x + BACKGROUND_DEST.w)
					ship.x = BACKGROUND_DEST.x + BACKGROUND_DEST.w - DIST_BORD_VAISS.w;
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

		for(int i=0; i<nbAllyMissiles; i++)
			allyMissiles[i].y -= 10;

		// // // // //
		// Hitbox //`
		// // // // //

			//ship-enemy
			if( shipHitEnemy(ship, enemy1) || shipHitEnemy(ship, enemy2) || shipHitEnemy(ship, enemy3) )
				SDL_SetTextureColorMod(textures[SH_SHIP],255,0,0);
			else
				SDL_SetTextureColorMod(textures[SH_SHIP],255,255,255);

			//ship-missiles

			//enemy-missiles
			SDL_SetTextureColorMod(textures[SH_ENEMY_ROND], 255,255,255);
			SDL_SetTextureColorMod(textures[SH_ENEMY_LASER], 255,255,255);
			SDL_SetTextureColorMod(textures[SH_ENEMY_BOSS], 255,255,255);
			for(int i=0; i< nbAllyMissiles; i++){
				if(missileHitEnemy(allyMissiles[i], enemy1))
					SDL_SetTextureColorMod(textures[SH_ENEMY_ROND], 255,0,0);

				if(missileHitEnemy(allyMissiles[i], enemy2))
					SDL_SetTextureColorMod(textures[SH_ENEMY_LASER], 255,0,0);

				if(missileHitEnemy(allyMissiles[i], enemy3))
					SDL_SetTextureColorMod(textures[SH_ENEMY_BOSS], 255,0,0);

			}

		 // // //
		// Draw //`
		 // // //
		 	SDL_SetRenderDrawColor(renderer, 255,0,0,255);
		 	SDL_RenderFillRect(renderer, &BACKGROUND_DEST);

			drawBackground(renderer, textures[SH_BACKGROUND], backgroundSrc, backgroundDest);



			//if(&backgroundSrc[0])
			drawEnemy(renderer, enemy1, textures);
			drawEnemy(renderer, enemy2, textures);
			drawEnemy(renderer, enemy3, textures);

			for(int i=0; i<nbAllyMissiles; i++)
				drawMissile(renderer, allyMissiles[i], textures[SH_MISSILE]);

			drawShip(renderer, ship, textures);





			//ellipseColor (renderer, hitbox.x + hitbox.w/2, hitbox.y + hitbox.h/2, hitbox.w/2, hitbox.h/2, 0xFFFFFFFF);
			//ellipseColor (renderer, circle.x + circle.w/2, circle.y + circle.h/2, circle.w/2, circle.h/2, 0xFFFFFFFF);


			SDL_RenderPresent(renderer);



		 // // // // //
		// Next frame //`
		 // // // // //
		 	enemy1.rota += 6;

			for(int i=0; i<=ship.nbWeapon; i++){
				if(ship.weapons[i].frame_reload)
					ship.weapons[i].frame_reload--;
			}

			//background
			backgroundSrc[0].y -= SCROLL_SPEED_BACKGROUND[0];
			if(backgroundSrc[0].y < -backgroundSrc[0].h){
				backgroundSrc[0].y = BACKGROUND_GLOBAL_SRC.h - BACKGROUND_SRC.h - (-backgroundSrc[0].h -backgroundSrc[0].y);
				backgroundSrc[0].x += backgroundSrc[0].w;
				if(backgroundSrc[0].x == 3*backgroundSrc[0].w)
					backgroundSrc[0].x = 0;
			}

			backgroundSrc[1].y -= SCROLL_SPEED_BACKGROUND[1];
			if(backgroundSrc[1].y < -backgroundSrc[1].h){
				backgroundSrc[1].y = BACKGROUND_GLOBAL_SRC.h - BACKGROUND_SRC.h - (-backgroundSrc[1].h -backgroundSrc[1].y);
				backgroundSrc[1].x += backgroundSrc[1].w;
				if(backgroundSrc[1].x == 6*backgroundSrc[1].w)
					backgroundSrc[1].x = 3*backgroundSrc[1].w;
			}

			backgroundSrc[2].y -= SCROLL_SPEED_BACKGROUND[2];
			if(backgroundSrc[2].y < -backgroundSrc[2].h){
				backgroundSrc[2].y = BACKGROUND_GLOBAL_SRC.h - BACKGROUND_SRC.h - (-backgroundSrc[2].h -backgroundSrc[2].y);
				backgroundSrc[2].x += backgroundSrc[2].w;
				if(backgroundSrc[2].x == 9*backgroundSrc[2].w)
					backgroundSrc[2].x = 6*backgroundSrc[2].w;
			}


		 	frame++;
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
