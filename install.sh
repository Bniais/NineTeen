cd launcher/
gcc -c launcher.c `sdl2-config --cflags --libs` 
cd ..
cd room/
gcc -c room.c `sdl2-config --cflags --libs`
cd ..
make

