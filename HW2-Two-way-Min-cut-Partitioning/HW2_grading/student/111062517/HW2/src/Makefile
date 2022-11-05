all:	hw2

hw2:	main.cpp fm.o
	g++ -std=c++11 -O3 -g main.cpp fm.o -o ../bin/hw2

fm.o:	fm.h fm.cpp header.h
	g++ -std=c++11 -O3 -c -g fm.cpp

clean:
	rm ../bin/hw2 *.o
