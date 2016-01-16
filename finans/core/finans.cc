#include "finans/core/finans.h"

#include "finans/core/finans-proto.h"

#include "finans/core/os.h"
#include "finans/core/configuration.h"
#include "finans/core/os.h"
#include "finans/core/file.h"
#include "finans/core/proto.h"
#include "finans/core/stringutils.h"

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

int Finans::GetAccountByName(const std::string& short_name) {
  const auto name = ToLower(short_name);
  for (int i = 0; i < finans_->accounts_size(); ++i) {
    if (ToLower(finans_->accounts(i).short_name()) == name) return i;
  }

  return -1;
}

void Finans::AddAccount(const std::string& long_name, const std::string& short_name, int currency) {
  auto sn = Trim(short_name);
  if (GetAccountByName(sn) != -1) throw "Account already added";
  auto* a = finans_->add_accounts();
  a->set_long_name(Trim(long_name));
  a->set_short_name(sn);
  a->set_prefered_currency(currency);
}

//////////////////////////////////////////////////////////////////////////

int Finans::NumberOfCurrencies() {
  return finans_->currencies_size();
}

int Finans::GetCurrencyByName(const std::string& short_name) const {
  const auto name = ToLower(short_name);
  for (int i = 0; i < finans_->currencies_size(); ++i) {
    if ( ToLower(finans_->currencies(i).short_name()) == name) return i;
  }

  return -1;
}

void Finans::AddCurency(const std::string& full_name, const std::string& short_name, const std::string before, const std::string& after) {
  const auto sn = Trim(short_name);
  if (GetCurrencyByName(sn) != -1) throw "Currency already added";
  auto* cur = finans_->add_currencies();
  cur->set_full_name(Trim(full_name));
  cur->set_short_name(sn);
  cur->set_value_before(before);
  cur->set_value_after(after);
}
