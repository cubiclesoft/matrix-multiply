#!/bin/bash

gcc -std=c++0x -pedantic -Wall -Wextra -Wshadow -Wpointer-arith -Wcast-qual -O3 matrixmult.cpp -o matrixmult -lstdc++
