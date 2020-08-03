// TODO: add header of the file
#ifndef STOC_SEMANTICANALYSIS_H
#define STOC_SEMANTICANALYSIS_H

#include "stoc/AST/ASTVisitor.h"
#include "stoc/SemanticAnalysis/SymbolTable.h"
#include "stoc/SrcFile/SrcFile.h"

#include <memory>

/// Phase of the compiler after parsing. It takes the AST and gathers information about the meaning
/// of the program. The information gathered includes type checking, ensuring that all the variables
/// are declared before being used, ...
class Semantic : public ASTVisitor {
  // It has been implemented as a subclass of the ASTVisitor, so the semantic analysis is done in
  // in every node

  // TODO: add variable aliasing that facilitates codegeneration of variables with same identifier
  // but different scopes since llvm ir is plain and does not have scopes
public:
  /// Use to perform semantic analysis on some nodes depending on the context. For example, check
  /// check that a return statement only appears inside a function
  enum class ScopeType { NONE, FUNCTION };

private:
  std::shared_ptr<SrcFile> file; /// stoc source file, list of tokens and AST

  // State of the parser
  std::shared_ptr<SymbolTable> symbolTable;

  /// Depth level of the current scope
  int scopeLevel;

  // Use in function declaration
  ScopeType scopeType;

  // Use in return statement inside function declaration
  std::shared_ptr<FunctionType> signature; // needed for type checking of return statement

  // Use in call expression or other nodes that need information about most recent resolved symbol
  std::vector<Symbol> resolvedSymbols;

  // WRAPPER METHODS for ASTVisitor methods
  void analyse(const std::shared_ptr<Decl> &decl);
  void analyse(const std::shared_ptr<Expr> &expr);
  void analyse(const std::shared_ptr<Stmt> &stmt);
  void analyse(const std::vector<std::shared_ptr<Stmt>> &stmts);

  // HELPER METHODS
  /// It creates a new scope by creating a new symbol table
  void beginScope();

  /// It ends current scope and returns \symbolTable to previous state
  void endScope();

  static std::pair<bool, std::shared_ptr<Type>>
  isValidBinaryOperatorForType(const Token &op, std::shared_ptr<Type> typeOperands);

  static std::pair<bool, std::shared_ptr<Type>>
  isValidUnaryOperatorForType(const Token &op, std::shared_ptr<Type> typeOperands);

  std::shared_ptr<Type> tokenTypeToType(Token token);

  std::shared_ptr<FunctionType> createSignature(std::shared_ptr<FuncDecl> node);

  void reportError(std::string error_msg, int line, int column);

public:
  explicit Semantic(std::shared_ptr<SrcFile> file);

  /// main method: analyses the AST (in \file)
  void analyse();

  // Methods for ASTVisitor
  void visit(std::shared_ptr<VarDecl> node) override;
  void visit(std::shared_ptr<ConstDecl> node) override;
  void visit(std::shared_ptr<ParamDecl> node) override;
  void visit(std::shared_ptr<FuncDecl> node) override;

  void visit(std::shared_ptr<DeclarationStmt> node) override;
  void visit(std::shared_ptr<ExpressionStmt> node) override;
  void visit(std::shared_ptr<BlockStmt> node) override;
  void visit(std::shared_ptr<IfStmt> node) override;
  void visit(std::shared_ptr<ForStmt> node) override;
  void visit(std::shared_ptr<WhileStmt> node) override;
  void visit(std::shared_ptr<AssignmentStmt> node) override;
  void visit(std::shared_ptr<ReturnStmt> node) override;

  void visit(std::shared_ptr<BinaryExpr> node) override;
  void visit(std::shared_ptr<UnaryExpr> node) override;
  void visit(std::shared_ptr<LiteralExpr> node) override;
  void visit(std::shared_ptr<IdentExpr> node) override;
  void visit(std::shared_ptr<CallExpr> node) override;
};

#endif // STOC_SEMANTICANALYSIS_H
