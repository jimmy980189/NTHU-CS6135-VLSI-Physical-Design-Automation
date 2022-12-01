CC = g++
CPPFLAGS = -g -std=c++11 
#CPPFLAGS = -g -std=c++11 -Wall -Werror -Wextra

all:	hw3 

hw3:	main.cpp floorplan.o
	$(CC) $(CPPFLAGS) main.cpp floorplan.o -o ../bin/hw3

floorplan.o:	floorplan.cpp floorplan.h header.h 
	$(CC) $(CPPFLAGS) -c floorplan.cpp 

clean:
	rm *.o ../bin/hw3


