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

#include "stringutils.h"

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
    const bool is_empty() const;
    const std::string name() const;
    const size_t size() const;
    const std::string ConsumeOne(const std::string& error = "no more arguments available");
  private:
    std::string name_;
    std::vector<std::string> args_;
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

#define ARGPARSE_DEFINE_ENUM(TYPE, NAME, VALUES) \
  template<>\
  TYPE argparse::StandardConverter(const std::string& type)\
  {\
    static const auto values = StringConverter<TYPE>{ NAME } VALUES;\
    return values.Convert(type);\
  }

  class Count
  {
  public:
    enum Type
    {
      Const, MoreThanOne, Optional, None, ZeroOrMore
    };

    explicit Count(size_t c);
    explicit Count(Type t);

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

    virtual void ConsumeArguments(Running& r, Arguments& args, const std::string& argname) = 0;
  };

  class CommonArgument : public Argument
  {
  public:
    explicit CommonArgument(const Count& co);

    virtual void OnArgument(const std::string& str) = 0;
    virtual void ConsumeArguments(Running&, Arguments& args, const std::string& argname) override;
  private:
    Count count_;
  };

  template <typename T, typename V>
  class ArgumentT : public CommonArgument
  {
  public:
    ArgumentT(T& t, const Count& co, CombinerFunction(T, V) com, ConverterFunction(V) c)
      : CommonArgument(co)
      , target_(t)
      , combiner_(com)
      , converter_(c)
    {
    }

    virtual void OnArgument(const std::string& arg) override
    {
      combiner_(target_, converter_(arg));
    }
  private:
    T& target_;
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

  std::string ToUpper(const std::string& s);

  class Help
  {
  public:
    Help(const std::string& name, const Extra& e);

    const std::string GetUsage() const;

    const std::string GetMetavarReprestentation() const;

    const std::string GetMetavarName() const;

    const std::string GetHelpCommand() const;

    const std::string& help() const;
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

  template<typename T>
  class StringConverter {
  public:
    explicit StringConverter(const std::string name) : name_(name){
    }

    StringConverter& operator()(const std::string& s, const T& t) {
      entries_.push_back(std::make_pair(ToUpper(s), t));
      exact_.insert(std::make_pair(ToUpper(s), t));
      return *this;
    }

    T Convert(const std::string& a) const {
      

      const auto s = ToUpper(a);

      const auto r = exact_.find(s);
      if (r != exact_.end()) {
        return r->second;
      }

        std::vector<T> res;
      for (const auto& n : entries_ ) {
        if (StartsWith(n.first, s)) {
          res.push_back(n.second);
        }
      }

      if (res.empty()) {
        throw ParserError("Unable to parse " + s + " to a " + name_);
      }
      else if (res.size() == 1) {
        return res[0];
      }
      else {
        // todo: list all values...
        throw ParserError("Unable to parse " + s + ": Ambiguous value.");
      }
    }

  private:
    std::string name_;
    std::vector<std::pair<std::string, T>> entries_;
    std::map<std::string, T> exact_;
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

    explicit Parser(const std::string& d, const std::string aappname = "");

    template<typename T>
    Parser& operator()(const std::string& name, T& var, const Extra& extra = Extra(), CombinerFunction(T, T) combiner = Assign<T, T>, ConverterFunction(T) co = StandardConverter<T>)
    {
      return Add<T, T>(name, var, extra, combiner);
    }

    template<typename T>
    Parser& AddGreedy(const std::string& name, std::vector<T>& strings, const std::string& metavar) {
      return Add<std::vector<T>, T>(name, strings, argparse::Extra().count(Count(argparse::Count::MoreThanOne)).metavar(metavar), argparse::PushBackVector<T>);
    }

    template<typename T>
    Parser& AddVector(const std::string& name, std::vector<T>& strings, const std::string& metavar="") {
      return Add<std::vector<T>, T>(name, strings, argparse::Extra().count(Count(1)).metavar(metavar), argparse::PushBackVector<T>);
    }

    template<typename T>
    Parser& operator()(const std::string& name, std::vector<T>& strings, const std::string& metavar="") {
      return AddVector<T>(name, strings, metavar);
    }

    template<typename T, typename V>
    Parser& Add(const std::string& name, T& var, const Extra& extra = Extra(), CombinerFunction(T, V) combiner = Assign<T, V>, ConverterFunction(V) co = StandardConverter<V>)
    {
      ArgumentPtr arg(new ArgumentT<T, V>(var, extra.count(), combiner, co));
      return AddArgument(name, arg, extra);
    }

    ParseStatus ParseArgs(int argc, char* argv[], std::ostream& out = std::cout, std::ostream& error = std::cerr) const;
    ParseStatus ParseArgs(const Arguments& arguments, std::ostream& out = std::cout, std::ostream& error = std::cerr) const;


    void WriteHelp(Running& r) const;
    void WriteUsage(Running& r) const;
  private:
    typedef std::shared_ptr<Argument> ArgumentPtr;

    Parser& AddArgument(const std::string& name, ArgumentPtr arg, const Extra& extra);

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
