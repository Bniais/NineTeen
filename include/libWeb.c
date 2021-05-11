#include <stdio.h>
FILE *EXT_FILE;

#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <SDL2/SDL.h>
// secure protocol //
#include <time.h>
#include <openssl/md5.h>

/////////////////////////////////////////////////
///	\file libWeb.c
///	\author Samy M.
///	\version 1.0
///	\date 31 janvier 2020
///	\brief Regroupe les fonctions de connexion au serveur
/////////////////////////////////////////////////

#define URL_CONNECT_EMAIL "https://nineteen.samymahi.eu/connect.php"
#define URL_CONNECT_KEY "https://nineteen.samymahi.eu/connect.php"
#define URL_UPDATE_SCORE "https://nineteen.samymahi.eu/updateYourScore.php"
#define URL_PING "https://nineteen.samymahi.eu/ping.php"
#define URL_TIMESTAMP "https://nineteen.samymahi.eu/include/timestamp.php"
#define URL_GET_COINS "https://nineteen.samymahi.eu/coins.php"
#define URL_CHECK_VERSION "https://nineteen.samymahi.eu/checkVersion.php"
#define URL_LEADERBOARD "https://nineteen.samymahi.eu/leaderboard.php"



#define ERR_REQUIERED_FIELD -1
#define ERR_SQL_FAILED -2
#define ERR_INCORRECT_ID_PW -3
#define ERR_INVALIDE_SECUR -17

#define CLOCKS_PER_MS 1000

#define MD5_SIZE 32

/**
*\struct string
*\brief stock une chaine avec ca longueur
*/
struct string {
	char *ptr;
	size_t len;
};

/**
*\struct ConnectStruct
*\brief information sur l'envoie de la requete connexion en structure pour le passer en param d'un thread SDL
*/
typedef struct{
	char *key;
	char *email;
	char *password;
}ConnectStruct;

/**
*\struct EnvoiScore
*\brief information sur l'envoie d un score en structure pour le passer en param d'un thread SDL
*/
typedef struct{
	char *gameID;
	char *score;
	char *key;
}EnvoiScore;



/////////////////////////////////////////////////////
/// \fn void init_string(struct string *s)
/// \brief initilalise la structure qui contient la reponse
///
/// \param s
///
/// \return void
/////////////////////////////////////////////////////
void init_string(struct string *s) {
	s->len = 0;
	s->ptr = malloc(s->len+1);
	if (s->ptr == NULL) {
			fprintf(EXT_FILE, "libWeb.c : init_string() : malloc failed\n");

		exit(EXIT_FAILURE);
	}
	s->ptr[0] = '\0';
}

/////////////////////////////////////////////////////
/// \fn size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
/// \brief Ecrit la reponse de la requete dans une chaine
///
/// \param ptr
/// \param size
/// \param nmemb
/// \param s
///
/// \return void
/////////////////////////////////////////////////////
size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
	size_t new_len = s->len + size*nmemb;
	s->ptr = realloc(s->ptr, new_len+1);
	if (s->ptr == NULL) {
			fprintf(EXT_FILE, "libWeb.c : writefunc() : realloc failed\n");
		exit(EXIT_FAILURE);
	}
	memcpy(s->ptr+s->len, ptr, size*nmemb);
	s->ptr[new_len] = '\0';
	s->len = new_len;

	return size*nmemb;
}

/////////////////////////////////////////////////////
/// \fn void md5Hash (char *string, char *hash)
/// \brief hash la chaine en md5
///
/// \param string
/// \param hash
///
/// \return void
/////////////////////////////////////////////////////
void md5Hash (char *string, char *hash)
{
    int i;
    char unsigned md5[MD5_DIGEST_LENGTH] = {0};

     MD5((const unsigned char *)string, strlen(string), md5);

    for (i=0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(hash + 2*i, "%02x", md5[i]);
    }
}

/////////////////////////////////////////////////////





/////////////////////////////////////////////////////
/// \fn int envoyez_requet(char **response, char *url, char *request)
/// \brief Envoi une requet et ecrit la reponse dans **response
///
/// \param url url de destination
/// \param request requete a envoyez
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int envoyez_requet(char **response, char *url, char *request)
{
	struct string s;
	init_string(&s);
	CURL *curl = NULL;
	CURLcode res;

	/* get a curl handle */
	curl = curl_easy_init();
	if(curl) {

		// construction de la requet
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);

		// envoyez la requete
		res = curl_easy_perform(curl);
		if(res != CURLE_OK)
		{
				fprintf(EXT_FILE, "libWeb.c : envoyez_requet() : curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

		}
		else
		{
			*response = malloc( sizeof(char) * strlen(s.ptr)+1 );
			strcpy(*response,s.ptr);

			// memory clean
			free(s.ptr);
			s.ptr = NULL;
			curl = NULL;

			//end
			return EXIT_SUCCESS;
		}



	}
	fprintf(EXT_FILE, "libWeb.c : envoyez_requet() : curl_easy_init() failed\n");
	//nettoyage
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	free(curl);
	curl = NULL;
	free(s.ptr);
	s.ptr = NULL;

	return EXIT_FAILURE;
}

