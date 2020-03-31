
// fonction principal


//verifier l'existance d'un fichier
// passer le repertoire complet en
// parametre de la fonction

int verify_file_existe(char name[]);


SDL_Texture *load_texture_png(SDL_Renderer* renderer, char directory[]);


int apply_renderer_texture(SDL_Renderer* renderer , SDL_Texture * texture);

void pauseClavier(Uint32 tempsMax);

// compile .o commande : gcc -c launcher.c `sdl2-config --cflags --libs` -lSDL2_image

#define NB_FILE 20
char verifierFichier[NB_FILE][128]={"room/01.wav","room/02.wav","room/03.wav","room/walk.wav","room/sega.ttf","room/police.ttf","room/salle.obj","room/salle.mtl",
                            "games/3_flappy_bird/Textures/tilset.png","games/3_flappy_bird/Textures/birds.png","games/3_flappy_bird/Textures/medals.png","games/3_flappy_bird/Textures/pipes.png","games/3_flappy_bird/Textures/scoreBoard.png","games/3_flappy_bird/Textures/backgrounds.png","games/3_flappy_bird/Textures/chiffre.png","games/3_flappy_bird/Textures/sol.png","games/3_flappy_bird/Textures/high_score.png","games/3_flappy_bird/Sounds/flap.wav","games/3_flappy_bird/Sounds/hurt.wav","games/3_flappy_bird/Sounds/score.wav"};
