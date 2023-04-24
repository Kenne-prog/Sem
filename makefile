all: game.c barbarian.c wizard.c rogue.c dungeon.o

game: game.c dungeon.o
	gcc game.c dungeon.o -o  dungeon -lrt -pthread

barbarian: barbarian.c dungeon.o
	gcc barbarian.c -o barbarian.o -lrt

wizard: wizard.c dungeon.o
	gcc wizard.c -o wizard.o -lrt

rogue: rogue.c dungeon.o
	gcc rogue.c -o rogue.o -lrt

clean:
	$(RM) barbarian.o wizard.o rogue.o game.o
