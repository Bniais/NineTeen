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





typedef struct{char *key; char *email; char *password; int retour;}ConnectStruct;
/////////////////////////////////////////////////////
/// \fn int connectWithUsername(char *key, char *email, char *password)
/// \brief connexion avec nom utilisateur
///
/// \param char *key Ecriture de la clé dans key
/// \param char *email Email de connexion
/// \param char *password Mot de passe de connexion
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int connectWithUsername(ConnectStruct connectStruct);

/////////////////////////////////////////////////////
/// \fn updateMeilleureScoreStruct(char *key,int *coins)
/// \brief recuperer notre somme d'argent
///
/// \param char *key Ecriture de la clé dans key
/// \param int *coins Valeur de retour de notre somme d'argent
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int updateMeilleureScoreStruct(char *key,char *coins);

/////////////////////////////////////////////////////
/// \fn buyGamePass(char *key, char *gameID)
/// \brief acheter un pass pour un jeu
///
/// \param char *key Ecriture de la clé dans key
/// \param char *gameID numero du jeux
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int buyGamePass(char *key, char *gameID);





/////////////////////////////////////////////////////
/// \fn int connectWithKey(char *key)
/// \brief connexion avec key
///
/// \param char *key Ecriture de la clé dans key
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int connectWithKey(char *key);


typedef struct{char *gameID; char *score; char *key;}EnvoiScore;
/////////////////////////////////////////////////////
/// \fn int updateScore(char *key, char *gameID, char *score)
/// \brief update le score
///
/// \param char *gameID id du jeux
/// \param char *score score à update
/// \param char *key cle du joueur
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int updateScore(EnvoiScore * envoiScore );


/////////////////////////////////////////////////////
/// \fn int ping()
/// \brief permet de calculer de delai de reponse du serveur
///
/// \return DELAY MS
/////////////////////////////////////////////////////
int ping();


/////////////////////////////////////////////////////
/// \fn int checkVersionOnline(char message[])
/// \brief permet de verifier la bonne version de l'utilisateur
///
/// \return DELAY MS
/////////////////////////////////////////////////////
int checkVersionOnline(char message[]);
