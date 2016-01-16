// Copyright (2015) Gustav

#ifndef CORE_CONFIGURATION_H_
#define CORE_CONFIGURATION_H_

#include <string>

#include "finans/core/finans-proto.h"

bool LoadConfiguration(finans::DeviceConfigutation* device);
void InstallConfiguration(const std::string& finans_path, bool create_if_missing);

#endif  // CORE_CONFIGURATION_H_
