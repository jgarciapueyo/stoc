#include "stoc/AST/Decl.h"

// Variable Declaration node
VarDecl::VarDecl(std::string identifier, Token type, std::shared_ptr<Expr> value)
    : identifier(identifier), type(type), value(value) {}

void VarDecl::accept(ASTVisitor *visitor) { visitor->visit(*this); }

const std::string &VarDecl::getIdentifier() const { return identifier; }
const Token &VarDecl::getType() const { return type; }
const std::shared_ptr<Expr> &VarDecl::getValue() const { return value; }

// Constant Declaration node
ConstDecl::ConstDecl(std::string identifier, Token type, std::shared_ptr<Expr> value)
    : identifier(identifier), type(type), value(value) {}

void ConstDecl::accept(ASTVisitor *visitor) { visitor->visit(*this); }

const std::string &ConstDecl::getIdentifier() const { return identifier; }
const Token &ConstDecl::getType() const { return type; }
const std::shared_ptr<Expr> &ConstDecl::getValue() const { return value; }

// Parameter Declaration node
ParamDecl::ParamDecl(Token type, Token name) : type(type), name(name) {}

void ParamDecl::accept(ASTVisitor *visitor) { visitor->visit(*this); }

const Token &ParamDecl::getType() const { return type; }
const Token &ParamDecl::getName() const { return name; }

// Function Declaration node
FuncDecl::FuncDecl(Token name, std::vector<std::shared_ptr<ParamDecl>> params, Token returnType,
                   std::shared_ptr<Stmt> body)
    : name(name), params(params), returnType(returnType), body(body) {}

void FuncDecl::accept(ASTVisitor *visitor) { visitor->visit(*this); }

const Token &FuncDecl::getName() const { return name; }
const std::vector<std::shared_ptr<ParamDecl>> &FuncDecl::getParams() const { return params; }
const Token &FuncDecl::getReturnType() const { return returnType; }
const std::shared_ptr<Stmt> &FuncDecl::getBody() const { return body; }
