#ifndef STOC_STMT_H
#define STOC_STMT_H

#include "BasicNode.h"
#include "stoc/Scanner/Token.h"

#include <memory>
#include <vector>

class Stmt : public BasicNode {};

class ExpressionStmt : public Stmt {
private:
  std::shared_ptr<Expr> expr;

public:
  ExpressionStmt(std::shared_ptr<Expr> expr);

  void accept(ASTVisitor *visitor) override;

  const std::shared_ptr<Expr> &getExpr() const;
};

class DeclarationStmt : public Stmt {
private:
  std::shared_ptr<Decl> decl;

public:
  DeclarationStmt(std::shared_ptr<Decl> decl);

  void accept(ASTVisitor *visitor) override;

  const std::shared_ptr<Decl> &getDecl() const;
};

// TODO: add tokens that define the statement '{', 'FOR', 'WHILE'

class BlockStmt : public Stmt {
private:
  std::vector<std::shared_ptr<Stmt>> stmts;

public:
  BlockStmt(std::vector<std::shared_ptr<Stmt>> stmts);

  void accept(ASTVisitor *visitor) override;

  const std::vector<std::shared_ptr<Stmt>> &getStmts() const;
};

class IfStmt : public Stmt {
private:
  Token ifToken;
  std::shared_ptr<Expr> condition;
  std::shared_ptr<Stmt> thenBranch;
  std::shared_ptr<Stmt> elseBranch;
  bool hasElse;

public:
  IfStmt(Token ifToken, std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch,
         std::shared_ptr<Stmt> elseBranch);

  IfStmt(Token ifToken, std::shared_ptr<Expr> condition, std::shared_ptr<Stmt> thenBranch);

  void accept(ASTVisitor *visitor) override;

  const Token &getIfToken() const;
  const std::shared_ptr<Expr> &getCondition() const;
  const std::shared_ptr<Stmt> &getThenBranch() const;
  const std::shared_ptr<Stmt> &getElseBranch() const;
  bool isHasElse() const;
};

class ForStmt : public Stmt {
private:
  Token forToken;
  std::shared_ptr<Stmt> init;
  std::shared_ptr<Expr> cond;
  std::shared_ptr<Stmt> post;
  std::shared_ptr<Stmt> body;

public:
  ForStmt(Token forToken, std::shared_ptr<Stmt> init, std::shared_ptr<Expr> cond, std::shared_ptr<Stmt> post,
          std::shared_ptr<Stmt> body);

  void accept(ASTVisitor *visitor) override;

  const Token &getForToken() const;
  const std::shared_ptr<Stmt> &getInit() const;
  const std::shared_ptr<Expr> &getCond() const;
  const std::shared_ptr<Stmt> &getPost() const;
  const std::shared_ptr<Stmt> &getBody() const;
};

class WhileStmt : public Stmt {
private:
  Token whileToken;
  std::shared_ptr<Expr> cond;
  std::shared_ptr<Stmt> body;

public:
  WhileStmt(Token whileToken, std::shared_ptr<Expr> cond, std::shared_ptr<Stmt> body);

  void accept(ASTVisitor *visitor) override;

  const Token &getWhileToken() const;
  const std::shared_ptr<Expr> &getCond() const;
  const std::shared_ptr<Stmt> &getBody() const;
};

class AssignmenStmt : public Stmt {
private:
  std::shared_ptr<Expr> lhs; // if we accept later different types of structures, change this to Expr
  std::shared_ptr<Expr> rhs;

public:
  AssignmenStmt(std::shared_ptr<Expr> lhs, std::shared_ptr<Expr> rhs);

  void accept(ASTVisitor *visitor) override;

  const std::shared_ptr<Expr> &getLhs() const;
  const std::shared_ptr<Expr> &getRhs() const;
};

class ReturnStmt : public Stmt {
private:
  Token keyword;
  std::shared_ptr<Expr> value;

public:
  ReturnStmt(Token keyword, std::shared_ptr<Expr> value);

  void accept(ASTVisitor *visitor) override;

  const Token &getKeyword() const;
  const std::shared_ptr<Expr> &getValue() const;
};

// TODO: add empty statement

#endif // STOC_STMT_H
