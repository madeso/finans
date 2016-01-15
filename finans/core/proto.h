// Copyright (2015) Gustav

#ifndef CORE_PROTO_H_
#define CORE_PROTO_H_

#include <google/protobuf/message.h>
#include <string>

std::string LoadProtoJson(google::protobuf::Message* t, const std::string& path);
std::string SaveProtoJson(const google::protobuf::Message& t, const std::string& path);

#endif  // CORE_PROTO_H_
