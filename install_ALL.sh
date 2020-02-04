cd launcher/
gcc -c launcher.c `sdl2-config --cflags --libs` 
cd ..
cd room/
gcc -c room.c `sdl2-config --cflags --libs`
cd ..
cd games/2_snake
gcc -c snake.c `sdl2-config --cflags --libs` -lSDL2_image -lm
cd ../..
make

