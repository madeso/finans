// Copyright (2015) Gustav

#include "finans/core/commandline.h"

#include <cassert>
#include <algorithm>

namespace argparse {

  ParserError::ParserError(const std::string& error)
    : runtime_error("error: " + error)
  {
  }


  Arguments::Arguments(int argc, char* argv[]) : name_(argv[0])
  {
    for (int i = 1; i < argc; ++i)
    {
      args_.push_back(argv[i]);
    }
  }
  Arguments::Arguments(const std::string& name, const std::vector<std::string>& args) : name_(name), args_(args) {
  }

  const std::string Arguments::operator[](int index) const
  {
    return args_[index];
  }
  const bool Arguments::empty() const
  {
    return args_.empty();
  }
  const std::string Arguments::name() const
  {
    return name_;
  }
  const size_t Arguments::size() const
  {
    return args_.size();
  }
  const std::string Arguments::get(const std::string& error)
  {
    if (empty()) throw ParserError(error);
    const std::string r = args_[0];
    args_.erase(args_.begin());
    return r;
  }


  Count::Count(size_t c)
    : mCount(c)
    , mType(Const)
  {
  }

  Count::Count(Count::Type t)
    : mCount(0)
    , mType(t)
  {
    assert(t != Const);
  }

  Count::Type Count::type() const
  {
    return mType;
  }
  size_t Count::count() const
  {
    return mCount;
  }


  Running::Running(const std::string& aapp, std::ostream& ao)
    : app(aapp)
    , o(ao)
  {
  }

  Argument::~Argument()
  {
  }

  bool IsOptional(const std::string& arg)
  {
    if (arg.empty()) return false; // todo: assert this?
    return arg[0] == '-';
  }

  Extra::Extra()
    : mCount(1)
  {
  }


  Extra& Extra::help(const std::string& h)
  {
    mHelp = h;
    return *this;
  }
  const std::string& Extra::help() const
  {
    return mHelp;
  }

  Extra& Extra::count(const Count c)
  {
    mCount = c;
    return *this;
  }
  const Count& Extra::count() const
  {
    return mCount;
  }

  Extra& Extra::metavar(const std::string& metavar)
  {
    mMetavar = metavar;
    return *this;
  }

  const std::string& Extra::metavar() const
  {
    return mMetavar;
  }

  std::string Upper(const std::string& s)
  {
    std::string str = s;
    std::transform(str.begin(), str.end(), str.begin(), toupper);
    return str;
  }

  Help::Help(const std::string& aname, const Extra& e)
    : name(aname)
    , help(e.help())
    , metavar(e.metavar())
    , count(e.count().type())
    , countcount(e.count().count())

  {
  }

  const std::string Help::usage() const
  {
    if (IsOptional(name))
    {
      return "[" + name + " " + metavarrep() + "]";
    }
    else
    {
      return metavarrep();
    }
  }

  const std::string Help::metavarrep() const
  {
    switch (count)
    {
    case Count::None:
      return "";
    case Count::MoreThanOne:
      return metavarname() + " [" + metavarname() + " ...]";
    case Count::Optional:
      return "[" + metavarname() + "]";
    case Count::ZeroOrMore:
      return "[" + metavarname() + " [" + metavarname() + " ...]]";
    case Count::Const:
    {
      std::ostringstream ss;
      ss << "[";
      for (size_t i = 0; i < countcount; ++i)
      {
        if (i != 0)
        {
          ss << " ";
        }
        ss << metavarname();
      }
      ss << "]";
      return ss.str();
    }
    default:
      assert(false && "missing case");
      throw "invalid count type in " __FUNCTION__;
    }
  }

  const std::string Help::metavarname() const
  {
    if (metavar.empty() == false)
    {
      return metavar;
    }
    else
    {
      if (IsOptional(name))
      {
        return Upper(name.substr(1));
      }
      else
      {
        return name;
      }
    }
  }

  const std::string Help::helpCommand() const
  {
    if (IsOptional(name))
    {
      return name + " " + metavarrep();
    }
    else
    {
      return metavarname();
    }
  }

