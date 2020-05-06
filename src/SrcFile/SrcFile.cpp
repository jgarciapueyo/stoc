#include "SrcFile.h"

#include <fstream>
#include <iostream>
#include <string>

SrcFile::SrcFile(std::string &path) {
  std::ifstream ifs(path, std::ifstream::in);

  if (ifs.is_open()) {
    std::size_t index = path.find_last_of("/\\");
    this->path = path;
    this->directory = path.substr(0, index);
    this->filename = path.substr(index + 1);
    this->data.assign((std::istreambuf_iterator<char>(ifs)),
                      std::istreambuf_iterator<char>());
    this->length = this->data.length();
    ifs.close();
  } else {
    ifs.close();
    throw std::runtime_error("Failed to open source file " + path);
  }
}

std::string SrcFile::get_directory() { return this->directory; }

std::string SrcFile::get_path() { return this->path; }

std::string SrcFile::get_filename() { return this->filename; }

std::string SrcFile::get_data() { return this->data; }

int SrcFile::get_length() { return this->length; }