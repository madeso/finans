// Copyright (2015) Gustav

#ifndef CORE_COMMANDLINE_H_
#define CORE_COMMANDLINE_H_

#include <iostream>

#include <string>
#include <sstream>
#include <functional>
#include <vector>
#include <map>
#include <memory>
#include <cassert>

#define ConverterFunction(V) std::function<V (const std::string&)>
#define CombinerFunction(T,V) std::function<void (T& t, const V&)>

namespace argparse
{
  class ParserError : public std::runtime_error
  {
  public:
    ParserError(const std::string& error);
  };

  class Arguments
  {
  public:
    Arguments(int argc, char* argv[]);
    Arguments(const std::string& name, const std::vector<std::string>& args);

    const std::string operator[](int index) const;
    const bool empty() const;
    const std::string name() const;
    const size_t size() const;
    const std::string get(const std::string& error = "no more arguments available");
  private:
    std::string name_;
    std::vector<std::string> args_;
  };

  template <typename T>
  class Convert
  {
  public:
    Convert(const std::string& name, T t)
    {
    }

    Convert& operator()(const std::string& name, T t)
    {
    }

    T operator()(const std::string& in)
    {
    }
  };

  template<typename T>
  T StandardConverter(const std::string& type)
  {
    std::istringstream ss(type);
    T t;
    if (ss >> t)
    {
      return t;
    }
    else
    {
      throw ParserError("Failed to parse " + type);
    }
  }

  class Count
  {
  public:
    enum Type
    {
      Const, MoreThanOne, Optional, None, ZeroOrMore
    };

    Count(size_t c);
    Count(Type t);

    Type type() const;
    size_t count() const;
  private:
    size_t count_;
    Type type_;
  };

  /// basic class for passing along variables that only exist when parsing
  struct Running
  {
  public:
    Running(const std::string& aapp, std::ostream& ao);

    const std::string& app;
    std::ostream& o;
  private:
    Running(const Running&);
    void operator=(const Running&);
  };

  class Argument
  {
  public:
    virtual ~Argument();

    virtual void parse(Running& r, Arguments& args, const std::string& argname) = 0;
  };

  typedef std::function<void(Running& r, Arguments&, const std::string&)> ArgumentCallback;

  class FunctionArgument : public Argument
  {
  public:
    FunctionArgument(const ArgumentCallback& func);
    void parse(Running& r, Arguments& args, const std::string& argname);
  private:
    ArgumentCallback function_;
  };

  template <typename T, typename V>
  class ArgumentT : public Argument
  {
  public:
    ArgumentT(T& t, const Count& co, CombinerFunction(T, V) com, ConverterFunction(V) c)
      : target_(t)
      , count_(co)
      , combiner_(com)
      , converter_(c)
    {
    }

    virtual void parse(Running&, Arguments& args, const std::string& argname)
    {
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
          combiner_(target_, converter_(args.get(ss.str())));

          // abort on optional?
        }
        return;
      case Count::MoreThanOne:
        combiner_(target_, converter_(args.get("argument " + argname + ": expected atleast one argument")));
      case Count::ZeroOrMore:
        while (args.empty() == false && IsOptional(args[0]) == false)
        {
          combiner_(target_, converter_(args.get("internal error")));
        }
        return;
      case Count::Optional:
        if (args.empty()) return;
        if (IsOptional(args[0])) return;
        combiner_(target_, converter_(args.get("internal error")));
        return;
      case Count::None:
        return;
      default:
        assert(0 && "internal error, ArgumentT::parse invalid Count");
        throw "internal error, ArgumentT::parse invalid Count";
      }
    }
  private:
    T& target_;
    Count count_;
    CombinerFunction(T, V) combiner_;
    ConverterFunction(V) converter_;
  };

  /// internal function.
  /// @returns true if arg is to be considered as an optional
  bool IsOptional(const std::string& arg);

  // Utility class to provide optional arguments for the commandline arguments.
  class Extra
  {
  public:
    Extra();

    /// set the extended help for the argument
    Extra& help(const std::string& h);
    const std::string& help() const;

    /// the number of values a argument can support
    Extra& count(const Count c);
    const Count& count() const;

    /// the meta variable, used in usage display and help display for the arguments
    Extra& metavar(const std::string& metavar);
    const std::string& metavar() const;
  private:
    std::string help_;
    Count count_;
    std::string metaVar_;
  };

  std::string Upper(const std::string& s);

  class Help
  {
  public:
    Help(const std::string& aname, const Extra& e);

    const std::string usage() const;

    const std::string metavarrep() const;

    const std::string metavarname() const;

    const std::string helpCommand() const;

    const std::string& helpDescription() const;
  private:
    std::string name_;
    std::string help_;
    std::string metavar_;
    Count::Type count_;
    size_t countcount_;
  };

  template<typename A, typename B>
  void Assign(A& a, const B& b)
  {
    a = b;
  }

  template<typename T>
  void PushBackVector(std::vector<T>& vec, const T& t)
  {
    vec.push_back(t);
  }

  class Parser;

  struct CallHelp
  {
    CallHelp(Parser* on);

    void operator()(Running& r, Arguments& args, const std::string& argname);

    Parser* parser;
  };

  /// main entry class that contains all arguments and does all the parsing.
  class Parser
  {
  public:
    enum ParseStatus
    {
      ParseFailed,
      ParseComplete
    };

    Parser(const std::string& d, const std::string aappname = "");

    template<typename T>
    Parser& operator()(const std::string& name, T& var, const Extra& extra = Extra(), CombinerFunction(T, T) combiner = Assign<T, T>, ConverterFunction(T) co = StandardConverter<T>)
    {
      return add<T, T>(name, var, extra, combiner);
    }

    template<typename T>
    Parser& operator()(const std::string& name, std::vector<T>& strings, const std::string& metavar) {
      return add<std::vector<T>, T>(name, strings, argparse::Extra().count(argparse::Count::MoreThanOne).metavar(metavar), argparse::PushBackVector<std::string>);
    }

    Parser& operator()(const std::string& name, ArgumentCallback func, const Extra& extra = Extra());

    template<typename T, typename V>
    Parser& add(const std::string& name, T& var, const Extra& extra = Extra(), CombinerFunction(T, V) combiner = Assign<T, V>, ConverterFunction(V) co = StandardConverter<V>)
    {
      ArgumentPtr arg(new ArgumentT<T, V>(var, extra.count(), combiner, co));
      return insert(name, arg, extra);
    }

    Parser& addFunction(const std::string& name, ArgumentCallback func, const Extra& extra);

    ParseStatus parseArgs(int argc, char* argv[], std::ostream& out = std::cout, std::ostream& error = std::cerr) const;
    ParseStatus parseArgs(const Arguments& arguments, std::ostream& out = std::cout, std::ostream& error = std::cerr) const;


    void writeHelp(Running& r) const;
    void writeUsage(Running& r) const;
  private:
    typedef std::shared_ptr<Argument> ArgumentPtr;

    Parser& insert(const std::string& name, ArgumentPtr arg, const Extra& extra);

    std::string description_;
    std::string appname_;

    typedef std::map<std::string, ArgumentPtr> Optionals;
    Optionals optionals_;

    typedef std::vector<ArgumentPtr> Positionals;
    Positionals positionals_;
    mutable size_t positionalIndex_; // todo: mutable or change parseArgs to nonconst?

    std::vector<Help> helpOptional_;
    std::vector<Help> helpPositional_;
  };
}



#endif  // CORE_COMMANDLINE_H_
