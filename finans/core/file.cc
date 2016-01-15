// Copyright (2015) Gustav

#include "finans/core/file.h"

#include <fstream>

bool FileExist(const std::string& file) {
  std::ifstream ff(file.c_str());
  return ff.is_open();
}
