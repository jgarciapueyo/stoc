//===- stoc/AST/Stmt.h - Defintion of nodes related to Statements in AST ------------*- C++ -*-===//
//
//===------------------------------------------------------------------------------------------===//
//
// This file defines the classes of nodes related to Statements in AST.
// The statement nodes inherit from Stmt, like BlockStmt, IfStmt, ....
// A statement is a node in the AST that works as a syntactic unit of some action to be carried out.
//
// To add an Stmt node in the AST, define it here and make it inherit from Stmt. Also, add the
// statement in stoc/AST/ASTVisitor.h
//===------------------------------------------------------------------------------------------===//

#ifndef STOC_STMT_H
#define STOC_STMT_H

#include <memory>
#include <vector>

#include "stoc/AST/BasicNode.h"
#include "stoc/Scanner/Token.h"

/// An statement is a node in the AST that expresses some action (controlling flow, ...)
class Stmt : public BasicNode, public std::enable_shared_from_this<Stmt> {
  // It needs to inherit from std::enable_shared_from_this<Stmt> to be able to create a shared_ptr
  // from inside the class (return this) without creating a new shared_ptr (which would copy
  // the content of the class) and, when updating the instance of class, the changes are applied
  // everywhere. [see src/AST/Expr.cpp:AssignmentStmt::accept() for example]. Useful when traversing
  // the AST and modifying it.

public:
  enum class Kind {
    DECLARATIONSTMT,
    EXPRESSIONSTMT,
    BLOCKSTMT,
    IFSTMT,
    FORSTMT,
    WHILESTMT,
    ASSIGNMENTSTMT,
    RETURNSTMT
  };

protected:
  Kind stmtKind;

public:
  explicit Stmt(Stmt::Kind stmtKind);
  Stmt::Kind getStmtKind() const;
};

/// An expression statement is a node in the AST that represents an expression in a block statement
class ExpressionStmt : public Stmt {
private:
  std::shared_ptr<Expr> expr;

public:
  explicit ExpressionStmt(std::shared_ptr<Expr> expr);

  /// method needed for the Visitor Pattern
  void accept(ASTVisitor *visitor) override;

  // Getters
  [[nodiscard]] const std::shared_ptr<Expr> &getExpr() const;
};

/// A declaration statement is a node in the AST that represents a declaration in a block statement
class DeclarationStmt : public Stmt {
private:
  std::shared_ptr<Decl> decl;

public:
  explicit DeclarationStmt(std::shared_ptr<Decl> decl);

  /// method needed for the Visitor Pattern
  void accept(ASTVisitor *visitor) override;

  // Getters
  [[nodiscard]] const std::shared_ptr<Decl> &getDecl() const;
};

/// An assignment statement is a node in the AST that represents an assignment of the value of the
///  \rhs right hand side node to the \lhs left hand side node
class AssignmentStmt : public Stmt {
private:
  // TODO: check if expr lhs can really appear in the LeftHandSide and if rhs can really appear in
  // the RightHandSide
  std::shared_ptr<Expr> lhs;
  std::shared_ptr<Expr> rhs;

  /// keyword = for an assignment
  Token equalToken;

public:
  AssignmentStmt(std::shared_ptr<Expr> lhs, std::shared_ptr<Expr> rhs, Token equalToken);

  /// method needed for the Visitor Pattern
  void accept(ASTVisitor *visitor) override;

  // Getters
  [[nodiscard]] const Token &getEqualToken() const;
  [[nodiscard]] const std::shared_ptr<Expr> &getLhs() const;
  [[nodiscard]] const std::shared_ptr<Expr> &getRhs() const;
};

/// A block assignment is a node in the AST that represents a list of statements that goes between
/// braces '{' <list of satements> '}'
class BlockStmt : public Stmt {
private:
  Token lbrace;
  std::vector<std::shared_ptr<Stmt>> stmts;
  Token rbrace;

public:
  BlockStmt(Token lbrace, std::vector<std::shared_ptr<Stmt>> stmts, Token rbrace);

