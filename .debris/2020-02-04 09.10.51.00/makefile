all : flappy_bird.c
	gcc -Wall -ansi -D_APPLE_ flappy_bird.c -o flappy `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer 
debug : flappy_bird.c
	gcc -g flappy_bird.c -o flappy_debug `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer