//===- stoc/AST/Expr.cpp - Implementation of nodes related to Expressions in AST ----*- C++ -*-===//
//
//===------------------------------------------------------------------------------------------===//
//
// This file implements the classes of nodes related to Expressions in AST.
//
//===------------------------------------------------------------------------------------------===//

#include "stoc/AST/Expr.h"

// Binary Expression node
ExprBinary::ExprBinary(std::shared_ptr<Expr> lhs, std::shared_ptr<Expr> rhs, Token &op)
    : lhs(std::move(lhs)), rhs(std::move(rhs)), op(op) {}

void ExprBinary::accept(ASTVisitor *visitor) { visitor->visit(*this); }

const std::shared_ptr<Expr> &ExprBinary::getLhs() const { return lhs; }
const std::shared_ptr<Expr> &ExprBinary::getRhs() const { return rhs; }
const Token &ExprBinary::getOp() const { return op; };

// Unary Expression node
ExprUnary::ExprUnary(std::shared_ptr<Expr> rhs, Token &op) : rhs(std::move(rhs)), op(op) {}

void ExprUnary::accept(ASTVisitor *visitor) { visitor->visit(*this); }

const std::shared_ptr<Expr> &ExprUnary::getRhs() const { return rhs; }
const Token &ExprUnary::getOp() const { return op; };

// Literal Expression node
ExprLiteral::ExprLiteral(Token token) : token(token) {}

void ExprLiteral::accept(ASTVisitor *visitor) { visitor->visit(*this); }

const Token &ExprLiteral::getToken() const { return token; }