/////////////////////////////////////////////////////
/// \fn void securePass(char secure[])
/// \brief securise la requete avec une cle unique
///
/// \param secure
///
/// \return void
/////////////////////////////////////////////////////
void securePass(char secure[])
{

	char *t_server;
	envoyez_requet(&t_server, URL_TIMESTAMP, "");



	int year,mon,day,hour,min,sec;
	sscanf(t_server, " %d %d %d %d %d %d",&year , &mon , &day, &hour , &min, &sec);


	char temp[MD5_SIZE*2];

	//printf("HEURE A l'ENVOI DE LA REQUET %d-%02d-%02d %02d  %02d  %d\n",year , mon, day, hour , min, sec);
	sprintf(temp, "%d-%02d-%02d JDlaliljasnc329832 %02d 0 %02d D(ancIjaa) %d", year  , mon , day, hour , min, sec);
	md5Hash(temp, secure);

	free(t_server);

}

/////////////////////////////////////////////////////
/// \fn int construire_requete(char **dest, char *username, char *password, char *key, char *gameID, char *score, char *offset, char *limite)
/// \brief Permet de crée une requet dans *dest
///
/// \param dest Ecriture de la requet
/// \param username Username de connexion (Optionnal)
/// \param password Mot de passe de connexion (Optionnal)
/// \param key Key de connexion (Optionnal)
/// \param gameID id du jeux (Optional)
/// \param score score (Optional)
/// \param offset nombre d element recherche (Optional)
/// \param limite limit recherche score (Optional)
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int construire_requete(char **dest, char *username, char *password, char *key, char *gameID, char *score, char *offset, char *limite)
{
	int lenght; // longueur de la chaine malloc

	if( key && !username && !password && !gameID && !score)
	{
		// secure request
		char secure[MD5_SIZE];
		securePass(secure);
		// requet connexion avec key
		lenght = strlen(key) + 1 + 12 + 32 ; // 12 = "key= &secure="

		*dest = malloc( sizeof(char) * lenght );
		if(*dest == NULL) {
			fprintf(EXT_FILE,"libWeb.c : construire_requete() : Failed malloc");
			 return EXIT_FAILURE;
		 }

		// concatenation dans dest;
		strcpy(*dest,"");
		sprintf(*dest,"key=%s&secure=%s",key,secure);

		return EXIT_SUCCESS;
	}
	else if( !key && username && password && !gameID && !score)
	{
		// secure request
		char secure[MD5_SIZE];
		securePass(secure);
		// requet connexion avec email/password

		lenght = strlen(username) + 1 + strlen(password) + 1 + 25 + 32; // 17 = "username= &pwd= &secure"
		*dest = malloc( sizeof(char) * lenght );

		if(*dest == NULL) {
			fprintf(EXT_FILE,"libWeb.c : construire_requete() : Failed malloc");
			 return EXIT_FAILURE;
		 }

		// concatenation dans dest;
		strcpy(*dest,"");
		sprintf(*dest,"username=%s&pwd=%s&secure=%s",username,password,secure);

		return EXIT_SUCCESS;
	}
	else if( key && !username && !password && gameID && score)
	{
		// secure request
		char secure[MD5_SIZE];
		securePass(secure);
		// requet update your score
		lenght = strlen(gameID) + 1 + strlen(score) + 1 + strlen(key) + 1 + 29 + 32; // 29 = "gameID= & score= &key= &secure"
		*dest = malloc( sizeof(char) * lenght );

		if(*dest == NULL) {
			fprintf(EXT_FILE,"libWeb.c : construire_requete() : Failed malloc");
			 return EXIT_FAILURE;
		 }

		// concatenation dans dest;
		strcpy(*dest,"");
		sprintf(*dest,"gameID=%s&score=%s&key=%s&secure=%s",gameID,score,key,secure);
			return EXIT_SUCCESS;
	}
	else if ( key && !username && !password && gameID && !score )
	{
		// secure request
		char secure[MD5_SIZE];
		securePass(secure);
		// requet bug game passe
		lenght = strlen(gameID) + 1 + strlen(key) + 1 + 24 + 32; // 24 = "gameID= &key= &secure"
		*dest = malloc( sizeof(char) * lenght );

		if(*dest == NULL) {
			fprintf(EXT_FILE,"libWeb.c : construire_requete() : Failed malloc");
			 return EXIT_FAILURE;
		 }

		// concatenation dans dest;
		strcpy(*dest,"");
		sprintf(*dest,"gameID=%s&key=%s&secure=%s",gameID,key,secure);
			return EXIT_SUCCESS;

	}
	else if( !key && !password && !score && offset && limite)
	{
		lenght = 0;
		if(username)
		{
			lenght += strlen(username) + 1 + 10; //&username=
		}
		if(gameID)
		{
			lenght += strlen(gameID) + 1 + 9; // &gameID=
		}
		if(offset)
		{
			lenght += strlen(offset) + 1 + 9; // &offset=
		}
		if(limite)
		{
			lenght += strlen(offset) + 1 + 9; // &limite=
		}

		*dest = malloc(sizeof(char) * lenght );
		if(*dest == NULL) {
			fprintf(EXT_FILE,"libWeb.c : construire_requete() : Failed malloc");
			 return EXIT_FAILURE;
		 }

		strcpy(*dest,"");
		sprintf(*dest,"gameID=%s&username=%s&offset=%s&limite=%s",gameID,username,offset,limite);
			return EXIT_SUCCESS;
	}
	else
	{
		fprintf(EXT_FILE,"libWeb.c : construire_requete() : Vérifiez les parametres d'entrer\n");
	}

	return EXIT_FAILURE;
}