  /// method needed for the Visitor Pattern
  void accept(ASTVisitor *visitor) override;

  // Getters
  [[nodiscard]] const Token &getLbrace() const;
  [[nodiscard]] const Token &getRbrace() const;
  [[nodiscard]] const std::vector<std::shared_ptr<Stmt>> &getStmts() const;
};

/// An if statement is a node in the AST that represents the if conditional control flow structure
class IfStmt : public Stmt {
private:
  /// keyword IF (needed for printing AST)
  Token ifKeyword;
  std::shared_ptr<Expr> condition;
  std::shared_ptr<BlockStmt> thenBranch;
  std::shared_ptr<Stmt> elseBranch;
  bool hasElse;

public:
  IfStmt(Token ifKeyword, std::shared_ptr<Expr> condition, std::shared_ptr<BlockStmt> thenBranch,
         std::shared_ptr<Stmt> elseBranch);

  IfStmt(Token ifKeyword, std::shared_ptr<Expr> condition, std::shared_ptr<BlockStmt> thenBranch);

  /// method needed for the Visitor Pattern
  void accept(ASTVisitor *visitor) override;

  // Getters
  [[nodiscard]] const Token &getIfKeyword() const;
  [[nodiscard]] const std::shared_ptr<Expr> &getCondition() const;
  [[nodiscard]] const std::shared_ptr<BlockStmt> &getThenBranch() const;
  [[nodiscard]] const std::shared_ptr<Stmt> &getElseBranch() const;
  [[nodiscard]] bool isHasElse() const;
};

/// A for statement is a node in the AST that represents the for loop control flow structure
class ForStmt : public Stmt {
private:
  /// keyword FOR (needed for printing AST)
  Token forKeyword;
  std::shared_ptr<Stmt> init;
  std::shared_ptr<Expr> cond;
  std::shared_ptr<Stmt> post;
  std::shared_ptr<BlockStmt> body;

public:
  ForStmt(Token forKeyword, std::shared_ptr<Stmt> init, std::shared_ptr<Expr> cond,
          std::shared_ptr<Stmt> post, std::shared_ptr<BlockStmt> body);

  /// method needed for the Visitor Pattern
  void accept(ASTVisitor *visitor) override;

  // Getters
  [[nodiscard]] const Token &getForKeyword() const;
  [[nodiscard]] const std::shared_ptr<Stmt> &getInit() const;
  [[nodiscard]] const std::shared_ptr<Expr> &getCond() const;
  [[nodiscard]] const std::shared_ptr<Stmt> &getPost() const;
  [[nodiscard]] const std::shared_ptr<BlockStmt> &getBody() const;
};

/// A while statement is a node in the AST that represents the while loop control flow structure
class WhileStmt : public Stmt {
private:
  /// keyword WHILE (needed for printing AST)
  Token whileKeyword;
  std::shared_ptr<Expr> cond;
  std::shared_ptr<BlockStmt> body;

public:
  WhileStmt(Token whileKeyword, std::shared_ptr<Expr> cond, std::shared_ptr<BlockStmt> body);

  /// method needed for the Visitor Pattern
  void accept(ASTVisitor *visitor) override;

  // Getters
  [[nodiscard]] const Token &getWhileKeyword() const;
  [[nodiscard]] const std::shared_ptr<Expr> &getCond() const;
  [[nodiscard]] const std::shared_ptr<BlockStmt> &getBody() const;
};

/// A return statement is a node in the AST that represents the return value of a function
class ReturnStmt : public Stmt {
private:
  /// keyword RETURN (needed for printing the AST)
  Token returnKeyword;
  std::shared_ptr<Expr> value;

public:
  ReturnStmt(Token returnKeyword, std::shared_ptr<Expr> value);

  /// method needed for the Visitor Pattern
  void accept(ASTVisitor *visitor) override;

  // Getters
  [[nodiscard]] const Token &getReturnKeyword() const;
  [[nodiscard]] const std::shared_ptr<Expr> &getValue() const;
};

// TODO: add EmptyStmt

#endif // STOC_STMT_H
