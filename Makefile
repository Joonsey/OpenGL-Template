CC = gcc

main:
	$(CC) main.cpp -o test -Wall -lstdc++ -lglfw -lGL -lm -lX11 -lpthread -lXi -lXrandr -lGLEW
