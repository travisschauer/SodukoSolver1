project2: main.o 
	g++ -o project2 main.o 

main.o: main.cc stack.h
	g++ -c main.cc
