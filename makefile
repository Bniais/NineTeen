game : main.c
	gcc main.c launcher/launcher.c room/room.c `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf -lcurl
