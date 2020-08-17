# Stoc Programming Language

Stoc is a simple programming language with C-like syntax.
Stoc is statically typed, imperative and compiled.

# Table of contents
1. [Installation](#system-requirements)
2. [Project Structure](#project-structure)
3. [Syntax Guide](#syntax-guide)

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
./src/stoc <file.st>
```
You can try any of the [examples](./examples) or create your own program in Stoc!

### Building with Docker
* Dockerfile.stoc-build is a Dockerfile that contains the necessary dependencies to build the Stoc compiler.
    
    - Go to the root path of the repository
    - Build the image `stoc-build` 
    ```sh
    docker build -t stoc-build -f utils/docker/Dockerfile.stoc-build .
    ```
    - Run the container
    ```sh
    bash utils/docker/stoc-build-container.sh
    ```
    or 
    ```sh
    docker run -it --mount src="$(pwd)",target=/stoc,type=bind stoc-build
    ```
    As you can see, when running the docker container, you make use of the bind mount functionality to mount the directory
    of the host machine into the container. Please, see [Use bind mounts](https://docs.docker.com/storage/bind-mounts/)
    to learn more.
    - Already inside the container, change directory to: `/stoc`. Then, build the compiler.
    ```sh
    mkdir build
    cd build
    cmake ..
    make
    ```
    - Run the compiler
    ```sh
    ./src/stoc <file.st>
    ```
  
- For the development of `stoc`, it has been used the [Clion IDE](https://www.jetbrains.com/clion/) using docker containers
for building the project through ssh remote development. To do this in Clion or any other IDE or code editor that supports it:
    
    - Go to the root path of the repository
    - Build the image `stoc-remote-dev`
    ```sh
    docker build -t stoc-remote-dev -f utils/docker/Dockerfile.stoc-remote-dev .
    ```
    - Run the container
    ```sh
    bash utils/docker/stoc-remote-dev-conainer.sh
    ```
    or
    ```sh
    docker run -d --cap-add sys_ptrace -p127.0.0.1:2222:22 --name stoc-remote-dev stoc-remote-dev
    ```
    This opens in localhost the port 2222 to be able to ssh into the container with user dev and password dev.
    - Configure [Clion IDE to develop in remote mode with docker containers](https://blog.jetbrains.com/clion/2020/01/using-docker-with-clion/)
    or any other IDE or code editor that supports it.

## Project Structure

## Syntax Guide

## Notes
This project is part of the course [DD2466 Second Advanced, Individual Course in Computer Science](https://www.kth.se/student/kurser/kurs/DD2466?l=en)
during my studies at [KTH](https://www.kth.se/en). You can read the report for the [project](./Report_StocCompiler.pdf).
