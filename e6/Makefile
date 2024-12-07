all : main

main : main.cpp
	g++ -O3 -march=native -std=c++11 -lgomp -fopenmp ./main.cpp -o main

png : ulam_spiral.ppm
	magick ulam_spiral.ppm ulam_spiral.png
