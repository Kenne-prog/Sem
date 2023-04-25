all: game.c barbarian.c wizard.c rogue.c dungeon.o
	
	gcc barbarian.c -o barbarian.o -lrt
	gcc wizard.c -o wizard.o -lrt
	gcc rogue.c -o rogue.o -lrt 
	gcc game.c dungeon.o -o  game -lrt -pthread
clean: 
	$(RM) barbarian.o wizard.o rogue.o game.o