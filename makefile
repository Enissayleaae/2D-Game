menu : main.o source.o
	gcc  main.o source.o -o menu -lSDL -lSDL_image -lSDL_ttf -lSDL_mixer
main.o : main.c
	gcc -c main.c
source.o : source.c
	gcc -c source.c