  const std::string& Help::helpDescription() const
  {
    return help;
  }

  CallHelp::CallHelp(Parser* on)
    : parser(on)
  {
  }

  void CallHelp::operator()(Running& r, Arguments& args, const std::string& argname)
  {
    parser->writeHelp(r);
    exit(0);
  }

  Parser::Parser(const std::string& d, const std::string aappname)
    : positionalIndex(0)
    , description(d)
    , appname(aappname)
  {
    addFunction("-h", CallHelp(this), Extra().count(Count::None).help("show this help message and exit"));
  }

  Parser& Parser::operator()(const std::string& name, ArgumentCallback func, const Extra& extra)
  {
    return addFunction(name, func, extra);
  }

  Parser& Parser::addFunction(const std::string& name, ArgumentCallback func, const Extra& extra)
  {
    ArgumentPtr arg(new FunctionArgument(func));
    return insert(name, arg, extra);
  }

  Parser::ParseStatus Parser::parseArgs(int argc, char* argv[], std::ostream& out, std::ostream& error) const
  {
    Arguments args(argc, argv);
    return parseArgs(args, out, error);
  }

  Parser::ParseStatus Parser::parseArgs(const Arguments& arguments, std::ostream& out, std::ostream& error) const {
    Arguments args = arguments;
    const std::string app = arguments.name();
    Running running(app, out);

    try
    {
      while (false == args.empty())
      {
        if (IsOptional(args[0]))
        {
          // optional
          const std::string arg = args.get();
          Optionals::const_iterator r = optionals.find(arg);
          if (r == optionals.end())
          {
            throw ParserError("Unknown optional argument: " + arg); // todo: implement partial matching of arguments?
          }
          r->second->parse(running, args, arg);
        }
        else
        {
          if (positionalIndex >= positionals.size())
          {
            throw ParserError("All positional arguments have been consumed: " + args[0]);
          }
          ArgumentPtr p = positionals[positionalIndex];
          ++positionalIndex;
          p->parse(running, args, "POSITIONAL"); // todo: give better name or something
        }
      }

      if (positionalIndex != positionals.size())
      {
        throw ParserError("too few arguments"); // todo: list a few missing arguments...
      }

      return ParseComplete;
    }
    catch (ParserError& p)
    {
      writeUsage(running);
      error << app << ": " << p.what() << std::endl << std::endl;
      return ParseFailed;
    }
  }

  void Parser::writeHelp(Running& r) const
  {
    writeUsage(r);
    r.o << std::endl << description << std::endl << std::endl;

    const std::string sep = "\t";
    const std::string ins = "  ";

    if (helpPositional.empty() == false)
    {
      r.o << "positional arguments: " << std::endl;
      for (const Help& positional : helpPositional)
      {
        r.o << ins << positional.helpCommand() << sep << positional.helpDescription() << std::endl;
      }

      r.o << std::endl;
    }

    if (helpOptional.empty() == false)
    {
      r.o << "optional arguments: " << std::endl;
      for (const Help& optional : helpOptional)
      {
        r.o << ins << optional.helpCommand() << sep << optional.helpDescription() << std::endl;
      }
    }

    r.o << std::endl;
  }

  void Parser::writeUsage(Running& r) const
  {
    r.o << "usage: " << r.app;
    for (const Help& optional : helpOptional)
    {
      r.o << " " << optional.usage();
    }

    for (const Help& positional : helpPositional)
    {
      r.o << " " << positional.usage();
    }
    r.o << std::endl;
  }


  Parser& Parser::insert(const std::string& name, ArgumentPtr arg, const Extra& extra)
  {
    if (IsOptional(name))
    {
      optionals.insert(Optionals::value_type(name, arg));
      helpOptional.push_back(Help(name, extra));
      return *this;
    }
    else
    {
      positionals.push_back(arg);
      helpPositional.push_back(Help(name, extra));
      return *this;
    }
  }
}
