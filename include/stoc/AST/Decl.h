// TODO: add header of the file
#ifndef STOC_DECL_H
#define STOC_DECL_H

#include "BasicNode.h"
#include "stoc/Scanner/Token.h"

#include <memory>
#include <string>
#include <vector>

/// A declaration is a node in the AST that declares a new name (variable, constant, function)
class Decl : public BasicNode {
};

/// A variable declaration is a node in the AST that declares and defines a variable
///  (e.g. var type name = value)
class VarDecl : public Decl {
private:
  /// keyword VAR for declaring a variable (needed for printing AST)
  Token varKeyword;
  Token type;
  Token identifier;
  std::shared_ptr<Expr> value;

public:
  // TODO: change order of identifier and type (fields, constructor and getters)
  VarDecl(Token varKeyword, Token identifier, Token type, std::shared_ptr<Expr> value);

  /// method needed for the Visitor Pattern
  void accept(ASTVisitor *visitor) override;

  // Getters
  [[nodiscard]] const Token &getVarKeyword() const;
  [[nodiscard]] const Token &getIdentifier() const;
  [[nodiscard]] const Token &getType() const;
  [[nodiscard]] const std::shared_ptr<Expr> &getValue() const;
};

/// A constant declaration is a node in the AST that declares and defines a constant
///  (e.g. const type name = value)
class ConstDecl : public Decl {
private:
  /// keyword CONST for declaring a constant (needed for printing AST)
  Token constKeyword;
  Token identifier;
  Token type;
  std::shared_ptr<Expr> value;

public:
  // TODO: change order of identifier and type (fields, constructor and getters)
  ConstDecl(Token constKeyword, Token identifier, Token type, std::shared_ptr<Expr> value);

  /// method needed for the Visitor Pattern
  void accept(ASTVisitor *visitor) override;

  // Getters
  [[nodiscard]] const Token &getConstKeyword() const;
  [[nodiscard]] const Token &getIdentifier() const;
  [[nodiscard]] const Token &getType() const;
  [[nodiscard]] const std::shared_ptr<Expr> &getValue() const;
};

/// A parameter declaration is a node in the AST that declares a parameter in a function
class ParamDecl : public Decl {
private:
  /// keyword VAR/CONST for declaring a variable/constant as a parameter
  Token keyword;
  Token type;
  Token identifier;

public:
  ParamDecl(Token keyword, Token type, Token identifier);

  /// method needed for the Visitor Pattern
  void accept(ASTVisitor *visitor) override;

  // Getters
  [[nodiscard]] const Token &getKeyword() const;
  [[nodiscard]] const Token &getType() const;
  [[nodiscard]] const Token &getIdentifier() const;
};

/// A function declaration is a node in the AST that declares and defines a function
///  (e.g. func identifier(param, param) returnType body)
class FuncDecl : public Decl {
private:
  /// keyword FUNC for declaring a function
  Token funcKeyword;
  Token identifier;
  std::vector<std::shared_ptr<ParamDecl>> params;
  Token returnType;
  /// true if function returns something, false otherwise
  bool hasReturnType;
  std::shared_ptr<BlockStmt> body;

public:
  FuncDecl(Token funcKeyword, Token identifier, std::vector<std::shared_ptr<ParamDecl>> params,
           Token returnType, std::shared_ptr<BlockStmt> body);

  FuncDecl(Token funcKeyword, Token identifier, std::vector<std::shared_ptr<ParamDecl>> params,
           std::shared_ptr<BlockStmt> body);

  /// method needed for the Visitor Pattern
  void accept(ASTVisitor *visitor) override;

  // Getters
  [[nodiscard]] const Token &getFuncKeyword() const;
  [[nodiscard]] const Token &getIdentifier() const;
  [[nodiscard]] const std::vector<std::shared_ptr<ParamDecl>> &getParams() const;
  [[nodiscard]] const Token &getReturnType() const;
  [[nodiscard]] bool isHasReturnType() const;
  [[nodiscard]] const std::shared_ptr<BlockStmt> &getBody() const;
};

#endif // STOC_DECL_H
