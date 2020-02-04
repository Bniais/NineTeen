#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL.h>
#include "../define/define.h"
// fonction principal
int launcher( SDL_Renderer* renderer, char *identifiant, char *token);

//verifier l'existance d'un fichier
// passer le repertoire complet en
// parametre de la fonction

int verify_file_existe(char name[]);


SDL_Texture *load_texture_png(SDL_Renderer* renderer, char directory[]);


int apply_renderer_texture(SDL_Renderer* renderer , SDL_Texture * texture);

void pauseClavier(Uint32 tempsMax);


// compile .o commande : gcc -c launcher.c `sdl2-config --cflags --libs` -lSDL2_image
