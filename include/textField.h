///////////////////////////////////////////////////////////////
/// \file textField.h
/// \author Mahi S.
/// \brief Permet l'interaction avec une zone de saisi
/// \version 1.0
/// \date 19 Janvier 2020
///////////////////////////////////////////////////////////////

#define TF_MOUSE_OUT_CLICK 10
#define TF_RETURN 11
#define TF_MAX_SIZE 12
#define TF_TEXT_TYPED 13
#define TF_TAB 14

#define RESPONDER_TRUE 1
#define RESPONDER_FALSE 2

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
int textField(SDL_Renderer* renderer, TTF_Font *police, SDL_Color color, char *chaine, int startWhere, SDL_Rect *cible, SDL_Point *mouse,int *pressMaj);


///////////////////////////////////////////////////////////////
/// \fn void renduTextField (SDL_Renderer *renderer,char *chaine ,TTF_Font *font , SDL_Color color, SDL_Rect cible)
/// \brief sous fonction de textField()
///
/// Aucun parametre n'est a saisir, elle est appeler par textField().
/// Elle permet de copier la texture dans la zone de rendu.
///
///////////////////////////////////////////////////////////////
void renduTextField(SDL_Renderer *renderer,char *chaine ,TTF_Font *font , SDL_Color color, SDL_Rect cible);

int TF_ClickIn(SDL_Rect rect, SDL_Point mouse);
