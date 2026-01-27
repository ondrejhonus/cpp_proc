#!/bin/bash

# Build project
g++ -Wall -Wextra -g3 ./src/main.cpp ./src/cpm.cpp -o ./output/main
# put binary into /bin/cpm
sudo cp ./output/main /bin/cpm