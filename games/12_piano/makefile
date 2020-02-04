all : piano.c
	gcc piano.c -o piano `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer 
debug : piano.c
	gcc -g piano.c -o piano_debug `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf -lSDL2_mixer