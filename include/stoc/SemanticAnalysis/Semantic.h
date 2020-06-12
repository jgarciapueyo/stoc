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

  // Use in function declaration
  FunctionType functionType;

  // Use in return statement inside function declaration
  std::string returnType; // needed for type checking of return statement

  // Use in call expression or other nodes that need information about most recent resolved symbol
  Symbol resolvedSymbol;

  // WRAPPER METHODS for ASTVisitor methods
  void analyse(const std::shared_ptr<Decl>& decl);
  void analyse(const std::shared_ptr<Expr>& expr);
  void analyse(const std::shared_ptr<Stmt>& stmt);
  void analyse(const std::vector<std::shared_ptr<Stmt>>& stmts);

  // HELPER METHODS
  /// It creates a new scope by creating a new symbol table
  void beginScope();

  /// It ends current scope and returns \symbolTable to previous state
  void endScope();

  std::pair<bool, std::string> isValidOperatorForType(const Token& op, std::string typeOperands);
  std::pair<bool, std::string> isValidOperatorForNumericType(const Token& op, std::string typeOperands);
  std::pair<bool, std::string> isValidOperatorForStringType(const Token& op, std::string typeOperands);
  std::pair<bool, std::string> isValidOperatorForBoolType(const Token& op, std::string typeOperands);

  static std::string tokenTypeToType(TokenType tokenType);
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
