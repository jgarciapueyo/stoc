// TODO: add header of the file
#ifndef STOC_SEMANTICANALYSIS_H
#define STOC_SEMANTICANALYSIS_H

#include "SymbolTable.h"
#include "stoc/AST/ASTVisitor.h"
#include "stoc/SrcFile/SrcFile.h"

#include <memory>

/// Phase of the compiler after parsing. It takes the AST and gathers information about the meaning
/// of the program. The information gathered includes type checking, ensuring that all the variables
/// are declared before being used, ...
class Semantic : public ASTVisitor {
  // It has been implemented as a subclass of the ASTVisitor, so the semantic analysis is done in
  // in every node
public:
  /// Use to perform semantic analysis on some nodes depending on the context. For example, check
  /// check that a return statement only appears inside a function
  enum FunctionType { NONE, FUNCTION };

private:
  std::shared_ptr<SrcFile> file; /// stoc source file, list of tokens and AST

  // State of the parser
  std::shared_ptr<SymbolTable> symbolTable;

  /// Depth level of the current scope
  int scopeLevel;

  FunctionType functionType;

  // WRAPPER METHODS for ASTVisitor methods
  void analyse(std::shared_ptr<Decl> decl);
  void analyse(std::shared_ptr<Expr> expr);
  void analyse(std::shared_ptr<Stmt> stmt);
  void analyse(std::vector<std::shared_ptr<Stmt>> stmts);

  // HELPER METHODS
  /// It creates a new scope by creating a new symbol table
  void beginScope();

  /// It ends current scope and returns \symbolTable to previous state
  void endScope();

public:
  explicit Semantic(std::shared_ptr<SrcFile> file);

  /// main method: analyses the AST (in \file)
  void analyse();

  // Methods for ASTVisitor
  void visit(VarDecl node) override;
  void visit(ConstDecl node) override;
  void visit(ParamDecl node) override;
  void visit(FuncDecl node) override;

  void visit(DeclarationStmt node) override;
  void visit(ExpressionStmt node) override;
  void visit(BlockStmt node) override;
  void visit(IfStmt node) override;
  void visit(ForStmt node) override;
  void visit(WhileStmt node) override;
  void visit(AssignmentStmt node) override;
  void visit(ReturnStmt node) override;

  void visit(BinaryExpr node) override;
  void visit(UnaryExpr node) override;
  void visit(LiteralExpr node) override;
  void visit(IdentExpr node) override;
  void visit(CallExpr node) override;
};

#endif // STOC_SEMANTICANALYSIS_H
