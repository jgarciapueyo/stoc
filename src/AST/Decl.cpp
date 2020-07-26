//===- src/AST/Decl.cpp - Implementation of nodes related to Declarations in AST ----*- C++ -*-===//
//
//===------------------------------------------------------------------------------------------===//
//
// This file implements the classes of nodes related to declarations in AST.
//
//===------------------------------------------------------------------------------------------===//

#include "stoc/AST/Decl.h"

// Decl node
Decl::Decl(Decl::Kind declKind) : declKind(declKind) {}
Decl::Kind Decl::getDeclKind() const { return declKind; }

// Variable Declaration node
VarDecl::VarDecl(Token varKeyword, Token identifier, Token type, std::shared_ptr<Expr> value)
    : varKeyword(varKeyword), identifier(identifier), type(type), value(value),
      Decl(Decl::Kind::VARDECL) {}

void VarDecl::accept(ASTVisitor *visitor) {
  visitor->visit(std::dynamic_pointer_cast<VarDecl>(Decl::shared_from_this()));
}

const Token &VarDecl::getVarKeyword() const { return varKeyword; }
const Token &VarDecl::getIdentifier() const { return identifier; }
const Token &VarDecl::getType() const { return type; }
const std::shared_ptr<Expr> &VarDecl::getValue() const { return value; }
bool VarDecl::isGlobal() const { return isGlobalVariable; }
void VarDecl::setIsGlobal(bool isGlobal) { this->isGlobalVariable = isGlobal; }

// Constant Declaration node
ConstDecl::ConstDecl(Token constKeyword, Token identifier, Token type, std::shared_ptr<Expr> value)
    : constKeyword(constKeyword), identifier(identifier), type(type), value(value),
      Decl(Decl::Kind::CONSTDECL) {}

void ConstDecl::accept(ASTVisitor *visitor) {
  visitor->visit(std::dynamic_pointer_cast<ConstDecl>(Decl::shared_from_this()));
}

const Token &ConstDecl::getConstKeyword() const { return constKeyword; }
const Token &ConstDecl::getIdentifier() const { return identifier; }
const Token &ConstDecl::getType() const { return type; }
const std::shared_ptr<Expr> &ConstDecl::getValue() const { return value; }
bool ConstDecl::isGlobal() const { return isGlobalConstant; }
void ConstDecl::setIsGlobal(bool isGlobal) { this->isGlobalConstant = isGlobal; }

// Parameter Declaration node
ParamDecl::ParamDecl(Token keyword, Token type, Token identifier)
    : keyword(keyword), type(type), identifier(identifier), Decl(Decl::Kind::PARAMDECL) {}

void ParamDecl::accept(ASTVisitor *visitor) {
  visitor->visit(std::dynamic_pointer_cast<ParamDecl>(Decl::shared_from_this()));
}

const Token &ParamDecl::getKeyword() const { return keyword; }
const Token &ParamDecl::getType() const { return type; }
const Token &ParamDecl::getIdentifier() const { return identifier; }

// Function Declaration node
FuncDecl::FuncDecl(Token funcKeyword, Token identifier,
                   std::vector<std::shared_ptr<ParamDecl>> params, Token returnType,
                   std::shared_ptr<BlockStmt> body)
    : funcKeyword(funcKeyword), identifier(identifier), params(params), returnType(returnType),
      body(body), hasReturnType(true), Decl(Decl::Kind::FUNCDECL) {}

FuncDecl::FuncDecl(Token funcKeyword, Token identifier,
                   std::vector<std::shared_ptr<ParamDecl>> params, std::shared_ptr<BlockStmt> body)
    : funcKeyword(funcKeyword), identifier(identifier), params(params), body(body),
      hasReturnType(false), Decl(Decl::Kind::FUNCDECL) {}

void FuncDecl::accept(ASTVisitor *visitor) {
  visitor->visit(std::dynamic_pointer_cast<FuncDecl>(Decl::shared_from_this()));
}

const Token &FuncDecl::getFuncKeyword() const { return funcKeyword; };
const Token &FuncDecl::getIdentifier() const { return identifier; }
const std::vector<std::shared_ptr<ParamDecl>> &FuncDecl::getParams() const { return params; }
const Token &FuncDecl::getReturnType() const { return returnType; }
bool FuncDecl::isHasReturnType() const { return hasReturnType; }
const std::shared_ptr<BlockStmt> &FuncDecl::getBody() const { return body; }
