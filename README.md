# Stoc Programming Language

Stoc is a simple programming language with C-like syntax.
Stoc is statically typed, imperative and compiled.

TODO: add file with guide of syntax

## Installation

### System Requirements
The Stoc compiler uses the LLVM toolchain to compile Stoc programs. For this reason, to build the Stoc compiler you will need the 
following dependencies:

- `build-essential`
- `python`
- `zlib`
- `cmake`
- `llvm`
- `clang`

### Building on Unix

1. Clone the [source code](https://github.com/jgarciapueyo/stoc) with `git`:
```sh
git clone https://github.com/jgarciapueyo/stoc.git
cd stoc
```
2. Build the compiler
```sh
mkdir build
cd build
cmake ..
make
```
3. Run the compiler
```sh
./stoc <file.st>
```

### Building with Docker
If you prefer to build the Stoc compiler on Docker, please see [Docker Instructions](/utils/docker). 

## Notes
This project is part of the course [DD2466 Second Advanced, Individual Course in Computer Science](https://www.kth.se/student/kurser/kurs/DD2466?l=en)
during my studies at [KTH](https://www.kth.se/en).
