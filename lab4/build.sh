#!/usr/bin/env bash

g++ -O3 -std=c++11 -pthread ./server.cpp -o server
gcc -O3 ./client.c -o client