int connectEnded;
/////////////////////////////////////////////////////
/// \fn int connectWithUsername(ConnectStruct * connectStruct)
/// \brief connexion avec nom d'utilisateur
///
/// \param connectStruct
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int connectWithUsername(ConnectStruct * connectStruct)
{
	char *request;
	char *response;
	if ( !construire_requete(&request, connectStruct->email, connectStruct->password, NULL, NULL, NULL, NULL, NULL) )
	{
		if ( !envoyez_requet(&response,URL_CONNECT_EMAIL,request) )
		{
			free(request);
			request = NULL;

			if ( strlen(response) >= 255  )
			{
				strcpy(connectStruct->key,response);
				free(response);
				response = NULL;
				connectEnded = 1;
				return EXIT_SUCCESS;
			}
			else if ( !strcmp(response,"-5") )
			{
				fprintf(EXT_FILE, "libWeb.c : connectWithUsername() : erreur de synchronisation avec le serveur \n" );

				free(response);
				response = NULL;
				connectEnded = 1;
				return -5;
			}
			else if ( !strcmp(response,"-3") )
			{
				fprintf(EXT_FILE, "libWeb.c : connectWithUsername() : mdp/login incorrect \n" );
				free(response);
				response = NULL;
					connectEnded = 1;
				return -3;
			}
			else
				fprintf(EXT_FILE, "libWeb.c : connectWithUsername() : erreur de connexion inconnu %s \n",response );


			free(response);
			response = NULL;

		}

	}
	fprintf(EXT_FILE,"libWeb.c : connectWithUsername() : Erreur construction requete\n");

		connectEnded = 1;
	return EXIT_FAILURE;
}

/////////////////////////////////////////////////////
/// \fn int connectWithKey(char *key)
/// \brief connexion avec key
///
/// \param key Ecriture de la clé dans key
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int connectWithKey(char *key)
{
	char *request;
	char *response;
	if ( !construire_requete(&request, NULL, NULL, key, NULL, NULL, NULL, NULL) )
	{
		if ( !envoyez_requet(&response,URL_CONNECT_KEY,request) )
		{
			if ( !strcmp(response,"SUCCESS")  )
			{
				free(request);
				request = NULL;
				free(response);
				response = NULL;
				return EXIT_SUCCESS;
			}

			fprintf(EXT_FILE,"libWeb.c : connectWithKey() : %s\n",response );
			free(response);
			response = NULL;
		}
	}

	fprintf(EXT_FILE,"libWeb.c : connectWithKey() : Erreur construction requete\n");
	free(request);
	request = NULL;
	return EXIT_FAILURE;
}


/////////////////////////////////////////////////////
/// \fn int updateMeilleureScoreStruct(char *key,char *retour)
/// \brief recuperer les meilleures score
///
/// \param key Ecriture de la clé dans key
/// \param retour reponse en retour
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int updateMeilleureScoreStruct(char *key,char *retour)
{
	char *request;
	char *response;
	if ( !construire_requete(&request, NULL, NULL, key, NULL, NULL, NULL,NULL) )
	{
		if ( !envoyez_requet(&response,URL_GET_COINS,request) )
		{
			if(strlen(response) <= 4)
			{
				fprintf(EXT_FILE,"libWeb.c : updateMeilleureScoreStruct() : %s\n",response);
				return EXIT_FAILURE;
			}

			strcpy(retour,response);
			free(request);
			request = NULL;
			free(response);
			response = NULL;
			return EXIT_SUCCESS;
		}
		fprintf(EXT_FILE,"libWeb.c : updateMeilleureScoreStruct() envoyez_requet() : EXIT_FAILURE request = %s rsponse = %s\n",request,response);
	}
	fprintf(EXT_FILE,"libWeb.c : updateMeilleureScoreStruct() : Erreur construction requete\n");
	free(request);
	request = NULL;
	return EXIT_FAILURE;
}


