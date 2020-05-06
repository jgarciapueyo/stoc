#ifndef STOC_SCANNER_H
#define STOC_SCANNER_H

#include <memory>
#include <string>
#include <vector>

#include "../SrcFile/SrcFile.h"
#include "Token.h"

class Scanner {
private:
  std::shared_ptr<SrcFile> file;
  std::vector<Token> tokens;

  int start;
  int current;
  int line;

  void updateStart();

  static bool isDigit(char c);

  static bool isAlpha(char c);

  static bool isAlphaNum(char c);

  bool isAtEnd() const;

  char peek() const;

  char peekNext() const;

  char advance();

  TokenType tokenType();

  void skipWhiteSpaces();

  Token makeToken(TokenType type) const;

  Token makeErrorToken(std::string errorMsg) const;

  void scanNumber();

  void scanIdentifier();

  void scanLineComment();

  void scanString();

  void scanNextToken();

public:
  explicit Scanner(std::shared_ptr<SrcFile> file);

  std::vector<Token> scan();
};

#endif // STOC_SCANNER_H
