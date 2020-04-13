/////////////////////////////////////////////
/// \file hashage.c
/// \author Mahi S.
/// \brief Fonction de hashage de int
/// \version 1.0
/// \date 19 Janvier 2020
/////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>


/////////////////////////////////////////////
/// \fn long long int hashage (int value, long keys[4])
/// \brief retourne une valeur hasher de la valeur d'entrer.
///
/// \param int value Valeur a hasher
/// \param keys Le tableau de clés de hashage
///
/// \return long long retourne la valeur hasher de value.
/////////////////////////////////////////////
long long int hashage (int value, long keys[4])
{

	long long hash;

	if(value){
		hash = value * keys[0];
		hash = hash - keys[1];
		hash = hash * keys[2];
		hash = hash / keys[3];
	} else {
		hash = keys[0];
		hash = hash - keys[1];
		hash = hash * keys[2];
		hash = hash / keys[3];
	}

	return hash;
}

/////////////////////////////////////////////
/// \fn void initialisationConstantHashage (long *const_1, long *const_2, long *const_3, long *const_4)
/// \brief initialise des valeurs aleatoire pour le hashage
///
/// \param keys Le tableau de clés de hashage à initialiser
///
/////////////////////////////////////////////
void initialisationConstantHashage(long keys[4])
{
	for(int i = 0; i<4; i++)
		keys[i] = rand() + 1;
}

/////////////////////////////////////////////
/// \fn int changeProtectedVar (long long int *score_hash,int *var, int updatedVar, long *const_1, long *const_2, long *const_3, long *const_4)
/// \brief Verifie que la valeur correspond a une valeur et re met a jours les constants pour un prochain hasahage
///
/// \param long long long int *score_hash valeur hasher
/// \param int *var valeur a verifier
/// \param int pdatedVar nouvelle valeur a hasher a la sortie.
/// \param keys Le tableau de clés de hashage
///
/// \return valeur <=> valeur_hasher retorune 1, sinon retourne 0.
/////////////////////////////////////////////
int changeProtectedVar(long long int *score_hash,int *var, int updatedVar, long keys[4])
{
	if( *score_hash == hashage(*var, keys)){
		keys[0] = rand() + 1;
		keys[1] = rand() + 1;
		keys[2] = rand() + 1;
		keys[3] = rand() + 1;


		*score_hash = (updatedVar ? updatedVar : 1) * keys[0];
		*score_hash = *score_hash - keys[1];
		*score_hash = *score_hash * keys[2];
		*score_hash = *score_hash / keys[3];


		*var = updatedVar;
		return 1;
	}
	else{
		// BAN
		printf("Baned\n");
		return 0;
	}
}
