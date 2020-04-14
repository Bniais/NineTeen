#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <SDL2/SDL.h>
typedef struct{char *key; char *email; char *password;}ConnectStruct;
typedef struct{char *gameID; char *score; char *key;}EnvoiScore;
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

#define URL_CONNECT_EMAIL "https://nineteen.recognizer.fr/connect.php"
#define URL_CONNECT_KEY "https://nineteen.recognizer.fr/connect.php"
#define URL_UPDATE_SCORE "https://nineteen.recognizer.fr/updateYourScore.php"
#define URL_PING "https://nineteen.recognizer.fr/ping.php"
#define URL_TIMESTAMP "https://nineteen.recognizer.fr/include/timestamp.php"
#define URL_GET_COINS "https://nineteen.recognizer.fr/coins.php"
#define URL_BUY_GAMEPASS "https://nineteen.recognizer.fr/buygamepass.php"

#define ERR_REQUIERED_FIELD -1
#define ERR_SQL_FAILED -2
#define ERR_INCORRECT_ID_PW -3
#define ERR_INVALIDE_SECUR -17

#define CLOCKS_PER_MS 1000

#define MD5_SIZE 32

struct string {
	char *ptr;
	size_t len;
};


void init_string(struct string *s) {
	s->len = 0;
	s->ptr = malloc(s->len+1);
	if (s->ptr == NULL) {
		fprintf(stderr, "malloc() failed\n");
		exit(EXIT_FAILURE);
	}
	s->ptr[0] = '\0';
}


size_t writefunc(void *ptr, size_t size, size_t nmemb, struct string *s)
{
	size_t new_len = s->len + size*nmemb;
	s->ptr = realloc(s->ptr, new_len+1);
	if (s->ptr == NULL) {
		fprintf(stderr, "realloc() failed\n");
		exit(EXIT_FAILURE);
	}
	memcpy(s->ptr+s->len, ptr, size*nmemb);
	s->ptr[new_len] = '\0';
	s->len = new_len;

	return size*nmemb;
}

/////////////////////////////////////////////////////

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
/// \param char *url url de destination
/// \param char *request requete a envoyez
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////

int envoyez_requet(char **response, char *url, char *request)
{
	struct string s;
	init_string(&s);
	CURL *curl;
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
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
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
/// \fn int construire_requete(char *dest, char *email, char *password, char *key)
/// \brief Permet de crée une requet dans *dest
///
/// \param char *dest Ecriture de la requet
/// \param char *username Username de connexion (Optionnal)
/// \param char *password Mot de passe de connexion (Optionnal)
/// \param char *key Key de connexion (Optionnal)
/// \param char *gameID id du jeux (Optional)
/// \param char *score score (Optional)
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int construire_requete(char **dest, char *username, char *password, char *key, char *gameID, char *score)
{
	int lenght; // longueur de la chaine malloc

	// secure request
	char secure[MD5_SIZE];
	securePass(secure);
	// end
	if( key && !username && !password && !gameID && !score)
	{
		// requet connexion avec key
		lenght = strlen(key) + 1 + 12 + 32; // 12 = "key= &secure="

		*dest = malloc( sizeof(char) * lenght );
		if(*dest == NULL) { printf("Failed malloc"); return EXIT_FAILURE; }

		// concatenation dans dest;
		strcpy(*dest,"");
		sprintf(*dest,"key=%s&secure=%s",key,secure);

		return EXIT_SUCCESS;
	}
	else if( !key && username && password && !gameID && !score)
	{
		// requet connexion avec email/password

		lenght = strlen(username) + 1 + strlen(password) + 1 + 25 + 32; // 17 = "username= &pwd= &secure"
		*dest = malloc( sizeof(char) * lenght );

		if(*dest == NULL) { printf("Failed malloc"); return EXIT_FAILURE; }

		// concatenation dans dest;
		strcpy(*dest,"");
		sprintf(*dest,"username=%s&pwd=%s&secure=%s",username,password,secure);

		return EXIT_SUCCESS;
	}
	else if( key && !username && !password && gameID && score)
	{
		// requet update your score
		lenght = strlen(gameID) + 1 + strlen(score) + 1 + strlen(key) + 1 + 29 + 32; // 29 = "gameID= & score= &key= &secure"
		*dest = malloc( sizeof(char) * lenght );

		if(*dest == NULL) { printf("Failed malloc"); return EXIT_FAILURE; }

		// concatenation dans dest;
		strcpy(*dest,"");
		sprintf(*dest,"gameID=%s&score=%s&key=%s&secure=%s",gameID,score,key,secure);
			return EXIT_SUCCESS;
	}
	else if ( key && !username && !password && gameID && !score )
	{
		// requet bug game passe
		lenght = strlen(gameID) + 1 + strlen(key) + 1 + 24 + 32; // 24 = "gameID= &key= &secure"
		*dest = malloc( sizeof(char) * lenght );

		if(*dest == NULL) { printf("Failed malloc"); return EXIT_FAILURE; }

		// concatenation dans dest;
		strcpy(*dest,"");
		sprintf(*dest,"gameID=%s&key=%s&secure=%s",gameID,key,secure);
			return EXIT_SUCCESS;

	}
	else
	{
		printf("ERROR: construire_requete() Vérifiez les parametres d'entrer\n");
	}

	return EXIT_FAILURE;
}



int connectEnded;
/////////////////////////////////////////////////////
/// \fn int connectWithUsername(char *key, char *email, char *password)
/// \brief connexion avec nom d'utilisateur
///
/// \param char *key Ecriture de la clé dans key
/// \param char *email Email de connexion
/// \param char *password Mot de passe de connexion
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int connectWithUsername(ConnectStruct * connectStruct)
{
	char *request;
	char *response;
	if ( !construire_requete(&request, connectStruct->email, connectStruct->password, NULL, NULL, NULL) )
	{
		if ( !envoyez_requet(&response,URL_CONNECT_EMAIL,request) )
		{
			free(request);
			request = NULL;

			if ( strlen(response) >= 255  )
			{
				//key = malloc( sizeof(char) * strlen(response) + 1 );
				strcpy(connectStruct->key,response);
				free(response);
				response = NULL;
				connectEnded = 1;
				return EXIT_SUCCESS;
			}
			else if ( !strcmp(response,"-5") )
			{
				free(response);
				response = NULL;
					connectEnded = 1;
				return -5;
			}
			else if ( !strcmp(response,"-3") )
			{
				printf("Mauvais login/mdp\n" );
				free(response);
				response = NULL;
					connectEnded = 1;
				return -3;
			}
			else
				printf("erreur connection : %s\n",response );

			free(response);
			response = NULL;

		}

	}


		connectEnded = 1;
	return EXIT_FAILURE;
}

/////////////////////////////////////////////////////
/// \fn int connectWithKey(char *key)
/// \brief connexion avec key
///
/// \param char *key Ecriture de la clé dans key
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int connectWithKey(char *key)
{
	char *request;
	char *response;
	if ( !construire_requete(&request, NULL, NULL, key, NULL, NULL) )
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
			printf("%s\n",response );
			free(response);
			response = NULL;
		}
	}
	free(request);
	request = NULL;
	return EXIT_FAILURE;
}


