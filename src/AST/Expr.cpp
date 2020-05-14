//===- stoc/AST/Expr.cpp - Implementation of nodes related to Expressions in AST ----*- C++ -*-===//
//
//===------------------------------------------------------------------------------------------===//
//
// This file implements the classes of nodes related to Expressions in AST.
//
//===------------------------------------------------------------------------------------------===//

#include "stoc/AST/Expr.h"

// Binary Expression node
BinaryExpr::BinaryExpr(std::shared_ptr<Expr> lhs, std::shared_ptr<Expr> rhs, Token &op)
    : lhs(std::move(lhs)), rhs(std::move(rhs)), op(op) {}

void BinaryExpr::accept(ASTVisitor *visitor) { visitor->visit(*this); }

const std::shared_ptr<Expr> &BinaryExpr::getLhs() const { return lhs; }
const std::shared_ptr<Expr> &BinaryExpr::getRhs() const { return rhs; }
const Token &BinaryExpr::getOp() const { return op; };

// Unary Expression node
UnaryExpr::UnaryExpr(std::shared_ptr<Expr> rhs, Token &op) : rhs(std::move(rhs)), op(op) {}

void UnaryExpr::accept(ASTVisitor *visitor) { visitor->visit(*this); }

const std::shared_ptr<Expr> &UnaryExpr::getRhs() const { return rhs; }
const Token &UnaryExpr::getOp() const { return op; };

// Literal Expression node
LiteralExpr::LiteralExpr(Token token) : token(token) {}

void LiteralExpr::accept(ASTVisitor *visitor) { visitor->visit(*this); }

const Token &LiteralExpr::getToken() const { return token; }

// Identifier Expression node
IdentExpr::IdentExpr(Token ident, std::string name) : ident(ident), name(name) {}

void IdentExpr::accept(ASTVisitor *visitor) { visitor->visit(*this); }
const Token &IdentExpr::getIdent() const { return ident; }
const std::string &IdentExpr::getName() const { return name; }

// Call Expression node
CallExpr::CallExpr(std::shared_ptr<Expr> func, std::vector<std::shared_ptr<Expr> > args)
    : func(func), args(args) {}

void CallExpr::accept(ASTVisitor *visitor) { visitor->visit(*this); }

const std::shared_ptr<Expr> &CallExpr::getFunc() const { return func; }
const std::vector<std::shared_ptr<Expr>> &CallExpr::getArgs() const { return args; }
