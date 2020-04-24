void rotatePiece(Piece *piece, int rotateSens, int matrix[GRILLE_W][GRILLE_H]){
	if( rotateSens ){

		RowColInfos originalInfos = {piece->firstCol, piece->lastCol, piece->firstRow, piece->lastRow};

		piece->rota += rotateSens;

		updateGrille(piece);
		getColRawInfos(piece);


		//try rotates :

		int downShift=0, rightShift=0, leftShift=0;
		int found = SDL_FALSE;
		//try down
		printf("d%d l%d r%d\n", originalInfos.firstRow - piece->firstRow, piece->firstCol - originalInfos.firstCol, originalInfos.lastCol - piece->lastCol);

		for(downShift=0; downShift <= originalInfos.firstRow - piece->firstRow && !found ; downShift++, piece->y++){
			printf("down : %d\n",downShift );
			//down left
			for(leftShift=0; leftShift <= piece->firstCol - originalInfos.firstCol && !found; leftShift++, piece->x++){
				printf("left : %d\n",leftShift );
				if( !tooCloseFromWall(*piece) && !tooCloseFromMatrix(*piece, matrix)){
					found = SDL_TRUE;
				}
				else
					printf("LEFT NOT FOUND\n");
			}
			//rever left
			if(!found){
				printf("LEFT NOT FOUND AT ALL\n");
				piece->x -= leftShift;
			}



			//down right
			for(rightShift=0; rightShift <= originalInfos.lastCol - piece->lastCol  &&  !found ; rightShift++, piece->x--){
				printf("right : %d\n",rightShift );
				if( !tooCloseFromWall(*piece) && !tooCloseFromMatrix(*piece, matrix)){
					found = SDL_TRUE;
				}
				else
					printf("RIGHT NOT FOUND\n");
			}
			//revert right
			if(!found){
				printf("RIGHT NOT FOUND AT ALL\n");
				piece->x += rightShift;
				printf("DOWN NOT FOUND\n");
			}
		}
		printf("final shift : d%d l%d r%d\n",downShift, leftShift, rightShift );
		//revert all
		if(!found){
			printf("DOWN NOT FOUND AT ALL\n");
			piece->y -= downShift;
			piece->rota -= rotateSens;
			updateGrille(piece);
			getColRawInfos(piece);
		}
	}
}

//En executant le programme comme ça, on se rend compte que la collision s'effectue normalement mais que la pièce est décallée.
// Les boucles for font en effet un deuxième tour même si on a trouvé un emplacement vialbe, donc il faut mettre un break si on a trouvé !

// Solution :
void rotatePiece(Piece *piece, int rotateSens, int matrix[GRILLE_W][GRILLE_H]){
	if( rotateSens ){

		RowColInfos originalInfos = {piece->firstCol, piece->lastCol, piece->firstRow, piece->lastRow};

		piece->rota += rotateSens;

		updateGrille(piece);
		getColRawInfos(piece);


		//try rotates :

		int downShift=0, rightShift=0, leftShift=0;
		int found = SDL_FALSE;
		//try down
		printf("d%d l%d r%d\n", originalInfos.firstRow - piece->firstRow, piece->firstCol - originalInfos.firstCol, originalInfos.lastCol - piece->lastCol);

		for(downShift=0; downShift <= originalInfos.firstRow - piece->firstRow && !found ; downShift++, piece->y++){
			printf("down : %d\n",downShift );
			//down left
			for(leftShift=0; leftShift <= piece->firstCol - originalInfos.firstCol && !found; leftShift++, piece->x++){
				printf("left : %d\n",leftShift );
				if( !tooCloseFromWall(*piece) && !tooCloseFromMatrix(*piece, matrix)){
					found = SDL_TRUE;
					break;
				}
				else
					printf("LEFT NOT FOUND\n");
			}
			//rever left
			if(!found){
				printf("LEFT NOT FOUND AT ALL\n");
				piece->x -= leftShift;
			}



			//down right
			for(rightShift=0; rightShift <= originalInfos.lastCol - piece->lastCol  &&  !found ; rightShift++, piece->x--){
				printf("right : %d\n",rightShift );
				if( !tooCloseFromWall(*piece) && !tooCloseFromMatrix(*piece, matrix)){
					found = SDL_TRUE;
					break;
				}
				else
					printf("RIGHT NOT FOUND\n");
			}
			//revert right
			if(!found){
				printf("RIGHT NOT FOUND AT ALL\n");
				piece->x += rightShift;
				printf("DOWN NOT FOUND\n");
			}
			else{
				break;
			}
		}
		printf("final shift : d%d l%d r%d\n",downShift, leftShift, rightShift );
		//revert all
		if(!found){
			printf("DOWN NOT FOUND AT ALL\n");
			piece->y -= downShift;
			piece->rota -= rotateSens;
			updateGrille(piece);
			getColRawInfos(piece);
		}
	}
}



