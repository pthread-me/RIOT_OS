COMPILER = gcc
VERSION  = --std=c18
FLAGS = -lm

main: main.o
	$(COMPILER) $(VERSION)  main.o -o a
	rm main.o

main.o:
	$(COMPILER) -c main.c
