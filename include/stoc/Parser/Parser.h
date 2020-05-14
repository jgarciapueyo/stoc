//===- stoc/Parser/Parser.h - Defintion of Parser class --------*- C++ -*-===//
//
//===---------------------------------------------------------------------===//
//
// This file defines the Parser class.
// Parsing is the second phase of a compiler and transforms the list of
// tokens into a abstract syntax tree (AST)
//
//===--------------------------------------------------------------------===//

#ifndef STOC_PARSER_H
#define STOC_PARSER_H

#include "stoc/AST/BasicNode.h"
#include "stoc/AST/Decl.h"
#include "stoc/AST/Expr.h"
#include "stoc/AST/Stmt.h"
#include "stoc/Scanner/Token.h"
#include "stoc/SrcFile/SrcFile.h"

#include <memory>
#include <vector>

/// A Parser takes a list of tokens and parses it into an AST
class Parser {
private:
  std::shared_ptr<SrcFile> file;               /// stoc source file and list of tokens
  std::vector<std::shared_ptr<BasicNode>> ast; /// abstract syntax tree

  // State of the parser

  // The parser is a recursive descendent parser similar to a Pratts Parser (uses precedence
  // numbers)
  int current; /// current token being analyzed

  // Helper methods

  /// prints the error \error_msg
  void reportError(std::string error_msg);

  /// true if \current token is T_EOF (end of file) or \current is equal to length of list of tokens
  bool isAtEnd();

  /// returns the token at \current position in list of tokens
  Token currentToken();

  /// returns the token at \current-1 position in list of tokens
  ///   If \current == 0 returns first token
  Token previousToken();

  /// true if \current token type is equal to \type, else false
  ///   If isAtEnd() returns false
  bool check(TokenType type);

  /// returns \current token and increments \current
  ///   If isAtEnd() returns \current token
  Token advance();

  /// true if \current token type is equal to \type and in that case increments \current, else false
  bool match(TokenType type);

  /// returns \current token if its type is equal to \type and increments \current
  ///    if its type is not equal to \type, it reports an \error_msg
  Token consume(TokenType type, std::string error_msg);

  Token parseType();

  std::vector<std::shared_ptr<ParamDecl>> parseParameters();

  Token parseReturnType();

  std::vector<std::shared_ptr<Expr>> parseArgs();

  std::shared_ptr<Expr> parseOperand();

  // Main parsing methods

  std::shared_ptr<Stmt> parseStmt();

  std::shared_ptr<Stmt> parseSimpleStmt(bool semicolonExp);

  std::shared_ptr<Stmt> parseBlockStmt();

  std::shared_ptr<Stmt> parseIfStmt();

  std::shared_ptr<Stmt> parseForStmt();

  std::shared_ptr<Stmt> parseWhileStmt();

  std::shared_ptr<Stmt> parseReturnStmt();

  std::shared_ptr<Decl> parseDecl();

  std::shared_ptr<Decl> parseVarConstDecl();

  std::shared_ptr<Decl> parseFuncDecl();

  /// parses any type of expression
  std::shared_ptr<Expr> parseExpr();

  /// parses a binary expression with higher precedence than \prec
  std::shared_ptr<Expr> parseBinaryExpr(int prec);

  /// parses a unary expression
  std::shared_ptr<Expr> parseUnaryExpr();

  /// parses a primary expression
  std::shared_ptr<Expr> parsePrimaryExpr();

public:
  Parser(const std::shared_ptr<SrcFile> &file);

  /// main method: parses the list of tokens (in \file) and transforms it into an AST
  ///  (the AST is stored in SrcFile \file)
  void parse();
};

#endif // STOC_PARSER_H
