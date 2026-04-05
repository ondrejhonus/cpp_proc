# C++ Process Manager (CPM)

An `htop`-like process manager made in `C++` using the [`ftxui`](https://github.com/ArthurSonzogni/FTXUI) library for TUI functionality.

<img src="./img/screenshot.png" width="75%" alt="Screenshot of CPM">


## Functions

- See all running processes
- See the processes' PID, name, state, memory usage, CPU usage
- See the process states (Sleeping, Running, Zombie...)
- Send a SIGnal to kill or control a specific process
- Order processes by PID, name, state, memory usage or CPU usage

## TO-DO

- Staticly define keybinds
- Separate useless stuff into functions to bottoms of files
- Search
- Filters

## Installation

#### Install dependencies:
- `cmake` for building the project
- `g++` for compiling the C++ code
- `libftxui-dev` for the TUI library

#### Quick CPM install with curl:
```bash
curl -sL https://raw.githubusercontent.com/ondrejhonus/cpp_proc/main/quick-install.sh | bash
```

#### Install with the provided install script:
To install the compiled binary into your system's local bin directory, use:

```bash
./install.sh
```

### Build from source manually
```bash
git clone https://github.com/ondrejhonus/cpp_proc.git
cd cpp_proc

cmake -B build

make -C build -j$(nproc)

sudo make -C build install
```

> You can then run the program with typing `cpm` into the terminal.


## Compilation and Execution (For development)

To compile and run the program, use the provided `compile-run.sh` script:

```bash
./compile-run.sh
```

## Contributing
- Fork the repository and create a new branch for your feature or bug fix.
- Create a pull request with a clear description of your changes and why they are needed.
