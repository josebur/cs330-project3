# This is (at least the start of) a makefile for CSP 310, 
#   project#1, when using the 'C++' programming language
## NOTE: any LINE starting with a '#' in a Makefile is a comment

CC=g++ -Wall -ansi -pedantic -g
OBJS=Token.o main.o
GLBL=Makefile Token.h

heapify: $(OBJS)
	$(CC) -o heapify $(OBJS)

main.o: main.cpp $(GLBL)
	$(CC) -c main.cpp

Token.o: Token.cpp $(GLBL)
	$(CC) -c Token.cpp

clean:
	/bin/rm -f *.o parser *~ core
