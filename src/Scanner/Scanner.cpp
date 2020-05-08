//===- stoc/Scanner/Scanner.cpp - Implementation of Scanner class --------*- C++ -*-===//
//
//===-------------------------------------------------------------------------------===//
//
// This file implements the Scanner class.
//
//===-------------------------------------------------------------------------------===//


#include "stoc/Scanner/Scanner.h"

#include <iostream>
#include <utility>

Scanner::Scanner(std::shared_ptr<SrcFile> file) : file(std::move(file)), tokens({}) {
  this->start = 0;
  this->current = 0;
  this->line = 1;
}

void Scanner::scan() {
  while (!isAtEnd()) {
    scanNextToken();
  }

  updateStart(); // needed to correctly add T_EOF
  tokens.push_back(makeToken(T_EOF));
  this->file->setTokens(this->tokens);
}

bool Scanner::isDigit(char c) { return '0' <= c && c <= '9'; }

bool Scanner::isAlpha(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (c == '_');
}

bool Scanner::isAlphaNum(char c) { return isDigit(c) || isAlpha(c); }

void Scanner::reportError(std::string msg) {
  std::cerr << "<" << this->file->getFilename() << ":l." << this->line << ">"
            << " error: " << this->file->getData().at(this->start) << " " << msg << std::endl;

  this->file->setErrorInScanning(true);
}

void Scanner::updateStart() { this->start = this->current; }

bool Scanner::isAtEnd() const { return this->current >= this->file->getLength(); }

char Scanner::peek() const {
  if (isAtEnd()) {
    return '\0';
  } else {
    return this->file->getData().at(current);
  }
}

char Scanner::peekNext() const {
  if (this->current >= this->file->getLength() - 1) {
    return '\0';
  } else {
    return this->file->getData().at(current + 1);
  }
}

char Scanner::advance() {
  if (isAtEnd()) {
    return '\0';
  } else {
    this->current++;
    return this->file->getData().at(current - 1);
  }
}

TokenType Scanner::tokenType() {
  std::string identifier = this->file->getData().substr(start, current - start);
  if (identifier == "var") {
    return VAR;
  } else if (identifier == "const") {
    return CONST;
  } else if (identifier == "if") {
    return IF;
  } else if (identifier == "else") {
    return ELSE;
  } else if (identifier == "for") {
    return FOR;
  } else if (identifier == "while") {
    return WHILE;
  } else if (identifier == "func") {
    return FUNC;
  } else if (identifier == "return") {
    return RETURN;
  } else if (identifier == "bool") {
    return BOOL;
  } else if (identifier == "int") {
    return INT;
  } else if (identifier == "float") {
    return FLOAT;
  } else if (identifier == "string") {
    return STRING;
  } else if (identifier == "true") {
    return LIT_TRUE;
  } else if (identifier == "false") {
    return LIT_FALSE;
  } else if (identifier == "nil") {
    return LIT_NIL;
  } else {
    return IDENTIFIER;
  }
}

Token Scanner::makeToken(TokenType type) const {
  return Token(type, this->start, this->line, this->file->getData().substr(start, current - start));
}

Token Scanner::makeErrorToken(std::string errorMsg = "") {
  reportError(errorMsg);
  return Token(ILLEGAL, this->start, this->line, std::move(errorMsg));
}

void Scanner::skipWhiteSpaces() {
  while (true) {
    switch (peek()) {
    case ' ':
    case '\r':
    case '\t':
      advance();
      break;
    case '\n':
      this->line++; // keep track of the line where \current is
      advance();
      break;
    default:
      return;
    }
  }
}

void Scanner::scanNumber() {
  // TODO: support numbers as 1_000_000 with underscore _
  // integer part
  while (isDigit(peek())) {
    advance();
  }

  if (peek() == '.') {
    // fractional part
    advance();
    while (isDigit(peek())) {
      advance();
    }

    tokens.push_back(makeToken(LIT_FLOAT));
  } else {
    tokens.push_back(makeToken(LIT_INT));
  }
}

void Scanner::scanIdentifier() {
  // we know first character is alpha from scanNextToken
  while (isAlphaNum(peek())) {
    advance();
  }

  TokenType type = tokenType(); // check if the string is a keyword
  tokens.push_back(makeToken(type));
}

void Scanner::scanLineComment() {
  // The first characters '//' defining the comment have been treated
  while (peek() != '\n' && !isAtEnd()) {
    advance();
  }

  if (isAtEnd())
    return;
  else {
    this->line++;
    // advance after end of line character
    advance();
  }
}

void Scanner::scanString() {
  // TODO: improve string recognition with escape sequences
  // Multi line strings are supported
  // first quotes defining the string have been treated
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n') {
      this->line++;
    }
    advance();
  }

  if (isAtEnd()) {
    tokens.push_back(makeErrorToken("missing \" character for defining strings"));
  } else {
    tokens.push_back(makeToken(LIT_STRING));
  }
}

void Scanner::scanNextToken() {
  skipWhiteSpaces();
  updateStart();

  char c = peek();
  if (isDigit(c)) {
    scanNumber();
  } else if (isAlpha(c)) { // identifiers must start with alphabet character
    scanIdentifier();
  } else {
    advance(); // make progress
    switch (c) {
    case '+':
      tokens.push_back(makeToken(ADD));
      break;
    case '-':
      tokens.push_back(makeToken(SUB));
      break;
    case '*':
      tokens.push_back(makeToken(STAR));
      break;
    case '/':
      if (peek() == '/') {
        advance();
        scanLineComment();
      } else {
        tokens.push_back(makeToken(SLASH));
      }
      break;
    case '&':
      if (peek() == '&') {
        advance();
        tokens.push_back(makeToken(LAND));
      } else {
        tokens.push_back(makeErrorToken("Operand Bit And not supported yet"));
        advance();
      }
      break;
    case '|':
      if (peek() == '|') {
        advance();
        tokens.push_back(makeToken(LOR));
      } else {
        tokens.push_back(makeErrorToken("Operand Bit Or not supported yet"));
        advance();
      }
      break;
    case '!':
      if (peek() == '=') {
        advance();
        tokens.push_back(makeToken(NOT_EQUAL));
      } else {
        tokens.push_back(makeToken(NOT));
      }
      break;
    case '=':
      if (peek() == '=') {
        advance();
        tokens.push_back(makeToken(EQUAL));
      } else {
        tokens.push_back(makeToken(ASSIGN));
      }
      break;
    case '<':
      if (peek() == '=') {
        advance();
        tokens.push_back(makeToken(LESS_EQUAL));
      } else {
        tokens.push_back(makeToken(LESS));
      }
      break;
    case '>':
      if (peek() == '=') {
        advance();
        tokens.push_back(makeToken(GREATER_EQUAL));
      } else {
        tokens.push_back(makeToken(GREATER));
      }
      break;
    case '(':
      tokens.push_back(makeToken(LPAREN));
      break;
    case ')':
      tokens.push_back(makeToken(RPAREN));
      break;
    case '{':
      tokens.push_back(makeToken(LBRACE));
      break;
    case '}':
      tokens.push_back(makeToken(RBRACE));
      break;
    case ';':
      tokens.push_back(makeToken(SEMICOLON));
      break;
    case '"':
      scanString();
      break;
    default:
      tokens.push_back(makeErrorToken("Unrecognized token"));
      advance(); // if character not recognized it is represented by two characters \xxx\xxx
      break;
    }
  }
}
