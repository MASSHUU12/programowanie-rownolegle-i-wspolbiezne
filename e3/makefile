all : main maze png

main : main.cpp
	g++ -O3 -std=c++11 -pthread ./main.cpp -o maze

maze : main
	./maze

png : maze.ppm
	magick maze.ppm maze_original.png
	magick maze.ppm -resize 512x512 -filter Lanczos -quality 95 maze_upscaled.png
