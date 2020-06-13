#include "../../include/communFunctions.h"
#include "../../include/define.h"
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
		destWeapon.w = ceil( destWeapon.w / RATIO_SIZE_SHIP);
		destWeapon.h = ceil( destWeapon.h / RATIO_SIZE_SHIP);
		destWeapon.x += dest.x;
		destWeapon.y += dest.y;
		if(i<3)
			destWeapon.x ++;
		SDL_RenderCopy(renderer, textures[SH_WEAPON], &WEAPON_SRC, &destWeapon);
	}
}

void drawEnemy(SDL_Renderer *renderer, Enemy *enemy, SDL_Texture **textures){
	SDL_Rect srcEnemy = ENEMY_SRC[enemy->id];
	SDL_Rect destEnemy = srcEnemy;
	destEnemy.w /= RATIO_SIZE_ENEMY[enemy->id];
	destEnemy.h /= RATIO_SIZE_ENEMY[enemy->id];
	destEnemy.x = enemy->x;
	destEnemy.y = enemy->y;

	if( enemy->id == BASE_ENEMY){
		SDL_RenderCopyEx(renderer, textures[SH_ENEMY_ROND + enemy->id], &srcEnemy, &destEnemy, enemy->rota, NULL, SDL_FLIP_NONE);
		if(enemy->frameHit){
			srcEnemy.y += srcEnemy.h;
			SDL_SetTextureAlphaMod(textures[SH_ENEMY_ROND + enemy->id], ALPHA_HIT);
			SDL_RenderCopyEx(renderer, textures[SH_ENEMY_ROND + enemy->id], &srcEnemy, &destEnemy, enemy->rota, NULL, SDL_FLIP_NONE);
			srcEnemy.y -= srcEnemy.h;
			SDL_SetTextureAlphaMod(textures[SH_ENEMY_ROND + enemy->id], 255);
		}

		srcEnemy.x += srcEnemy.w;
	}
	if(enemy->id != ARM_ENEMY){
		SDL_RenderCopy(renderer, textures[SH_ENEMY_ROND + enemy->id], &srcEnemy, &destEnemy );
		if(enemy->frameHit){
			SDL_SetTextureAlphaMod(textures[SH_ENEMY_ROND + enemy->id], ALPHA_HIT);
			srcEnemy.y += srcEnemy.h;
			SDL_RenderCopy(renderer, textures[SH_ENEMY_ROND + enemy->id], &srcEnemy, &destEnemy );
			SDL_SetTextureAlphaMod(textures[SH_ENEMY_ROND + enemy->id], 255);
		}
	}
	else{
		SDL_Point rotaPoint = ROTATION_POINT_ARM;
		rotaPoint.x -= ENEMY_CENTER[enemy->id].x;
		rotaPoint.y -= ENEMY_CENTER[enemy->id].y;

		float xM = rotaPoint.x;
		float yM = rotaPoint.y;
		rotaPoint.x = xM*cos(enemy->rota * PI / 180 ) - yM * sin(enemy->rota * PI / 180);
		rotaPoint.y = xM * sin(enemy->rota * PI / 180) + yM * cos(enemy->rota * PI / 180);

		SDL_Point rotaPointAvant = ROTATION_POINT_ARM;
		rotaPointAvant.x -= ENEMY_CENTER[enemy->id].x;
		rotaPointAvant.y -= ENEMY_CENTER[enemy->id].y;

		xM = rotaPointAvant.x;
		yM = rotaPointAvant.y;
		rotaPointAvant.x = xM*cos(enemy->coefs[0] * PI / 180 ) - yM * sin(enemy->coefs[0] * PI / 180);
		rotaPointAvant.y = xM * sin(enemy->coefs[0] * PI / 180) + yM * cos(enemy->coefs[0] * PI / 180);



		for(int i=0; i<NB_ARM_COMPONENT; i++){
			if(enemy->hp/ENEMY_HP[enemy->id] > RATIO_HP_PIECES[i]){
				if(i == 0 || i>4){
					SDL_Point center = ENEMY_CENTER[enemy->id];
					SDL_RenderCopyEx(renderer, textures[SH_ENEMY_ROND + enemy->id], &srcEnemy, &destEnemy, enemy->rota, &center, SDL_FLIP_NONE);
					if(enemy->frameHit){
						SDL_SetTextureAlphaMod(textures[SH_ENEMY_ROND + enemy->id], ALPHA_HIT);
						srcEnemy.y += srcEnemy.h;
						//center.y += srcEnemy.h / RATIO_SIZE_ENEMY[enemy->id];
						SDL_RenderCopyEx(renderer, textures[SH_ENEMY_ROND + enemy->id], &srcEnemy, &destEnemy, enemy->rota, &center, SDL_FLIP_NONE);
						srcEnemy.y -= srcEnemy.h;
						SDL_SetTextureAlphaMod(textures[SH_ENEMY_ROND + enemy->id], 255);
					}

				}
				else{
					SDL_Point center = ENEMY_CENTER[enemy->id];//ROTATION_POINT_ARM;

					destEnemy.x += roundf(rotaPoint.x - rotaPointAvant.x);
					destEnemy.y += roundf(rotaPoint.y - rotaPointAvant.y);

					SDL_RenderCopyEx(renderer, textures[SH_ENEMY_ROND + enemy->id], &srcEnemy, &destEnemy, enemy->coefs[0], &center, SDL_FLIP_NONE);
					if(enemy->frameHit){
						SDL_SetTextureAlphaMod(textures[SH_ENEMY_ROND + enemy->id], ALPHA_HIT);
						srcEnemy.y += srcEnemy.h;
						SDL_RenderCopyEx(renderer, textures[SH_ENEMY_ROND + enemy->id], &srcEnemy, &destEnemy, enemy->coefs[0], &center, SDL_FLIP_NONE);
						srcEnemy.y -= srcEnemy.h;
						SDL_SetTextureAlphaMod(textures[SH_ENEMY_ROND + enemy->id], 255);
					}
					destEnemy.x -= roundf(rotaPoint.x- rotaPointAvant.x);
					destEnemy.y -= roundf(rotaPoint.y- rotaPointAvant.y);
				}

			}
			srcEnemy.x += srcEnemy.w;
		}

		//if(DRAW_DEBUG){
			rotaPoint.x += enemy->x + ENEMY_CENTER[enemy->id].x;
			rotaPoint.y += enemy->y + ENEMY_CENTER[enemy->id].y;
			/*SDL_RenderDrawPoint(renderer,enemy->x + ROTATION_POINT_ARM.x, enemy->y + ROTATION_POINT_ARM.y);
			SDL_RenderDrawPoint(renderer,enemy->x + ROTATION_POINT_ARM.x+1,enemy->y +  ROTATION_POINT_ARM.y);
			SDL_RenderDrawPoint(renderer,enemy->x + ROTATION_POINT_ARM.x-1,enemy->y +  ROTATION_POINT_ARM.y);
			SDL_RenderDrawPoint(renderer,enemy->x + ROTATION_POINT_ARM.x,enemy->y +  ROTATION_POINT_ARM.y+1);
			SDL_RenderDrawPoint(renderer,enemy->x + ROTATION_POINT_ARM.x,enemy->y +  ROTATION_POINT_ARM.y-1);*/


			SDL_RenderDrawPoint(renderer,rotaPoint.x, rotaPoint.y);
			SDL_RenderDrawPoint(renderer,rotaPoint.x+1, rotaPoint.y);
			SDL_RenderDrawPoint(renderer,rotaPoint.x-1, rotaPoint.y);
			SDL_RenderDrawPoint(renderer,rotaPoint.x, rotaPoint.y+1);
			SDL_RenderDrawPoint(renderer,rotaPoint.x, rotaPoint.y-1);

			/*SDL_RenderDrawPoint(renderer,enemy->x, enemy->y);
			SDL_RenderDrawPoint(renderer,enemy->x+1, enemy->y);
			SDL_RenderDrawPoint(renderer,enemy->x-1, enemy->y);
			SDL_RenderDrawPoint(renderer,enemy->x, enemy->y+1);
			SDL_RenderDrawPoint(renderer,enemy->x, enemy->y-1);

			SDL_RenderDrawPoint(renderer,destEnemy.x + ENEMY_CENTER[enemy->id].x, destEnemy.y + ENEMY_CENTER[enemy->id].y);
			SDL_RenderDrawPoint(renderer,destEnemy.x + ENEMY_CENTER[enemy->id].x+1, destEnemy.y + ENEMY_CENTER[enemy->id].y);
			SDL_RenderDrawPoint(renderer,destEnemy.x + ENEMY_CENTER[enemy->id].x-1, destEnemy.y + ENEMY_CENTER[enemy->id].y);
			SDL_RenderDrawPoint(renderer,destEnemy.x + ENEMY_CENTER[enemy->id].x, destEnemy.y + ENEMY_CENTER[enemy->id].y+1);
			SDL_RenderDrawPoint(renderer,destEnemy.x + ENEMY_CENTER[enemy->id].x, destEnemy.y + ENEMY_CENTER[enemy->id].y-1);*/

			enemy->abscisses[0]=rotaPoint.x + DIST_ROTA_CENTER_HITBOX_ARM / RATIO_SIZE_ENEMY[enemy->id] * cos(enemy->coefs[0] * PI / 180 + PI/2);
			enemy->abscisses[1]=rotaPoint.y + DIST_ROTA_CENTER_HITBOX_ARM / RATIO_SIZE_ENEMY[enemy->id] * sin(enemy->coefs[0] * PI / 180 + PI/2);

			enemy->abscisses[2]=rotaPoint.x + DIST_ROTA_CENTER_ADDITIONAL_HITBOX_ARM / RATIO_SIZE_ENEMY[enemy->id] * cos(enemy->coefs[0] * PI / 180 + PI/2);
			enemy->abscisses[3]=rotaPoint.y + DIST_ROTA_CENTER_ADDITIONAL_HITBOX_ARM / RATIO_SIZE_ENEMY[enemy->id] * sin(enemy->coefs[0] * PI / 180 + PI/2);

			SDL_RenderDrawPoint(renderer,	enemy->abscisses[0], enemy->abscisses[1]);
			SDL_RenderDrawPoint(renderer,	enemy->abscisses[0]+1, enemy->abscisses[1]);
			SDL_RenderDrawPoint(renderer,	enemy->abscisses[0]-1, enemy->abscisses[1]);
			SDL_RenderDrawPoint(renderer,	enemy->abscisses[0], enemy->abscisses[1]+1);
			SDL_RenderDrawPoint(renderer,	enemy->abscisses[0], enemy->abscisses[1]-1);

			/*SDL_RenderDrawPoint(renderer,	enemy->abscisses[2], enemy->abscisses[3]);
			SDL_RenderDrawPoint(renderer,	enemy->abscisses[2]+1, enemy->abscisses[3]);
			SDL_RenderDrawPoint(renderer,	enemy->abscisses[2]-1, enemy->abscisses[3]);
			SDL_RenderDrawPoint(renderer,	enemy->abscisses[2], enemy->abscisses[3]+1);
			SDL_RenderDrawPoint(renderer,	enemy->abscisses[2], enemy->abscisses[3]-1);*/

			rotaPoint.x += DIST_ROTA_CANON_ARM / RATIO_SIZE_ENEMY[enemy->id] * cos(enemy->coefs[0] * PI / 180 + PI/2);
			rotaPoint.y += DIST_ROTA_CANON_ARM / RATIO_SIZE_ENEMY[enemy->id] * sin(enemy->coefs[0] * PI / 180 + PI/2);
			enemy->coefs[2]=rotaPoint.x;
			enemy->coefs[3]=rotaPoint.y;
			/*SDL_RenderDrawPoint(renderer,rotaPoint.x, rotaPoint.y);
			SDL_RenderDrawPoint(renderer,rotaPoint.x+1, rotaPoint.y);
			SDL_RenderDrawPoint(renderer,rotaPoint.x-1, rotaPoint.y);
			SDL_RenderDrawPoint(renderer,rotaPoint.x, rotaPoint.y+1);
			SDL_RenderDrawPoint(renderer,rotaPoint.x, rotaPoint.y-1);*/

			if(enemy->infos[5] >= 0){
				if(enemy->infos[5] >= NB_ROW_LASER_BEAM*NB_COL_LASER_BEAM - FRAME_LASER_BEAM_DISAPEAR)
					SDL_SetTextureAlphaMod(textures[SH_LASER_BEAM], 255-200/(NB_ROW_LASER_BEAM*NB_COL_LASER_BEAM-enemy->infos[5]));
				else
					SDL_SetTextureAlphaMod(textures[SH_LASER_BEAM], 255);
				SDL_Rect srcbeam = {(((int)enemy->infos[5])%NB_COL_LASER_BEAM) * LASER_BEAM_SRC.w, (((int)enemy->infos[5])/NB_COL_LASER_BEAM) * LASER_BEAM_SRC.h, LASER_BEAM_SRC.w, LASER_BEAM_SRC.h};
				SDL_Rect destbeam = {enemy->coefs[2] - LASER_BEAM_CENTER.x, enemy->coefs[3] - LASER_BEAM_CENTER.y, LASER_BEAM_SRC.w*RATIO_ALLONGEMENT_LASER, LASER_BEAM_SRC.h};
				printf("draw : %f \n", LASER_BEAM_SRC.w*RATIO_ALLONGEMENT_LASER);
				SDL_RenderCopyEx(renderer, textures[SH_LASER_BEAM], &srcbeam, &destbeam, enemy->coefs[0] + 90, &LASER_BEAM_CENTER, SDL_FLIP_NONE);

				enemy->infos[5]++;
			}
			if(enemy->infos[5] >= NB_ROW_LASER_BEAM*NB_COL_LASER_BEAM)
				enemy->infos[5] = -1;
		//}
	}
}

