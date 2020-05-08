//===- stoc/Scanner/Scanner.h - Defintion of Scanner class --------*- C++ -*-===//
//
//===------------------------------------------------------------------------===//
//
// This file defines the Scanner class.
// Scanning is the first phase of a compiler and transforms the raw source code
// into a list of tokens such as ADD, SUB, IDENTIFIER, ...
//
//===------------------------------------------------------------------------===//

#ifndef STOC_SCANNER_H
#define STOC_SCANNER_H

#include "Token.h"
#include "stoc/SrcFile/SrcFile.h"

#include <memory>
#include <string>
#include <vector>

/// A Scanner takes Stoc source code and tokenize it
class Scanner {
private:
  std::shared_ptr<SrcFile> file; /// stoc source file
  std::vector<Token> tokens; /// list of tokens

  // State of the scanner

  // The scanner works by incrementing the \current counter. When it believes the string from \start
  // to \current could be a token, it creates it and updates \start to \current, and starts again.
  int start; /// start of token
  int current; /// current character being analyzed

  int line; /// line of source code where \current is

  // Helper methods

  /// true if \c is between 0-9
  static bool isDigit(char c);

  /// true if \c is a letter from a-zA-Z or underscore _
  static bool isAlpha(char c);

  /// true if \c is a digit or a alphabet character
  static bool isAlphaNum(char c);

  /// prints the error \error_msg
  void reportError(std::string msg);

  /// updates \start counter to the position of \current counter
  void updateStart();

  /// true if \current is equal to the length of source code
  bool isAtEnd() const;

  /// returns the character at \current position in source code
  ///   If isAtEnd() returns the null character
  char peek() const;

  /// returns the character at \current+1 position in source code
  ///   If isAtEnd() returns the null character
  char peekNext() const;

  /// returns the character at \current position and increments \current.
  ///   If isAtEnd() returns the null character
  char advance();

  /// returns the TokenType of the string from \start to \current in source code
  TokenType tokenType();

  /// constructs a Token of \type with value the string from \start to \current in source code
  Token makeToken(TokenType type) const;

  /// reports the error and creates an error token
  Token makeErrorToken(std::string errorMsg);

  /// advances \current while it is a blank character
  void skipWhiteSpaces();

  // Main scanning methods

  /// scans a literal number token (LIT_INT or LIT_FLOAT)
  void scanNumber();

  /// scans an identifier token
  void scanIdentifier();

  /// scans a line comment (only one line)
  void scanLineComment();

  /// scans a literal string token (LIT_STRING)
  void scanString();

  /// main private method: the source code is scanned through repeated calls to scanNextToken()
  void scanNextToken();

public:
  explicit Scanner(std::shared_ptr<SrcFile> file);

  /// main method: scans the source code and transforms it into a list of tokens
  ///  (the list of tokens is stored in SrcFile file)
  void scan();
};

#endif // STOC_SCANNER_H
