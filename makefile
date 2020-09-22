CC = g++
LIBS = -lSDL2 -lSDL2_image

main: main.cpp game.o game.h sprite.o
	$(CC) -o main main.cpp game.o sprite.o $(LIBS)
	
game.o: game.cpp sprite.h sprite.o entity.h
	$(CC) -c game.cpp sprite.o $(LIBS)
	
entity.o: entity.cpp entity.h
	$(CC) -c entity.cpp $(LIBS)
	
sprite.o: sprite.cpp sprite.h
	$(CC) -c sprite.cpp $(LIBS)

tile.o: tile.cpp tile.h sprite.h sprite.o
	$(CC) -c tile.cpp sprite.o $(LIBS)
