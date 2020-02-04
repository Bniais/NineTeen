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
/// \fn int connexion_avec_email(char **key, char *email, char *password)
/// \brief connexion avec adresse email
///
/// \param char **key Ecriture de la clé dans key
/// \param char *email Email de connexion
/// \param char *password Mot de passe de connexion
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int connectWithUsername(char **key, char *email, char *password);



/////////////////////////////////////////////////////
/// \fn int connexion_with_key(char *key)
/// \brief connexion avec key
///
/// \param char *key Ecriture de la clé dans key
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int connectWithKey(char *key);



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
