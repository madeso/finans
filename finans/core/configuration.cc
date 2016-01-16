// Copyright (2015) Gustav

#include "finans/core/configuration.h"

#include "finans/core/os.h"
#include "finans/core/proto.h"
#include "finans/core/file.h"
#include "finans/core/finans.h"

std::string DevicePath() {
  return FindUserPath() + "device.json";
}

bool LoadConfiguration(finans::DeviceConfigutation* device) {
  const auto path = DevicePath();

  // Install needed
  if (FileExist(path) == false) return false;
  
  const auto result = LoadProtoJson(device, path);
  
  if (result == "") return true;
  else return false; // error
}

void InstallConfiguration(const std::string& finans_path, bool create_if_missing) {
  const auto path = DevicePath();
  finans::DeviceConfigutation device;
  device.set_finans_path(finans_path);
  if( create_if_missing ) {
    Finans::CreateDefault(finans_path);
  }
  SaveProtoJson(device, path);
}
