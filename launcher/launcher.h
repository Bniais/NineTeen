
// fonction principal


//verifier l'existance d'un fichier
// passer le repertoire complet en
// parametre de la fonction

int verify_file_existe(char name[]);


SDL_Texture *load_texture_png(SDL_Renderer* renderer, char directory[]);


int apply_renderer_texture(SDL_Renderer* renderer , SDL_Texture * texture);

void pauseClavier(Uint32 tempsMax);

// compile .o commande : gcc -c launcher.c `sdl2-config --cflags --libs` -lSDL2_image

//#define NB_FILE 55
/*char verifierFichier[NB_FILE][128]={

	"../room/borne1.wav",
	"../room/borne2.wav",
	"../room/borne3.wav",
	"../room/walk.wav",
	"../room/sega.ttf",
	"../room/police.ttf",
	"../room/salle.obj",
	"../room/salle.mtl",

	"../games/3_flappy_bird/Textures/tilset.png",
	"../games/3_flappy_bird/Textures/birds.png",
	"../games/3_flappy_bird/Textures/medals.png",
	"../games/3_flappy_bird/Textures/pipes.png",
	"../games/3_flappy_bird/Textures/scoreBoard.png",
	"../games/3_flappy_bird/Textures/backgrounds.png",
	"../games/3_flappy_bird/Textures/chiffre.png",
	"../games/3_flappy_bird/Textures/sol.png",
	"../games/3_flappy_bird/Textures/high_score.png",
	"../games/3_flappy_bird/Sounds/flap.wav",
	"../games/3_flappy_bird/Sounds/hurt.wav",
	"../games/3_flappy_bird/Sounds/score.wav",

	"../games/2_snake/Fonts/flappy.ttf",
	"../games/2_snake/Fonts/zorque.ttf",
	"../games/2_snake/Textures/anim.png",
	"../games/2_snake/Textures/backgroundSnake.png",
	"../games/2_snake/Textures/basket.png",
	"../games/2_snake/Textures/chiffre.png",
	"../games/2_snake/Textures/fruits.png",
	"../games/2_snake/Textures/hud.png",
	"../games/2_snake/Textures/snake.png",

	"../games/5_tetris/Textures/bonus.png",
	"../games/5_tetris/Textures/bricks.png",
	"../games/5_tetris/Textures/chiffre.png",
	"../games/5_tetris/Textures/fleche.png",
	"../games/5_tetris/Textures/hud_grille.png",
	"../games/5_tetris/Textures/laserAnim.png",
	"../games/5_tetris/Textures/speedJauge.png",
	"../games/5_tetris/Textures/turn.png",

	"../games/7_asteroid/Textures/asteroid.png",
	"../games/7_asteroid/Textures/background.png",
	"../games/7_asteroid/Textures/bomb.png",
	"../games/7_asteroid/Textures/bombIcon.png",
	"../games/7_asteroid/Textures/bonus.png",
	"../games/7_asteroid/Textures/bullet.png",
	"../games/7_asteroid/Textures/explo.png",
	"../games/7_asteroid/Textures/explo2.png",
	"../games/7_asteroid/Textures/explo2.png",
	"../games/7_asteroid/Textures/explo3.png",
	"../games/7_asteroid/Textures/fissure.png",
	"../games/7_asteroid/Textures/gem.png",
	"../games/7_asteroid/Textures/glace.png",
	"../games/7_asteroid/Textures/jauge.png",
	"../games/7_asteroid/Textures/laser_beam.png",
	"../games/7_asteroid/Textures/roue.png",
	"../games/7_asteroid/Textures/thrust.png",
	"../games/7_asteroid/Textures/vaisseau.png"

};*/
