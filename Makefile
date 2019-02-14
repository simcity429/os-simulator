all: simulator
simulator: simulator.c mydatastruct.c mydatastruct.h
	gcc -o simulator mydatastruct.c mydatastruct.h simulator.c
clean: 
	rm *.txt simulator
