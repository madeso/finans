// Copyright (2015) Gustav

#include "finans/core/commandline.h"

#include "gtest/gtest.h"

#define GTEST(x) GTEST_TEST(stringconverter, x)

GTEST(TestBasic) {
  const auto strings = argparse::StringConverter<int>{"animals"}("dog", 1)("cat", 2);
  EXPECT_EQ(1, strings.Convert("d"));
}

GTEST(TestExact) {
  const auto strings = argparse::StringConverter<int>{ "animals" }("dog", 1)("doggy", 5)("cat", 2);
  EXPECT_EQ(1, strings.Convert("dog"));
}
