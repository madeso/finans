// Copyright (2015) Gustav

#include "finans/core/commandline.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace testing;

struct CommandlineTest : public Test {
  std::ostringstream output;
  std::ostringstream error;
};

#define GTEST(x) TEST_F(CommandlineTest, x)

/*
void main(int argc, char* argv[])
{
  enum MyEnum
  {
    MyVal, MyVal2
  };

  std::string compiler;
  int i;
  int op = 2;
  std::vector<std::string> strings;
  //MyEnum v;
  bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    ("compiler", compiler)
    ("int", i)
    ("-op", op)
    .add<std::vector<std::string>, std::string>("-strings", strings, argparse::Extra().count(argparse::Count::MoreThanOne).metavar("string"), argparse::PushBackVector<std::string>) // todo: is this beautifiable?
    //("-enum", &v, Convert<MyEnum>("MyVal", MyEnum::MyVal)("MyVal2", MyEnum::MyVal2) )
    .parseArgs(argc, argv);
  if (ok == false) return;
  std::cout << compiler << " " << i << " " << op << std::endl;
  BOOST_FOREACH(const std::string& s, strings)
  {
    std::cout << s << " " << std::endl;
  }
}
*/

GTEST(TestEmpty) {
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    .ParseArgs(argparse::Arguments("app.exe", {}), output, error);
  EXPECT_EQ(true, ok);
}

GTEST(TestError) {
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    .ParseArgs(argparse::Arguments("app.exe", { "hello", "world" }), output, error);
  EXPECT_EQ(false, ok);
}

GTEST(TestOptionalDefault) {
  int op = 2;
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    ("-op", op)
    .ParseArgs(argparse::Arguments("app.exe", {}), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_EQ(2, op);
}

GTEST(TestOptionalValue) {
  int op = 2;
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    ("-op", op)
    .ParseArgs(argparse::Arguments("app.exe", {"-op", "42"}), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_EQ(42, op);
}

GTEST(TestPositionalValue) {
  int op = 2;
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    ("op", op)
    .ParseArgs(argparse::Arguments("app.exe", { "42" }), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_EQ(42, op);
}

GTEST(TestPositionalValueErr) {
  int op = 42;
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    ("op", op)
    .ParseArgs(argparse::Arguments("app.exe", {}), output, error);
  EXPECT_EQ(false, ok);
  EXPECT_EQ(42, op); // not touched
}

GTEST(TestStdVector) {
  std::vector<std::string> strings;
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    .AddGreedy("-strings", strings, "string")
    .ParseArgs(argparse::Arguments("app.exe", {"-strings", "cat", "dog", "fish"}), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_THAT(strings, ElementsAre("cat", "dog", "fish"));
}

GTEST(TestStdVectorInts) {
  std::vector<int> ints;
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    .AddGreedy("-ints", ints, "string")
    .ParseArgs(argparse::Arguments("app.exe", { "-ints", "2", "3", "-5", "4" }), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_THAT(ints, ElementsAre(2, 3, -5, 4));
}

GTEST(TestNonGreedyVector) {
  std::vector<std::string> strings;
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    ("-s", strings)
    .ParseArgs(argparse::Arguments("app.exe", { "-s", "cat", "-s", "dog", "-s", "fish" }), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_THAT(strings, ElementsAre("cat", "dog", "fish"));
}

enum class Day
{
  TODAY, YESTERDAY, TOMORROW
};

template<>
Day argparse::StandardConverter(const std::string& type)
{
  static const auto values = StringConverter<Day>{ "day" }("Today", Day::TODAY)("Tomorrow", Day::TOMORROW)("Yesterday", Day::YESTERDAY);
  return values.Convert(type);
}

GTEST(TestEnum) {
  Day op = Day::TOMORROW;
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    ("op", op)
    .ParseArgs(argparse::Arguments("app.exe", { "tod" }), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_EQ(Day::TODAY, op);
}
