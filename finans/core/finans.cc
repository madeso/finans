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
  if( false == LoadConfiguration(&device) ) throw "Unable to load configuration, install required";

  const auto target = EndWithSlash(device.finans_path()) + DEFAULT_NAME;
  if (FileExist(target) == false) throw "Missing " + DEFAULT_NAME + ", create required";
  std::shared_ptr<Finans> f(new Finans(target));
  f->Load();
  return f;
}

void Finans::CreateDefault(const std::string& src) {
  const auto target = EndWithSlash(src) + DEFAULT_NAME;
  if (FileExist(target)) return;
  Finans f(target);
  f.Save();
}

void Finans::Install(const std::string& path, bool create_if_missing) {
  InstallConfiguration(path, create_if_missing);
}

Finans::Finans(const std::string& path) : path_(path), finans_(new finans::Finans()) {
}

Finans::~Finans() {
}

//////////////////////////////////////////////////////////////////////////

void Finans::Load() {
  LoadProtoJson(finans_.get(), path_);
}

void Finans::Save() {
  SaveProtoJson(*finans_.get(), path_);
}

//////////////////////////////////////////////////////////////////////////

int Finans::NumberOfAccounts() {
  return finans_->accounts_size();
}

//////////////////////////////////////////////////////////////////////////

int Finans::NumberOfCurrencies() {
  return finans_->currencies_size();
}

void Finans::AddCurency(const std::string& full_name, const std::string& short_name, const std::string before, const std::string& after) {
  auto* cur = finans_->add_currencies();
  cur->set_full_name(full_name);
  cur->set_short_name(short_name);
  cur->set_value_before(before);
  cur->set_value_after(after);
}
