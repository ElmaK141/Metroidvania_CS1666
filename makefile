CC = g++
LIBS = -lSDL2 -lSDL2_image

main: main.cpp game.o game.h sprite.o button.o background.o entity.o tilemap.o tile.o physics.o block.o enemies.o gamemap.o
	$(CC) -o main main.cpp game.o sprite.o button.o background.o entity.o tilemap.o tile.o physics.o block.o enemies.o gamemap.o $(LIBS)
	
game.o: game.cpp sprite.h game.h sprite.h entity.h tile.h background.h button.h
	$(CC) -c game.cpp 
	
entity.o: entity.cpp entity.h physics.h
	$(CC) -c entity.cpp 
	
sprite.o: sprite.cpp sprite.h
	$(CC) -c sprite.cpp 

tile.o: tile.cpp tile.h
	$(CC) -c tile.cpp  
	
background.o: background.cpp background.h
	$(CC) -c background.cpp

tilemap.o: tilemap.cpp tilemap.h block.h
	$(CC) -c tilemap.cpp

button.o: button.cpp button.h
	$(CC) -c button.cpp

physics.o: physics.cpp physics.h
	$(CC) -c physics.cpp
	
#New stuff
block.o: block.cpp block.h
	$(CC) -c block.cpp
	
enemies.o: enemies.cpp enemies.h
	$(CC) -c enemies.cpp
	
gamemap.o: gamemap.cpp gamemap.h
	$(CC) -c gamemap.cpp