// finished :
void rotatePiece(Piece *piece, int rotateSens, int matrix[GRILLE_W][GRILLE_H]){
	if( rotateSens ){

		RowColInfos originalInfos = {piece->firstCol, piece->lastCol, piece->firstRow, piece->lastRow};

		piece->rota += rotateSens;

		updateGrille(piece);
		getColRawInfos(piece);


		//try rotates :

		int downShift=0, rightShift=0, leftShift=0, upShift = 0;
		int maxDownShift=originalInfos.firstRow - piece->firstRow,
			maxUpShift = piece->lastRow -  originalInfos.lastRow,
			maxLeftShift=piece->lastCol - originalInfos.lastCol,
			maxRightShift= originalInfos.firstCol - piece->firstCol;

		int found = SDL_FALSE;

		printf("d%d u%d l%d r%d \n", maxDownShift, maxUpShift,maxLeftShift, maxRightShift);
		printf("piece->x : %f\n",piece->x );
		//try normal
		if( !(tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix) )  ){
			found = SDL_TRUE;
		}
		printf("normal failed\n\n");

		//try sides
		//left
		if(!found){
			for(leftShift=0; leftShift <= maxLeftShift  && !found; leftShift++, piece->x--){
				printf("left : %d\n",leftShift );
				if( !(tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix) ) ){
					found = SDL_TRUE;
					break;
				}
				else
					printf("LEFT NOT FOUND\n\n");
			}
			//rever left
			if(!found){
				printf("LEFT NOT FOUND AT ALL\n\n");
				piece->x += leftShift;
			}
		}
		//right
		if(!found){
			for(rightShift=0; rightShift <= maxRightShift  &&  !found ; rightShift++, piece->x++){
				printf("right : %d\n",rightShift );
				if( !(tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix) )  ){
					found = SDL_TRUE;
					break;
				}
				else
					printf("RIGHT NOT FOUND\n\n");
			}
			//revert right
			if(!found){
				printf("RIGHT NOT FOUND AT ALL\n\n");
				piece->x -= rightShift;
				printf("DOWN NOT FOUND\n\n");
			}
		}

		if(!found){
			//try down
			for(downShift=0; downShift <= maxDownShift && !found ; downShift++, piece->y++){
				printf("down : %d\n",downShift );
				printf("piece->x : %f.0\n",piece->x );
				if( !(tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix) )  ){
					found = SDL_TRUE;
					break;
				}

				//down left
				if(!found){
					piece->x--;
					for(leftShift=1; leftShift <= maxLeftShift  && !found; leftShift++, piece->x--){
						printf("left : %d\n",leftShift );
						if( !(tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix) ) ){
							found = SDL_TRUE;
							break;
						}
						else
							printf("LEFT NOT FOUND\n\n");
					}
					//rever left
					if(!found){
						printf("LEFT NOT FOUND AT ALL\n\n");
						piece->x += leftShift;
					}
					else break;
				}
				else break;



				//down right
				if(!found){
					piece->x++;
					for(rightShift=1; rightShift <= maxRightShift  &&  !found ; rightShift++, piece->x++){
						printf("right : %d\n",rightShift );
						if( !(tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix) )  ){
							found = SDL_TRUE;
							break;
						}
						else
							printf("RIGHT NOT FOUND\n\n");
					}
					//revert right
					if(!found){
						printf("RIGHT NOT FOUND AT ALL\n\n");
						piece->x -= rightShift;
						printf("DOWN NOT FOUND\n\n");
					}
					else break;
				}
				else break;

			}
			//revert down
			if(!found){
				printf("DOWN NOT FOUND AT ALL\n\n");
				piece->y -= downShift;
			}
		}


		if(!found){
			//try up
			for(upShift=0; upShift <= maxUpShift && !found ; upShift++, piece->y--){
				printf("up : %d\n",upShift );
				printf("piece->x : %f.1\n",piece->x );
				if( !(tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix) )  ){
					found = SDL_TRUE;
					break;
				}

				//down left
				if(!found){
					piece->x--;
					for(leftShift=1; leftShift <= maxLeftShift  && !found; leftShift++, piece->x--){
						printf("left : %d\n",leftShift );
						if( !(tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix) ) ){
							found = SDL_TRUE;
							break;
						}
						else
							printf("LEFT NOT FOUND\n\n");
					}
					//rever left
					if(!found){
						printf("LEFT NOT FOUND AT ALL\n\n");
						piece->x += leftShift;
					}
					else break;
				}
				else break;



				//down right
				if(!found){
					piece->x++;
					for(rightShift=1; rightShift <= maxRightShift  &&  !found ; rightShift++, piece->x++){
						printf("right : %d\n",rightShift );
						if( !(tooCloseFromWall(*piece) || tooCloseFromMatrix(*piece, matrix) )  ){
							found = SDL_TRUE;
							break;
						}
						else
							printf("RIGHT NOT FOUND\n\n");
					}
					//revert right
					if(!found){
						printf("RIGHT NOT FOUND AT ALL\n\n");
						piece->x -= rightShift;
						printf("UP NOT FOUND\n\n");
					}
					else break;
				}
				else break;

			}
		}
		//revert down
		if(!found){
			printf("UP NOT FOUND AT ALL\n\n");
			piece->y += upShift;
		}





		if(!found){
			printf("final shift : d%d u%d l%d r%d\n",downShift, upShift, leftShift, rightShift );
			piece->rota -= rotateSens;
			updateGrille(piece);
			getColRawInfos(piece);
		}
		else{
			printf("couldnt shift\n");
		}
	}
}
