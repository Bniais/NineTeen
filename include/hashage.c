#include <stdio.h>

/////////////////////////////////////////////
/// \file hashage.c
/// \author Mahi S.
/// \brief Fonction de hashage de int
/// \version 1.0
/// \date 19 Janvier 2020
/////////////////////////////////////////////


/////////////////////////////////////////////
/// \fn long long int hashage (int value, long const_1, long const_2, long const_3, long const_4 )
/// \brief retourne une valeur hasher de la valeur d'entrer.
///
/// \param int value Valeur a hasher
/// \param long const_1 constante 1 pour réalisez ce hashage
/// \param long const_2 constante 2 pour réalisez ce hashage
/// \param long const_3 constante 3 pour réalisez ce hashage
/// \param long const_4 constante 4 pour réalisez ce hashage
///
/// \return long long retourne la valeur hasher de value.
/////////////////////////////////////////////
long long int hashage (int value, long const_1, long const_2, long const_3, long const_4 )
{

	long long hash;

	if(value){
		hash = value * const_1;
		hash = hash - const_2;
		hash = hash * const_3;
		hash = hash / const_4;
	} else {
		hash = const_1;
		hash = hash - const_2;
		hash = hash * const_3;
		hash = hash / const_4;
	}

	return hash;
}

/////////////////////////////////////////////
/// \fn void initialisationConstantHashage (long *const_1, long *const_2, long *const_3, long *const_4)
/// \brief initialise des valeurs aleatoire pour le hashage
///
/// \param long *const_1 init constante 1 pour réalisez ce hashage
/// \param long *const_2 init constante 2 pour réalisez ce hashage
/// \param long *const_3 init constante 3 pour réalisez ce hashage
/// \param long *const_4 init constante 4 pour réalisez ce hashage
///
/////////////////////////////////////////////
void initialisationConstantHashage(long *const_1, long *const_2, long *const_3, long *const_4)
{
	*const_1 = rand();
	*const_2 = rand();
	*const_3 = rand();
	*const_4 = rand();

}

/////////////////////////////////////////////
/// \fn int changeProtectedVar (long long int *score_hash,int *var, int updatedVar, long *const_1, long *const_2, long *const_3, long *const_4)
/// \brief Verifie que la valeur correspond a une valeur et re met a jours les constants pour un prochain hasahage
///
/// \param long long long int *score_hash valeur hasher
/// \param int *var valeur a verifier
/// \param int pdatedVar nouvelle valeur a hasher a la sortie.
/// \param long *const_1 constante 1 pour réalisez ce hashage
/// \param long *const_2 constante 2 pour réalisez ce hashage
/// \param long *const_3 constante 3 pour réalisez ce hashage
/// \param long *const_4 constante 4 pour réalisez ce hashage
///
/// \return valeur <=> valeur_hasher retorune 1, sinon retourne 0.
/////////////////////////////////////////////
int changeProtectedVar(long long int *score_hash,int *var, int updatedVar, long *const_1, long *const_2, long *const_3, long *const_4)
{
	if( *score_hash == hashage(*var, *const_1, *const_2, *const_3, *const_4)){
		*const_1 = rand();
		*const_2 = rand();
		*const_3 = rand();
		*const_4 = rand();

		*score_hash = updatedVar * *const_1;
		*score_hash = *score_hash - *const_2;
		*score_hash = *score_hash * *const_3;
		*score_hash = *score_hash / *const_4;

		*var = updatedVar;
		return 1;
	}
	else{
		// BAN
		printf("Baned\n");
		return 0;
	}
}