/////////////////////////////////////////////////////
/// \fn getCoinsValues(char *key,int *coins)
/// \brief recuperer notre somme d'argent
///
/// \param char *key Ecriture de la clé dans key
/// \param int *coins Valeur de retour de notre somme d'argent
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int getCoinsValues(char *key,char *retour)
{
	printf("getcoin\n");
	char *request;
	char *response;
	if ( !construire_requete(&request, NULL, NULL, key, NULL, NULL) )
	{
		if ( !envoyez_requet(&response,URL_GET_COINS,request) )
		{
			printf("%s\n",response );
			strcpy(retour,response);
			free(request);
			request = NULL;
			free(response);
			response = NULL;
			printf("getcoine\n");
			return EXIT_SUCCESS;
		}
	}
	free(request);
	request = NULL;
	return EXIT_FAILURE;
}



/////////////////////////////////////////////////////
/// \fn buyGamePass(char *key, char *gameID)
/// \brief acheter un pass pour un jeu
///
/// \param char *key Ecriture de la clé dans key
/// \param char *gameID numero du jeux
///
/// \return EXIT_SUCCESS / EXIT_FAILURE
/////////////////////////////////////////////////////
int buyGamePass(char *key, char *gameID)
{
	char *request;
	char *response;
	if ( !construire_requete(&request, NULL, NULL, key, gameID, NULL) )
	{
		if ( !envoyez_requet(&response,URL_BUY_GAMEPASS,request) )
		{
			if ( !strcmp(response,"SUCCESS")  )
			{
				free(request);
				request = NULL;
				free(response);
				response = NULL;
				return EXIT_SUCCESS;
			}
			printf("%s\n",response );
			free(response);
			response = NULL;
		}
	}
	free(request);
	request = NULL;
	return EXIT_FAILURE;
}


int updateEnded;
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
int updateScore(EnvoiScore * envoiScore )
{
	updateEnded = 0;
	char *request;
	char *response;
	if ( !construire_requete(&request, NULL, NULL, envoiScore->key, envoiScore->gameID, envoiScore->score) )
	{
		if ( !envoyez_requet(&response,URL_UPDATE_SCORE,request) )
		{
			if ( !strcmp(response, "SUCCESS") )
			{
				printf("%s\n",response);
				free(request);
				request = NULL;
				free(response);
				response = NULL;
				updateEnded = 1;
				return EXIT_SUCCESS;
			}
			printf("%s\n",response );
			free(response);
			response = NULL;

		}
	}
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
