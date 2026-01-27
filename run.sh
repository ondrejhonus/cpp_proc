#!/bin/bash

g++ -Wall -Wextra -g3 ./src/main.cpp ./src/cpm.cpp -o ./output/main && ./output/main | less