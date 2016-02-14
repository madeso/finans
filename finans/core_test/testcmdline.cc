// Copyright (2015) Gustav

#include "finans/core/commandline.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

using namespace testing;

class TestSubParser : public argparse::SubParser {
public:
  TestSubParser() {
  }

  void AddParser(argparse::Parser& parser) override {
    parser("-name", name);
  }

  void ParseCompleted() override {
  }

  std::string name;
};

struct CommandlineTest : public Test {
  std::ostringstream output;
  std::ostringstream error;

  argparse::Parser parser;
  std::string animal;
  std::string another;

  TestSubParser sp1;
  TestSubParser sp2;
  argparse::Parser sub;

  CommandlineTest() : parser("description"), sub("description") {
    parser("pos", animal)("-op", another);
    sub.AddSubParser("one", &sp1).AddSubParser("two", &sp2);
  }
};

#define GTEST(x) TEST_F(CommandlineTest, x)

GTEST(TestEmpty) {
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    .ParseArgs(argparse::Arguments("app.exe", {}), output, error);
  EXPECT_EQ(true, ok);

  EXPECT_EQ("", error.str());
  EXPECT_EQ("", output.str());
}

GTEST(TestError) {
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    .ParseArgs(argparse::Arguments("app.exe", { "hello", "world" }), output, error);
  EXPECT_EQ(false, ok);
  EXPECT_EQ("Usage: [-h]\n", output.str());
  EXPECT_EQ("error: All positional arguments have been consumed: hello\n", error.str());
}

