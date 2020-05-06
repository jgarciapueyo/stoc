#include "Token.h"

#include <iomanip>
#include <iostream>
#include <vector>

std::ostream &operator<<(std::ostream &os, TokenType type) {
  static std::vector<std::string> stringRepresentation = {
      "ADD", "SUB", "STAR", "SLASH",
      "LAND", "LOR", "NOT",
      "ASSIGN", "EQUAL", "LESS", "GREATER", "LESS_EQUAL", "GREATER_EQUAL",
      "LPAREN", "RPAREN", "LBRACE", "RBRACE", "SEMICOLON",
      "VAR", "CONST",
      "IF", "ELSE", "FOR", "WHILE", "FUNC", "RETURN",
      "BOOL", "INT", "FLOAT", "STRING",
      "LIT_TRUE", "LIT_FALSE", "LIT_INT", "LIT_FLOAT", "LIT_STRING", "LIT_NIL",
      "IDENTIFIER",
      "ILLEGAL", "T_EOF"
  };

  os << stringRepresentation[type];
  return os;
}

Token::Token(TokenType type, int begin, int line, std::string value)
    : type(type), begin(begin), line(line), value(value) {}

Token::Token(TokenType type, int begin, int line) : type(type), begin(begin), line(line) {}

std::ostream &operator<<(std::ostream &os, const Token t) {
  os << std::setw(5) << t.line << " "
     << std::setw(4) << t.begin << " "
     << std::setw(10) << t.value << " "
     << t.type;
  return os;
}