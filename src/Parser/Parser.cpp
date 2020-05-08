//===- stoc/Parser/Parser.cpp - Implementation of Parser class --------*- C++ -*-===//
//
//===----------------------------------------------------------------------------===//
//
// This file implements the Parser class.
// Parsing is the second phase of a compiler and transforms the list of
// tokens into a abstract syntax tree (AST)
//
//===---------------------------------------------------------------------------===//

#include "stoc/Parser/Parser.h"

#include <iostream>

Parser::Parser(const std::shared_ptr<SrcFile>& file) {
  this->file = file;
  this->current = 0;
}

void Parser::parse() {
  while (!isAtEnd()) {
    ast.push_back(parseExpr());
  }

  this->file->setAst(ast);
}

void Parser::reportError(std::string error_msg) {
  std::cerr << "<" << this->file->getFilename()
            << ":l." << currentToken().line << ">"
            << " error: " << currentToken().value
            << " " << error_msg << std::endl;

  this->file->setErrorInParsing(true);
}

bool Parser::isAtEnd() {
  return (currentToken().type == T_EOF) ||
         (this->current >= this->file->getTokens().size());
}

Token Parser::currentToken() {
  return this->file->getTokens()[current];
}

Token Parser::previousToken() {
  if(this->current > 0) { return this->file->getTokens()[current - 1]; }
  else { return this->file->getTokens()[current]; }
}

bool Parser::check(TokenType type) {
  if (isAtEnd()) {
    return false;
  } else {
    return currentToken().type == type;
  }
}

Token Parser::advance() {
  if (isAtEnd()) {
    return currentToken();
  } else {
    current++;
    return previousToken();
  }
}

bool Parser::match(TokenType type) {
  if (check(type)) {
    advance();
    return true;
  } else {
    return false;
  }
}

Token Parser::consume(TokenType type, std::string error_msg) {
  if (check(type)) {
    return advance();
  } else {
    reportError(error_msg);
    // TODO: change when parsing statements for an exception
    exit(1);
  }
}

std::shared_ptr<Expr> Parser::parseExpr() {
  return parseBinaryExpr(PREC_LOWEST + 1);
}

std::shared_ptr<Expr> Parser::parseBinaryExpr(int prec) {
  std::shared_ptr<Expr> e = parseUnaryExpr();

  // parse binary expression while the precedence of \op is higher than precedence of \prec
  // (e.g. if prec = precedence(ADD), it will parse multiplications(STAR), divisions(SLASH)
  // and unary operations. Also additions(ADD) and substractions(SUB) [left-associativity] while
  // creating the AST)
  Token op = currentToken();
  while (tokenPrec(op.type) >= prec) {
    advance();
    std::shared_ptr<Expr> rhs = parseBinaryExpr(tokenPrec(op.type) + 1);
    e = std::make_shared<ExprBinary>(e, rhs, op);
    op = this->file->getTokens()[current];
  }

  return e;
}

std::shared_ptr<Expr> Parser::parseUnaryExpr() {
  switch (currentToken().type) {
  case ADD: // we allow ADD(+) as unary operator
  case SUB:
  case NOT:
    Token t = advance();
    std::shared_ptr<Expr> rhs = parseBinaryExpr(PREC_UNARY);
    return std::make_shared<ExprUnary>(rhs, t);
  }

  return parsePrimaryExpr();
}

std::shared_ptr<Expr> Parser::parsePrimaryExpr() {
  switch (currentToken().type) {
  case LIT_TRUE:
  case LIT_FALSE:
  case LIT_INT:
  case LIT_FLOAT:
  case LIT_STRING:
  case LIT_NIL:
    return std::make_shared<ExprLiteral>(advance());
  case LPAREN:
    advance();
    std::shared_ptr<Expr> group = parseExpr();
    consume(RPAREN, "Expected ')' after expression");
    return group;
  }

  reportError("Expected expression");
  // TODO: change when parsing statements for an exception
  exit(1);
}