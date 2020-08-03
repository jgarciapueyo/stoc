# Now build stoc compiler: target
add_executable(stoc main.cpp
        SrcFile/SrcFile.cpp
        Scanner/Scanner.cpp
        Scanner/Token.cpp
        Parser/Parser.cpp
        AST/ASTPrinter.cpp
        AST/Expr.cpp
        AST/Decl.cpp
        AST/Stmt.cpp
        CodeGeneration/CGDecl.cpp
        CodeGeneration/CGExpr.cpp
        CodeGeneration/CGStmt.cpp
        CodeGeneration/CodeGeneration.cpp
        SemanticAnalysis/Semantic.cpp
        SemanticAnalysis/Symbol.cpp
        SemanticAnalysis/SymbolTable.cpp
        SemanticAnalysis/Type.cpp)


