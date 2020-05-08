//===- stoc/SrcFile.cpp - Implementation of the SrcFile class --------*- C++ -*-===//
//
//===---------------------------------------------------------------------------===//
//
// This file implements the SrcFile class
//
//===---------------------------------------------------------------------------===//

#include "stoc/SrcFile/SrcFile.h"

#include <fstream>
#include <iostream>
#include <string>

SrcFile::SrcFile(std::string &path) {
  std::ifstream ifs(path, std::ifstream::in);

  // if the file really exists and can be read
  if (ifs.is_open()) {
    std::size_t index = path.find_last_of("/\\");
    this->path = path;
    this->directory = path.substr(0, index);
    this->filename = path.substr(index + 1);

    // easy way of reading source file
    this->data.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    this->length = this->data.length();
    ifs.close();

    this->tokens = {};
    this->errorInScanning = false;
    this->ast = {};
    this->errorInParsing = false;
  } else {
    ifs.close();
    throw std::runtime_error("Failed to open source file " + path);
  }
}

const std::string &SrcFile::getPath() const { return path; }
const std::string &SrcFile::getDirectory() const { return directory; }
const std::string &SrcFile::getFilename() const { return filename; }
const std::string &SrcFile::getData() const { return data; }
int SrcFile::getLength() const { return length; }

const std::vector<Token> &SrcFile::getTokens() const { return tokens; }
void SrcFile::setTokens(const std::vector<Token> &t) { this->tokens = t; }
bool SrcFile::isErrorInScanning() const { return errorInScanning; }
void SrcFile::setErrorInScanning(bool error) { this->errorInScanning = error; }
const std::vector<std::shared_ptr<BasicNode>> &SrcFile::getAst() const { return ast; }
void SrcFile::setAst(const std::vector<std::shared_ptr<BasicNode>> &ast_nodes) {
  this->ast = ast_nodes;
}
bool SrcFile::isErrorInParsing() const { return errorInParsing; }
void SrcFile::setErrorInParsing(bool error) { this->errorInParsing = error; }
