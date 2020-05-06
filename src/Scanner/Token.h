#ifndef STOC_TOKEN_H
#define STOC_TOKEN_H

#include <string>

// TODO: enable to print enum type as string
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

std::ostream &operator<<(std::ostream &os, TokenType type);

class Token {
public:
  Token(TokenType type, int begin, int line, std::string value);

  Token(TokenType type, int begin, int line);

public:
  TokenType type;
  int begin;
  int line;
  std::string value;
};

std::ostream &operator<<(std::ostream &os, const Token t);

#endif // STOC_TOKEN_H
