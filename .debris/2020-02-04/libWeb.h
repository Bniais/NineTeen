#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

/////////////////////////////////////////////////
///	\file libWeb.h
///	\author Samy M.
///	\version 1.0
///	\date 31 janvier 2020
///	\brief Regroupe les fonctions de connexion au serveur
/////////////////////////////////////////////////

#define ERR_REQUIERED_FIELD -1
#define ERR_SQL_FAILED -2
#define ERR_INCORRECT_ID_PW -3

/////////////////////////////////////////////////////
/// \fn int envoyez_requet(char **response, char *url, char *request)
/// \brief Envoi une requet et ecrit la reponse dans **response
///
/// \param char *url url de destination
/// \param char *request requete a envoyez
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int envoyez_requet(char **response, char *url, char *request);



/////////////////////////////////////////////////////
/// \fn int construire_requete(char *dest, char *email, char *password, char *key)
/// \brief Permet de crée une requet dans *dest
///
/// \param char *dest Ecriture de la requet
/// \param char *email Email de connexion (Optionnal)
/// \param char *password Mot de passe de connexion (Optionnal)
/// \param char *key Key de connexion (Optionnal)
/// \param char *gameID id du jeux (Optional)
/// \param char *score score (Optional)
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int construire_requete(char **dest, char *email, char *password, char *key, char *gameID, char *score);



/////////////////////////////////////////////////////
/// \fn int connexion_avec_email(char **key, char *email, char *password)
/// \brief connexion avec adresse email
///
/// \param char **key Ecriture de la clé dans key
/// \param char *email Email de connexion 
/// \param char *password Mot de passe de connexion 
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int connexion_avec_email(char **key, char *email, char *password);



/////////////////////////////////////////////////////
/// \fn int connexion_with_key(char *key)
/// \brief connexion avec key
///
/// \param char *key Ecriture de la clé dans key
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int connexion_with_key(char *key);



/////////////////////////////////////////////////////
/// \fn int update_score(char *key, char *gameID, char *score)
/// \brief update le score
///
/// \param char *gameID id du jeux
/// \param char *score score à update
/// \param char *key cle du joueur
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int update_score(char *gameID, char *score, char *key);