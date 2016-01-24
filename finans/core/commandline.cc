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
  const bool Arguments::is_empty() const
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
  const std::string Arguments::Get(const std::string& error)
  {
    if (is_empty()) throw ParserError(error);
    const std::string r = args_[0];
    args_.erase(args_.begin());
    return r;
  }


  Count::Count(size_t c)
    : count_(c)
    , type_(Const)
  {
  }

  Count::Count(Count::Type t)
    : count_(0)
    , type_(t)
  {
    assert(t != Const);
  }

  Count::Type Count::type() const
  {
    return type_;
  }
  size_t Count::count() const
  {
    return count_;
  }


  Running::Running(const std::string& aapp, std::ostream& ao)
    : app(aapp)
    , o(ao)
  {
  }

  Argument::~Argument()
  {
  }

  CallbackArgument::CallbackArgument(const ArgumentCallback& func)
    : function_(func)
  {
  }

  void CallbackArgument::Parse(Running& r, Arguments& args, const std::string& argname)
  {
    function_(r, args, argname);
  }

  bool IsOptional(const std::string& arg)
  {
    if (arg.empty()) return false; // todo: assert this?
    return arg[0] == '-';
  }

  Extra::Extra()
    : count_(1)
  {
  }


  Extra& Extra::help(const std::string& h)
  {
    help_ = h;
    return *this;
  }
  const std::string& Extra::help() const
  {
    return help_;
  }

  Extra& Extra::count(const Count c)
  {
    count_ = c;
    return *this;
  }
  const Count& Extra::count() const
  {
    return count_;
  }

  Extra& Extra::metavar(const std::string& metavar)
  {
    metaVar_ = metavar;
    return *this;
  }

  const std::string& Extra::metavar() const
  {
    return metaVar_;
  }

  std::string ToUpper(const std::string& s)
  {
    std::string str = s;
    std::transform(str.begin(), str.end(), str.begin(), toupper);
    return str;
  }

  Help::Help(const std::string& name, const Extra& e)
    : name_(name)
    , help_(e.help())
    , metavar_(e.metavar())
    , count_(e.count().type())
    , countcount_(e.count().count())

  {
  }

  const std::string Help::GetUsage() const
  {
    if (IsOptional(name_))
    {
      return "[" + name_ + " " + GetMetavarReprestentation() + "]";
    }
    else
    {
      return GetMetavarReprestentation();
    }
  }

  const std::string Help::GetMetavarReprestentation() const
  {
    switch (count_)
    {
    case Count::None:
      return "";
    case Count::MoreThanOne:
      return GetMetavarName() + " [" + GetMetavarName() + " ...]";
    case Count::Optional:
      return "[" + GetMetavarName() + "]";
    case Count::ZeroOrMore:
      return "[" + GetMetavarName() + " [" + GetMetavarName() + " ...]]";
    case Count::Const:
    {
      std::ostringstream ss;
      ss << "[";
      for (size_t i = 0; i < countcount_; ++i)
      {
        if (i != 0)
        {
          ss << " ";
        }
        ss << GetMetavarName();
      }
      ss << "]";
      return ss.str();
    }
    default:
      assert(false && "missing case");
      throw "invalid count type in " __FUNCTION__;
    }
  }

  const std::string Help::GetMetavarName() const
  {
    if (metavar_.empty() == false)
    {
      return metavar_;
    }
    else
    {
      if (IsOptional(name_))
      {
        return ToUpper(name_.substr(1));
      }
      else
      {
        return name_;
      }
    }
  }

  const std::string Help::GetHelpCommand() const
  {
    if (IsOptional(name_))
    {
      return name_ + " " + GetMetavarReprestentation();
    }
    else
    {
      return GetMetavarName();
    }
  }

  const std::string& Help::help() const
  {
    return help_;
  }

  CallHelp::CallHelp(Parser* on)
    : parser(on)
  {
  }

  void CallHelp::operator()(Running& r, Arguments& args, const std::string& argname)
  {
    parser->WriteHelp(r);
    exit(0);
  }

  Parser::Parser(const std::string& d, const std::string aappname)
    : positionalIndex_(0)
    , description_(d)
    , appname_(aappname)
  {
    AddArgumentCallback("-h", CallHelp(this), Extra().count(Count::None).help("show this help message and exit"));
  }

  Parser& Parser::operator()(const std::string& name, ArgumentCallback func, const Extra& extra)
  {
    return AddArgumentCallback(name, func, extra);
  }

  Parser& Parser::AddArgumentCallback(const std::string& name, ArgumentCallback func, const Extra& extra)
  {
    ArgumentPtr arg(new CallbackArgument(func));
    return AddArgument(name, arg, extra);
  }

  Parser::ParseStatus Parser::ParseArgs(int argc, char* argv[], std::ostream& out, std::ostream& error) const
  {
    Arguments args(argc, argv);
    return ParseArgs(args, out, error);
  }

  Parser::ParseStatus Parser::ParseArgs(const Arguments& arguments, std::ostream& out, std::ostream& error) const {
    Arguments args = arguments;
    Running running(arguments.name(), out);

    try
    {
      while (false == args.is_empty())
      {
        if (IsOptional(args[0]))
        {
          // optional
          const std::string arg = args.Get();
          Optionals::const_iterator r = optionals_.find(arg);
          if (r == optionals_.end())
          {
            throw ParserError("Unknown optional argument: " + arg); // todo: implement partial matching of arguments?
          }
          r->second->Parse(running, args, arg);
        }
        else
        {
          if (positionalIndex_ >= positionals_.size())
          {
            throw ParserError("All positional arguments have been consumed: " + args[0]);
          }
          ArgumentPtr p = positionals_[positionalIndex_];
          ++positionalIndex_;
          p->Parse(running, args, "POSITIONAL"); // todo: give better name or something
        }
      }

      if (positionalIndex_ != positionals_.size())
      {
        throw ParserError("too few arguments"); // todo: list a few missing arguments...
      }

      return ParseComplete;
    }
    catch (ParserError& p)
    {
      WriteUsage(running);
      error << running.app << ": " << p.what() << std::endl << std::endl;
      return ParseFailed;
    }
  }

  void Parser::WriteHelp(Running& r) const
  {
    WriteUsage(r);
    r.o << std::endl << description_ << std::endl << std::endl;

    const std::string sep = "\t";
    const std::string ins = "  ";

    if (helpPositional_.empty() == false)
    {
      r.o << "Positional arguments: " << std::endl;
      for (const Help& positional : helpPositional_)
      {
        r.o << ins << positional.GetHelpCommand() << sep << positional.help() << std::endl;
      }

      r.o << std::endl;
    }

    if (helpOptional_.empty() == false)
    {
      r.o << "Optional arguments: " << std::endl;
      for (const Help& optional : helpOptional_)
      {
        r.o << ins << optional.GetHelpCommand() << sep << optional.help() << std::endl;
      }
    }

    r.o << std::endl;
  }

  void Parser::WriteUsage(Running& r) const
  {
    r.o << "Usage: " << r.app;
    for (const Help& optional : helpOptional_)
    {
      r.o << " " << optional.GetUsage();
    }

    for (const Help& positional : helpPositional_)
    {
      r.o << " " << positional.GetUsage();
    }
    r.o << std::endl;
  }


  Parser& Parser::AddArgument(const std::string& name, ArgumentPtr arg, const Extra& extra)
  {
    if (IsOptional(name))
    {
      optionals_.insert(Optionals::value_type(name, arg));
      helpOptional_.push_back(Help(name, extra));
      return *this;
    }
    else
    {
      positionals_.push_back(arg);
      helpPositional_.push_back(Help(name, extra));
      return *this;
    }
  }
}