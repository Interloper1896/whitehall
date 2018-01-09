#! /bin/sh
#gcc -o t2 -I/usr/include/SDL2 t2.c -lSDL2_image -lSDL2
#gcc -o t3 -I/usr/include/SDL2 t3.c -lSDL2_image -lSDL2 -lpthread
#gcc -o t4 -I/usr/include/SDL2 t4.c -lSDL2_image -lSDL2 -lpthread
#gcc -o t5 -I/usr/include/SDL2 t5.c -lSDL2_image -lSDL2 -lpthread
#gcc -o t6 -I/usr/include/SDL2 t6.c -lSDL2_image -lSDL2 -lpthread
gcc -o t7 -I/usr/include/SDL2 t7.c -lSDL2_image -lSDL2 -lpthread
gcc -o client client.c
gcc -o server_white server_white.c
