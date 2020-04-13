#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "private_image.h"

/////////////////////////////////////////////
/// \file private_image.c
/// \author Mahi S.
/// \brief Fonction globale liee a SDL_Image
/// \version 1.0
/// \date 19 Janvier 2020
/////////////////////////////////////////////




/////////////////////////////////////////////
/// \fn SDL_Texture* LoadImageWithErrorCode (SDL_Renderer *renderer ,char directory[])
/// \brief permet de charger une image au format PNG
///
/// \param SDL_Renderer *renderer Cible pour la texture
/// \param char directory[] lien du fichier a charger
///
/// \return retourne la texture charger NULL si erreur
/////////////////////////////////////////////
SDL_Texture* LoadImageWithErrorCode(SDL_Renderer *renderer ,char directory[])
{

	SDL_Texture* texture = IMG_LoadTexture(renderer,directory);
	if(!texture)
		printf("TEXTURE FAILURE : Erreur lors de la creation de %s SDL_ERROR : %s",directory,SDL_GetError());

	return texture;
}
