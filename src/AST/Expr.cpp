//===- src/AST/Expr.cpp - Implementation of nodes related to Expressions in AST -----*- C++ -*-===//
//
//===------------------------------------------------------------------------------------------===//
//
// This file implements the classes of nodes related to expressions in AST.
//
//===------------------------------------------------------------------------------------------===//

#include "stoc/AST/Expr.h"

// Expression node
Expr::Expr(Expr::Kind exprKind) : exprKind(exprKind) {}
Expr::Kind Expr::getExprKind() const { return exprKind; }

// Binary Expression node
BinaryExpr::BinaryExpr(std::shared_ptr<Expr> lhs, std::shared_ptr<Expr> rhs, Token &op)
    : lhs(std::move(lhs)), rhs(std::move(rhs)), op(op), Expr(Expr::Kind::BINARYEXPR) {}

void BinaryExpr::accept(ASTVisitor *visitor) {
  visitor->visit(std::dynamic_pointer_cast<BinaryExpr>(Expr::shared_from_this()));
}

const std::shared_ptr<Expr> &BinaryExpr::getLhs() const { return lhs; }
const std::shared_ptr<Expr> &BinaryExpr::getRhs() const { return rhs; }
const Token &BinaryExpr::getOp() const { return op; }
const std::string &BinaryExpr::getType() const { return type; }
void BinaryExpr::setType(const std::string &type) { this->type = type; }

// Unary Expression node
UnaryExpr::UnaryExpr(std::shared_ptr<Expr> rhs, Token &op)
    : rhs(std::move(rhs)), op(op), Expr(Expr::Kind::UNARYEXPR) {}

void UnaryExpr::accept(ASTVisitor *visitor) {
  visitor->visit(std::dynamic_pointer_cast<UnaryExpr>(Expr::shared_from_this()));
}

const std::shared_ptr<Expr> &UnaryExpr::getRhs() const { return rhs; }
const Token &UnaryExpr::getOp() const { return op; }
const std::string &UnaryExpr::getType() const { return type; }
void UnaryExpr::setType(const std::string &type) { this->type = type; };

// Literal Expression node
LiteralExpr::LiteralExpr(Token token) : token(token), Expr(Expr::Kind::LITERALEXPR) {}

void LiteralExpr::accept(ASTVisitor *visitor) {
  visitor->visit(std::dynamic_pointer_cast<LiteralExpr>(Expr::shared_from_this()));
}

const Token &LiteralExpr::getToken() const { return token; }
const std::string &LiteralExpr::getType() const { return type; }
void LiteralExpr::setType(const std::string &type) { this->type = type; }

// Identifier Expression node
IdentExpr::IdentExpr(Token ident) : ident(ident), Expr(Expr::Kind::IDENTEXPR) {}

void IdentExpr::accept(ASTVisitor *visitor) {
  visitor->visit(std::dynamic_pointer_cast<IdentExpr>(Expr::shared_from_this()));
}

const Token &IdentExpr::getIdent() const { return ident; }
const std::string &IdentExpr::getName() const { return ident.value; }
const std::string &IdentExpr::getType() const { return type; }
void IdentExpr::setType(const std::string &type) { this->type = type; }

// Call Expression node
CallExpr::CallExpr(std::shared_ptr<Expr> func, std::vector<std::shared_ptr<Expr>> args)
    : func(func), args(args), Expr(Expr::Kind::CALLEXPR) {}

void CallExpr::accept(ASTVisitor *visitor) {
  visitor->visit(std::dynamic_pointer_cast<CallExpr>(Expr::shared_from_this()));
}

const std::shared_ptr<Expr> &CallExpr::getFunc() const { return func; }
const std::vector<std::shared_ptr<Expr>> &CallExpr::getArgs() const { return args; }
const std::string &CallExpr::getType() const { return type; }
void CallExpr::setType(const std::string &type) { this->type = type; }
