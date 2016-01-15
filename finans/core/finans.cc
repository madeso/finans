#include "finans/core/finans.h"

#include "finans/core/finans-proto.h"

#include "finans/core/os.h"
#include "finans/core/configuration.h"
#include "finans/core/os.h"
#include "finans/core/file.h"
#include "finans/core/proto.h"

const std::string DEFAULT_NAME = "finans.json";

std::shared_ptr<Finans> Finans::CreateNew() {
  finans::DeviceConfigutation device;
  if( false == LoadConfiguration(&device) ) return nullptr;

  std::shared_ptr<Finans> f(new Finans(""));
  const auto target = EndWithSlash(device.finans_path()) + DEFAULT_NAME;
  if (FileExist(target)) return nullptr;
  f->Load();
  return f;
}

void Finans::CreateDefault(const std::string& src) {
  const auto target = EndWithSlash(src) + DEFAULT_NAME;
  if (FileExist(target)) return;
  Finans f(target);
  f.Save();
}

Finans::Finans(const std::string& path) : path_(path), finans_(new finans::Finans()) {
}

Finans::~Finans() {
}

void Finans::Load() {
  LoadProtoJson(finans_.get(), path_);
}

void Finans::Save() {
  SaveProtoJson(*finans_.get(), path_);
}
