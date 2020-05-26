//===- Scanner/Token.cpp - Implementation of Token related constructs --------*- C++ -*-===//
//
//===---------------------------------------------------------------------------------===//
//
// This file implements functions related to TokenType enum and Token class
//
//===--------------------------------------------------------------------------------===//

#include "stoc/Scanner/Token.h"

#include <iomanip>
#include <iostream>
#include <utility>
#include <vector>

std::string typeAsString(TokenType type) {
  static std::vector<std::string> stringRepresentation = {
      "'+'",       "'-'",        "'*'",     "'/'",        "'&&'",     "'||'",      "'!'",
      "'='",       "'=='",       "'!='",    "'<'",        "'>'",      "'<='",      "'>='",
      "'('",       "')'",        "'{'",     "'}'",        "';'",      "','",       "var",
      "const",     "if",         "else",    "for",        "while",    "func",      "return",
      "bool",      "int",        "float",   "string",     "LIT_TRUE", "LIT_FALSE", "LIT_INT",
      "LIT_FLOAT", "LIT_STRING", "LIT_NIL", "IDENTIFIER", "ILLEGAL",  "T_EOF"};

  return stringRepresentation[type];
}

std::ostream &operator<<(std::ostream &os, TokenType type) {
  os << typeAsString(type);
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
    : type(type), begin(begin), line(line), column(column), value(std::move(value)) {}

Token::Token(TokenType type, int begin, int line, int column)
    : type(type), begin(begin), line(line), column(column) {}

Token::Token() = default;

std::string Token::getTypeAsString() const { return typeAsString(this->type); }

std::ostream &operator<<(std::ostream &os, const Token &t) {
  os << std::setw(5) << t.line << " " << std::setw(4) << t.begin << " " << std::setw(10) << t.value
     << " " << t.type;
  return os;
}