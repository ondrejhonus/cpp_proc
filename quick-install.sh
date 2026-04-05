#!/bin/bash
set -e

echo "Installing CPM Task Manager..."

TEMP_DIR=$(mktemp -d)
cd "$TEMP_DIR"

echo "Downloading source code..."
git clone https://github.com/ondrejhonus/cpp_proc.git .

echo "🛠️  Building CPM (this might take a few seconds)..."
cmake -B build -DCMAKE_BUILD_TYPE=Release
make -C build -j$(nproc)

echo "Installing to /usr/local/bin (requires sudo)..."
sudo make -C build install

cd ~
rm -rf "$TEMP_DIR"

echo "CPM installed successfully! Type 'cpm' in your terminal to run."