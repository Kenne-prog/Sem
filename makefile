all: game.c barbarian.c wizard.c rogue.c dungeon.o
	gcc barbarian.c -o barbarian.o -lrt -pthread
	gcc wizard.c -o wizard.o -lrt -pthread
	gcc rogue.c -o rogue.o -lrt -lm -pthread
	gcc game.c dungeon.o -o dungeon -lrt -pthread
clean: 
	$(RM) barbarian.o wizard.o rogue.o game.o