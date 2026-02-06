#!/bin/bash

cmake -B build
cmake --build build
./build/cpp_proc
sudo cp ./build/cpp_proc /bin/cpm
echo -e "\nSuccessfully installed CPM!\nThank you for using my program! <3\n"