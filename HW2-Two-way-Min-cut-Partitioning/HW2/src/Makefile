all:	hw2

hw2:	main.cpp fm.o
	g++ --std=c++11 -g main.cpp fm.o -o ../bin/hw2

fm.o:	fm.h fm.cpp header.h
	g++ --std=c++11 -c -g fm.cpp

test:	hw2
	../bin/hw2 ../testcases/easy.cells ../testcases/easy.nets ../output/easy.out
	../bin/hw2 ../testcases/p2-1.cells ../testcases/p2-1.nets ../output/p2-1.out
	#../verifier/verify ../testcases/p2-1.cells ../testcases/p2-1.nets ../output/p2-1.out


clean:
	rm ../bin/hw2
