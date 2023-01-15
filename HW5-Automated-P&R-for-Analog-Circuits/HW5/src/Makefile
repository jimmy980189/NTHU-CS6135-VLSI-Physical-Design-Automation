CC = g++
CPPFLAGS = -g -std=c++11
#CPPFLAGS = -g -std=c++11 -O3
#CPPFLAGS = -g -std=c++11 -Wall -Werror -Wextra -O3

all:	hw5 

hw5:	main.cpp header.h die.o
	$(CC) $(CPPFLAGS) main.cpp die.o -o ../bin/hw5

die.o: 	die.cpp header.h
	$(CC) $(CPPFLAGS) -c die.cpp 

clean:
	rm *.o ../bin/hw5


