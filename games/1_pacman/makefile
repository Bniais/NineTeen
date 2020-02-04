all : pacman.c
	gcc pacman.c -o pacman `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer 
debug : pacman.c
	gcc -g pacman.c -o pacman_debug `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer