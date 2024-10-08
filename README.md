## Description

This project is a custom memory allocator implemented in C. It handles dynamic memory requests by using a linked list to track memory blocks, allowing for memory allocation, deallocation, and reallocation. The allocator mirrors C’s `malloc`, `free`, and `realloc` functions, but operates through a single large memory pool that it manages manually. This project served as an introduction to manual memory management and pointer manipulation.

## How to Run

### Source Code

To access the source code for this memory allocator, you can clone the repository from GitHub:

```bash
git clone https://github.com/Papaglory/Memory-Allocator-in-C
cd Memory-Allocator-in-C
```
### Running a Test File Using 'manfile'

1. With the C compiler 'gcc' and 'make' installed, run the following command.

    ```bash
    ./manfile
    ```
    
2. Run the executable test file:

    ```bash
    ./my_test
    ```
    
To compile with a different test file, edit line 8 in the 'manfile' to specify the desired test file. Note that the 'manfile' is built upon the 'makefile' due to issues compiling the 'makefile' directly with test files.

## Further Information

For a more in-depth look at this project, please refer to [mariusnaasen.com/projects/memory-allocator-in-c](https://mariusnaasen.com/projects/memory-allocator-in-c).

## Author
Marius H. Naasen, originally created September 2024.
