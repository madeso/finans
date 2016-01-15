// Copyright (2015) Gustav

#include "finans/core/proto.h"

#include <google/protobuf/text_format.h>

#include <streambuf>
#include <cassert>
#include <fstream>  // NOLINT this is how we use fstrean
#include <sstream>  // NOLINT this is how we use sstream

#include "pbjson.hpp"  // NOLINT this is how we use tinyxml2


std::string LoadProtoJson(google::protobuf::Message* message,
                       const std::string& path) {
  std::string err;
  int load_result = pbjson::json2pb_file(path, message, err);
  if (load_result < 0) {
    return err.c_str();
  }

  return "";
}

std::string SaveProtoJson(const google::protobuf::Message& t,
                       const std::string& path) {
  bool write_result = pbjson::pb2json_file(&t, path, true);
  if (write_result == false) {
    return "Unable to write to file";
  }

  return "";
}
