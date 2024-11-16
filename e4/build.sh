#!/usr/bin/env bash

g++ -O3 -std=c++11 -pthread -lgomp -fopenmp ./main.cpp
