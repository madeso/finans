#ifndef CORE_FINANCE_H_
#define CORE_FINANCE_H_

#include <string>
#include <memory>

namespace finans {
  class Finans;
}

class Finans {
public:
  /* Construction */
  static std::shared_ptr<Finans> CreateNew();
  static void CreateDefault(const std::string& src);
  static void Install(const std::string& path, bool create_if_missing);

  ~Finans();

public:
  void Load();
  void Save();

public:
  int NumberOfAccounts() const;
  int GetAccountByName(const std::string& short_name) const;
  void AddAccount(const std::string& long_name, const std::string& short_name, int currency);

public:
  int NumberOfCompanies() const;
  int GetCompanyByName(const std::string& name) const;
  void AddCompany(const std::string& name, int currency);

public:
  int NumberOfCurrencies() const;
  int GetCurrencyByName(const std::string& short_name) const;
  void AddCurency(const std::string& full_name, const std::string& short_name, const std::string before, const std::string& after);

public:
  int NumberOfCategories() const;
  int GetCategoryByName(const std::string& name) const;
  void AddCategory(const std::string& name);

private:
  Finans(const std::string& path);
  std::string path_;
  std::unique_ptr<finans::Finans> finans_;
};

#endif
