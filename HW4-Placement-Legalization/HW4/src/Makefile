CC = g++
#CPPFLAGS = -g -std=c++11 -O3
CPPFLAGS = -g -std=c++11 -Wall -Werror -Wextra -O3

all:	hw4 

hw4:	main.cpp header.h placement.o 
	$(CC) $(CPPFLAGS) main.cpp placement.o -o ../bin/hw4

placement.o:	header.h placement.cpp placement.h
	$(CC) $(CPPFLAGS) -c placement.cpp 

clean:
	rm *.o ../bin/hw4


