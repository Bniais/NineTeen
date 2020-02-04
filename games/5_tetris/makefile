all : tetris.c
	gcc tetris.c `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf -lm -Wall -std=c99 -lSDL2_mixer

debug : tetris.c
	gcc tetris.c -g `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf -lm -Wall -std=c99 -lSDL2_mixer 
