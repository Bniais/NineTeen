
//Window
#define FRAMES_PER_SECOND 30


//grille
#define TAILLE_GRILLE_LIGNE 16
#define TAILLE_GRILLE_COLONNE 25
const int NOMBRE_BOMBES_GRILLE = (TAILLE_GRILLE_LIGNE*TAILLE_GRILLE_COLONNE)*0.25;
#define TAILLE_CASE 50
enum{MASQUE,MASQUE_AVEC_BOMBES,BOMBE,LIBRE,DANGER_AVEC_LIBRE_AUTOUR,DANGER_SANS_LIBRE_AUTOUR, DRAPEAU_AVEC_BOMBES, DRAPEAU_SANS_BOMBES};

/*les différents types de case sont les suivants :
-Une case est MASQUE lorsqu'elle n'a pas été découverte et n'a pas de bombe qui se cache derrière
-Une case est MASQUE_AVEC_BOMBES lorsqu'elle n'a pas été découverte mais qu'une bombe se cache derrière
-Une case est BOMBE lorsque l'utilisateur a cliqué sur une bombe et que le jeu se termine, alors toutes les cases MASQUE_AVEC_BOMBES deviennent BOMBE
-Une case est LIBRE lorsqu'elle est démasqué et qu'aucune bombe n'est autour de cette case
-Une case DANGER_AVEC_LIBRE_AUTOUR est une case ayant au moins une bombe autour et ayant au moins une case libre autour
-Une case DANGER_SANS_LIBRE_AUTOUR est une case ayant au moins une bombe autour et n'ayant aucune case libre autour
-Une case DRAPEAU_AVEC_BOMBES est une case où l'utilisateur a mis un drapeau dessus et où se trouve une bombe caché dans cette case
-une case DRAPEAU_SANS_BOMBES est une case où l'utilisateur a mis un drapeau dessus mais où il n'y a pas de bombe caché en dessous */
