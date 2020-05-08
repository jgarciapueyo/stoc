//===- stoc/SrcFile.h - Defintion of the SrcFile class --------*- C++ -*-===//
//
//===--------------------------------------------------------------------===//
//
// This file defines the SrcFile class
//
//===--------------------------------------------------------------------===//

#ifndef STOC_SRCFILE_H
#define STOC_SRCFILE_H

#include "stoc/AST/BasicNode.h"
#include "stoc/Scanner/Token.h"

#include <memory>
#include <string>
#include <vector>

/// Representation of a Stoc source file
class SrcFile {
public:

  std::string path;
  std::string directory;
  std::string filename;

  // Fields for storing raw source data
  std::string data; /// textual information of the source file
  int length; /// length of the source file

  // Fields for storing data after the scanning phase
  std::vector<Token> tokens; /// list of tokens of the source file
  bool errorInScanning; /// represents if an error has occurred during the scanning phase
                        /// (e.g a character was not recognized, quotes (") missing, ...)

  // Fields for storing data after the parsing phase
  std::vector<std::shared_ptr<BasicNode>> ast; /// Abstract Syntax Tree representation
  bool errorInParsing; /// represents if an error has occurred during the parsing phase
                       /// (e.g. a malformed expression, closing parenthesis missing, ...)

public:
  /// Constructor
  /// \field path - path where the source file is stored
  SrcFile(std::string &path);

  const std::string &getPath() const;
  const std::string &getDirectory() const;
  const std::string &getFilename() const;
  const std::string &getData() const;
  int getLength() const;

  const std::vector<Token> &getTokens() const;
  void setTokens(const std::vector<Token> &t);
  bool isErrorInScanning() const;
  void setErrorInScanning(bool error);
  const std::vector<std::shared_ptr<BasicNode>> &getAst() const;
  void setAst(const std::vector<std::shared_ptr<BasicNode>> &ast_nodes);
  bool isErrorInParsing() const;
  void setErrorInParsing(bool error);
};

#endif // STOC_SRCFILE_H
