CC = gcc
CFLAGS  = -g -Wall -pthread -D_DEFAULT_SOURCE -Iinclude

SRC:= ./src
OBJDIR:=./obj

default: find

find:  MainFindFileThreaded.o FileFinder.o MyQueue.o 
	$(CC) $(CFLAGS) -o find MainFindFileThreaded.o FileFinder.o MyQueue.o 


MainFindFileThreaded.o:  $(SRC)/MainFindFileThreaded.c include/FileFinder.h
	$(CC) $(CFLAGS) -c $(SRC)/MainFindFileThreaded.c


FileFinder.o:  $(SRC)/FileFinder.c  include/FileFinder.h include/MyQueue.h
	$(CC) $(CFLAGS) -c $(SRC)/FileFinder.c


MyQueue.o:  $(SRC)/MyQueue.c include/MyQueue.h 
	$(CC) $(CFLAGS) -c $(SRC)/MyQueue.c


clean: 
	$(RM) -f *.o *~