#include <stdio.h>

/////////////////////////////////////////////
/// \file hashage.h
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
long long int hashage (int value, long const_1, long const_2, long const_3, long const_4 );

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
void initialisationConstantHashage(long *const_1, long *const_2, long *const_3, long *const_4);

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
int changeProtectedVar(long long int *score_hash,int *var, int updatedVar, long *const_1, long *const_2, long *const_3, long *const_4);