GTEST(TestOptionalDefault) {
  int op = 2;
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    ("-op", op)
    .ParseArgs(argparse::Arguments("app.exe", {}), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_EQ("", error.str());
  EXPECT_EQ("", output.str());
  EXPECT_EQ(2, op);
}

GTEST(TestOptionalValue) {
  int op = 2;
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    ("-op", op)
    .ParseArgs(argparse::Arguments("app.exe", {"-op", "42"}), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_EQ("", error.str());
  EXPECT_EQ("", output.str());
  EXPECT_EQ(42, op);
}

GTEST(TestPositionalValue) {
  int op = 2;
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    ("op", op)
    .ParseArgs(argparse::Arguments("app.exe", { "42" }), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_EQ("", error.str());
  EXPECT_EQ("", output.str());
  EXPECT_EQ(42, op);
}

GTEST(TestPositionalValueErr) {
  int op = 42;
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    ("op", op)
    .ParseArgs(argparse::Arguments("app.exe", {}), output, error);
  EXPECT_EQ(false, ok);
  EXPECT_EQ("error: too few arguments.\n", error.str());
  EXPECT_EQ("Usage: [-h] [op]\n", output.str());
  EXPECT_EQ(42, op); // not touched
}

GTEST(TestStdVector) {
  std::vector<std::string> strings;
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    .AddGreedy("-strings", strings, "string")
    .ParseArgs(argparse::Arguments("app.exe", {"-strings", "cat", "dog", "fish"}), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_EQ("", error.str());
  EXPECT_EQ("", output.str());
  EXPECT_THAT(strings, ElementsAre("cat", "dog", "fish"));
}

GTEST(TestStdVectorInts) {
  std::vector<int> ints;
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    .AddGreedy("-ints", ints, "string")
    .ParseArgs(argparse::Arguments("app.exe", { "-ints", "2", "3", "-5", "4" }), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_EQ("", error.str());
  EXPECT_EQ("", output.str());
  EXPECT_THAT(ints, ElementsAre(2, 3, -5, 4));
}

GTEST(TestNonGreedyVector) {
  std::vector<std::string> strings;
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    ("-s", strings)
    .ParseArgs(argparse::Arguments("app.exe", { "-s", "cat", "-s", "dog", "-s", "fish" }), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_EQ("", error.str());
  EXPECT_EQ("", output.str());
  EXPECT_THAT(strings, ElementsAre("cat", "dog", "fish"));
}

enum class Day
{
  TODAY, YESTERDAY, TOMORROW
};

ARGPARSE_DEFINE_ENUM(Day, "day", ("Today", Day::TODAY)("Tomorrow", Day::TOMORROW)("Yesterday", Day::YESTERDAY) )

GTEST(TestEnum) {
  Day op = Day::TOMORROW;
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    ("op", op)
    .ParseArgs(argparse::Arguments("app.exe", { "tod" }), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_EQ("", error.str());
  EXPECT_EQ("", output.str());
  EXPECT_EQ(Day::TODAY, op);
}

GTEST(TestCommaOp) {
  int op = 2;
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    ("-int,-i", op)
    .ParseArgs(argparse::Arguments("app.exe", { "-int", "42" }), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_EQ("", error.str());
  EXPECT_EQ("", output.str());
  EXPECT_EQ(42, op);
}

GTEST(TestSpecifyTwice) {
  int op = 2;
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    ("-int,-i", op)
    .ParseArgs(argparse::Arguments("app.exe", { "-int", "42", "-i", "50" }), output, error);
  EXPECT_EQ(false, ok);
  EXPECT_EQ("error: All positional arguments have been consumed: -i\n", error.str());
  EXPECT_EQ("Usage: [-h] [-int,-i [int]]\n", output.str());
  EXPECT_EQ(42, op);
}

GTEST(TestPrecedencePos) {
  const bool ok = argparse::Parser::ParseComplete ==
    parser.ParseArgs(argparse::Arguments("app.exe", {"dog"}), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_EQ("", error.str());
  EXPECT_EQ("", output.str());
  EXPECT_EQ("dog", animal);
  EXPECT_EQ("", another);
}

GTEST(TestPrecedencePosOp) {
  const bool ok = argparse::Parser::ParseComplete ==
    parser.ParseArgs(argparse::Arguments("app.exe", { "-dog", "-op", "cat" }), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_EQ("", output.str());
  EXPECT_EQ("", error.str());
  EXPECT_EQ("-dog", animal);
  EXPECT_EQ("cat", another);
}

GTEST(TestPrecedenceOpPos) {
  const bool ok = argparse::Parser::ParseComplete ==
    parser.ParseArgs(argparse::Arguments("app.exe", { "-op", "cat", "dog" }), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_EQ("", error.str());
  EXPECT_EQ("", output.str());
  EXPECT_EQ("dog", animal);
  EXPECT_EQ("cat", another);
}

GTEST(TestStoreConstInt) {
  int op = 12;
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    .StoreConst("-store", op, 42)
    .ParseArgs(argparse::Arguments("app.exe", { "-store" }), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_EQ("", output.str());
  EXPECT_EQ("", error.str());
  EXPECT_EQ(42, op);
}

GTEST(TestStoreConstString) {
  std::string op = "";
  const bool ok = argparse::Parser::ParseComplete ==
    argparse::Parser("description")
    .StoreConst<std::string>("-store", op, "dog")
    .ParseArgs(argparse::Arguments("app.exe", { "-store" }), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_EQ("", output.str());
  EXPECT_EQ("", error.str());
  EXPECT_EQ("dog", op);
}


GTEST(TestSubParserBasic) {
  const bool ok = argparse::Parser::ParseComplete ==
    sub.ParseArgs(argparse::Arguments("app.exe", { "one", "-name", "dog" }), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_EQ("", output.str());
  EXPECT_EQ("", error.str());
  EXPECT_EQ("dog", sp1.name);
  EXPECT_EQ("", sp2.name);
}

GTEST(TestSubParserOptional) {
  std::string op = "";
  sub("-op", op);
  const bool ok = argparse::Parser::ParseComplete ==
    sub.ParseArgs(argparse::Arguments("app.exe", { "-op", "cat", "on", "-name", "dog" }), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_EQ("", output.str());
  EXPECT_EQ("", error.str());
  EXPECT_EQ("cat", op);
  EXPECT_EQ("dog", sp1.name);
  EXPECT_EQ("", sp2.name);
}

GTEST(TestSubParserPositional) {
  std::string op = "";
  sub("op", op);
  const bool ok = argparse::Parser::ParseComplete ==
    sub.ParseArgs(argparse::Arguments("app.exe", { "cat", "on", "-name", "dog" }), output, error);
  EXPECT_EQ(true, ok);
  EXPECT_EQ("", output.str());
  EXPECT_EQ("", error.str());
  EXPECT_EQ("cat", op);
  EXPECT_EQ("dog", sp1.name);
  EXPECT_EQ("", sp2.name);
}

// todo: test error
// todo: test help string
