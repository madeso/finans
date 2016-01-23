// Copyright (2015) Gustav

#include "finans/core/commandline.h"

#include "gtest/gtest.h"

struct CommandlineTest : public ::testing::Test {
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
    .parseArgs(argparse::Arguments("app.exe", {}), output, error);
  EXPECT_EQ(true, ok);
}

GTEST(TestError) {
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    .parseArgs(argparse::Arguments("app.exe", { "hello", "world" }), output, error);
  EXPECT_EQ(false, ok);
}

GTEST(TestOptionalDefault) {
  int op = 2;
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    ("-op", op)
    .parseArgs(argparse::Arguments("app.exe", {}), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_EQ(2, op);
}

GTEST(TestOptionalValue) {
  int op = 2;
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    ("-op", op)
    .parseArgs(argparse::Arguments("app.exe", {"-op", "42"}), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_EQ(42, op);
}

GTEST(TestPositionalValue) {
  int op = 2;
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    ("op", op)
    .parseArgs(argparse::Arguments("app.exe", { "42" }), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_EQ(42, op);
}

GTEST(TestPositionalValueErr) {
  int op = 42;
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    ("op", op)
    .parseArgs(argparse::Arguments("app.exe", {}), output, error);
  EXPECT_EQ(false, ok);
  EXPECT_EQ(42, op); // not touched
}
