cmake_minimum_required(VERSION 3.13)

# Now build stoc compiler: target
add_executable(stoc main.cpp
        SrcFile/SrcFile.cpp
        SrcFile/SrcFile.h
        Scanner/Scanner.cpp
        Scanner/Scanner.h
        Scanner/Token.h
        Scanner/Token.cpp)


