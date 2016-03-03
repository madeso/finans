#include <iostream>

#include "finans/core/finans.h"

#include "finans/core/commandline.h"

const std::string USAGE_AND_HELP =
"Usage: fin CMD ARGUMENTS\n"
"Where ARGUMENTS depends on CMD\n"
"and CMD is one of:\n"
"* help\n"
"* install\n"
"* stat\n"
;

int ExceptionHandler() {
  try {
    throw;
  }
  catch (const std::string& error) {
    std::cerr << "error: " << error << "\n";
    return -13;
  }
  catch (const char* error) {
    std::cerr << "error: " << error << "\n";
    return -14;
  }
  catch (...) {
    std::cerr << "Unknown error.\n";
    return -42;
  }
}

//////////////////////////////////////////////////////////////////////////

class cmd_status : public argparse::SubParser {
  void AddParser(argparse::Parser& parser) override {
    parser.set_description("Give the status of your finances");
  }
  void ParseCompleted() override {
    try {
      auto finans = Finans::CreateNew();
      std::cout << "Number of accounts: " << finans->NumberOfAccounts() << "\n";
      std::cout << "Number of companies: " << finans->NumberOfCompanies() << "\n";
      std::cout << "Number of currencies: " << finans->NumberOfCurrencies() << "\n";
      std::cout << "Number of categories: " << finans->NumberOfCategories() << "\n";
    }
    catch (...)
    {
      ExceptionHandler();
    }
  }
};

class cmd_addcurrecy : public argparse::SubParser {
  std::string longNamneArg;
  std::string shortNameArg;
  std::string beforeArg;
  std::string afterArg;

  void AddParser(argparse::Parser& parser) override {
    parser.set_description("Add a currency to finans");
    parser.AddOption("name",       longNamneArg).help("The long name, ie. 'American Dollar'");
    parser.AddOption("short-name", shortNameArg).help("The short name ie. USD");
    parser.AddOption("before",     beforeArg   ).help("The string before a value, ie. the $ in $99");
    parser.AddOption("after",      afterArg    ).help("The string after a value, ie. the kr in 45 kr");
  }

  void ParseCompleted() override {
    try {
      auto finans = Finans::CreateNew();
      finans->AddCurency(longNamneArg, shortNameArg, beforeArg, afterArg);
      finans->Save();
      std::cout << "Added " << shortNameArg << ".\n";
    }
    catch (...)
    {
      ExceptionHandler();
    }
  }
};

class cmd_addaccount : public argparse::SubParser {
  std::string long_name_;
  std::string short_name_;
  std::string currency_name_;

  void AddParser(argparse::Parser& parser) override {
    parser.set_description("Add a account to finans");
    parser.AddOption("name",      long_name_).help( "The name, ie. 'My card'");
    parser.AddOption("short-name",short_name_).help( "The short name ie. Visa");
    parser.AddOption("currency",  currency_name_ ).help( "The default currency for this account");
  }

  void ParseCompleted() override {
    try {
      auto finans = Finans::CreateNew();
      auto currency = finans->GetCurrencyByName(currency_name_);
      if (currency == -1) throw "Unknown currency";
      finans->AddAccount(long_name_, short_name_, currency);
      finans->Save();
      std::cout << "Added " << short_name_ << ".\n";
    }
    catch (...)
    {
      ExceptionHandler();
    }
  }
};

class cmd_addcompany : public argparse::SubParser {
  std::string company_name_;
  std::string currency_name_;

  void AddParser(argparse::Parser& parser) override {
    parser.set_description("Add a company to finans");
    parser.AddOption("name", company_name_).help( "The name, ie. 'Acme'");
    parser.AddOption("currency", currency_name_).help( "The default currency this company works in");
  }

  void ParseCompleted() override {
    try {
      auto finans = Finans::CreateNew();
      auto currency = finans->GetCurrencyByName(currency_name_);
      if (currency == -1) throw "Unknown currency";
      finans->AddCompany(company_name_, currency);
      finans->Save();
      std::cout << "Added " << company_name_ << ".\n";
    }
    catch (...)
    {
      ExceptionHandler();
    }
  }
};

class cmd_addcategory : public argparse::SubParser {
  std::string category_name_;

  void AddParser(argparse::Parser& parser) override {
    parser.set_description("Add a category/envelope to finans");
    parser.AddOption("name", category_name_).help( "The name, ie. 'Savings'");
  }

  void ParseCompleted() override {
    try {
      auto finans = Finans::CreateNew();
      finans->AddCategory(category_name_);
      finans->Save();
      std::cout << "Added " << category_name_ << ".\n";
    }
    catch (...)
    {
      ExceptionHandler();
    }
  }
};

class cmd_install : public argparse::SubParser {
  std::string folder_;
  bool dont_create_;

public:
  cmd_install() : dont_create_(true) { }

  void AddParser(argparse::Parser& parser) override {
    parser.set_description("Install finans to your system");
    parser.AddOption("folder", folder_).help( "Where to install");
    parser.StoreConst("dont-create", dont_create_, false);
  }

  void ParseCompleted() override {
    try {
      Finans::Install(folder_, !dont_create_);
      std::cout << "Install complete.\n";
    }
    catch (...)
    {
      ExceptionHandler();
    }
  }
};

//////////////////////////////////////////////////////////////////////////

std::vector<std::string> Collect(int argc, char** argv) {
  std::vector<std::string> ret;
  for (int i = 0; i < argc; ++i) {
    ret.push_back(argv[i]);
  }
  return ret;
}

int main(int argc, char** argv) {
  argparse::Parser parser("Finans command line client");

  
  cmd_status status;
  parser.AddSubParser("status", &status);
  cmd_install install;
  parser.AddSubParser("install", &install);
  cmd_addcurrecy addcurr;
  parser.AddSubParser("addcurrency", &addcurr);
  cmd_addaccount addacc;
  parser.AddSubParser("addaccount", &addacc);
  cmd_addcompany addcom;
  parser.AddSubParser("addcompany", &addcom);
  cmd_addcategory addcat;
  parser.AddSubParser("addcategory", &addcat);

  auto ret = parser.ParseArgs(argparse::Arguments(argc, argv));
  if (ret == argparse::Parser::ParseFailed) return -1;
  else return 0;
}
