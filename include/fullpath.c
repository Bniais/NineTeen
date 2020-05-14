/////////////////////////////////////////////////
///	\file fullpath.c
///	\author Samy M.
///	\version 1.0
///	\date 31 janvier 2020
///	\brief permet de reconstruire un lien de fichier complet sur mac OS
/////////////////////////////////////////////////

#include <stdio.h>
FILE *EXT_FILE;
#include <stdlib.h>
#include <string.h>
#include <limits.h>



/////////////////////////////////////////////
/// \fn int nbOccurence(char s[], char c)
/// \brief compte le nombre d occurence d un caractere
///
/// \param s
/// \param c
///
/// \return retourne le nombre d occurence d un caractere
/////////////////////////////////////////////
static
int nbOccurence(char s[], char c)
{
	int occurence = 0;

	for (int i=0; s[i] != '\0' ;i++)
	{
		if( s[i] == c )
			occurence++;
	}
	return occurence;
}


/////////////////////////////////////////////
/// \fn char *completPathOf(char s[],int mode)
/// \brief complete le path
///
/// \param s
/// \param mode
///
/// \return la nouvelle chaine
/////////////////////////////////////////////
static
char *completPathOf(char s[],int mode)
{
	////////////////////////////////////
	// SUPPRIME LE NOM DU PROGRAMME
	int maxCopie;
	for(maxCopie = strlen(s) ; s[maxCopie] != '/' ; maxCopie--);
	////////////////////////////////////
	// ALLOUE DE LA MEMOIRE ON RETIRE LE MODE ./ OU SANS DANS L'APPEL
	char *chaine = malloc(sizeof(char) * (maxCopie - mode + 2) );

	////////////////////////////////////
	// ON COPIE DANS LA NOUVELLE CHAINE
	for(int i = 0 ; i <= maxCopie - mode ; i++)
		chaine[i] = s[i + mode];

	if(chaine[maxCopie-1] != '/')
	{
		chaine[maxCopie] = '/';
		chaine[maxCopie+1] = '\0';
	}
	else
		chaine[maxCopie] = '\0';
	////////////////////////////////////
	// ON RETOURNE LE RESULTAT
	return chaine;
}

/////////////////////////////////////////////
/// \fn int detectionMode(char s[])
/// \brief detecte le mode si besoin ou non
///
/// \param s
///
/// \return 1/0
/////////////////////////////////////////////
static
int detectionMode(char s[])
{
	int i = 0;
	while(s[i++] != '.' );
	if(s[i] == '/')
		return 1;
	else
		return 0;
}


/////////////////////////////////////////////
/// \fn char *fullPath(char s[])
/// \brief fonction principal que ce charge de faire l appel de chaque fonction
///
/// \param s
///
/// \return nouvelle chaine avec le path complet pour mac
/////////////////////////////////////////////
char *fullPath(char s[])
{
	////////////////////////////////////
	// OCCURENCE POUR CHAQUE ELLEMENT
	int point = nbOccurence(s,'.');
	int slash = nbOccurence(s,'/');
	char *path = NULL;

	////////////////////////////////////
	// DETECTION DU MODE ./ OU NON
	if(point >= 1)
	{
		if( detectionMode(s) )
		{
			if(slash > 1)
			{
				fprintf(EXT_FILE,"fullpath.c : fullePath() : il faut remonter %d dossie\n",slash -1);
				path = completPathOf(s,2);
			}
			else
			{
				fprintf(EXT_FILE,"fullpath.c : fullePath() : arborescence correct");
				path = malloc(sizeof(char));
				path[0] = '\0';
			}

		}
		else
		{
			if(slash == 0)
			{
				fprintf(EXT_FILE,"fullpath.c : fullePath() : arborescence correct");
				path = malloc(sizeof(char));
				path[0] = '\0';
			}
			else
				path = completPathOf(s,0);
			}
	}
	else
	{
		if(slash == 0)
		{
			fprintf(EXT_FILE,"fullpath.c : fullePath() : arborescence correct");
			path = malloc(sizeof(char));
			path[0] = '\0';
		}

		else
			path = completPathOf(s,0);
	}

	fprintf(EXT_FILE,"fullpath.c : fullePath() : return value : %s\n",path );
	return path;

}
