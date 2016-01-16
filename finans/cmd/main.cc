#include <iostream>

#include "finans/core/finans.h"

#include "tclap/CmdLine.h"

const std::string USAGE_AND_HELP =
"Usage: finans CMD ARGUMENTS\n"
"Where ARGUMENTS depends on CMD\n"
"and CMD is one of:\n"
"* help\n"
"* install\n"
"* stat\n"
;

int cmd_name(std::vector<std::string> args) {
  try {
    TCLAP::CmdLine cmd("Command description message", ' ', "", false);
    TCLAP::ValueArg<std::string> nameArg("n", "name", "Name to print", true, "homer", "string");
    cmd.add(nameArg);
    TCLAP::SwitchArg reverseSwitch("r", "reverse", "Print name backwards", cmd, false);
    cmd.parse(args);

    std::string name = nameArg.getValue();
    bool reverseName = reverseSwitch.getValue();

    if (reverseName)
      std::cout << "My name (spelled backwards) is: " << name << std::endl;
    else
      std::cout << "My name is: " << name << std::endl;
  }
  catch (TCLAP::ArgException &e)
  {
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    return -12;
  }

  return 0;
}

std::vector<std::string> Collect(int argc, char** argv) {
  std::vector<std::string> ret;
  for (int i = 0; i < argc; ++i) {
    ret.push_back(argv[i]);
  }
  return ret;
}

int useless() {
  auto finans = Finans::CreateNew();
  if (finans == nullptr) {
    std::cout << "Finans is not installed\n";
  }
  std::cout << "Hello world\n";

  return 0;
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
  if (cmd == "name") {
    return cmd_name(cmd_args);
  }

  std::cerr << "Unknown command " << cmd << ".\n";
  std::cout << USAGE_AND_HELP;
  return -2;
}
