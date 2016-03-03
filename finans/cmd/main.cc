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
    parser.AddOption("name",       longNamneArg); //"The long name, ie. 'American Dollar'"
    parser.AddOption("short-name", shortNameArg); //"The short name ie. USD"
    parser.AddOption("before",     beforeArg   ); //"The string before a value, ie. the $ in $99"
    parser.AddOption("after",      afterArg    ); //"The string after a value, ie. the kr in 45 kr"
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
  std::string longNamneArg;
  std::string shortNameArg;
  std::string currencyArg;

  void AddParser(argparse::Parser& parser) override {
    parser.set_description("Add a account to finans");
    parser.AddOption("name",      longNamneArg); // "The name, ie. 'My card'"
    parser.AddOption("short-name",shortNameArg); // "The short name ie. Visa"
    parser.AddOption("currency",  currencyArg ); // "The default currency for this account"
  }

  void ParseCompleted() override {
    try {
      auto finans = Finans::CreateNew();
      auto currency = finans->GetCurrencyByName(currencyArg);
      if (currency == -1) throw "Unknown currency";
      finans->AddAccount(longNamneArg, shortNameArg, currency);
      finans->Save();
      std::cout << "Added " << shortNameArg << ".\n";
    }
    catch (...)
    {
      ExceptionHandler();
    }
  }
};

class cmd_addcompany : public argparse::SubParser {
  std::string nameArg;
  std::string currencyArg;

  void AddParser(argparse::Parser& parser) override {
    parser.set_description("Add a company to finans");
    parser.AddOption("name", nameArg); // "The name, ie. 'Acme'"
    parser.AddOption("currency", currencyArg); // "The default currency this company works in"
  }

  void ParseCompleted() override {
    try {
      auto finans = Finans::CreateNew();
      auto currency = finans->GetCurrencyByName(currencyArg);
      if (currency == -1) throw "Unknown currency";
      finans->AddCompany(nameArg, currency);
      finans->Save();
      std::cout << "Added " << nameArg << ".\n";
    }
    catch (...)
    {
      ExceptionHandler();
    }
  }
};

class cmd_addcategory : public argparse::SubParser {
  std::string nameArg;

  void AddParser(argparse::Parser& parser) override {
    parser.set_description("Add a category/envelope to finans");
    parser.AddOption("name", nameArg); // "The name, ie. 'Savings'"
  }

  void ParseCompleted() override {
    try {
      auto finans = Finans::CreateNew();
      finans->AddCategory(nameArg);
      finans->Save();
      std::cout << "Added " << nameArg << ".\n";
    }
    catch (...)
    {
      ExceptionHandler();
    }
  }
};

class cmd_install : public argparse::SubParser {
  std::string folderArg;
  bool dontCreateSwitch;

public:
  cmd_install() : dontCreateSwitch(true) { }

  void AddParser(argparse::Parser& parser) override {
    parser.set_description("Install finans to your system");
    parser.AddOption("folder", folderArg); // "Where to install"
    parser.StoreConst("dont-create", dontCreateSwitch, false);
  }

  void ParseCompleted() override {
    try {
      const std::string folder = folderArg;
      const bool dontCreate = dontCreateSwitch;

      Finans::Install(folder, !dontCreate);
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
