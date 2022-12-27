CC = g++
CPPFLAGS = -g -std=c++11 
#CPPFLAGS = -g -std=c++11 -Wall -Werror -Wextra

all:	hw4 

hw4:	main.cpp header.h placement.o 
	$(CC) $(CPPFLAGS) main.cpp placement.o -o ../bin/hw4

floorplan.o:	header.h placement.cpp placement.h
	$(CC) $(CPPFLAGS) -c placement.cpp 

clean:
	rm *.o ../bin/hw4