void drawEnemyParts(SDL_Renderer *renderer, EnemyPart *enemy, SDL_Texture **textures){
	SDL_Rect srcEnemy = ENEMY_SRC[enemy->idSrc];
	int leftPart = SDL_FALSE;

	if(enemy->id < NB_ARM_COMPONENT && enemy->id !=6 && enemy->id >=2){
		srcEnemy.w/=2;
	}
	SDL_Rect destEnemy = srcEnemy;
	destEnemy.w /= RATIO_SIZE_ENEMY[enemy->idSrc];
	destEnemy.h /= RATIO_SIZE_ENEMY[enemy->idSrc];
	destEnemy.x = enemy->x;
	destEnemy.y = enemy->y;

	if(enemy->id > NB_ARM_COMPONENT){
		leftPart = SDL_TRUE;
		enemy->id -= NB_ARM_COMPONENT;
		srcEnemy.y += ENEMY_SRC[enemy->idSrc].h*2;
	}

	srcEnemy.x += enemy->id * ENEMY_SRC[enemy->idSrc].w;


	SDL_Point center = ENEMY_CENTER[enemy->idSrc];
	if(enemy->id == 0 || enemy->id>4){
		SDL_RenderCopyEx(renderer, textures[SH_ENEMY_ROND + enemy->idSrc], &srcEnemy, &destEnemy, enemy->rota[0], &center, SDL_FLIP_NONE);
	}
	else{
		SDL_Point rotaPoint = ROTATION_POINT_ARM;
		rotaPoint.x -= ENEMY_CENTER[enemy->idSrc].x;
		rotaPoint.y -= ENEMY_CENTER[enemy->idSrc].y;

		float xM = rotaPoint.x;
		float yM = rotaPoint.y;
		rotaPoint.x = xM*cos(enemy->rota[0] * PI / 180 ) - yM * sin(enemy->rota[0] * PI / 180);
		rotaPoint.y = xM * sin(enemy->rota[0] * PI / 180) + yM * cos(enemy->rota[0] * PI / 180);

		SDL_Point rotaPointAvant = ROTATION_POINT_ARM;
		rotaPointAvant.x -= ENEMY_CENTER[enemy->idSrc].x;
		rotaPointAvant.y -= ENEMY_CENTER[enemy->idSrc].y;

		xM = rotaPointAvant.x;
		yM = rotaPointAvant.y;
		rotaPointAvant.x = xM*cos(enemy->rota[1] * PI / 180 ) - yM * sin(enemy->rota[1] * PI / 180);
		rotaPointAvant.y = xM * sin(enemy->rota[1] * PI / 180) + yM * cos(enemy->rota[1] * PI / 180);

		destEnemy.x += roundf(rotaPoint.x - rotaPointAvant.x);
		destEnemy.y += roundf(rotaPoint.y - rotaPointAvant.y);

		SDL_RenderCopyEx(renderer, textures[SH_ENEMY_ROND + enemy->idSrc], &srcEnemy, &destEnemy, enemy->rota[1], &center, SDL_FLIP_NONE);
	}

	if(leftPart)
		enemy->id += NB_ARM_COMPONENT;
}


int shipHitEnemy(Ship ship, Enemy enemy){
	return (collideOblique(SHIP_HITBOX[ship.form].x + ship.x, SHIP_HITBOX[ship.form].y + ship.y, SHIP_HITBOX[ship.form].rx , 0, SHIP_HITBOX[ship.form].ry/SHIP_HITBOX[ship.form].rx,
			ENEMY_HITBOX[enemy.id].x + enemy.x, ENEMY_HITBOX[enemy.id].y + enemy.y, cos(enemy.rota * PI / 180) * ENEMY_HITBOX[enemy.id].rx, sin(enemy.rota * PI / 180) * ENEMY_HITBOX[enemy.id].rx, ENEMY_HITBOX[enemy.id].ry/ENEMY_HITBOX[enemy.id].rx, 0)
			||
			collideOblique(ENEMY_ADDITIONAL_HITBOX[enemy.id].x + enemy.x, ENEMY_ADDITIONAL_HITBOX[enemy.id].y + enemy.y, cos(enemy.rota * PI / 180) * ENEMY_ADDITIONAL_HITBOX[enemy.id].rx, sin(enemy.rota * PI / 180) * ENEMY_ADDITIONAL_HITBOX[enemy.id].rx, ENEMY_ADDITIONAL_HITBOX[enemy.id].ry/ENEMY_ADDITIONAL_HITBOX[enemy.id].rx,
				SHIP_HITBOX[ship.form].x + ship.x, SHIP_HITBOX[ship.form].y + ship.y,SHIP_HITBOX[ship.form].rx ,  0, SHIP_HITBOX[ship.form].ry/SHIP_HITBOX[ship.form].rx, 1)

			||

			(enemy.id == ARM_ENEMY &&
			(collideOblique(SHIP_HITBOX[ship.form].x + ship.x, SHIP_HITBOX[ship.form].y + ship.y, SHIP_HITBOX[ship.form].rx , 0, SHIP_HITBOX[ship.form].ry/SHIP_HITBOX[ship.form].rx,
					enemy.abscisses[0], enemy.abscisses[1], cos(enemy.coefs[0] * PI / 180) * ENEMY_HITBOX_ARM.rx, sin(enemy.coefs[0] * PI / 180) * ENEMY_HITBOX_ARM.rx, ENEMY_HITBOX_ARM.ry/ENEMY_HITBOX_ARM.rx, 0)
			||
			collideOblique(enemy.abscisses[2], enemy.abscisses[3], cos(enemy.coefs[0] * PI / 180) * ENEMY_ADDITIONAL_HITBOX_ARM.rx, sin(enemy.coefs[0] * PI / 180) * ENEMY_ADDITIONAL_HITBOX_ARM.rx, ENEMY_ADDITIONAL_HITBOX_ARM.ry/ENEMY_ADDITIONAL_HITBOX_ARM.rx,
				SHIP_HITBOX[ship.form].x + ship.x, SHIP_HITBOX[ship.form].y + ship.y,SHIP_HITBOX[ship.form].rx ,  0, SHIP_HITBOX[ship.form].ry/SHIP_HITBOX[ship.form].rx, 1)))
			);

}

