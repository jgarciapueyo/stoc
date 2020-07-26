//===- src/AST/Stmt.cpp - Implementation of nodes related to Statements in AST ------*- C++ -*-===//
//
//===------------------------------------------------------------------------------------------===//
//
// This file implements the classes of nodes related to statements in AST.
//
//===------------------------------------------------------------------------------------------===//

#include "stoc/AST/Stmt.h"

// Stmt node
Stmt::Stmt(Stmt::Kind stmtKind) : stmtKind(stmtKind) {}
Stmt::Kind Stmt::getStmtKind() const { return stmtKind; }

// Expression Statement node
ExpressionStmt::ExpressionStmt(std::shared_ptr<Expr> expr)
    : expr(expr), Stmt(Stmt::Kind::EXPRESSIONSTMT) {}

void ExpressionStmt::accept(ASTVisitor *visitor) {
  visitor->visit(std::dynamic_pointer_cast<ExpressionStmt>(Stmt::shared_from_this()));
}

const std::shared_ptr<Expr> &ExpressionStmt::getExpr() const { return expr; }

// Declaration Statement node
DeclarationStmt::DeclarationStmt(std::shared_ptr<Decl> decl)
    : decl(decl), Stmt(Stmt::Kind::DECLARATIONSTMT) {}

void DeclarationStmt::accept(ASTVisitor *visitor) {
  visitor->visit(std::dynamic_pointer_cast<DeclarationStmt>(Stmt::shared_from_this()));
}

const std::shared_ptr<Decl> &DeclarationStmt::getDecl() const { return decl; }

// Block Statement node
BlockStmt::BlockStmt(Token lbrace, std::vector<std::shared_ptr<Stmt>> stmts, Token rbrace)
    : lbrace(lbrace), stmts(stmts), rbrace(rbrace), Stmt(Stmt::Kind::BLOCKSTMT) {}

void BlockStmt::accept(ASTVisitor *visitor) {
  visitor->visit(std::dynamic_pointer_cast<BlockStmt>(Stmt::shared_from_this()));
}

const Token &BlockStmt::getLbrace() const { return lbrace; }
const Token &BlockStmt::getRbrace() const { return rbrace; }
const std::vector<std::shared_ptr<Stmt>> &BlockStmt::getStmts() const { return stmts; }

// If Statement node
IfStmt::IfStmt(Token ifKeyword, std::shared_ptr<Expr> condition,
               std::shared_ptr<BlockStmt> thenBranch, std::shared_ptr<Stmt> elseBranch)
    : ifKeyword(ifKeyword), condition(condition), thenBranch(thenBranch), elseBranch(elseBranch),
      hasElse(true), Stmt(Stmt::Kind::IFSTMT) {}

IfStmt::IfStmt(Token ifToken, std::shared_ptr<Expr> condition,
               std::shared_ptr<BlockStmt> thenBranch)
    : ifKeyword(ifToken), condition(condition), thenBranch(thenBranch), elseBranch(nullptr),
      hasElse(false), Stmt(Stmt::Kind::IFSTMT) {}

void IfStmt::accept(ASTVisitor *visitor) {
  visitor->visit(std::dynamic_pointer_cast<IfStmt>(Stmt::shared_from_this()));
}

const Token &IfStmt::getIfKeyword() const { return ifKeyword; }
const std::shared_ptr<Expr> &IfStmt::getCondition() const { return condition; }
const std::shared_ptr<BlockStmt> &IfStmt::getThenBranch() const { return thenBranch; }
const std::shared_ptr<Stmt> &IfStmt::getElseBranch() const { return elseBranch; }
bool IfStmt::isHasElse() const { return hasElse; }

// For Statement node
ForStmt::ForStmt(Token forKeyword, std::shared_ptr<Stmt> init, std::shared_ptr<Expr> cond,
                 std::shared_ptr<Stmt> post, std::shared_ptr<BlockStmt> body)
    : forKeyword(forKeyword), init(init), cond(cond), post(post), body(body),
      Stmt(Stmt::Kind::FORSTMT) {}

void ForStmt::accept(ASTVisitor *visitor) {
  visitor->visit(std::dynamic_pointer_cast<ForStmt>(Stmt::shared_from_this()));
}

const Token &ForStmt::getForKeyword() const { return forKeyword; }
const std::shared_ptr<Stmt> &ForStmt::getInit() const { return init; }
const std::shared_ptr<Expr> &ForStmt::getCond() const { return cond; }
const std::shared_ptr<Stmt> &ForStmt::getPost() const { return post; }
const std::shared_ptr<BlockStmt> &ForStmt::getBody() const { return body; }

// While Statement node
WhileStmt::WhileStmt(Token whileKeyword, std::shared_ptr<Expr> cond,
                     std::shared_ptr<BlockStmt> body)
    : whileKeyword(whileKeyword), cond(cond), body(body), Stmt(Stmt::Kind::WHILESTMT) {}

void WhileStmt::accept(ASTVisitor *visitor) {
  visitor->visit(std::dynamic_pointer_cast<WhileStmt>(Stmt::shared_from_this()));
}

const Token &WhileStmt::getWhileKeyword() const { return whileKeyword; }
const std::shared_ptr<Expr> &WhileStmt::getCond() const { return cond; }
const std::shared_ptr<BlockStmt> &WhileStmt::getBody() const { return body; }

// Assignment Statement node
AssignmentStmt::AssignmentStmt(std::shared_ptr<Expr> lhs, std::shared_ptr<Expr> rhs,
                               Token equalToken)
    : lhs(lhs), rhs(rhs), equalToken(equalToken), Stmt(Stmt::Kind::ASSIGNMENTSTMT) {}

void AssignmentStmt::accept(ASTVisitor *visitor) {
  visitor->visit(std::dynamic_pointer_cast<AssignmentStmt>(Stmt::shared_from_this()));
}

const Token &AssignmentStmt::getEqualToken() const { return equalToken; }
const std::shared_ptr<Expr> &AssignmentStmt::getLhs() const { return lhs; }
const std::shared_ptr<Expr> &AssignmentStmt::getRhs() const { return rhs; }

// Return Statement node
ReturnStmt::ReturnStmt(Token returnKeyword, std::shared_ptr<Expr> value)
    : returnKeyword(returnKeyword), value(value), Stmt(Stmt::Kind::RETURNSTMT) {}

void ReturnStmt::accept(ASTVisitor *visitor) {
  visitor->visit(std::dynamic_pointer_cast<ReturnStmt>(Stmt::shared_from_this()));
}

const Token &ReturnStmt::getReturnKeyword() const { return returnKeyword; }
const std::shared_ptr<Expr> &ReturnStmt::getValue() const { return value; }
