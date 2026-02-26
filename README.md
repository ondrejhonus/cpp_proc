# C++ Process Manager (CPM)
For now, this is just a program, that lists all the running processes on a Linux system.
Later, it will be more similar to a task manager like htop.

## Functions
- See all running processes
- See the processes' PID, name, state, memory usage
- See the process states (Sleeping, Running, Zombie)
- Send a SIGnal to kill or controll a specific process

## TO-DO\
- CPU usage
- Order processes by other factors than PID
- Search by several factors
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