#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

//#define RAND_MAX 2147483647


//
//	X = ( ( X * const_1  - const_2 * const_3 ) / const_4 ) 
//

long long hashage (int value, long const_1, long const_2, long const_3, long const_4 ){
	
	long long hash;
	hash = value * const_1;
	hash = hash - const_2;
	hash = hash * const_3;	
	hash = hash / const_4;
	return hash;
}


void init_const(long *const_1, long *const_2, long *const_3, long *const_4){
	srand (time (NULL));
	*const_1 = rand();
	*const_2 = rand();
	*const_3 = rand();
	*const_4 = rand();

}


int main() {
	int valeur = 7111;
	
	long long hash;
	
	long const_1, const_2, const_3, const_4;
	init_const(&const_1, &const_2, &const_3, &const_4);
	printf("CONST_1 = %ld \nCONST_2 = %ld \nCONST_3 = %ld \nCONST_4 = %ld \n",const_1,const_2,const_3,const_4);
	
	
	hash = hashage(valeur, const_1, const_2, const_3, const_4);

	printf("HASH VALUE %lld",hash);
	
	printf("\nAdresse de valeur dans la memoire -> %p",&valeur);
	
	do {
		sleep(1);
		if( hash == hashage(valeur, const_1, const_2, const_3, const_4) ){
			printf("\nValeur identique : %d",valeur);
			// si modification necessaire du score
			// rappel de la fonction init_const pour generer une nouvelle valeur de hashage
			init_const(&const_1, &const_2, &const_3, &const_4);
			// nouveau hasage de la valeur changer
			valeur ++;
			hash = hashage(valeur, const_1, const_2, const_3, const_4);

		} else {
			printf("\nValeur modifier");
			return EXIT_FAILURE;
		}
	
	} while(1);
	
}