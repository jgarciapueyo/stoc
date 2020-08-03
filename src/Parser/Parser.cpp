//===- src/Parser/Parser.cpp - Implementation of Parser class --------*- C++ -*-===//
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

// MAIN PARSING METHODS

//------- Declarations -------

std::shared_ptr<Decl> Parser::parseDecl() {
  try {
    switch (currentToken().tokenType) {
    case VAR:
    case CONST:
      return parseVarConstDecl();
    case FUNC:
      return parseFuncDecl();
    default:
      reportError("Expected declaration");
      return nullptr;
    }
  } catch (Parser::ParsingError &parseError) {
    synchronize(); // set parser in good state
    // return nullptr because it does not matter what it is returned since there has been an error
    // and the AST will not be traversed
    return nullptr;
  }
}

std::shared_ptr<Decl> Parser::parseVarConstDecl() {
  // we know current token is VAR or CONST
  Token varConstKeyword = advance();
  Token type = parseType();
  Token identifier =
      consume(IDENTIFIER, "Expected identifier after '" + to_string(varConstKeyword.tokenType) +
                              "' in variable declaration");
  consume(ASSIGN, "Expected '=' after identifier in variable declaration");
  std::shared_ptr<Expr> value = parseExpr();
  consume(SEMICOLON, "Expected ';' after variable declaration");

  if (varConstKeyword.tokenType == VAR) {
    return std::make_shared<VarDecl>(varConstKeyword, type, identifier, value);
  } else {
    return std::make_shared<ConstDecl>(varConstKeyword, type, identifier, value);
  }
}

std::shared_ptr<Decl> Parser::parseFuncDecl() {
  // we know current token is FUNC
  Token funcKeyword = advance();
  Token name = consume(IDENTIFIER, "Expected identifier after 'func' in function declaration");
  std::vector<std::shared_ptr<ParamDecl>> params = parseParameters();

  if (!check(LBRACE)) {
    Token returnType = parseReturnType();
    std::shared_ptr<BlockStmt> body = parseBlockStmt();
    return std::make_shared<FuncDecl>(funcKeyword, name, params, returnType, body);
  } else {
    std::shared_ptr<BlockStmt> body = parseBlockStmt();
    return std::make_shared<FuncDecl>(funcKeyword, name, params, body);
  }
}

//------- Statements -------

