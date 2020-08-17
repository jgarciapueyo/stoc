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

#include <memory>
#include <vector>

#include "stoc/AST/BasicNode.h"
#include "stoc/AST/Decl.h"
#include "stoc/AST/Expr.h"
#include "stoc/AST/Stmt.h"
#include "stoc/Scanner/Token.h"
#include "stoc/SrcFile/SrcFile.h"

/// A Parser takes a list of tokens and parses it into an AST
class Parser {
private:
  std::shared_ptr<SrcFile> file; /// stoc source file and list of tokens

  /// abstract syntax tree composed of a list of top-level declarations
  std::vector<std::shared_ptr<Decl>> ast;

  // State of the parser

  // The parser is a recursive descendent parser similar to a Pratts Parser (uses precedence
  // numbers)
  int current; /// current token being analyzed

  // Exception if there is an error while parsing
  class ParsingError : public std::exception {};

  // HELPER METHODS

  /// prints the error \error_msg
  void reportError(std::string error_msg);

  /// in case of parsing error, advances tokens until the parser is in a correct position to
  /// continue parsing the file. This is useful to find multiple errors and not just fail on the
  /// first error during parsing.
  void synchronize();

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

  /// parses the type of a declaration
  Token parseType();

  /// parses the parameters of a function
  std::vector<std::shared_ptr<ParamDecl>> parseParameters();

  /// parses the return type of a function
  Token parseReturnType();

  /// parses the arguments of a call expression
  std::vector<std::shared_ptr<Expr>> parseArgs();

  /// parses an operand in an expression
  std::shared_ptr<Expr> parseOperand();

  // MAIN PARSING METHODS

  //------- Declarations -------

  /// parses any type of declaration
  std::shared_ptr<Decl> parseDecl();

  /// parses a variable or constant declaration
  std::shared_ptr<Decl> parseVarConstDecl();

  /// parses a function declaration
  std::shared_ptr<Decl> parseFuncDecl();

  //------- Statements -------

  /// parses any type of statement
  std::shared_ptr<Stmt> parseStmt();

  /// parses a simple statement and a semicolon depending on \semicolonExp
  std::shared_ptr<Stmt> parseSimpleStmt(bool semicolonExp);

  /// parses a block statement
  std::shared_ptr<BlockStmt> parseBlockStmt();

  /// parses an if statement
  std::shared_ptr<Stmt> parseIfStmt();

  /// parses a for statement
  std::shared_ptr<Stmt> parseForStmt();

  /// parses a while statement
  std::shared_ptr<Stmt> parseWhileStmt();

  /// parses a return statement
  std::shared_ptr<Stmt> parseReturnStmt();

  //------- Expressions -------

  /// parses any type of expression
  std::shared_ptr<Expr> parseExpr();

  /// parses a binary expression with higher precedence than \prec
  std::shared_ptr<Expr> parseBinaryExpr(int prec);

  /// parses a unary expression
  std::shared_ptr<Expr> parseUnaryExpr();

  /// parses a primary expression
  std::shared_ptr<Expr> parsePrimaryExpr();

public:
  explicit Parser(const std::shared_ptr<SrcFile> &file);

  /// main method: parses the list of tokens (in \file) and transforms it into an AST
  ///  (the AST is stored in SrcFile \file)
  void parse();
};

#endif // STOC_PARSER_H
