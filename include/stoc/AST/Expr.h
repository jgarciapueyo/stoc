//===- stoc/AST/Expr.h - Defintion of nodes related to Expressions in AST --------*- C++ -*-===//
//
//===---------------------------------------------------------------------------------------===//
//
// This file defines the classes of nodes related to Expressions in AST.
// The expression nodes inherit from Expr, like ExprBinary, ExprUnary, ....
// An expression is a node in the AST that produces a value
//
//===---------------------------------------------------------------------------------------===//

#ifndef STOC_EXPR_H
#define STOC_EXPR_H

#include "BasicNode.h"
#include "stoc/Scanner/Token.h"

#include <memory>
#include <string>

/// An expression is a node in the AST that produces a value
class Expr : public BasicNode {};

/// A binary expression is a node in the AST that contains two other nodes "joined" by an operator
/// (e.g. 3+4 -> node(3), node(4), operator(+))
class ExprBinary : public Expr {
private:
  /// left-hand and right-hand side nodes
  std::shared_ptr<Expr> lhs, rhs;

  /// binary operator
  Token op;

public:
  ExprBinary(std::shared_ptr<Expr> lhs, std::shared_ptr<Expr> rhs, Token &op);

  /// method needed for the Visitor Pattern
  void accept(ASTVisitor *visitor) override;

  // Getters
  const std::shared_ptr<Expr> &getLhs() const;
  const std::shared_ptr<Expr> &getRhs() const;
  const Token &getOp() const;
};

/// An unary expression is a node in the AST that contains another node and an operator
/// (e.g. !true -> node(true), operator(!))
class ExprUnary : public Expr {
private:
  /// right-hand side node
  std::shared_ptr<Expr> rhs;

  /// unary operator
  Token op;

public:
  ExprUnary(std::shared_ptr<Expr> rhs, Token &op);

  /// method needed for the Visitor Pattern
  void accept(ASTVisitor *visitor) override;

  // Getters
  const std::shared_ptr<Expr> &getRhs() const;
  const Token &getOp() const;
};

/// A literal expression is a node in the AST that represent integers, floats, strings,
/// booleans or nil. (e.g. literal(5), literal(true), literal("string"))
class ExprLiteral : public Expr {
private:
  /// token containing the literal value
  Token token;

public:
  ExprLiteral(Token lit);

  /// method needed for the Visitor Pattern
  void accept(ASTVisitor *visitor) override;

  // Getters
  const Token &getToken() const;
};

#endif // STOC_EXPR_H