int laserHitShip(Ship ship, Enemy enemy){
	int longueur;
	if(enemy.infos[5] >= FIRST_FRAME_LASER &&  enemy.infos[5] < FIRST_FRAME_LASER + NB_FRAME_START_LASER_BEAM)
		longueur = LASER_BEAM_DIST_START[(int)enemy.infos[5] - FIRST_FRAME_LASER];
	else if(enemy.infos[5] >= FIRST_END_FRAME_LASER && enemy.infos[5] <  FIRST_END_FRAME_LASER + NB_FRAME_END_LASER_BEAM)
		longueur = LASER_BEAM_DIST_END[(int)enemy.infos[5] - FIRST_END_FRAME_LASER];
	else if(enemy.infos[5] >= FIRST_FRAME_LASER && enemy.infos[5] <= FIRST_END_FRAME_LASER)
		longueur = LASER_BEAM_DIST_START[NB_FRAME_START_LASER_BEAM-1];
	else
		return 0;

	longueur *= RATIO_ALLONGEMENT_LASER;
	longueur += LASER_BEAM_ADDITIONAL_HITBOX;

	return collideOblique(enemy.abscisses[2], enemy.abscisses[3], cos(enemy.coefs[0] * PI / 180) * LASER_BEAM_WIDTH_HITBOX, sin(enemy.coefs[0] * PI / 180) * LASER_BEAM_WIDTH_HITBOX, longueur/LASER_BEAM_WIDTH_HITBOX,
		SHIP_HITBOX[ship.form].x + ship.x, SHIP_HITBOX[ship.form].y + ship.y,SHIP_HITBOX[ship.form].rx ,  0, SHIP_HITBOX[ship.form].ry/SHIP_HITBOX[ship.form].rx, 1);
}

