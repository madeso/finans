#include <iostream>

#include "finans/core/finans.h"

#include "tclap/CmdLine.h"

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
  catch (TCLAP::ArgException &e)
  {
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    return -12;
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

int cmd_status(std::vector<std::string> args) {
  try {
    TCLAP::CmdLine cmd("Give the status of your finances", ' ', "", false);
    cmd.parse(args);

    auto finans = Finans::CreateNew();
    std::cout << "Number of accounts: " << finans->NumberOfAccounts() << "\n";
    std::cout << "Number of currencies: " << finans->NumberOfCurrencies() << "\n";
  }
  catch (...)
  {
    return ExceptionHandler();
  }

  return 0;
}

int cmd_addcurrecy(std::vector<std::string> args) {
  try {
    TCLAP::CmdLine cmd("Add a currency to finans", ' ', "", false);
    TCLAP::ValueArg<std::string> longNamneArg("n", "name", "The long name, ie. 'American Dollar'", true, "", "string", cmd);
    TCLAP::ValueArg<std::string> shortNameArg("s", "short-name", "The short name ie. USD", true, "", "string", cmd);
    TCLAP::ValueArg<std::string> beforeArg("b", "before", "The string before a value, ie. the $ in $99", false, "", "string", cmd);
    TCLAP::ValueArg<std::string> afterArg("a", "after", "The string after a value, ie. the kr in 45 kr", false, "", "string", cmd);
    cmd.parse(args);

    auto finans = Finans::CreateNew();
    finans->AddCurency(longNamneArg.getValue(), shortNameArg.getValue(), beforeArg.getValue(), afterArg.getValue());
    finans->Save();
    std::cout << "Added " << shortNameArg.getValue() << ".\n";
  }
  catch (...)
  {
    return ExceptionHandler();
  }

  return 0;
}

int cmd_install(std::vector<std::string> args) {
  try {
    TCLAP::CmdLine cmd("Install finans to your system", ' ', "", false);
    TCLAP::ValueArg<std::string> folderArg("f", "folder", "Where to install", true, "", "string", cmd);
    TCLAP::SwitchArg dontCreateSwitch("c", "dont-create", "Don't create the finans database", cmd, false);
    cmd.parse(args);

    const std::string folder = folderArg.getValue();
    const bool dontCreate = dontCreateSwitch.getValue();

    Finans::Install(folder, !dontCreate);
    std::cout << "Install complete.\n";
  }
  catch (...)
  {
    return ExceptionHandler();
  }

  return 0;
}

//////////////////////////////////////////////////////////////////////////

std::vector<std::string> Collect(int argc, char** argv) {
  std::vector<std::string> ret;
  for (int i = 0; i < argc; ++i) {
    ret.push_back(argv[i]);
  }
  return ret;
}

int main(int argc, char** argv) {
  const auto args = Collect(argc, argv);

  if (args.size() <= 1) {
    std::cerr << "Missing command.\n";
    std::cout << USAGE_AND_HELP;
    return -1;
  }

  const auto cmd = args[1];

  auto cmd_args = args;
  cmd_args.erase(++cmd_args.begin()); // remove the second element
  cmd_args[0] += " " + cmd;

  if (cmd == "help") {
    return 0;
  }
  if (cmd == "status" || cmd == "stat") {
    return cmd_status(cmd_args);
  }
  if (cmd == "install") {
    return cmd_install(cmd_args);
  }
  if (cmd == "addcurr") {
    return cmd_addcurrecy(cmd_args);
  }

  std::cerr << "Unknown command " << cmd << ".\n";
  std::cout << USAGE_AND_HELP;
  return -2;
}
