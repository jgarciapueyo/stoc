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

Parser::Parser(const std::shared_ptr<SrcFile> &file) {
  this->file = file;
  this->current = 0;
}

void Parser::parse() {
  while (!isAtEnd()) {
    ast.push_back(parseDecl());
  }

  this->file->setAst(ast);
}

void Parser::reportError(std::string error_msg) {
  std::cerr << "<" << this->file->getFilename() << ":l." << currentToken().line << ">"
            << " error: " << currentToken().value << " " << error_msg << std::endl;

  this->file->setErrorInParsing(true);
}

bool Parser::isAtEnd() {
  return (currentToken().type == T_EOF) || (this->current >= this->file->getTokens().size());
}

Token Parser::currentToken() { return this->file->getTokens()[current]; }

Token Parser::previousToken() {
  if (this->current > 0) {
    return this->file->getTokens()[current - 1];
  } else {
    return this->file->getTokens()[current];
  }
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

// TODO: add method expected(type, error_msg) similar to consume but does not advance()

Token Parser::consume(TokenType type, std::string error_msg) {
  if (check(type)) {
    return advance();
  } else {
    reportError(error_msg);
    // TODO: change when parsing statements for an exception
    exit(1);
  }
}

Token Parser::parseType() {
  switch (currentToken().type) {
  case BOOL:
  case INT:
  case FLOAT:
  case STRING:
    return advance();
  default:
    reportError("Expected type: " + currentToken().getTypeAsString() + " is not a type");
    // TODO: change when parsing statements for an exception
    exit(1);
  }
}

std::vector<std::shared_ptr<ParamDecl>> Parser::parseParameters() {
  consume(LPAREN, "Expected '(' in parameter definition");
  std::vector<std::shared_ptr<ParamDecl>> params = {};

  while (!check(RPAREN) && !check(T_EOF)) {
    // for now only var TODO: think about if it should be CONST, always var ???
    consume(VAR, "Expected VAR as variable definition in parameters");
    Token type = parseType();
    Token ident = consume(IDENTIFIER, "Expected IDENTIFIER in variable definition in parameters");
    params.push_back(std::make_shared<ParamDecl>(type, ident));
    if (check(COMMA)) {
      advance();
    }
  }

  consume(RPAREN, "Expected ')' after parameter definition");
  return params;
}

Token Parser::parseReturnType() { return parseType(); }

std::shared_ptr<Stmt> Parser::parseStmt() {
  switch (currentToken().type) {
  case LBRACE:
    return parseBlockStmt();
  case IF:
    return parseIfStmt();
  case FOR:
    return parseForStmt();
  case WHILE:
    return parseWhileStmt();
  case RETURN:
    return parseReturnStmt();
  default:
    return parseSimpleStmt(true);
  }
}

std::shared_ptr<Stmt> Parser::parseSimpleStmt(bool semicolonExp) {
  if (check(CONST) || check(VAR)) {
    std::shared_ptr<Decl> decl = parseDecl();
    return std::make_shared<DeclarationStmt>(decl);
  } else {
    std::shared_ptr<Expr> lhsExpr = parseExpr();

    if (check(ASSIGN)) {
      advance();
      std::shared_ptr<Expr> rhsExpr = parseExpr();
      if (semicolonExp) {
        consume(SEMICOLON, "Expected ';' after expression");
      }
      return std::make_shared<AssignmenStmt>(lhsExpr, rhsExpr);
    } else {
      if (semicolonExp) {
        consume(SEMICOLON, "Expected ';' after expression");
      }
      return std::make_shared<ExpressionStmt>(lhsExpr);
    }
  }
}

std::shared_ptr<Stmt> Parser::parseBlockStmt() {
  std::vector<std::shared_ptr<Stmt>> stmts = {};

  // we know current token is '{'
  advance();
  while (!check(RBRACE) && !isAtEnd()) {
    stmts.push_back(parseStmt());
  }

  consume(RBRACE, "Expected '}' after block");
  return std::make_shared<BlockStmt>(stmts);
}

std::shared_ptr<Stmt> Parser::parseIfStmt() {
  // we know current token is IF
  Token ifToken = consume(IF, "Expected 'IF' as keyword");
  std::shared_ptr<Expr> cond = parseExpr();
  std::shared_ptr<Stmt> thenBranch = parseBlockStmt();

  std::shared_ptr<Stmt> elseBranch = nullptr;
  if (match(ELSE)) {
    switch (currentToken().type) {
    case IF:
      elseBranch = parseIfStmt();
      break;
    case LBRACE:
      elseBranch = parseBlockStmt();
      break;
    default:
      reportError("Expected 'if' statement or block");
      // TODO: change when parsing statements for an exception
      exit(1);
    }
  }

  if (elseBranch == nullptr) {
    return std::make_shared<IfStmt>(ifToken, cond, thenBranch);
  } else {
    return std::make_shared<IfStmt>(ifToken, cond, thenBranch, elseBranch);
  }
}

std::shared_ptr<Stmt> Parser::parseForStmt() {
  // we know current token is FOR
  Token forToken = consume(FOR, "Expected 'FOR' as keyword");

  std::shared_ptr<Stmt> init = nullptr;
  if (!check(SEMICOLON)) {
    // there is init
    init = parseSimpleStmt(true);
  } else {
    advance(); // is semicolon
  }

  std::shared_ptr<Expr> cond = nullptr;
  if (!check(SEMICOLON)) {
    // there is condition
    cond = parseExpr();
    consume(SEMICOLON, "Expected ';' after condition in for");
  } else {
    advance(); // is semicolon
  }

  std::shared_ptr<Stmt> post = nullptr;
  if (!check(LBRACE)) {
    // there is post
    post = parseSimpleStmt(false);
  }

  std::shared_ptr<Stmt> body = parseBlockStmt();
  return std::make_shared<ForStmt>(forToken, init, cond, post, body);
}

std::shared_ptr<Stmt> Parser::parseWhileStmt() {
  // we know current token is WHILE
  Token whileToken = consume(WHILE, "Expected 'WHILE' as keyword");

  std::shared_ptr<Expr> cond = parseExpr();

  if (currentToken().type != LBRACE) {
    // TODO: change for an exception
    reportError("Expected Block Statement after while condition");
    exit(1);
  }

  std::shared_ptr<Stmt> body = parseBlockStmt();
  return std::make_shared<WhileStmt>(whileToken, cond, body);
}

std::shared_ptr<Stmt> Parser::parseReturnStmt() {
  // we know curret token is RETURN
  Token returnToken = consume(RETURN, "Expected 'RETURN' as keyword");

  if (!check(SEMICOLON)) {
    std::shared_ptr<Expr> value = parseExpr();
    consume(SEMICOLON, "Expected ';' after RETURN statement");
    return std::make_shared<ReturnStmt>(returnToken, value);
  } else {
    consume(SEMICOLON, "Expected ';' after RETURN statement");
    return std::make_shared<ReturnStmt>(returnToken, nullptr);
  }
}

std::shared_ptr<Decl> Parser::parseDecl() {
  switch (currentToken().type) {
  case VAR:
  case CONST:
    return parseVarConstDecl();
  case FUNC:
    return parseFuncDecl();
  default:
    reportError("Expected declaration");
    // TODO: change when parsing statements for an exception
    exit(1);
  }
}

std::shared_ptr<Decl> Parser::parseVarConstDecl() {
  // we know current token is VAR or CONST
  Token varConst = advance();
  Token type = parseType();
  Token identifier = consume(IDENTIFIER, "Expected identifier after " + varConst.getTypeAsString() +
                                             " declaration");
  consume(ASSIGN, "Expected '=' after " + varConst.getTypeAsString() + " declaration");
  std::shared_ptr<Expr> value = parseExpr();
  consume(SEMICOLON, "Expected ';' after variable declaration");

  if (varConst.type == VAR) {
    return std::make_shared<VarDecl>(identifier.value, type, value);
  } else {
    return std::make_shared<ConstDecl>(identifier.value, type, value);
  }
}

std::shared_ptr<Decl> Parser::parseFuncDecl() {
  // we know current token is FUNC
  advance();
  Token name = consume(IDENTIFIER, "Expected IDENTIFIER in after 'func' in function declaration");
  std::vector<std::shared_ptr<ParamDecl>> params = parseParameters();

  // TODO: improve this of checking the return type (how to create a token to indicate this??)
  Token returnType;
  if (!check(LBRACE)) {
    returnType = parseReturnType();
  }

  std::shared_ptr<Stmt> body = parseBlockStmt();

  return std::make_shared<FuncDecl>(name, params, returnType, body);
}

std::shared_ptr<Expr> Parser::parseExpr() { return parseBinaryExpr(PREC_LOWEST + 1); }

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
    e = std::make_shared<BinaryExpr>(e, rhs, op);
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
    return std::make_shared<UnaryExpr>(rhs, t);
  }

  return parsePrimaryExpr();
}

