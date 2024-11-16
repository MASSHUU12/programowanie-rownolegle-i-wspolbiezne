all : comp run

comp : main.cpp
	g++ -O3 -std=c++11 -pthread -lgomp -fopenmp ./main.cpp -o main

run : comp
	./main 8
