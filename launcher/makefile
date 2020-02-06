game : launcher.c
	gcc ../include/libWeb.c ../include/textField.c  `sdl2-config --cflags --libs` launcher.c -o launcher  -lSDL2 -lcurl -lcrypto -lSDL2_ttf -lSDL2_image
debug : launcher.c
		gcc ../include/libWeb.c ../include/textField.c `sdl2-config --cflags --libs` launcher.c -g -Wall -o launcher  -lSDL2 -lcurl -lcrypto -lSDL2_ttf -lSDL2_image