std::shared_ptr<Stmt> Parser::parseStmt() {
  try {
    switch (currentToken().tokenType) {
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
  } catch (Parser::ParsingError &parseError) {
    synchronize(); // set parser in good state
    // return nullptr because it does not matter what it is returned since there has been an error
    // and the AST will not be traversed
    return nullptr;
  }
}

std::shared_ptr<Stmt> Parser::parseSimpleStmt(bool semicolonExp) {
  switch (currentToken().tokenType) {
  case CONST:
  case VAR: {
    std::shared_ptr<Decl> decl = parseDecl();
    return std::make_shared<DeclarationStmt>(decl);
  }
  default: {
    std::shared_ptr<Expr> lhsExpr = parseExpr();

    if (check(ASSIGN)) { // check if it is an assignment
      Token equalToken = advance();
      std::shared_ptr<Expr> rhsExpr = parseExpr();
      if (semicolonExp) {
        consume(SEMICOLON, "Expected ';' after expression");
      }
      return std::make_shared<AssignmentStmt>(lhsExpr, rhsExpr, equalToken);
    } else {
      if (semicolonExp) {
        consume(SEMICOLON, "Expected ';' after expression");
      }
      return std::make_shared<ExpressionStmt>(lhsExpr);
    }
  }
  }
}

std::shared_ptr<BlockStmt> Parser::parseBlockStmt() {
  std::vector<std::shared_ptr<Stmt>> stmts = {};

  // we know current token is '{'
  Token lbrace = advance();
  while (!check(RBRACE) && !isAtEnd()) {
    stmts.push_back(parseStmt());
  }

  Token rbrace = consume(RBRACE, "Expected '}' after block stmt");
  return std::make_shared<BlockStmt>(lbrace, stmts, rbrace);
}

std::shared_ptr<Stmt> Parser::parseIfStmt() {
  // we know current token is IF
  Token ifKeyword = advance();
  std::shared_ptr<Expr> cond = parseExpr();
  std::shared_ptr<BlockStmt> thenBranch = parseBlockStmt();

  std::shared_ptr<Stmt> elseBranch = nullptr;
  if (match(ELSE)) {
    switch (currentToken().tokenType) {
    case IF:
      elseBranch = parseIfStmt();
      break;
    case LBRACE:
      elseBranch = parseBlockStmt();
      break;
    default:
      reportError("Expected 'if' statement or block after 'else'");
    }
  }

  if (elseBranch == nullptr) {
    return std::make_shared<IfStmt>(ifKeyword, cond, thenBranch);
  } else {
    return std::make_shared<IfStmt>(ifKeyword, cond, thenBranch, elseBranch);
  }
}

std::shared_ptr<Stmt> Parser::parseForStmt() {
  // we know current token is FOR
  Token forKeyword = advance();

  std::shared_ptr<Stmt> init = nullptr;
  if (!check(SEMICOLON)) {
    // there is initialization
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

  std::shared_ptr<BlockStmt> body = parseBlockStmt();
  return std::make_shared<ForStmt>(forKeyword, init, cond, post, body);
}

std::shared_ptr<Stmt> Parser::parseWhileStmt() {
  // we know current token is WHILE
  Token whileKeyword = advance();
  std::shared_ptr<Expr> cond = parseExpr();
  std::shared_ptr<BlockStmt> body = parseBlockStmt();

  return std::make_shared<WhileStmt>(whileKeyword, cond, body);
}

std::shared_ptr<Stmt> Parser::parseReturnStmt() {
  // we know curret token is RETURN
  Token returnKeyword = advance();

  if (!check(SEMICOLON)) {
    std::shared_ptr<Expr> value = parseExpr();
    consume(SEMICOLON, "Expected ';' after return statement");
    return std::make_shared<ReturnStmt>(returnKeyword, value);
  } else {
    consume(SEMICOLON, "Expected ';' after return statement");
    return std::make_shared<ReturnStmt>(returnKeyword, nullptr);
  }
}

//------- Expressions -------

std::shared_ptr<Expr> Parser::parseExpr() { return parseBinaryExpr(PREC_LOWEST + 1); }

std::shared_ptr<Expr> Parser::parseBinaryExpr(int prec) {
  std::shared_ptr<Expr> e = parseUnaryExpr();

  // parse binary expression while the precedence of \op is higher than precedence of \prec
  // (e.g. if prec = precedence(ADD), it will parse multiplications(STAR), divisions(SLASH)
  // and unary operations. Also additions(ADD) and substractions(SUB) [left-associativity] while
  // creating the AST)
  Token op = currentToken();
  while (tokenPrec(op.tokenType) >= prec) {
    advance();
    std::shared_ptr<Expr> rhs = parseBinaryExpr(tokenPrec(op.tokenType) + 1);
    e = std::make_shared<BinaryExpr>(e, rhs, op);
    op = this->file->getTokens()[current];
  }

  return e;
}

std::shared_ptr<Expr> Parser::parseUnaryExpr() {
  switch (currentToken().tokenType) {
  case ADD: // we allow ADD(+) as unary operator
  case SUB:
  case NOT: {
    Token t = advance();
    std::shared_ptr<Expr> rhs = parseBinaryExpr(PREC_UNARY);
    return std::make_shared<UnaryExpr>(rhs, t);
  }
  default:
    return parsePrimaryExpr();
  }


}

std::shared_ptr<Expr> Parser::parsePrimaryExpr() {
  std::shared_ptr<Expr> operand = parseOperand();

  switch (currentToken().tokenType) {
  case LPAREN: { // it is a function call
    std::vector<std::shared_ptr<Expr>> args = parseArgs();
    return std::make_shared<CallExpr>(operand, args);
  }
  default:
    return operand;
  }
}

// HELPER METHODS

Token Parser::parseType() {
  switch (currentToken().tokenType) {
  case BOOL:
  case INT:
  case FLOAT:
  case STRING:
    return advance();
  default:
    reportError("Expected type: " + to_string(currentToken().tokenType) + " is not a type");
    return Token();
  }
}

std::vector<std::shared_ptr<ParamDecl>> Parser::parseParameters() {
  // we know current token is '('
  consume(LPAREN, "Expected '(' before parameter definition");
  std::vector<std::shared_ptr<ParamDecl>> params = {};

  while (!check(RPAREN) && !check(T_EOF)) {
    Token keyword = consume(VAR, "Expected VAR as variable definition in parameters");
    Token type = parseType();
    Token ident = consume(IDENTIFIER, "Expected IDENTIFIER in variable definition in parameters");
    params.push_back(std::make_shared<ParamDecl>(keyword, type, ident));
    if (check(COMMA)) {
      advance();
    }
  }

  consume(RPAREN, "Expected ')' after parameter definition");
  return params;
}

Token Parser::parseReturnType() { return parseType(); }

std::vector<std::shared_ptr<Expr>> Parser::parseArgs() {
  // we know current token is '('
  consume(LPAREN, "Expected '(' before function arguments");
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
  switch (currentToken().tokenType) {
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
    return std::make_shared<IdentExpr>(ident);
  }
  default:
    reportError("Expected expression");
    return nullptr;
  }
}

void Parser::reportError(std::string error_msg) {
  std::cerr << "<" << this->file->getFilename() << ":l." << currentToken().line << ":c"
            << currentToken().column << "> Parsing error: " << error_msg << std::endl;

  this->file->setErrorInParsing(true);
  throw Parser::ParsingError();
}

void Parser::synchronize() {
  bool loop = true;
  while (!isAtEnd() && loop) {
    switch (currentToken().tokenType) {
    // If the start of a new declaration is found
    case VAR:
    case CONST:
    case FUNC:
    // If the start of a new statement is found
    case IF:
    case FOR:
    case WHILE:
    case RETURN:
    case LBRACE:
      loop = false;
      break;
    // If the end of a statement is found
    case SEMICOLON:
      advance(); // to start new statement or declaration
      loop = false;
      break;
    default:
      advance();
    }
  }
}

bool Parser::isAtEnd() {
  return (currentToken().tokenType == T_EOF) || (this->current >= this->file->getTokens().size());
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
    return currentToken().tokenType == type;
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
    return Token();
  }
}