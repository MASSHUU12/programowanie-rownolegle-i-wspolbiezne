#!/usr/bin/env bash

g++ -O3 -std=c++11 -pthread ./server.cpp -o server
g++ -O3 -std=c++11 -pthread ./client.cpp -o client
