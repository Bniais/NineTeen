///////////////////////////////////////////////////////////////
/// \file textField.h
/// \author Mahi S.
/// \brief Permet l'interaction avec une zone de saisi
/// \version 1.0
/// \date 19 Janvier 2020
///////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////
/// \fn int textField (SDL_Renderer* renderer, TTF_Font *police, SDL_Color color, char *chaine, int startWhere, SDL_Rect cible)
/// \brief Recuperer une chaine de caractere saisi au clavier.
///
/// Copier la nouvelle texture dans SDL_Renderer, il est necessaire de realiser un RenderPresent().
///
/// \param SDL_Renderer *renderer Pointeur sur le rendu SDL.
/// \param TTF_Font *police Pointeur sur la police SDL.
/// \param SDL_Color color Couleur du texte a affichager.
/// \param char *chaine ecrit la chaine de caracatere dans la variable en entre.
/// \param int startWhere mettre strlen(chaine) pour reprendre a la fin de la chaine la saisi.
/// \param SDL_Rect cible indiquer la cible ou afficher le resultat.
/// \return retourne la raison de sorti (TF_MAX_SIZE, TF_TEXT_TYPED..).
///////////////////////////////////////////////////////////////
int textField(SDL_Renderer* renderer, TTF_Font *police, SDL_Color color, char *chaine, int startWhere, SDL_Rect cible);