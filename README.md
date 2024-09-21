# Thread-Pool-Project
# Thread Pool Example

This repository contains an implementation of a thread pool in C, including:

1. **Client Program** (`client.c`): A user-space program that demonstrates the usage of the thread pool by submitting tasks.
2. **Thread Pool Implementation** (`threadpool.c`): Contains the logic for managing a pool of threads and handling task execution.

## Files in the Repository

- `client.c` - A user-space C program that initializes the thread pool and submits tasks.
- `threadpool.c` - The implementation of the thread pool functionality.
- `threadpool.h` - Header file for the thread pool.
- `Makefile` - A Makefile to compile the project.
- `README.md` - Documentation for this project.

---

## Compilation, Usage, and Cleanup

The following code snippet will **compile**, **run**, and **clean up** the project:

```bash
# 1. Compile the code
make

# 2. Run the client program to see the thread pool in action
./example

# 3. Clean up the compiled files
make clean
