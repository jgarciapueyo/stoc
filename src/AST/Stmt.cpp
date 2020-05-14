#include "stoc/AST/Stmt.h"

// Expression Statement node
ExpressionStmt::ExpressionStmt(std::shared_ptr<Expr> expr) : expr(expr) {}

void ExpressionStmt::accept(ASTVisitor *visitor) { visitor->visit(*this); }

const std::shared_ptr<Expr> &ExpressionStmt::getExpr() const { return expr; }

// Declaration Statement node
DeclarationStmt::DeclarationStmt(std::shared_ptr<Decl> decl) : decl(decl) {}

void DeclarationStmt::accept(ASTVisitor *visitor) { visitor->visit(*this); }

const std::shared_ptr<Decl> &DeclarationStmt::getDecl() const { return decl; }

// Block Statement node
BlockStmt::BlockStmt(std::vector<std::shared_ptr<Stmt>> stmts) : stmts(stmts) {}

void BlockStmt::accept(ASTVisitor *visitor) { visitor->visit(*this); }

const std::vector<std::shared_ptr<Stmt>> &BlockStmt::getStmts() const { return stmts; }

// If Statement node
IfStmt::IfStmt(Token ifToken, std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch,
               std::shared_ptr<Stmt> elseBranch)
    : ifToken(ifToken), condition(condition), thenBranch(thenBranch), elseBranch(elseBranch),
      hasElse(true) {}

IfStmt::IfStmt(Token ifToken, std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch)
    : ifToken(ifToken), condition(condition), thenBranch(thenBranch), hasElse(false) {}

void IfStmt::accept(ASTVisitor *visitor) { visitor->visit(*this); }

const Token &IfStmt::getIfToken() const { return ifToken; }
const std::shared_ptr<Expr> &IfStmt::getCondition() const { return condition; }
const std::shared_ptr<Stmt> &IfStmt::getThenBranch() const { return thenBranch; }
const std::shared_ptr<Stmt> &IfStmt::getElseBranch() const { return elseBranch; }
bool IfStmt::isHasElse() const { return hasElse; }

// For Statement node
ForStmt::ForStmt(Token forToken, std::shared_ptr<Stmt> init, std::shared_ptr<Expr> cond,
                 std::shared_ptr<Stmt> post, std::shared_ptr<Stmt> body)
    : forToken(forToken), init(init), cond(cond), post(post), body(body) {}

void ForStmt::accept(ASTVisitor *visitor) { visitor->visit(*this); }

const Token &ForStmt::getForToken() const { return forToken; }
const std::shared_ptr<Stmt> &ForStmt::getInit() const { return init; }
const std::shared_ptr<Expr> &ForStmt::getCond() const { return cond; }
const std::shared_ptr<Stmt> &ForStmt::getPost() const { return post; }
const std::shared_ptr<Stmt> &ForStmt::getBody() const { return body; }

// While Statement node
WhileStmt::WhileStmt(Token whileToken, std::shared_ptr<Expr> cond, std::shared_ptr<Stmt> body)
    : whileToken(whileToken), cond(cond), body(body) {}

void WhileStmt::accept(ASTVisitor *visitor) { visitor->visit(*this); }

const Token &WhileStmt::getWhileToken() const { return whileToken; }
const std::shared_ptr<Expr> &WhileStmt::getCond() const { return cond; }
const std::shared_ptr<Stmt> &WhileStmt::getBody() const { return body; }

// Assignment Statement node
AssignmenStmt::AssignmenStmt(std::shared_ptr<Expr> lhs, std::shared_ptr<Expr> rhs)
    : lhs(lhs), rhs(rhs) {}

void AssignmenStmt::accept(ASTVisitor *visitor) { visitor->visit(*this); }

const std::shared_ptr<Expr> &AssignmenStmt::getLhs() const { return lhs; }
const std::shared_ptr<Expr> &AssignmenStmt::getRhs() const { return rhs; }

// Return Statement node
ReturnStmt::ReturnStmt(Token keyword, std::shared_ptr<Expr> value)
    : keyword(keyword), value(value) {}

void ReturnStmt::accept(ASTVisitor *visitor) { visitor->visit(*this); }

const Token &ReturnStmt::getKeyword() const { return keyword; }
const std::shared_ptr<Expr> &ReturnStmt::getValue() const { return value; }
