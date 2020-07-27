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
VarDecl::VarDecl(Token varKeywordToken, Token typeToken, Token identifierToken,
                 std::shared_ptr<Expr> value)
    : varKeywordToken(varKeywordToken), typeToken(typeToken), identifierToken(identifierToken),
      value(value), Decl(Decl::Kind::VARDECL) {}

void VarDecl::accept(ASTVisitor *visitor) {
  visitor->visit(std::dynamic_pointer_cast<VarDecl>(Decl::shared_from_this()));
}

const Token &VarDecl::getVarKeywordToken() const { return varKeywordToken; }
const Token &VarDecl::getTypeToken() const { return typeToken; }
const Token &VarDecl::getIdentifierToken() const { return identifierToken; }
const std::shared_ptr<Expr> &VarDecl::getValue() const { return value; }
bool VarDecl::isGlobal() const { return isGlobalVariable; }
void VarDecl::setIsGlobal(bool isGlobal) { this->isGlobalVariable = isGlobal; }

// Constant Declaration node
ConstDecl::ConstDecl(Token constKeywordToken, Token typeToken, Token identifierToken,
                     std::shared_ptr<Expr> value)
    : constKeywordToken(constKeywordToken), identifierToken(identifierToken), typeToken(typeToken),
      value(value), Decl(Decl::Kind::CONSTDECL) {}

void ConstDecl::accept(ASTVisitor *visitor) {
  visitor->visit(std::dynamic_pointer_cast<ConstDecl>(Decl::shared_from_this()));
}

const Token &ConstDecl::getConstKeywordToken() const { return constKeywordToken; }
const Token &ConstDecl::getTypeToken() const { return typeToken; }
const Token &ConstDecl::getIdentifierToken() const { return identifierToken; }
const std::shared_ptr<Expr> &ConstDecl::getValue() const { return value; }
bool ConstDecl::isGlobal() const { return isGlobalConstant; }
void ConstDecl::setIsGlobal(bool isGlobal) { this->isGlobalConstant = isGlobal; }

// Parameter Declaration node
ParamDecl::ParamDecl(Token keywordToken, Token typeToken, Token identifierToken)
    : keywordToken(keywordToken), typeToken(typeToken), identifierToken(identifierToken),
      Decl(Decl::Kind::PARAMDECL) {}

void ParamDecl::accept(ASTVisitor *visitor) {
  visitor->visit(std::dynamic_pointer_cast<ParamDecl>(Decl::shared_from_this()));
}

const Token &ParamDecl::getKeywordToken() const { return keywordToken; }
const Token &ParamDecl::getTypeToken() const { return typeToken; }
const Token &ParamDecl::getIdentifierToken() const { return identifierToken; }

// Function Declaration node
FuncDecl::FuncDecl(Token funcKeywordToken, Token identifierToken,
                   std::vector<std::shared_ptr<ParamDecl>> params, Token returnTypeToken,
                   std::shared_ptr<BlockStmt> body)
    : funcKeywordToken(funcKeywordToken), identifierToken(identifierToken), params(params),
      returnTypeToken(returnTypeToken), body(body), hasReturnType(true),
      Decl(Decl::Kind::FUNCDECL) {}

FuncDecl::FuncDecl(Token funcKeywordToken, Token identifierToken,
                   std::vector<std::shared_ptr<ParamDecl>> params, std::shared_ptr<BlockStmt> body)
    : funcKeywordToken(funcKeywordToken), identifierToken(identifierToken), params(params),
      body(body), hasReturnType(false), Decl(Decl::Kind::FUNCDECL) {}

void FuncDecl::accept(ASTVisitor *visitor) {
  visitor->visit(std::dynamic_pointer_cast<FuncDecl>(Decl::shared_from_this()));
}

const Token &FuncDecl::getFuncKeywordToken() const { return funcKeywordToken; };
const Token &FuncDecl::getIdentifierToken() const { return identifierToken; }
const std::vector<std::shared_ptr<ParamDecl>> &FuncDecl::getParams() const { return params; }
const Token &FuncDecl::getReturnTypeToken() const { return returnTypeToken; }
bool FuncDecl::isHasReturnType() const { return hasReturnType; }
const std::shared_ptr<BlockStmt> &FuncDecl::getBody() const { return body; }
