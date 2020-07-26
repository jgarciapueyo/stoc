//===- src/Scanner/Token.cpp - Implementation of Token related constructs -----------*- C++ -*-===//
//
//===------------------------------------------------------------------------------------------===//
//
// This file implements functions related to TokenType enum and Token class
//
//===------------------------------------------------------------------------------------------===//

#include "stoc/Scanner/Token.h"

#include <iomanip>
#include <iostream>
#include <vector>

std::string to_string(TokenType type) {
  std::vector<std::string> TokenTypeAsString = {
      "'+'",       "'-'",        "'*'",     "'/'",        "'&&'",     "'||'",      "'!'",
      "'='",       "'=='",       "'!='",    "'<'",        "'>'",      "'<='",      "'>='",
      "'('",       "')'",        "'{'",     "'}'",        "';'",      "','",       "var",
      "const",     "if",         "else",    "for",        "while",    "func",      "return",
      "bool",      "int",        "float",   "string",     "LIT_TRUE", "LIT_FALSE", "LIT_INT",
      "LIT_FLOAT", "LIT_STRING", "LIT_NIL", "IDENTIFIER", "ILLEGAL",  "T_EOF"};

  return TokenTypeAsString[type];
}

std::ostream &operator<<(std::ostream &os, TokenType type) {
  os << to_string(type);
  return os;
}

int tokenPrec(TokenType type) {
  switch (type) {
  case LOR:
    return PREC_OR;
  case LAND:
    return PREC_AND;
  case EQUAL:
  case NOT_EQUAL:
  case LESS:
  case GREATER:
  case LESS_EQUAL:
  case GREATER_EQUAL:
    return PREC_EQUALITY;
  case ADD:
  case SUB:
    return PREC_TERM;
  case STAR:
  case SLASH:
    return PREC_FACTOR;
  default:
    return PREC_LOWEST; // non-operators
  }
}

Token::Token(TokenType type, int begin, int line, int column, std::string value)
    : tokenType(type), begin(begin), line(line), column(column), value(std::move(value)) {}

Token::Token(TokenType type, int begin, int line, int column)
    : tokenType(type), begin(begin), line(line), column(column) {}

Token::Token() = default;

std::ostream &operator<<(std::ostream &os, const Token &t) {
  os << std::setw(5) << t.line << " " << std::setw(4) << t.column << " " << std::setw(10)
     << t.value << " " << to_string(t.tokenType);
  return os;
}