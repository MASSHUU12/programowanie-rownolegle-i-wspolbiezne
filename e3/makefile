all : main png

main : main.cpp
	./build.sh

maze : main
	./maze

png : maze.ppm
	magick maze.ppm maze_original.png
	magick maze.ppm -resize 512x512 -filter Lanczos -quality 95 maze_upscaled.png