int missileHitEnemy(Missile missile, Enemy enemy){
	return (collideOblique(missile.x, missile.y, cos(missile.rota) * MISSILE_HITBOX[missile.id].rx , sin(missile.rota) * MISSILE_HITBOX[missile.id].rx,MISSILE_HITBOX[missile.id].ry/MISSILE_HITBOX[missile.id].rx,
			ENEMY_HITBOX[enemy.id].x + enemy.x, ENEMY_HITBOX[enemy.id].y + enemy.y, cos(enemy.rota * PI / 180) * ENEMY_HITBOX[enemy.id].rx, sin(enemy.rota * PI / 180) * ENEMY_HITBOX[enemy.id].rx, ENEMY_HITBOX[enemy.id].ry/ENEMY_HITBOX[enemy.id].rx, 0)
			||
			collideOblique(ENEMY_ADDITIONAL_HITBOX[enemy.id].x + enemy.x, ENEMY_ADDITIONAL_HITBOX[enemy.id].y + enemy.y, cos(enemy.rota * PI / 180) * ENEMY_ADDITIONAL_HITBOX[enemy.id].rx, sin(enemy.rota * PI / 180) * ENEMY_ADDITIONAL_HITBOX[enemy.id].rx, ENEMY_ADDITIONAL_HITBOX[enemy.id].ry/ENEMY_ADDITIONAL_HITBOX[enemy.id].rx,
			missile.x, missile.y, cos(missile.rota) * MISSILE_HITBOX[missile.id].rx , sin(missile.rota) * MISSILE_HITBOX[missile.id].rx, MISSILE_HITBOX[missile.id].ry/MISSILE_HITBOX[missile.id].rx, 1)

			||

			(enemy.id == ARM_ENEMY &&
			(collideOblique(missile.x, missile.y, cos(missile.rota) * MISSILE_HITBOX[missile.id].rx , sin(missile.rota) * MISSILE_HITBOX[missile.id].rx,MISSILE_HITBOX[missile.id].ry/MISSILE_HITBOX[missile.id].rx,
					enemy.abscisses[0], enemy.abscisses[1], cos(enemy.coefs[0] * PI / 180) * ENEMY_HITBOX_ARM.rx, sin(enemy.coefs[0] * PI / 180) * ENEMY_HITBOX_ARM.rx, ENEMY_HITBOX_ARM.ry/ENEMY_HITBOX_ARM.rx, 0)
			||
			collideOblique(enemy.abscisses[2], enemy.abscisses[3], cos(enemy.coefs[0] * PI / 180) * ENEMY_ADDITIONAL_HITBOX_ARM.rx, sin(enemy.coefs[0] * PI / 180) * ENEMY_ADDITIONAL_HITBOX_ARM.rx, ENEMY_ADDITIONAL_HITBOX_ARM.ry/ENEMY_ADDITIONAL_HITBOX_ARM.rx,
				missile.x, missile.y, cos(missile.rota) * MISSILE_HITBOX[missile.id].rx , sin(missile.rota) * MISSILE_HITBOX[missile.id].rx, MISSILE_HITBOX[missile.id].ry/MISSILE_HITBOX[missile.id].rx, 1)))
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

static float calculer_angle(float x1, float y1, float x2, float y2){
	return atan2((y2-y1) , (x2-x1));
}

void enemyFire(Enemy *enemy, Missile **enemyMissiles, int *nbEnemyMissiles, Ship ship){
	for(int i = 0; i <= enemy->nbWeapon; i++){
		while(enemy->weapons[i].frame_reload == 0){
			int wrongMissile = SDL_FALSE;
			*enemyMissiles = realloc(*enemyMissiles, ++(*nbEnemyMissiles)  *  sizeof(Missile));
			(*enemyMissiles)[*nbEnemyMissiles - 1].x = ((enemy->id == ARM_ENEMY) ? (enemy->coefs[2]) : (enemy->x + WEAPON_DEST_ENEMY[enemy->id][i].x));
			(*enemyMissiles)[*nbEnemyMissiles - 1].y = ((enemy->id == ARM_ENEMY) ? (enemy->coefs[3]) : (enemy->y + WEAPON_DEST_ENEMY[enemy->id][i].y));
			(*enemyMissiles)[*nbEnemyMissiles - 1].id = enemy->weapons[i].id;
			(*enemyMissiles)[*nbEnemyMissiles - 1].rota = PI/2 + (TYPE_ENEMY_FIRE[enemy->id][i] == AIMED ? calculer_angle((*enemyMissiles)[*nbEnemyMissiles - 1].x, (*enemyMissiles)[*nbEnemyMissiles - 1].y, ship.x + SHIP_HITBOX[ship.form].x, ship.y+ SHIP_HITBOX[ship.form].y) : PI/2);
			(*enemyMissiles)[*nbEnemyMissiles - 1].rota += (ANGLE_ENEMY_FIRE[enemy->id][i] ? (randSign() * ((rand()%(int)(PRECISION_RAND_FLOAT*ANGLE_ENEMY_FIRE[enemy->id][i])) / PRECISION_RAND_FLOAT)) : 0);

			if(enemy->id == ARM_ENEMY)
				(*enemyMissiles)[*nbEnemyMissiles - 1].rota = enemy->coefs[0] * PI / 180 + PI;
			else if(enemy->id == ARM_BOSS_ENEMY){
				(*enemyMissiles)[*nbEnemyMissiles - 1].rota = (i==0?1:-1)*ANGLE_ENEMY_ARM_BOSS_FIRE*(enemy->weapons[i].combo + (i==0?1:0)) * PI / 180 + PI + enemy->infos[0];//+ (i==0?-1:1) * BASE_ANGLE_ARM_BOSS_FIRE;
				while((*enemyMissiles)[*nbEnemyMissiles - 1].rota < -PI && i==1)
					(*enemyMissiles)[*nbEnemyMissiles - 1].rota += 2*PI;

				while((*enemyMissiles)[*nbEnemyMissiles - 1].rota >= PI  && i==0)
					(*enemyMissiles)[*nbEnemyMissiles - 1].rota -= 2*PI;

					while((*enemyMissiles)[*nbEnemyMissiles - 1].rota >= PI+PI/50  && i==1)
						(*enemyMissiles)[*nbEnemyMissiles - 1].rota -= 2*PI;

				if(((*enemyMissiles)[*nbEnemyMissiles - 1].rota > -PI/2 + PI/18  && i == 0) || ( (*enemyMissiles)[*nbEnemyMissiles - 1].rota < PI/2 - PI/18  && i == 1)){
					(*nbEnemyMissiles)--;
					if(*nbEnemyMissiles)
						*enemyMissiles = realloc(*enemyMissiles, *nbEnemyMissiles  *  sizeof(Missile));
					wrongMissile = SDL_TRUE;
				}
			}


			if(!wrongMissile){
				(*enemyMissiles)[*nbEnemyMissiles - 1].damage = 1;
				(*enemyMissiles)[*nbEnemyMissiles - 1].frame = 0;
				(*enemyMissiles)[*nbEnemyMissiles - 1].speed = MISSILE_SPEED_ENEMY[enemy->id][i];
			}

			if(enemy->weapons[i].combo){
				enemy->weapons[i].frame_reload = RELOAD_FRAME_COMBO_ENEMY[enemy->id][i];
				enemy->weapons[i].combo--;
			}
			else{
				if(RELOAD_FRAME_RAND_ENEMY[enemy->id][i] >0){
					enemy->weapons[i].frame_reload = RELOAD_FRAME_ENEMY[enemy->id][i] + rand()%RELOAD_FRAME_RAND_ENEMY[enemy->id][i];
					if(enemy->id == ARM_BOSS_ENEMY){
						enemy->weapons[i].frame_reload += enemy->infos[1] * NERF_FREQUENCY_PER_ARM;
						enemy->infos[0]+= BASE_ANGLE_ARM_BOSS_FIRE;
					}

				}
				else{
					enemy->weapons[i].frame_reload = enemy->weapons[-RELOAD_FRAME_RAND_ENEMY[enemy->id][i]].frame_reload;
				} //symetry


				//combo reset
				if(COMBO_RAND_ENEMY[enemy->id][i] > 0)
					enemy->weapons[i].combo = COMBO_ENEMY[enemy->id][i] + rand()%COMBO_RAND_ENEMY[enemy->id][i];
				else
					enemy->weapons[i].combo = enemy->weapons[-COMBO_RAND_ENEMY[enemy->id][i]].combo;
			}
		}
	}
}

void drawMissile(SDL_Renderer *renderer, Missile missile, SDL_Texture *textureMissile){
	SDL_Rect dest = MISSILE_SRC[missile.id];
	dest.w *= RATIO_SIZE_MISSILE[missile.id];
	dest.h *= RATIO_SIZE_MISSILE[missile.id];
	dest.x = missile.x - MISSILE_CENTER[missile.id].x;
	dest.y = missile.y - MISSILE_CENTER[missile.id].y ;

	SDL_Rect src = MISSILE_SRC[missile.id];
	if(missile.id == W_BASE_ENEMY && missile.frame%4 < 2){
		src.y += src.h ;
	}


	SDL_RenderCopyEx(renderer, textureMissile, &src, &dest, missile.rota * 180/PI, &MISSILE_CENTER[missile.id], SDL_FLIP_NONE);

	SDL_SetRenderDrawColor(renderer, 0,255,0,255);
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
	if(!explosion.delay){
		SDL_Rect src = EXPLO_SRCS[explosion.id];
		src.x += (int)((FRAME_EXPLOSIONS[explosion.id] - explosion.frame) / ((float)FRAME_EXPLOSIONS[explosion.id] /NB_ANIM_EXPLOSIONS[explosion.id])) * src.w;
		SDL_Rect dest = {explosion.x, explosion.y, explosion.taille, explosion.taille};

		SDL_RenderCopy(renderer, explo_texture, &src, &dest);
	}
}

int moveMissile(Missile **missiles , int *nbMissiles, int iMissile){
	(*missiles)[iMissile].x += (*missiles)[iMissile].speed * cos((*missiles)[iMissile].rota - PI/2);
	(*missiles)[iMissile].y += (*missiles)[iMissile].speed * sin((*missiles)[iMissile].rota - PI/2);
	((*missiles)[iMissile].frame)++;

	if((*missiles)[iMissile].x < BACKGROUND_DEST.x - (MISSILE_SRC[(*missiles)[iMissile].id].w * RATIO_SIZE_MISSILE[(*missiles)[iMissile].id])/2 - 1  || (*missiles)[iMissile].x > BACKGROUND_DEST.x + BACKGROUND_DEST.w + (MISSILE_SRC[(*missiles)[iMissile].id].w * RATIO_SIZE_MISSILE[(*missiles)[iMissile].id])/2+ 1
			|| (*missiles)[iMissile].y < (-MISSILE_SRC[(*missiles)[iMissile].id].h * RATIO_SIZE_MISSILE[(*missiles)[iMissile].id])/2 - 1 || (*missiles)[iMissile].y > BACKGROUND_DEST.h +(MISSILE_SRC[(*missiles)[iMissile].id].h * RATIO_SIZE_MISSILE[(*missiles)[iMissile].id])/2 +1){

        shiftMissile(*missiles, *nbMissiles, iMissile);
        (*nbMissiles)--;
        if(*nbMissiles !=0){
			*missiles=realloc(*missiles, sizeof(Missile)* *nbMissiles );
			return SDL_TRUE;
		}



    }

	return SDL_FALSE;
}

int moveEnemy(Enemy **enemies, int * nbEnemy, int iEnemy){
	if((*enemies)[iEnemy].frameWait == 0){
		if(TYPE_MOVE_ENEMY[(*enemies)[iEnemy].id] == INTERPOLATION){
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
		else if(TYPE_MOVE_ENEMY[(*enemies)[iEnemy].id] == ROUND_TRIP){
			(*enemies)[iEnemy].x += (*enemies)[iEnemy].dir * SPEED_ENEMY[(*enemies)[iEnemy].id];
			if((*enemies)[iEnemy].nbMove && (((*enemies)[iEnemy].dir == 1 &&(*enemies)[iEnemy].x + ENEMY_SRC[(*enemies)[iEnemy].id].w / RATIO_SIZE_ENEMY[(*enemies)[iEnemy].id] > (2-RESET_MOVE_ENEMY[(*enemies)[iEnemy].id])*BACKGROUND_DEST.w) || ((*enemies)[iEnemy].dir == -1 && (*enemies)[iEnemy].x < (1+RESET_MOVE_ENEMY[(*enemies)[iEnemy].id]) * BACKGROUND_DEST.w))){
				(*enemies)[iEnemy].dir *= -1;
				(*enemies)[iEnemy].nbMove--;
			}
		}
		else if( TYPE_MOVE_ENEMY[(*enemies)[iEnemy].id] == BESACE){

			(*enemies)[iEnemy].coefs[0]+= (*enemies)[iEnemy].dir * PI/90;
			while((*enemies)[iEnemy].coefs[0] > 2* PI ){
				(*enemies)[iEnemy].coefs[0] -= 2 * PI;
				(*enemies)[iEnemy].nbMove--;
			}
			while((*enemies)[iEnemy].coefs[0] < 0 ){
				(*enemies)[iEnemy].coefs[0] += 2 * PI;
				(*enemies)[iEnemy].nbMove--;
			}


			(*enemies)[iEnemy].coefs[1] += (*enemies)[iEnemy].coefs[3] * (*enemies)[iEnemy].abscisses[2] / ((*enemies)[iEnemy].nbMove > 0 ? 5 : 1); // shift x
			if((*enemies)[iEnemy].nbMove > 0 && ((*enemies)[iEnemy].coefs[1] > 100 || (*enemies)[iEnemy].coefs[1] < -100))
				(*enemies)[iEnemy].coefs[3] *= -1;

			(*enemies)[iEnemy].x = BASE_WINDOW_W/2 - (ENEMY_SRC[(*enemies)[iEnemy].id].w / RATIO_SIZE_ENEMY[(*enemies)[iEnemy].id])/2 +(*enemies)[iEnemy].abscisses[0] * cos((*enemies)[iEnemy].coefs[0]) - (*enemies)[iEnemy].abscisses[1] * sin((*enemies)[iEnemy].coefs[0]);
			(*enemies)[iEnemy].y = (*enemies)[iEnemy].coefs[2] - sin((*enemies)[iEnemy].coefs[0]) * ((*enemies)[iEnemy].x - BASE_WINDOW_W/2 + (ENEMY_SRC[(*enemies)[iEnemy].id].w / RATIO_SIZE_ENEMY[(*enemies)[iEnemy].id])/2);

			(*enemies)[iEnemy].x += (*enemies)[iEnemy].coefs[1];

			if((*enemies)[iEnemy].coefs[2] < MAX_Y_BOSS)
				(*enemies)[iEnemy].coefs[2] += MOVE_Y_BOSS;
		}
		else if( TYPE_MOVE_ENEMY[(*enemies)[iEnemy].id] == ARM){

			SDL_Point rotaPoint = ROTATION_POINT_ARM;
			rotaPoint.x -= ENEMY_CENTER[(*enemies)[iEnemy].id].x;
			rotaPoint.y -= ENEMY_CENTER[(*enemies)[iEnemy].id].y;

			float xM = rotaPoint.x;
			float yM = rotaPoint.y;
			rotaPoint.x = xM*cos((*enemies)[iEnemy].rota * PI / 180 ) - yM * sin((*enemies)[iEnemy].rota * PI / 180);
			rotaPoint.y = xM * sin((*enemies)[iEnemy].rota * PI / 180) + yM * cos((*enemies)[iEnemy].rota * PI / 180);
			rotaPoint.x += DIST_ROTA_REAL_HITBOX_ARM * cos(((*enemies)[iEnemy].rota + 60) * PI / 180);
			rotaPoint.y += DIST_ROTA_REAL_HITBOX_ARM * sin(((*enemies)[iEnemy].rota + 60) * PI / 180);



			if((*enemies)[iEnemy].infos[2] > 0){
				float smooth = 1;

				if((*enemies)[iEnemy].infos[4] < NB_FRAME_WARMUP)
					smooth /= abs((*enemies)[iEnemy].infos[4] - NB_FRAME_WARMUP);

				if((*enemies)[iEnemy].infos[2] < NB_FRAME_WARMUP)
					smooth /= NB_FRAME_WARMUP - (*enemies)[iEnemy].infos[2] +1;

				(*enemies)[iEnemy].rota += smooth * (((*enemies)[iEnemy].infos[0] - (*enemies)[iEnemy].rota ) / (*enemies)[iEnemy].infos[2]);
				(*enemies)[iEnemy].coefs[0] += smooth * (((*enemies)[iEnemy].infos[1] - (*enemies)[iEnemy].coefs[0]) / (*enemies)[iEnemy].infos[2]);
			}
			else if((*enemies)[iEnemy].infos[2] == FRAME_RESET_ARM_MOVE){
				(*enemies)[iEnemy].infos[4] = -1;


				double info0 = (*enemies)[iEnemy].infos[0], info1 = (*enemies)[iEnemy].infos[1];

				if((*enemies)[iEnemy].infos[3]){
					(*enemies)[iEnemy].infos[2] = FRAME_ARM_MOVE + 1;
					while(abs(info0 - (*enemies)[iEnemy].infos[0]) < MIN_ANGLE_ARM || abs(info0 - (*enemies)[iEnemy].infos[0]) > MAX_ANGLE_ARM)
						(*enemies)[iEnemy].infos[0] = (*enemies)[iEnemy].dir * (-15 - rand()%51); //entre -15 et -65

					while(abs(info1 - (*enemies)[iEnemy].infos[1]) < MIN_ANGLE_AVANT_ARM || abs(info1 - (*enemies)[iEnemy].infos[1]) > MAX_ANGLE_AVANT_ARM)
						(*enemies)[iEnemy].infos[1] = (*enemies)[iEnemy].dir * (15 - rand()%71); //entre 15 et -55 -

					(*enemies)[iEnemy].infos[1] -= RATIO_ADAPT_ANGLE_ARM * (*enemies)[iEnemy].dir * (40 - abs((*enemies)[iEnemy].infos[0]));
				}
				else{
					(*enemies)[iEnemy].infos[2] = FRAME_ARM_ROTATE + 1;
					if(((*enemies)[iEnemy].dir == 1 && (*enemies)[iEnemy].infos[1] > -20) || ((*enemies)[iEnemy].dir == -1 && (*enemies)[iEnemy].infos[1] < 20))
						(*enemies)[iEnemy].infos[1] -= (*enemies)[iEnemy].dir * ANGLE_TURN_BEAM_ARM;
					else
						(*enemies)[iEnemy].infos[1] += (*enemies)[iEnemy].dir * ANGLE_TURN_BEAM_ARM;

					(*enemies)[iEnemy].infos[2] = FRAME_ARM_BEAM_MOVE + 1;


				}

			}
			else if((*enemies)[iEnemy].infos[2] == FRAME_SHOOT_ARM){
				(*enemies)[iEnemy].infos[3]--;
				if(!(*enemies)[iEnemy].infos[3])
					(*enemies)[iEnemy].infos[5]= 0;
				else if((*enemies)[iEnemy].infos[3] > 0)
					(*enemies)[iEnemy].weapons[0].frame_reload = 0;
				else if((*enemies)[iEnemy].infos[3] < 0){
					if(iEnemy>0 && (*enemies)[iEnemy-1].id == (*enemies)[iEnemy].id)
						(*enemies)[iEnemy].infos[3] = (*enemies)[iEnemy-1].infos[3];
					else
						(*enemies)[iEnemy].infos[3] = NB_SMALL_ATK_ARM + rand()%RAND_SMALL_ATK_ARM;
				}
			}


			(*enemies)[iEnemy].infos[2]--;
			(*enemies)[iEnemy].infos[4]++;


			SDL_Point rotaPointAfter = ROTATION_POINT_ARM;
			rotaPointAfter.x -= ENEMY_CENTER[(*enemies)[iEnemy].id].x;
			rotaPointAfter.y -= ENEMY_CENTER[(*enemies)[iEnemy].id].y;

			xM = rotaPointAfter.x;
			yM = rotaPointAfter.y;
			rotaPointAfter.x = xM*cos((*enemies)[iEnemy].rota * PI / 180 ) - yM * sin((*enemies)[iEnemy].rota * PI / 180);
			rotaPointAfter.y = xM * sin((*enemies)[iEnemy].rota * PI / 180) + yM * cos((*enemies)[iEnemy].rota * PI / 180);
			rotaPointAfter.x += DIST_ROTA_REAL_HITBOX_ARM * cos(((*enemies)[iEnemy].rota + 60) * PI / 180);
			rotaPointAfter.y += DIST_ROTA_REAL_HITBOX_ARM * sin(((*enemies)[iEnemy].rota + 60) * PI / 180);

			(*enemies)[iEnemy].x -= (rotaPoint.x - rotaPointAfter.x);
			(*enemies)[iEnemy].y -= (rotaPoint.y - rotaPointAfter.y);


		}
	}

	if(!(TYPE_MOVE_ENEMY[(*enemies)[iEnemy].id] == BESACE && (*enemies)[iEnemy].coefs[2] < MAX_Y_BOSS))
		if((*enemies)[iEnemy].x < BACKGROUND_DEST.x - ENEMY_SRC[(*enemies)[iEnemy].id].w / RATIO_SIZE_ENEMY[(*enemies)[iEnemy].id] - 1  || (*enemies)[iEnemy].x > BACKGROUND_DEST.x + BACKGROUND_DEST.w + 1
				|| (*enemies)[iEnemy].y < -ENEMY_SRC[(*enemies)[iEnemy].id].h / RATIO_SIZE_ENEMY[(*enemies)[iEnemy].id] - 1 || (*enemies)[iEnemy].y > BACKGROUND_DEST.h +1){

			shiftEnemy(*enemies, *nbEnemy, iEnemy);
			(*nbEnemy)--;

			if(*nbEnemy !=0){
				*enemies=realloc(*enemies, sizeof(Enemy)* *nbEnemy );
				return SDL_TRUE;
			}


		}

	return SDL_FALSE;
}

int moveEnemyParts(EnemyPart **enemyParts, int * nbEnemyParts, int i){

	(*enemyParts)[i].x += (*enemyParts)[i].dirX;
	(*enemyParts)[i].y += (*enemyParts)[i].dirY;

	if((*enemyParts)[i].id%NB_ARM_COMPONENT > 4)
		(*enemyParts)[i].rota[0] += (*enemyParts)[i].dirX;
	else
		(*enemyParts)[i].rota[1] += (*enemyParts)[i].dirX;

	(*enemyParts)[i].dirY -= GRAVITY_ENEMY_PARTS;

	return SDL_FALSE;
}

void spawnEnemy(Enemy ** enemies, int *nbEnemy, int id, int nbSpawn){
	*nbEnemy += nbSpawn;
	*enemies = realloc( *enemies, *nbEnemy * sizeof(Enemy));
	//Polynomial interpolation
	double x[NMAX];
	double f[NMAX];
	double d[NMAX];
	if(TYPE_MOVE_ENEMY[id] == INTERPOLATION){
		int path = rand()%3;
		if(path == 2){
			x[0] = BACKGROUND_DEST.x - ENEMY_SRC[BASE_ENEMY].w/RATIO_SIZE_ENEMY[BASE_ENEMY];
			f[0] = rand()%360+90;
			for(int i=1; i<NMAX; i++){
				x[i] = x[i-1] + (BACKGROUND_DEST.w + ENEMY_SRC[BASE_ENEMY].w/RATIO_SIZE_ENEMY[BASE_ENEMY])/3;
				f[i] = rand()%350+100;
			}
			it_coef_lagrange(3,x,f,d);
		}
		else if(path ==1){
			x[0] = BACKGROUND_DEST.x - ENEMY_SRC[BASE_ENEMY].w/RATIO_SIZE_ENEMY[BASE_ENEMY];
			f[0] = rand()%360+90;
			x[NMAX-1] = 2*BACKGROUND_DEST.w - rand()%120 -10 - ENEMY_SRC[BASE_ENEMY].w/RATIO_SIZE_ENEMY[BASE_ENEMY];
			f[NMAX-1] = -ENEMY_SRC[BASE_ENEMY].h/RATIO_SIZE_ENEMY[BASE_ENEMY];
			for(int i=NMAX-2; i>0; i--){
				x[i] = x[i+1] - (2*BACKGROUND_DEST.w - x[0] )/3;
				f[i] = rand()%360+90;
			}
			it_coef_lagrange(3,x,f,d);
		}
		else {
			x[0] = BACKGROUND_DEST.w + rand()%120 +10;
			f[0] = -ENEMY_SRC[BASE_ENEMY].h/RATIO_SIZE_ENEMY[BASE_ENEMY];
			for(int i=1; i<NMAX; i++){
				x[i] = x[i-1] + (2*BACKGROUND_DEST.w - x[0] )/3;
				f[i] = rand()%360+90;
			}
			it_coef_lagrange(3,x,f,d);
		}
	}

    int dir = randSign();

	for(int i = *nbEnemy-1; i> *nbEnemy - 1 - nbSpawn; i--){
		if(id == ARM_ENEMY)
			dir *= -1;

		(*enemies)[i] = (Enemy){
			id, //id
			BACKGROUND_DEST.x - ENEMY_SRC[id].w/RATIO_SIZE_ENEMY[id], //x
			SPAWN_ENEMY_Y[id], //y
			0, //rota
			ENEMY_HP[id], //hp
			0, // framehit
			0, //dist
			FRAME_MULTI_SPAWN[id] * (*nbEnemy-1 -i),//frameWait
			dir, //dir
			NB_MOVE_ENEMY[id], //nbMove
			NB_WEAPON_ENEMY[id] //nbWeapon
		};

		for(int j=0; j<= (*enemies)[i].nbWeapon; j++){
			(*enemies)[i].weapons[j] = WEAPONS_ENEMY[id][j];
			if(RELOAD_FRAME_RAND_ENEMY[(*enemies)[i].id][j] > 0)
				(*enemies)[i].weapons[j].frame_reload +=  FRAME_MULTI_SPAWN[id] * (*nbEnemy-1 -i) + rand()%RELOAD_FRAME_RAND_ENEMY[(*enemies)[i].id][j];
			else
				(*enemies)[i].weapons[j].frame_reload = (*enemies)[i].weapons[-RELOAD_FRAME_RAND_ENEMY[(*enemies)[i].id][j]].frame_reload;

			if(COMBO_RAND_ENEMY[(*enemies)[i].id][j] > 0)
				(*enemies)[i].weapons[j].combo = COMBO_ENEMY[(*enemies)[i].id][j] + rand()%COMBO_RAND_ENEMY[(*enemies)[i].id][j];
			else
				(*enemies)[i].weapons[j].combo = (*enemies)[i].weapons[-COMBO_RAND_ENEMY[(*enemies)[i].id][j]].combo;
		}

		if(TYPE_MOVE_ENEMY[id] == INTERPOLATION){
			for(int j=0; j<NMAX; j++){
				(*enemies)[i].abscisses[j] = x[j];
				(*enemies)[i].coefs[j] = d[j];
			}
			(*enemies)[i].x = x[(*enemies)[i].dir == 1 ? 0: NMAX-1];
			(*enemies)[i].y = f[(*enemies)[i].dir == 1 ? 0: NMAX-1];
		}
		else if(TYPE_MOVE_ENEMY[id] == ROUND_TRIP){
			if(dir == -1){
				(*enemies)[i].x += BACKGROUND_DEST.w +ENEMY_SRC[id].w/RATIO_SIZE_ENEMY[id];
			}
		}
		else if(TYPE_MOVE_ENEMY[id] == BESACE){/*https://fr.wikipedia.org/wiki/Besace_(math%C3%A9matiques)*/
			(*enemies)[i].abscisses[0] = 50 + rand()%80;//a
			(*enemies)[i].abscisses[1] = 40 + rand()%60;//b
			(*enemies)[i].abscisses[2] = rand()%2 + 2; //x ammount
			(*enemies)[i].coefs[0] = (rand()%(int)( 2*PI * PRECISION_RAND_FLOAT))/PRECISION_RAND_FLOAT; //t
			(*enemies)[i].coefs[1] = rand()%201 -100; //dx
			(*enemies)[i].coefs[2] = -220; //dy
			(*enemies)[i].coefs[3] = randSign(); //dx
			(*enemies)[i].x = BASE_WINDOW_W/2 + (*enemies)[i].coefs[1] - (ENEMY_SRC[(*enemies)[i].id].w / RATIO_SIZE_ENEMY[(*enemies)[i].id])/2 +(*enemies)[i].abscisses[0] * cos((*enemies)[i].coefs[0]) - (*enemies)[i].abscisses[1] * sin((*enemies)[i].coefs[0]);
			(*enemies)[i].y = (*enemies)[i].coefs[2] - sin((*enemies)[i].coefs[0]) * ((*enemies)[i].x - BASE_WINDOW_W/2 + (ENEMY_SRC[(*enemies)[i].id].w / RATIO_SIZE_ENEMY[(*enemies)[i].id])/2);

		}

		if(id == ARM_ENEMY){
			(*enemies)[i].x = BACKGROUND_DEST.x - SHIFT_X_ARM / RATIO_SIZE_ENEMY[id];
			if((*enemies)[i].dir == -1)
				(*enemies)[i].x +=  BACKGROUND_DEST.w + (2*SHIFT_X_ARM) / RATIO_SIZE_ENEMY[id] - ENEMY_SRC[id].w/RATIO_SIZE_ENEMY[id];//+ (2 * SHIFT_X_ARM) / RATIO_SIZE_ENEMY[(*enemies)[i].id] ;//+ ENEMY_SRC[(*enemies)[i].id].w / RATIO_SIZE_ENEMY[(*enemies)[i].id];
			(*enemies)[i].rota = -(*enemies)[i].dir * 30;
			(*enemies)[i].coefs[0] = -(*enemies)[i].dir * 50; // angle avant bras
			(*enemies)[i].infos[0] = (*enemies)[i].rota; //angle dest
			(*enemies)[i].infos[1] = (*enemies)[i].coefs[0]; // angle dest avant bras
			(*enemies)[i].infos[2] = 0; // frame turn
			(*enemies)[i].infos[3] = NB_SMALL_ATK_ARM + rand()%RAND_SMALL_ATK_ARM; // nbturn
			(*enemies)[i].infos[4]= 0; //smooth warmup
			(*enemies)[i].infos[5]= -1; //laserbeam
			if(i<*nbEnemy-1 && (*enemies)[i+1].id == id)
				(*enemies)[i].infos[3] = (*enemies)[i+1].infos[3];
		}

		if(id == ARM_BOSS_ENEMY){
			(*enemies)[i].infos[0] = 0;
			(*enemies)[i].infos[1] = 2;
			(*enemies)[i].x = BACKGROUND_DEST.x;
		}
	}
}



void myFrees(Enemy ** enemies, Missile ** allyMissiles, Missile ** enemyMissiles){
	if(*enemies)
		free(*enemies);

	if(*allyMissiles)
		free(*allyMissiles);

	if(*enemyMissiles)
		free(*enemyMissiles);
}

void armExplosion(Enemy enemy, Explosion ** explosions, int *nbExplosions){
	SDL_Point rotaPoint = ROTATION_POINT_ARM;
	rotaPoint.x -= ENEMY_CENTER[enemy.id].x;
	rotaPoint.y -= ENEMY_CENTER[enemy.id].y;

	float xM = rotaPoint.x;
	float yM = rotaPoint.y;
	rotaPoint.x = xM*cos(enemy.rota * PI / 180 ) - yM * sin(enemy.rota * PI / 180);
	rotaPoint.y = xM * sin(enemy.rota * PI / 180) + yM * cos(enemy.rota * PI / 180);
	rotaPoint.x += enemy.x + ENEMY_CENTER[enemy.id].x;
	rotaPoint.y += enemy.y + ENEMY_CENTER[enemy.id].y;
	/*rotaPoint.x -= DIST_ROTA_HITBOX_BASIC_ARM / RATIO_SIZE_ENEMY[enemy.id] * cos(enemy.rota * PI / 180 + PI/2);
	rotaPoint.y -= DIST_ROTA_HITBOX_BASIC_ARM / RATIO_SIZE_ENEMY[enemy.id] * sin(enemy.rota * PI / 180 + PI/2);*/

	*nbExplosions += NB_ARM_EXPLOSIONS;
	*explosions = realloc(*explosions, *nbExplosions * sizeof(Explosion));
	int j;
	for(int i=*nbExplosions-NB_ARM_EXPLOSIONS, j=0; i<=*nbExplosions-1; i++, j++){
		(*explosions)[i].id = EXPLO_SHIP;
		(*explosions)[i].taille = SIZE_ARM_EXPLO[j];
		int rota = (SIZE_ARM_EXPLO[j]==0?enemy.rota:enemy.coefs[0]);
		(*explosions)[i].x = rotaPoint.x + (PART_ARM_EXPLO[j]==0?(-DIST_ROTA_HITBOX_BASIC_ARM):DIST_ROTA_CENTER_HITBOX_ARM) / RATIO_SIZE_ENEMY[enemy.id] * cos(rota * PI / 180 + PI/2)
		 								 + EXPLO_COOR[j].x * cos(rota * PI / 180 + PI/2) + EXPLO_COOR[j].y * sin(rota * PI / 180) - (*explosions)[i].taille/2;
		(*explosions)[i].y = rotaPoint.y + (PART_ARM_EXPLO[j]==0?(-DIST_ROTA_HITBOX_BASIC_ARM):DIST_ROTA_CENTER_HITBOX_ARM) / RATIO_SIZE_ENEMY[enemy.id] * sin(rota * PI / 180 + PI/2)
		 								 + EXPLO_COOR[j].x * sin(rota * PI / 180 + PI/2) + EXPLO_COOR[j].y * cos(rota * PI / 180) - (*explosions)[i].taille/2;
		(*explosions)[i].frame = FRAME_EXPLOSIONS[(*explosions)[i].id];
		(*explosions)[i].delay = rand()%16+1;
	}
}
void armBossExplosion(Explosion ** explosions, int *nbExplosions){
	*nbExplosions += NB_ARM_BOSS_EXPLO;
	*explosions = realloc(*explosions, *nbExplosions * sizeof(Explosion));
	for(int i=*nbExplosions-NB_ARM_BOSS_EXPLO; i<=*nbExplosions-1; i++){
		(*explosions)[i].id = EXPLO_SHIP;
		(*explosions)[i].taille = MIN_SIZE_EXPLO_BOSS + rand()%(int)RAND_SIZE_EXPLO_BOSS;
		(*explosions)[i].x = BACKGROUND_DEST.x + rand()%(BACKGROUND_DEST.w + 1) - (*explosions)[i].taille/2;
		(*explosions)[i].y = rand()%MAX_Y_EXPLO_BOSS - (*explosions)[i].taille/2;
		(*explosions)[i].frame = FRAME_EXPLOSIONS[(*explosions)[i].id];
		(*explosions)[i].delay = rand()%24+2;
	}
}

void armPartExplosion(Enemy enemy, Explosion ** explosions, int *nbExplosions, int idPart){
	SDL_Point rotaPoint = ROTATION_POINT_ARM;
	rotaPoint.x -= ENEMY_CENTER[enemy.id].x;
	rotaPoint.y -= ENEMY_CENTER[enemy.id].y;

	float xM = rotaPoint.x;
	float yM = rotaPoint.y;
	rotaPoint.x = xM*cos(enemy.rota * PI / 180 ) - yM * sin(enemy.rota * PI / 180);
	rotaPoint.y = xM * sin(enemy.rota * PI / 180) + yM * cos(enemy.rota * PI / 180);
	rotaPoint.x += enemy.x + ENEMY_CENTER[enemy.id].x;
	rotaPoint.y += enemy.y + ENEMY_CENTER[enemy.id].y;
	rotaPoint.x += DIST_ARM_PART[idPart] / RATIO_SIZE_ENEMY[enemy.id] * cos((PART_ARM_SIDE[idPart]==0?enemy.rota:enemy.coefs[0]) * PI / 180 + PI/2);
	rotaPoint.y += DIST_ARM_PART[idPart] / RATIO_SIZE_ENEMY[enemy.id] * sin((PART_ARM_SIDE[idPart]==0?enemy.rota:enemy.coefs[0]) * PI / 180 + PI/2);

	(*nbExplosions)++;
	*explosions = realloc(*explosions, *nbExplosions * sizeof(Explosion));
	(*explosions)[*nbExplosions -1].id = EXPLO_SHIP;
	(*explosions)[*nbExplosions -1].taille = PART_ARM_SIZE[idPart] / RATIO_SIZE_ENEMY[enemy.id];
	(*explosions)[*nbExplosions -1].x = rotaPoint.x - (*explosions)[*nbExplosions -1].taille/2;
	(*explosions)[*nbExplosions -1].y = rotaPoint.y - (*explosions)[*nbExplosions -1].taille/2;
	(*explosions)[*nbExplosions -1].frame = FRAME_EXPLOSIONS[(*explosions)[*nbExplosions -1].id];
	(*explosions)[*nbExplosions -1].delay = 0;
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
	int rdyToTab = SDL_FALSE;

	SDL_Rect backgroundSrc[3] = {BACKGROUND_SRC, BACKGROUND_SRC, BACKGROUND_SRC};
	backgroundSrc[1].x += 3* backgroundSrc[1].w;
	backgroundSrc[2].x += 6* backgroundSrc[2].w;

	SDL_Rect backgroundDest[3] = {BACKGROUND_DEST, BACKGROUND_DEST, BACKGROUND_DEST};


	Missile * allyMissiles = malloc(sizeof(Missile));
	Missile * enemyMissiles = malloc(sizeof(Missile));

	Enemy *enemies = malloc(sizeof(Enemy));
	EnemyPart * enemyParts = malloc(sizeof(EnemyPart));
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
		int nbEnemyParts = 0;

		int nbAllyMissiles = 0;
		int nbEnemyMissiles = 0;
		int nbExplosions = 0;

		 // // // // // // // //
		//   Initialize vars   //
		 // // // // // // // //
		 int r = 0;
		 spawnEnemy(&enemies, &nbEnemy, ARM_BOSS_ENEMY, 1);
		 spawnEnemy(&enemies, &nbEnemy, ARM_ENEMY, 2);
	 // // // // // // //
	//   BOUCLE DU JEU  //``
	 // // // // // // //
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
				myFrees(&enemies, &allyMissiles, &enemyMissiles);
				return 0;
			}

			ship.dir = 0;
			if( keystate[SDL_SCANCODE_LEFT] || keystate[SDL_SCANCODE_A] ){
				ship.x-=SHIP_SPEED;
				ship.dir = 1;

				if(ship.x + DIST_BORD_VAISS.x < BACKGROUND_DEST.x)
					ship.x = BACKGROUND_DEST.x - DIST_BORD_VAISS.x;
			}
			else if( keystate[SDL_SCANCODE_RIGHT] || keystate[SDL_SCANCODE_D]){
				ship.x+=SHIP_SPEED;
				ship.dir = 2;

				if(ship.x + DIST_BORD_VAISS.w > BACKGROUND_DEST.x + BACKGROUND_DEST.w)
					ship.x = BACKGROUND_DEST.x + BACKGROUND_DEST.w - DIST_BORD_VAISS.w;
			}

			if( keystate[SDL_SCANCODE_UP] || keystate[SDL_SCANCODE_W]){
				ship.y-=SHIP_SPEED;
				ship.dir += 3;

				if(ship.y + DIST_BORD_VAISS.y < BACKGROUND_DEST.y)
					ship.y = BACKGROUND_DEST.y - DIST_BORD_VAISS.y;
			}
			else if( keystate[SDL_SCANCODE_DOWN] || keystate[SDL_SCANCODE_S]){
				ship.y += RATIO_SPEED_DOWN * SHIP_SPEED;
				ship.dir += 6;

				if(ship.y + DIST_BORD_VAISS.h > BACKGROUND_DEST.y + BACKGROUND_DEST.h)
					ship.y = BACKGROUND_DEST.y + BACKGROUND_DEST.h - DIST_BORD_VAISS.h;
			}

			if( keystate[SDL_SCANCODE_SPACE] || keystate[SDL_SCANCODE_LCTRL]){
				fire(&ship, &allyMissiles, &nbAllyMissiles);
			}


			if( keystate[SDL_SCANCODE_TAB] && rdyToTab){
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
				rdyToTab = SDL_FALSE;
			}
			else if (!keystate[SDL_SCANCODE_TAB]){
				rdyToTab = SDL_TRUE;
			}

		// // // // //
		// Gameplay //`
		// // // // //
		if(nbEnemy == 0){

		}

		for(int i=0; i<nbEnemy; i++)
			enemyFire(&(enemies[i]), &enemyMissiles, &nbEnemyMissiles, ship);
		//move missiles
		for(int i=0; i<nbAllyMissiles; i++)
			if(moveMissile(&allyMissiles, &nbAllyMissiles, i))
				i--;

		for(int i=0; i<nbEnemyMissiles; i++)
			if(moveMissile(&enemyMissiles, &nbEnemyMissiles, i))
				i--;

		for(int i=0; i<nbEnemy; i++){
			if(moveEnemy(&enemies, &nbEnemy, i))
				i--;
		}

		for(int i=0; i<nbEnemyParts; i++)
			if(moveEnemyParts(&enemyParts, &nbEnemyParts, i))
				i--;


		// // // // //
		// Hitbox //`
		// // // // //
			SDL_SetTextureColorMod(textures[SH_SHIP],255,255,255);


			//ship-missiles
			for(int i=0; i< nbEnemyMissiles; i++){
				if(missileHitShip(enemyMissiles[i], ship) )
					SDL_SetTextureColorMod(textures[SH_SHIP],255,150,0);
			}

			//laser
			for(int i=0; i<nbEnemy; i++)
				if( enemies[i].id == ARM_ENEMY && laserHitShip(ship, enemies[i]) )
					SDL_SetTextureColorMod(textures[SH_SHIP],255,150,0);
			//ship-enemy

			for(int i=0; i<nbEnemy; i++)
				if( shipHitEnemy(ship, enemies[i]) )
					SDL_SetTextureColorMod(textures[SH_SHIP],255,0,0);




			//enemy-missiles
			for(int i=0; i< nbAllyMissiles; i++){
				for(int iEnemy = 0; iEnemy < nbEnemy; iEnemy++){
					//enemy hitted
					if(missileHitEnemy(allyMissiles[i], enemies[iEnemy])){
						if(enemies[iEnemy].id == ARM_ENEMY){
							for(int j=0; j<NB_ARM_COMPONENT; j++){
								if(enemies[iEnemy].hp/ENEMY_HP[enemies[iEnemy].id] > RATIO_HP_PIECES[j] && (enemies[iEnemy].hp-allyMissiles[i].damage)/ENEMY_HP[enemies[iEnemy].id] <= RATIO_HP_PIECES[j]){
									nbEnemyParts+= ((j==6||j<2)? 1:2);
									enemyParts = realloc(enemyParts, nbEnemyParts * sizeof(EnemyPart));
									enemyParts[nbEnemyParts-1] = (EnemyPart){
										enemies[iEnemy].id,
										j,//id
										enemies[iEnemy].x,//x
										enemies[iEnemy].y,//y
										{enemies[iEnemy].rota,enemies[iEnemy].coefs[0]},//rota
										((j==6||j<2)? 0.6*randSign():1) * (-X_PARTS - (1/PRECISION_RAND_FLOAT) * (rand() %(int)(RAND_X_PARTS*PRECISION_RAND_FLOAT)))/(j<=4 ? 1.5:1),//dirX
										(Y_PARTS + (1/PRECISION_RAND_FLOAT) * (rand()%(int)(RAND_Y_PARTS*PRECISION_RAND_FLOAT)))*(j<=4 ? 1:1.4),//dirY
									};

									armPartExplosion(enemies[iEnemy], &explosions, &nbExplosions, j);

									if(!(j==6||j<2))
										enemyParts[nbEnemyParts-2] = (EnemyPart){
											enemies[iEnemy].id,
											j + NB_ARM_COMPONENT,//id
											enemies[iEnemy].x,//x
											enemies[iEnemy].y,//y
											{enemies[iEnemy].rota,enemies[iEnemy].coefs[0]},//rota
											(j==6? 0.6*randSign():1) * (X_PARTS + (1/PRECISION_RAND_FLOAT) * (rand() %(int)(RAND_X_PARTS*PRECISION_RAND_FLOAT)))/(j<=4 ? 1.5:1),//dirX
											(Y_PARTS + (1/PRECISION_RAND_FLOAT) * (rand()%(int)(RAND_Y_PARTS*PRECISION_RAND_FLOAT)))*(j<=4 ? 1:1.4)//dirY
										};
								}
							}
						}

						enemies[iEnemy].hp -= allyMissiles[i].damage;
						enemies[iEnemy].frameHit = FRAME_HIT_ANIM;
						if(enemies[iEnemy].hp <= 0){
							if(enemies[iEnemy].id == ARM_ENEMY){
								int iBoss;
								for(iBoss = 0; iBoss < nbEnemy && enemies[iBoss].id != ARM_BOSS_ENEMY; iBoss++);
								enemies[iBoss].infos[1]--;
								armExplosion(enemies[iEnemy], &explosions, &nbExplosions);
							}
							else if(enemies[iEnemy].id == ARM_BOSS_ENEMY){
								int iArm;
								for(iArm = 0; iArm < nbEnemy; iArm++){
									if(enemies[iArm].id == ARM_ENEMY){
										for(int j=0; j<NB_ARM_COMPONENT; j++){
											nbEnemyParts+= ((j==6||j<2)? 1:2);
											enemyParts = realloc(enemyParts, nbEnemyParts * sizeof(EnemyPart));
											enemyParts[nbEnemyParts-1] = (EnemyPart){
												enemies[iArm].id,
												j,//id
												enemies[iArm].x,//x
												enemies[iArm].y,//y
												{enemies[iArm].rota,enemies[iArm].coefs[0]},//rota
												((j==6||j<2)? 0.6*randSign():1) * (-X_PARTS - (1/PRECISION_RAND_FLOAT) * (rand() %(int)(RAND_X_PARTS*PRECISION_RAND_FLOAT)))/(j<=4 ? 1.5:1),//dirX
												(Y_PARTS + (1/PRECISION_RAND_FLOAT) * (rand()%(int)(RAND_Y_PARTS*PRECISION_RAND_FLOAT)))*(j<=4 ? 1:1.4),//dirY
											};

											armPartExplosion(enemies[iArm], &explosions, &nbExplosions, j);

											if(!(j==6||j<2))
												enemyParts[nbEnemyParts-2] = (EnemyPart){
													enemies[iArm].id,
													j + NB_ARM_COMPONENT,//id
													enemies[iArm].x,//x
													enemies[iArm].y,//y
													{enemies[iArm].rota,enemies[iArm].coefs[0]},//rota
													(j==6? 0.6*randSign():1) * (X_PARTS + (1/PRECISION_RAND_FLOAT) * (rand() %(int)(RAND_X_PARTS*PRECISION_RAND_FLOAT)))/(j<=4 ? 1.5:1),//dirX
													(Y_PARTS + (1/PRECISION_RAND_FLOAT) * (rand()%(int)(RAND_Y_PARTS*PRECISION_RAND_FLOAT)))*(j<=4 ? 1:1.4)//dirY
												};
										}
										armExplosion(enemies[iArm], &explosions, &nbExplosions);
										shiftEnemy(enemies, nbEnemy, iArm);
										nbEnemy--;
										if(nbEnemy !=0){
											enemies=realloc(enemies, sizeof(Enemy)* nbEnemy );
											iArm--;
										}
									}
								}
								armBossExplosion(&explosions, &nbExplosions);
							}

							explosions = realloc(explosions, ++nbExplosions * sizeof(Explosion));
							explosions[nbExplosions-1].id = EXPLO_SHIP;
							explosions[nbExplosions-1].taille = 2 * ENEMY_HITBOX[enemies[iEnemy].id].rx * RATIO_SHIP_EXPLO_SIZE;
							if(enemies[iEnemy].id == ARM_BOSS_ENEMY)
								explosions[nbExplosions-1].taille/=2;
							explosions[nbExplosions-1].x = enemies[iEnemy].x + ENEMY_HITBOX[enemies[iEnemy].id].x - explosions[nbExplosions-1].taille/2;
							explosions[nbExplosions-1].y = enemies[iEnemy].y + ENEMY_HITBOX[enemies[iEnemy].id].y - explosions[nbExplosions-1].taille/2;
							if(enemies[iEnemy].id == ARM_BOSS_ENEMY)
								explosions[nbExplosions-1].y += MAX_Y_BOSS;
							explosions[nbExplosions-1].frame = FRAME_EXPLOSIONS[explosions[nbExplosions-1].id];
							explosions[nbExplosions-1].delay = 0;

							shiftEnemy(enemies, nbEnemy, iEnemy);
							nbEnemy--;
							if(nbEnemy !=0){
								enemies=realloc(enemies, sizeof(Enemy)* nbEnemy );
								iEnemy--;
							}
						}
						explosions = realloc(explosions, ++nbExplosions * sizeof(Explosion));
						explosions[nbExplosions-1].id = EXPLO_MISSILE;
						explosions[nbExplosions-1].x = allyMissiles[i].x - TAILLE_EXPLOSIONS[allyMissiles[i].id]/2 - MISSILE_CENTER[allyMissiles[i].id].x;
						explosions[nbExplosions-1].y = allyMissiles[i].y - TAILLE_EXPLOSIONS[allyMissiles[i].id]/2 - MISSILE_HITBOX[allyMissiles[i].id].ry;
						explosions[nbExplosions-1].frame = FRAME_EXPLOSIONS[explosions[nbExplosions-1].id];
						explosions[nbExplosions-1].taille = BASE_TAILLE_EXPLOSION * TAILLE_EXPLOSIONS[allyMissiles[i].id];
						explosions[nbExplosions-1].delay = 0;

						shiftMissile(allyMissiles, nbAllyMissiles, i);
						nbAllyMissiles--;
						if(nbAllyMissiles !=0){
							allyMissiles=realloc(allyMissiles, sizeof(Missile)* nbAllyMissiles );
							i--;
						}

						break;
					}
				}
			}

		 // // //
		// Draw //`
		 // // //
			drawBackground(renderer, textures[SH_BACKGROUND], backgroundSrc, backgroundDest);

			for(int i=0; i<nbAllyMissiles; i++)
				drawMissile(renderer, allyMissiles[i], textures[SH_MISSILE]);

			for(int i=0; i<nbEnemyMissiles; i++)
				drawMissile(renderer, enemyMissiles[i], textures[SH_MISSILE]);

			for(int i=0; i<nbEnemy; i++)
                drawEnemy(renderer, &(enemies[i]) , textures);

			for(int i=0; i<nbEnemyParts; i++)
				drawEnemyParts(renderer, &(enemyParts[i]) , textures);

			drawShip(renderer, ship, textures);


			for(int i=0; i<nbExplosions; i++)
				drawExplosion(renderer, explosions[i], textures[SH_EXPLO_MISSILE+explosions[i].id]);




			SDL_SetRenderDrawColor(renderer, BACKGROUND_COLOR.r, BACKGROUND_COLOR.g, BACKGROUND_COLOR.b, 255);
			SDL_RenderFillRect(renderer, &LEFT_BACK);
			SDL_RenderFillRect(renderer, &RIGHT_BACK);

			drawBeta(renderer, font ,1, (SDL_Color){0xFF,0x00,0x00},28);

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

				if(enemies[i].id == BASE_ENEMY )
					enemies[i].rota += enemies[i].dir * 6;
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

			//explo
			for(int i=0; i<nbExplosions; i++){
				if(!explosions[i].delay){
					explosions[i].frame--;

					if(!explosions[i].frame){
						shiftExplosion(explosions, nbExplosions, i);
						nbExplosions--;
						if(nbExplosions != 0){
							explosions=realloc(explosions,sizeof(Explosion) * nbExplosions);
							i--;
						}
					}
				}
				else{
					explosions[i].delay--;
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
