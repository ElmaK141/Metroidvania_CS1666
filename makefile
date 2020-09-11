LIBS = -lSDL2 -lSDL2_image

main: main.cpp game.o
	g++ -o main main.cpp game.o $(LIBS)
	
game.o: game.cpp game.h
	g++ -c game.cpp $(LIBS)