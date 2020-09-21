# Stoc Programming Language
Stoc is a statically strongly typed language with C-like syntax. It is imperative and compiled. Stoc is a simple programming language whose aim is to be able to write basic programs and print the reuslts to the screen.

The compiler for the Stoc language transforms Stoc source code into LLVM IR, and uses the [LLVM](https://llvm.org/) infrastructure to transform the LLVM IR into native machine code.

![Structure of the compiler implemented for Stoc](assets/structure_stoc.png)

For a more complete description of the project than the one found in this README.md, please see the [Report](Report_StocCompiler.pdf) of the project.

# Table of contents
1. [Syntax Guide](#syntax-guide)
2. [Project Structure](#project-structure)
3. [Installation](#system-requirements)
4. [Notes](#notes)

## Syntax Guide
### Variables and Constants
Stoc only contains simple types: bool, int, float and string.
A variable (or constant) is declared with the keyword var (or const) followed by the type and the identifier, and it always has to be initialized. It ends with a semicolon:
```c++
// Declaring a variable
var bool a = true;
var int integer = 5;
const float b6 = 6.0;
const string charaters = "string";
```
Use double forward-slashes `//` to define single-line comments.

### Basic Operators
There are basic operators for the different types:
- Assignment operator (=), Arithmetic operators (+ - * / ), Comparison operators (== !=), Order operators (< > <= >=), and Logical operators (! && ||).

```c++
const bool a = true && true;
var int integer = 5 + 5 * (7 - 4) / 3;
var float f = 6.0 * 7.0 / 3.5;
const string characters = "string";
const string characters2 = "strings2";
var bool isEqual = f == g;
```

### Control Flow
#### If-Else
```c++
if testexpression {
    // body
} else if testexpression2 {
    // body
} else {
    // body
}
```
Example:
```c++
var int temp = 12;
if temp > 30 {
    println ( " It ’ s hot " ) ;
    if temp > 40 {
        print ( " It ’ s very very hot " ) ;
    }
} else if temp >= 15 {
    println ( " It ’ s warm " ) ;
} else {
    println ( " It ’ s cold " ) ;
}
```
#### For loop
Structure:
```c++
for init; condition; post {
    // body
}
```
Example:
```c++
for var int i = 0; i < 3; i = i + 1 {
    print("Number is: ");
    println(i);
}
```
#### While loop
Structure:
```c++
while testexpression {
    // body
}
```
Example:
```c++
// Example of while loop : calculate c = a % b
var int a = 10;
var int b = 3;
var int c = 10;
while c >= b {
    c = c - b;
}
print (" 10 % 3 is ");
print (c);
```
### Functions
toc has functions and are defined by the keyword func followed by the identi-
fier, the parameter list and, optionally, the return type.

To return values from a function, Stoc uses the keyword return. All programs require a main function where the program will start executing. Stoc has two builtin functions to output text: `print()` and `println()` that adds a new line.

```c++
// Program for printing the factorial of a number
func factorial (var int n) int {
    var int res = 1;
    while n > 0 {
        res = res * n;
        n = n - 1;
    }
    return res;
}

func main () {
    const int c = 10;
    print(" Factorial of number ");
    print(c);
    print(" is ");
    println (factorial(c));
}
```

Stoc also supports function overloading depending on the number and type of pa-
rameters of the function.

## Project Structure
```
Stoc
 |-- assets/                     <- images used in the README.md
 |-- examples/                   <- examples of Stoc source code
 |-- include/                    <- public header files
 |    `-- stoc/
 |         |-- AST/
 |         |-- CodeGeneration/
 |         |-- Parser/
 |         |-- Scanner/
 |         |-- SemanticAnalysis/
 |         `-- SrcFile/
 |
 |-- libs/                       <- header-only external libraries 
 |-- src/                        <- implementation files
 |   |-- AST/
 |   |-- CodeGeneration/
 |   |-- Parser/
 |   |-- Scanner/
 |   |-- SemanticAnalysis/
 |   `-- SrcFile/
 |
 |-- utils/                      <- files for running and developping in Docker 
 `-- CMakeLists.txt              <- main CMake file
```
## Installation
### Building on Unix
#### System Requirements
The Stoc compiler uses the LLVM toolchain to compile Stoc programs. For this reason, to build the Stoc compiler you will need the 
following dependencies:

- `build-essential`
- `python`
- `zlib`
- `cmake`
- `llvm`
- `clang`

#### Setting up

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
#### Using Docker for running the compiler
Dockerfile.stoc-build is a Dockerfile that contains the necessary dependencies to build the Stoc compiler.
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
You can try any of the [examples](./examples) or create your own program in Stoc!

#### Using Docker for developping the compiler
For the development of `stoc`, it has been used the [Clion IDE](https://www.jetbrains.com/clion/) using docker containers
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

## Notes
This project is part of the course [DD2466 Second Advanced, Individual Course in Computer Science](https://www.kth.se/student/kurser/kurs/DD2466?l=en)
during my studies at [KTH](https://www.kth.se/en). You can read the report for the [project](./Report_StocCompiler.pdf).
