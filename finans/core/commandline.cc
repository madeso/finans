// Copyright (2015) Gustav

#include "finans/core/commandline.h"

#include <cassert>
#include <algorithm>
#include <string>
#include <set>

namespace argparse {

  ParserError::ParserError(const std::string& error)
    : runtime_error("error: " + error), has_displayed_usage(false)
  {
  }

  void OnParserError(Running& running, const Parser* parser, ParserError& p) {
    if( p.has_displayed_usage == false ) {
      running.o << "Usage:";
      parser->WriteUsage(running);
      p.has_displayed_usage = true;
    }
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
  const std::string Arguments::ConsumeOne(const std::string& error)
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


  Running::Running(const std::string& aapp, std::ostream& ao, std::ostream& ae)
    : app(aapp)
    , o(ao)
    , e(ae)
    , quit(false)
  {
  }

  Argument::~Argument()
  {
  }

  Argument::Argument(const Count& co)
    : count_(co)
    , has_been_parsed_(false)
    , has_several_(false)
  {
  }

  bool Argument::has_been_parsed() const {
    return has_been_parsed_;
  }

  void Argument::set_has_been_parsed(bool v) {
    if (has_several_) return;
    has_been_parsed_ = v;
  }
    
  void Argument::ConsumeArguments(Running& r, Arguments& args, const std::string& argname) {
    switch (count_.type())
    {
    case Count::Const:
      for (size_t i = 0; i < count_.count(); ++i)
      {
        std::stringstream ss;
        ss << "argument " << argname << ": expected ";
        if (count_.count() == 1)
        {
          ss << "one argument";
        }
        else
        {
          ss << count_.count() << " argument(s), " << i << " already given";
        }
        OnArgument(r, args.ConsumeOne(ss.str()));

        // abort on optional?
      }
      return;
    case Count::MoreThanOne:
      OnArgument(r, args.ConsumeOne("argument " + argname + ": expected atleast one argument"));
      // fall through
    case Count::ZeroOrMore:
      while (args.is_empty() == false)
      {
        OnArgument(r, args.ConsumeOne("internal error"));
      }
      return;
    case Count::Optional:
      if (args.is_empty()) {
        OnArgument(r, "");
        return;
      }
      if (IsOptional(args[0])) {
        OnArgument(r, "");
        return;
      }
      OnArgument(r, args.ConsumeOne("internal error"));
      return;
    case Count::None:
      OnArgument(r, "");
      return;
    default:
      assert(0 && "internal error, ArgumentT::parse invalid Count");
      throw "internal error, ArgumentT::parse invalid Count";
    }
  }

  void Argument::set_has_several() {
    has_several_ = true;
  }

  bool IsOptional(const std::string& arg)
  {
    if (arg.empty()) return false; // todo: assert this?
    return arg[0] == '-';
  }

  ParserOptions::ParserOptions()
    : count_(1)
    , has_several_(false)
  {
  }


  ParserOptions& ParserOptions::help(const std::string& h)
  {
    help_ = h;
    return *this;
  }
  const std::string& ParserOptions::help() const
  {
    return help_;
  }

  ParserOptions& ParserOptions::count(const Count c)
  {
    count_ = c;
    return *this;
  }
  const Count& ParserOptions::count() const
  {
    return count_;
  }

  ParserOptions& ParserOptions::metavar(const std::string& metavar)
  {
    metaVar_ = metavar;
    return *this;
  }

  const std::string& ParserOptions::metavar() const
  {
    return metaVar_;
  }

  ParserOptions& ParserOptions::several() {
    has_several_ = true;
    return *this;
  }
  bool ParserOptions::has_several() const {
    return has_several_;
  }

  std::string ToUpper(const std::string& s)
  {
    std::string str = s;
    std::transform(str.begin(), str.end(), str.begin(), toupper);
    return str;
  }

  Help::Help(const std::string& name, const ParserOptions& e)
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
      const auto rep = GetMetavarReprestentation();
      if( rep.empty()) return "[" + name_ + "]";
      return "[" + name_ + " " + rep + "]";
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
      for (size_t i = 0; i < countcount_; ++i)
      {
        if (i != 0)
        {
          ss << " ";
        }
        ss << GetMetavarName();
      }
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
      const auto meta = GetMetavarReprestentation();
      if (meta.empty()) {
        return name_;
      }
      else {
        return name_ + " " + meta;
      }
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

  void Help::SetMetavar(const std::string& metavar) {
    metavar_ = metavar;
  }

  void Help::SetHelp(const std::string& help) {
    help_ = help;
  }

  NotDefaultArgumentData& NotDefaultArgumentData::metavar(const std::string& metavar) {
    for (std::shared_ptr<Help> h : helps_) {
      h->SetMetavar(metavar);
    }
    return *this;
  }

  NotDefaultArgumentData& NotDefaultArgumentData::help(const std::string& help) {
    for (std::shared_ptr<Help> h : helps_) {
      h->SetHelp(help);
    }
    return *this;
  }

  void NotDefaultArgumentData::AddHelp(std::shared_ptr<Help> help) {
    helps_.push_back(help);
  }

  struct CallHelp : public Argument
  {
    CallHelp(Parser* on)
      : Argument( Count(Count::Optional) )
      , parser(on)
    {
    }

    void OnArgument(Running& r, const std::string& argname) override
    {
      parser->WriteHelp(r);
      r.quit = true;
    }

    Parser* parser;
  };

  class ParserChild : public Parser {
  public:
    ParserChild(const std::string& desc, const Parser* parent) : Parser(desc), parent_(parent) {}

    void WriteUsage(Running& r) const override {
      parent_->WriteUsage(r);
      Parser::WriteUsage(r);
    }

  private:
    const Parser* parent_;
  };

  Parser::Parser(const std::string& d, const std::string aappname)
    : positionalIndex_(0)
    , description_(d)
    , appname_(aappname)
    , sub_parsers_("subparser")
  {
    std::shared_ptr<Argument> arg(new CallHelp(this));
    AddArgument("-h", arg, ParserOptions().count(Count(Count::None)).help("Show this help message and exit."));
  }

  void Parser::set_description(const std::string& desc) {
    description_ = desc;
  }

  void Parser::set_appname(const std::string& appname) {
    appname_ = appname;
  }

  Parser::ParseStatus Parser::ParseArgs(int argc, char* argv[], std::ostream& out, std::ostream& error) const
  {
    Arguments args(argc, argv);
    return ParseArgs(args, out, error);
  }

  void Parser::AddSubParser(const std::string& name, SubParser* parser) {
    sub_parsers_(name, parser);
  }

  Parser::ParseStatus Parser::ParseArgs(const Arguments& arguments, std::ostream& out, std::ostream& error) const {
    Arguments args = arguments;
    Running running(arguments.name(), out, error);

    try {
      return DoParseArgs(args, running);
    }
    catch (ParserError& p) {
      OnParserError(running, this, p);
      running.e << p.what() << "\n";
      running.o << "\n";
      return Parser::ParseStatus::ParseFailed;
    }
  }

  Parser::ParseStatus Parser::DoParseArgs(Arguments& args, Running& running) const {
    try
    {
      while (false == args.is_empty())
      {
        bool isParsed = false;
        const bool isParsingPositionals = positionalIndex_ < positionals_.size();

        if (IsOptional(args[0]))
        {
          // optional
          const std::string arg = args[0];
          Optionals::const_iterator r = optionals_.find(arg);
          if (r == optionals_.end())
          {
            if (isParsingPositionals == false) {
              throw ParserError("Unknown optional argument: " + arg); // todo: implement partial matching of arguments?
            }
          }
          else {
            if (false == r->second->has_been_parsed()) {
              isParsed = true;
              args.ConsumeOne(); // the optional command = arg[0}
              r->second->ConsumeArguments(running, args, arg);
              r->second->set_has_been_parsed(true);
              if (running.quit) return ParseStatus::ParseQuit;
            }
          }
        }

        bool consumed = false;

        if (isParsed == false) {
          if (positionalIndex_ >= positionals_.size())
          {
            if (sub_parsers_.empty()) {
              throw ParserError("All positional arguments have been consumed: " + args[0]);
            }
            else {
              std::string subname;
              SubParser* sub = sub_parsers_.Convert(args[0], &subname);
              sub_parser_used_ = subname;
              ParserChild parser(args[0], this);
              sub->AddParser(parser);
              consumed = true;
              args.ConsumeOne("SUBCOMMAND");
              try {
                auto parsestatus = parser.DoParseArgs(args, running);
                if (parsestatus == ParseComplete) {
                  sub->ParseCompleted();
                }
                return parsestatus;
              }
              catch (ParserError& p) {
                OnParserError(running, &parser, p);
                running.e << "error: Failed to parse " << subname << ":\n";
                throw;
              }
            }
          }
          if (consumed == false) {
            ArgumentPtr p = positionals_[positionalIndex_];
            ++positionalIndex_;
            p->ConsumeArguments(running, args, "POSITIONAL"); // todo: give better name or something
          }
        }
      }

      if (positionalIndex_ != positionals_.size())
      {
        throw ParserError("too few arguments."); // todo: list a few missing arguments...
      }

      return ParseComplete;
    }
    catch (ParserError& p)
    {
      OnParserError(running, this, p);
      throw;
    }
  }

  void Parser::WriteHelp(Running& r) const
  {
    r.o << "Usage:";
    r.o << " " << (appname_.empty() ? r.app : appname_);
    WriteUsage(r);
    r.o << std::endl << description_ << std::endl << std::endl;

    const std::string sep = "\t";
    const std::string ins = "  ";

    if (helpPositional_.empty() == false)
    {
      r.o << "Positional arguments:" << std::endl;
      for (const std::shared_ptr<Help> positional : helpPositional_)
      {
        r.o << ins << positional->GetHelpCommand();
        const auto h = positional->help();
        if (h.empty() == false) {
          r.o << sep << h;
        }
        r.o << std::endl;
      }

      r.o << std::endl;
    }

    if (helpOptional_.empty() == false)
    {
      r.o << "Optional arguments:" << std::endl;
      std::set<std::string> opts;
      for (const std::shared_ptr<Help> optional : helpOptional_)
      {
        std::ostringstream ss;
        ss << ins << optional->GetHelpCommand();
        const auto h = optional->help();
        if( h.empty() == false) {
          ss << sep << h;
        }
        opts.insert(ss.str());
      }
      for (const auto& s : opts) {
        r.o << s << "\n";
      }
    }

    r.o << std::endl;
  }

  void Parser::WriteUsage(Running& r) const
  {
    for (const std::shared_ptr<Help> optional : helpOptional_)
    {
      r.o << " " << optional->GetUsage();
    }

    if (false == sub_parsers_.empty()) {
      // if the sub-parser is set, use it instead of the array
      if (false == sub_parser_used_.empty()) {
        r.o << " " << sub_parser_used_;
      }
      else {
        const auto sp = sub_parsers_.names();
        r.o << " {";
        bool first = true;
        for (const auto& p : sp) {
          if (first == false) {
            r.o << "|";
          }
          first = false;
          r.o << p;
        }
        r.o << "}";
      }
    }

    for (const std::shared_ptr<Help>& positional : helpPositional_)
    {
      r.o << " " << positional->GetUsage();
    }
  }


  NotDefaultArgumentData Parser::AddArgument(const std::string& commands, ArgumentPtr arg, const ParserOptions& extra)
  {
    NotDefaultArgumentData ret;

    if( extra.has_several() ) {
      arg->set_has_several();
    }
    const auto names = Tokenize(commands, ",", true);
    std::string thename = "";
    int optionalcount = 0;
    int positionalcount = 0;
    for(const auto name: names) {
      if (IsOptional(name))
      {
        optionals_.insert(Optionals::value_type(name, arg));
        if (thename.empty()) thename = name.substr(1);
        ++optionalcount;
      }
      else
      {
        positionals_.push_back(arg);
        if( thename.empty() ) thename = name;
        ++positionalcount;
      }
    }
    ParserOptions e = extra;
    if (e.metavar().empty()) {
      e.metavar(thename);
    }
    if (positionalcount > 0 && optionalcount > 0) {
      assert(false && "Optional and positional in argument list is not supported.");
    }
    if( positionalcount > 0 ) {
      std::shared_ptr<Help> h(new Help(commands, e));
      ret.AddHelp(h);
      helpPositional_.push_back(h);
    }
    else {
      assert(optionalcount > 0);
      std::shared_ptr<Help> h(new Help(commands, e));
      ret.AddHelp(h);
      helpOptional_.push_back(h);
    }

    return ret;
  }
}
