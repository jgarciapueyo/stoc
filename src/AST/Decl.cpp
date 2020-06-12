// TODO: add header of the file
#include "stoc/AST/Decl.h"

// Variable Declaration node
VarDecl::VarDecl(Token varKeyword, Token identifier, Token type, std::shared_ptr<Expr> value)
    : varKeyword(varKeyword), identifier(identifier), type(type), value(value) {}

void VarDecl::accept(ASTVisitor *visitor) { visitor->visit(std::make_shared<VarDecl>(*this)); }

const Token &VarDecl::getVarKeyword() const { return varKeyword; }
const Token &VarDecl::getIdentifier() const { return identifier; }
const Token &VarDecl::getType() const { return type; }
const std::shared_ptr<Expr> &VarDecl::getValue() const { return value; }

// Constant Declaration node
ConstDecl::ConstDecl(Token constKeyword, Token identifier, Token type, std::shared_ptr<Expr> value)
    : constKeyword(constKeyword), identifier(identifier), type(type), value(value) {}

void ConstDecl::accept(ASTVisitor *visitor) { visitor->visit(std::make_shared<ConstDecl>(*this)); }

const Token &ConstDecl::getConstKeyword() const { return constKeyword; }
const Token &ConstDecl::getIdentifier() const { return identifier; }
const Token &ConstDecl::getType() const { return type; }
const std::shared_ptr<Expr> &ConstDecl::getValue() const { return value; }

// Parameter Declaration node
ParamDecl::ParamDecl(Token keyword, Token type, Token identifier)
    : keyword(keyword), type(type), identifier(identifier) {}

void ParamDecl::accept(ASTVisitor *visitor) { visitor->visit(std::make_shared<ParamDecl>(*this)); }

const Token &ParamDecl::getKeyword() const { return keyword; }
const Token &ParamDecl::getType() const { return type; }
const Token &ParamDecl::getIdentifier() const { return identifier; }

// Function Declaration node
FuncDecl::FuncDecl(Token funcKeyword, Token identifier,
                   std::vector<std::shared_ptr<ParamDecl>> params, Token returnType,
                   std::shared_ptr<BlockStmt> body)
    : funcKeyword(funcKeyword), identifier(identifier), params(params), returnType(returnType),
      body(body), hasReturnType(true) {}

FuncDecl::FuncDecl(Token funcKeyword, Token identifier,
                   std::vector<std::shared_ptr<ParamDecl>> params, std::shared_ptr<BlockStmt> body)
    : funcKeyword(funcKeyword), identifier(identifier), params(params), body(body),
      hasReturnType(false) {}

void FuncDecl::accept(ASTVisitor *visitor) { visitor->visit(std::make_shared<FuncDecl>(*this)); }

const Token &FuncDecl::getFuncKeyword() const { return funcKeyword; };
const Token &FuncDecl::getIdentifier() const { return identifier; }
const std::vector<std::shared_ptr<ParamDecl>> &FuncDecl::getParams() const { return params; }
const Token &FuncDecl::getReturnType() const { return returnType; }
bool FuncDecl::isHasReturnType() const { return hasReturnType; }
const std::shared_ptr<BlockStmt> &FuncDecl::getBody() const { return body; }
