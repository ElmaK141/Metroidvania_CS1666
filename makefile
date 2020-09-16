LIBS = -lSDL2 -lSDL2_image

main: main.cpp game.o game.h
	g++ -o main main.cpp game.o $(LIBS)
	
Game.o: game.cpp game.h
	g++ -c game.cpp $(LIBS)
