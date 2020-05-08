# Now build stoc compiler: target
add_executable(stoc main.cpp
        SrcFile/SrcFile.cpp
        Scanner/Scanner.cpp
        Scanner/Token.cpp
        Parser/Parser.cpp
        AST/ASTPrinter.cpp
        AST/Expr.cpp)