/////////////////////////////////////////////////////
/// \fn int getLeaderboard(char *gameID,char *username, char *offset,char *limite ,char *retour)
/// \brief recupere le classement des joueurs
///
/// \param gameID identifiant du jeu (Optinal)
/// \param username non d utulisateur
/// \param offset specifier le debut des retour pour la requete sql
/// \param limite specifier la limite de la rpeonse sql
/// \param retour chaine de retour de la reponse
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int getLeaderboard(char *gameID,char *username, char *offset,char *limite, char *retour)
{

	char *request;
	char *response;
	if ( !construire_requete(&request, username, NULL, NULL, gameID, NULL, offset,limite) )
	{

		if ( !envoyez_requet(&response,URL_LEADERBOARD,request) )
		{
			if(strlen(response) <= 1)
			{
				fprintf(EXT_FILE,"libWeb.c : getLeaderboard() : Erreur %s\n",response);
				return EXIT_FAILURE;
			}
			strcpy(retour,response);
			free(request);
			request = NULL;
			free(response);
			response = NULL;
			return EXIT_SUCCESS;
		}
	}

	fprintf(EXT_FILE,"libWeb.c : getLeaderboard() : Erreur construction requete \n");
	free(request);
	request = NULL;
	return EXIT_FAILURE;
}






int updateEnded;
/////////////////////////////////////////////////////
/// \fn int updateScore(EnvoiScore * envoiScore)
/// \brief update le score
///
/// \param envoiScore structure comprenant les parametre necessaire a l envoo des scores
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int updateScore(EnvoiScore * envoiScore )
{
	updateEnded = 0;
	char *request;
	char *response;
	int attempt =0;
	do {
		if ( !construire_requete(&request, NULL, NULL, envoiScore->key, envoiScore->gameID, envoiScore->score, NULL , NULL) )
		{

			if ( !envoyez_requet(&response,URL_UPDATE_SCORE,request) )
			{
				printf("%s\n",response);
				if(!strcmp(response, "SUCCESS")){

					fprintf(EXT_FILE,"libWeb.c : updateScore() : Nombre de tentative : %d Reponse : %s\n",attempt + 1,response);

					free(request);
					request = NULL;
					free(response);
					response = NULL;
					updateEnded = 1;
					return EXIT_SUCCESS;
				}

			}

		}
	}while ( ++attempt < 5 );

	fprintf(EXT_FILE,"libWeb.c : updateScore() : Erreur constriction requete attempt : %d \n",attempt);

	free(request);
	request = NULL;
	updateEnded = 1;
	return EXIT_FAILURE;
}

/////////////////////////////////////////////////////
/// \fn int ping()
/// \brief permet de calculer de delai de reponse du serveur
///
/// \return DELAY MS
/////////////////////////////////////////////////////
int ping()
{
	clock_t start, finish;
	char *response;
	start = clock();
	if ( !envoyez_requet(&response,URL_PING,"ping=test") )
	{
		finish = clock();
		if ( !strcmp(response, "PONG") )
		{
			free(response);
			response = NULL;
			return (double)(finish - start)/ CLOCKS_PER_MS;
		}
		free(response);
		response = NULL;
	}
	finish = clock();
	return (int)(finish - start)/ CLOCKS_PER_MS;
}

/////////////////////////////////////////////////////
/// \fn int checkVersionOnline(char message[])
/// \brief permet de verifier la bonne version de l'utilisateur
/// \param requete construite comprenant la version du jeu
///
/// \return DELAY MS
/////////////////////////////////////////////////////
int checkVersionOnline(char message[])
{
	char *response;
	if ( !envoyez_requet(&response,URL_CHECK_VERSION,message) )
	{
		if ( !strcmp(response, "1") )
		{
			free(response);
			response = NULL;
			return EXIT_SUCCESS;
		}
		fprintf(EXT_FILE,"libWeb.c : checkVersionOnline() : erreur %s version actuel %s \n",response,message);
		free(response);
		response = NULL;
		return EXIT_FAILURE;

	}
	fprintf(EXT_FILE,"libWeb.c : checkVersionOnline() : erreur envoi requete %s \n",message);

	return EXIT_FAILURE;
}
