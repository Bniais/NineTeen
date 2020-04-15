game : launcher.c
	gcc ../room/opengl/room.c ../room/opengl/import.c ../games/3_flappy_bird/flappy_bird.c ../include/libWeb.c ../include/textField.c  `sdl2-config --cflags --libs` launcher.c -o launcher  -lSDL2 -lcurl -L/usr/local/opt/openssl/lib -lcrypto -lSDL2_ttf -lSDL2_image -lSDL2_mixer -framework OpenGL -framework GLUT -lm -lassimp
	rm .token
debug : launcher.c
	gcc ../include/libWeb.c ../include/textField.c `sdl2-config --cflags --libs` launcher.c -g -Wall -o launcher  -lSDL2 -lcurl -lcrypto -lSDL2_ttf -lSDL2_image