std::shared_ptr<Expr> Parser::parsePrimaryExpr() {
  std::shared_ptr<Expr> operand = parseOperand();

  switch (currentToken().type) {
  case LPAREN: {
    std::vector<std::shared_ptr<Expr>> args = parseArgs();
    return std::make_shared<CallExpr>(operand, args);
  }
  default:
    // it should be a SEMICOLON
    return operand;
  }
}

std::vector<std::shared_ptr<Expr>> Parser::parseArgs() {
  // we know current token is '('
  consume(LPAREN, "Expected '(' for function arguments");

  std::vector<std::shared_ptr<Expr>> args = {};
  while (!check(RPAREN) && !check(T_EOF)) {
    args.push_back(parseExpr());
    if (check(COMMA)) {
      advance();
    }
  }

  consume(RPAREN, "Expected ')' after function arguments");
  return args;
}

std::shared_ptr<Expr> Parser::parseOperand() {
  switch (currentToken().type) {
  case LIT_TRUE:
  case LIT_FALSE:
  case LIT_INT:
  case LIT_FLOAT:
  case LIT_STRING:
  case LIT_NIL:
    return std::make_shared<LiteralExpr>(advance());
  case LPAREN: {
    advance();
    std::shared_ptr<Expr> group = parseExpr();
    consume(RPAREN, "Expected ')' after expression");
    return group;
  }
  case IDENTIFIER: {
    Token ident = advance();
    return std::make_shared<IdentExpr>(ident, ident.value);
  }
  }

  reportError("Expected expression");
  // TODO: change when parsing statements for an exception
  exit(1);
}