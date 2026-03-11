# C++ Process Manager (CPM)

An `htop`-like process manager made in `C++` using the `ftxui` library for TUI functionality.

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

## Compilation and Execution

To compile and run the program, use the provided `compile-run.sh` script:

```bash
./compile-run.sh
```

## Installation into System

To install the compiled binary into your system's local bin directory, use:

```bash
./install.sh
```

> You can then run the program with typing `cpm` into the terminal.
