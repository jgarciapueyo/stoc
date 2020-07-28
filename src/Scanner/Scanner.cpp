//===- src/Scanner/Scanner.cpp - Implementation of Scanner class -------------------*- C++ -*-===//
//
//===------------------------------------------------------------------------------------------===//
//
// This file implements the Scanner class.
// Scanning is the first phase of a compiler and transforms the raw source code into a list of
// tokens such as ADD, SUB, IDENTIFIER, ...
//
//===------------------------------------------------------------------------------------------===//

#include "stoc/Scanner/Scanner.h"

#include <iostream>
#include <utility>

Scanner::Scanner(std::shared_ptr<SrcFile> file) : file(std::move(file)), tokens({}) {
  this->start = 0;
  this->current = 0;
  this->line = 1;
  this->column = 1;
  this->lineStart = 1;
  this->columnStart = 1;
}

void Scanner::scan() {
  while (!isAtEnd()) {
    scanNextToken();
  }

  updateStart(); // needed to correctly add T_EOF
  tokens.push_back(makeToken(T_EOF));
  this->file->setTokens(this->tokens);
}

void Scanner::printTokens() {
  if (!tokens.empty()) {
    for (const auto &token : tokens) {
      std::cout << token << std::endl;
    }
  } else {
    std::cout << "No token has been parsed" << std::endl;
  }
}

bool Scanner::isDigit(char c) { return '0' <= c && c <= '9'; }

bool Scanner::isAlpha(char c) {
  return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (c == '_');
}

bool Scanner::isAlphaNum(char c) { return isDigit(c) || isAlpha(c); }

void Scanner::reportError(const std::string &msg) {
  std::cerr << "<" << this->file->getFilename() << ":l." << this->lineStart << ":c."
            << this->columnStart << "> Scanning error: " << msg << std::endl;

  this->file->setErrorInScanning(true);
}

void Scanner::updateStart() {
  this->start = this->current;
  this->lineStart = this->line;
  this->columnStart = this->column;
}

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
    this->column++;
    return this->file->getData().at(current - 1);
  }
}

TokenType Scanner::tokenType() {
  // TODO: (improvement code) define keywords in Token.h so everything related to TokenTypes and its
  //       string representation is together inside a file. For example, if we change func to def
  //       we would have to change it in that file twice and another here. Better if is all in one
  //       file
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
  return Token(type, this->start, this->line, this->columnStart,
               this->file->getData().substr(start, current - start));
}

Token Scanner::makeErrorToken(std::string errorMsg = "") {
  reportError(errorMsg);
  return Token(ILLEGAL, this->start, this->line, this->columnStart, std::move(errorMsg));
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
      this->line++;     // keep track of the line where \current is
      this->column = 0; // keep track of the column where \current is
                        // because advance will increase it
      advance();
      break;
    default:
      return;
    }
  }
}

void Scanner::scanNumber() {
  // TODO: (Improvement) support numbers as 1_000_000 with underscore _
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
    this->column = 0;
    // advance after end of line character
    advance();
  }
}

void Scanner::scanString() {
  // TODO: (Improvement) string recognition with escape sequences
  // Multi line strings are supported
  // first quotes defining the string have been treated
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n') {
      this->line++;
      this->column = 0;
    }
    advance();
  }

  if (isAtEnd()) {
    tokens.push_back(makeErrorToken("missing \" character for defining strings"));
  } else {
    advance(); // advance to process ending quotes
    tokens.push_back(makeToken(LIT_STRING));
  }
}

void Scanner::scanNextToken() {
  skipWhiteSpaces();
  updateStart();

  char c = peek();
  if (isDigit(c)) {
    scanNumber();
  } else if (isAlpha(c)) { // identifiers must start with alphabet character or underscore
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
    case ',':
      tokens.push_back(makeToken(COMMA));
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
