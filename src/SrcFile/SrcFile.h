#ifndef STOC_SRCFILE_H
#define STOC_SRCFILE_H

#include <string>

class SrcFile {
public:
  std::string path;
  std::string directory;
  std::string filename;

  std::string data;
  int length;

public:
  SrcFile(std::string &path);

  std::string get_path();
  std::string get_directory();
  std::string get_filename();
  std::string get_data();
  int get_length();
};

#endif // STOC_SRCFILE_H
