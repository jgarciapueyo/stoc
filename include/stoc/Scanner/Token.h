//===- stoc/Scanner/Token.h - Defintion of Token related constructs --------*- C++ -*-===//
//
//===---------------------------------------------------------------------------------===//
//
// This file defines the TokenType enum and Token class
//
//===--------------------------------------------------------------------------------===//

#ifndef STOC_TOKEN_H
#define STOC_TOKEN_H

#include <string>

/// Representation of the types a token can be
enum TokenType {
  // Operators
  ADD,   // +
  SUB,   // -
  STAR,  // *
  SLASH, // /

  LAND, // && (logical AND)
  LOR,  // || (logical OR)
  NOT,  // !

  ASSIGN,        // =
  EQUAL,         // == (logical EQUAL)
  NOT_EQUAL,     // !=
  LESS,          // <
  GREATER,       // >
  LESS_EQUAL,    // <=
  GREATER_EQUAL, // >=

  // Delimiters
  LPAREN,    // (
  RPAREN,    // )
  LBRACE,    // {
  RBRACE,    // }
  SEMICOLON, // ;

  // Keywords
  VAR,   //
  CONST, //

  IF,     // if
  ELSE,   // else
  FOR,    // for
  WHILE,  // while
  FUNC,   // func
  RETURN, // return

  // Basic types keywords
  BOOL,   // bool
  INT,    // int
  FLOAT,  // float
  STRING, // string

  // Basic type literals
  LIT_TRUE,   // true
  LIT_FALSE,  // false
  LIT_INT,    // 12345
  LIT_FLOAT,  // 123.45
  LIT_STRING, // "a"
  LIT_NIL,    // nil (absence of value)

  IDENTIFIER, //

  // Special tokens
  ILLEGAL, // tokens that are not allowed or erros
  T_EOF    // end of file
};

/// Representation of the precedence of TokenTypes
/// e.g. A FACTOR (*, /) binds tighter than a TERM(+,/) so it has higher precedence
/// Important: all equality and comparison have the same precedence (different from C precedence)
enum Prec {
  PREC_LOWEST,    // 0 (non operators)
  PREC_OR,        // 1 Token: LOR
  PREC_AND,       // 2 Token: LAND
  PREC_EQUALITY,  // 3 Token: EQUAL, NOT_EQUAL, LESS, GREATER, LESS_EQUAL, GREATER_EQUAL
  PREC_TERM,      // 4 Token: ADD, SUB
  PREC_FACTOR,    // 5 Token: STAR, SLASH
  PREC_UNARY,     // 6 Token: NOT, unary SUB, unary ADD
};

/// returns the precedence of \param type (as a binary operator)
int tokenPrec(TokenType type);

std::ostream &operator<<(std::ostream &os, TokenType type);

/// Representation of a token.
/// It contains a type and the string representing its value.
class Token {
public:
  Token(TokenType type, int begin, int line, std::string value);

  Token(TokenType type, int begin, int line);

public:
  TokenType type;

  /// position where the token starts in the raw source code
  int begin;

  /// line where the token is in the raw source code
  int line;

  /// value of the token
  /// (e.g. for ADD is '+', for INT is 'int',
  ///     for an identifier is the string representing it 'identifier)
  std::string value;
};

std::ostream &operator<<(std::ostream &os, const Token t);

#endif // STOC_TOKEN_H
