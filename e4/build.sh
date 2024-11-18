#!/usr/bin/env bash

g++ -O3 -march=native -std=c++11 -pthread -lgomp -fopenmp ./main.cpp
