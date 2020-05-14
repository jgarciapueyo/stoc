#ifndef STOC_DECL_H
#define STOC_DECL_H

#include "BasicNode.h"
#include "stoc/Scanner/Token.h"

#include <memory>
#include <string>
#include <vector>

class Decl : public BasicNode {};

class VarDecl : public Decl {
private:
  // TODO: see if Token VAR should be added to better show line and start when ast-dump
  std::string identifier;
  Token type;
  std::shared_ptr<Expr> value;

public:
  VarDecl(std::string identifier, Token type, std::shared_ptr<Expr> value);

  void accept(ASTVisitor *visitor) override;

  const std::string &getIdentifier() const;
  const Token &getType() const;
  const std::shared_ptr<Expr> &getValue() const;
};

class ConstDecl : public Decl {
private:
  std::string identifier;
  Token type;
  std::shared_ptr<Expr> value;

public:
  ConstDecl(std::string identifier, Token type, std::shared_ptr<Expr> value);

  void accept(ASTVisitor *visitor) override;

  // Getters
  const std::string &getIdentifier() const;
  const Token &getType() const;
  const std::shared_ptr<Expr> &getValue() const;
};

class ParamDecl : public Decl {
private:
  Token type;
  Token name;

public:
  ParamDecl(Token type, Token name);

  void accept(ASTVisitor *visitor) override;

  const Token &getType() const;
  const Token &getName() const;
};

class FuncDecl : public Decl {
private:
  Token name;
  std::vector<std::shared_ptr<ParamDecl>> params;
  Token returnType;
  std::shared_ptr<Stmt> body;

public:
  FuncDecl(Token name, std::vector<std::shared_ptr<ParamDecl>> params, Token returnType,
           std::shared_ptr<Stmt> body);

  void accept(ASTVisitor *visitor) override;

  const Token &getName() const;
  const std::vector<std::shared_ptr<ParamDecl>> &getParams() const;
  const Token &getReturnType() const;
  // TODO: change to BlockStatement
  const std::shared_ptr<Stmt> &getBody() const;
};

#endif // STOC_DECL_H
