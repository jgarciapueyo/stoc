//===- stoc/AST/Expr.h - Defintion of nodes related to Expressions in AST -----------*- C++ -*-===//
//
//===------------------------------------------------------------------------------------------===//
//
// This file defines the classes of nodes related to Expressions in AST.
// The expression nodes inherit from Expr, like ExprBinary, ExprUnary, ....
// An expression is a node in the AST that produces a value.
//
// To add an Expr node in the AST, define it here and make it inherit from Expr. Also, add the
// expression in stoc/AST/ASTVisitor.h
//===------------------------------------------------------------------------------------------===//

#ifndef STOC_EXPR_H
#define STOC_EXPR_H

#include <memory>
#include <string>
#include <vector>

#include "stoc/AST/BasicNode.h"
#include "stoc/Scanner/Token.h"

/// An expression is a node in the AST that produces a value
class Expr : public BasicNode, public std::enable_shared_from_this<Expr> {
  // It needs to inherit from std::enable_shared_from_this<Expr> to be able to create a shared_ptr
  // from inside the class (return this) without creating a new shared_ptr (which would copy
  // the content of the class) and, when updating the instance of class, the changes are applied
  // everywhere. [see src/AST/Expr.cpp:BinaryExpr::accept() for example] Useful when traversing
  // the AST and modifying it.

public:
  /// Type of the expression of the node in the AST
  enum class Kind { BINARYEXPR, UNARYEXPR, LITERALEXPR, IDENTEXPR, CALLEXPR };

protected:
  Kind exprKind;

public:
  explicit Expr(Expr::Kind exprKind);
  [[nodiscard]] Expr::Kind getExprKind() const;

  virtual const std::string &getType() const = 0;
  virtual void setType(const std::string &type) = 0;
};

/// A binary expression is a node in the AST that contains two other nodes "joined" by an operator
/// (e.g. 3+4 -> node(3), node(4), operator(+))
class BinaryExpr : public Expr {
private:
  /// left-hand and right-hand side nodes
  std::shared_ptr<Expr> lhs, rhs;

  /// binary operator
  Token op;

  /// Expression's type for type checking
  std::string type;

public:
  BinaryExpr(std::shared_ptr<Expr> lhs, std::shared_ptr<Expr> rhs, Token &op);

  /// method needed for the Visitor Pattern
  void accept(ASTVisitor *visitor) override;

  // Getters
  [[nodiscard]] const std::shared_ptr<Expr> &getLhs() const;
  [[nodiscard]] const std::shared_ptr<Expr> &getRhs() const;
  [[nodiscard]] const Token &getOp() const;

  // Getters and setters
  const std::string &getType() const override;
  void setType(const std::string &type) override;
};

/// An unary expression is a node in the AST that contains another node and an operator
/// (e.g. !true -> node(true), operator(!))
class UnaryExpr : public Expr {
private:
  /// right-hand side node
  std::shared_ptr<Expr> rhs;

  /// unary operator
  Token op;

  /// Expression's type for type checking
  std::string type;

public:
  UnaryExpr(std::shared_ptr<Expr> rhs, Token &op);

  /// method needed for the Visitor Pattern
  void accept(ASTVisitor *visitor) override;

  // Getters
  [[nodiscard]] const std::shared_ptr<Expr> &getRhs() const;
  [[nodiscard]] const Token &getOp() const;

  // Getters and setters
  const std::string &getType() const override;
  void setType(const std::string &type) override;
};

/// A literal expression is a node in the AST that represent integers, floats, strings,
/// booleans or nil. (e.g. literal(5), literal(true), literal("string"))
class LiteralExpr : public Expr {
private:
  /// token containing the literal value
  Token token;

  /// Expression's type for type checking
  std::string type;

public:
  explicit LiteralExpr(Token lit);

  /// method needed for the Visitor Pattern
  void accept(ASTVisitor *visitor) override;

  // Getters
  [[nodiscard]] const Token &getToken() const;

  // Getters and setters
  const std::string &getType() const override;
  void setType(const std::string &type) override;
};

/// An identifier expression is a node in the AST that represents an identifier
/// (variable, constant, function)
class IdentExpr : public Expr {
private:
  Token ident;

  /// Expression's type for type checking
  std::string type;

public:
  explicit IdentExpr(Token ident);

  /// method needed for the Visitor Pattern
  void accept(ASTVisitor *visitor) override;

  // Getters
  [[nodiscard]] const Token &getIdent() const;
  [[nodiscard]] const std::string &getName() const;

  // Getters and setters
  const std::string &getType() const override;
  void setType(const std::string &type) override;
};

/// A call expression is a node in the AST that represents calling a function
class CallExpr : public Expr {
private:
  std::shared_ptr<Expr> func;
  std::vector<std::shared_ptr<Expr>> args;

  /// Expression's type for type checking
  std::string type;

public:
  CallExpr(std::shared_ptr<Expr> func, std::vector<std::shared_ptr<Expr>> args);

  /// method needed for the Visitor Pattern
  void accept(ASTVisitor *visitor) override;

  // Getters
  [[nodiscard]] const std::shared_ptr<Expr> &getFunc() const;
  [[nodiscard]] const std::vector<std::shared_ptr<Expr>> &getArgs() const;

  // Getters and setters
  const std::string &getType() const override;
  void setType(const std::string &type) override;
};

#endif // STOC_EXPR_H
