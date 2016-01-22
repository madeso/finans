// Copyright (2015) Gustav

#include "finans/core/datetime.h"

#include "gtest/gtest.h"

#define GTEST(x) GTEST_TEST(datetime, x)

GTEST(TestBasic) {
  DateTime dt(2001, Month::JANUARY, 1, 11, 12, 13);
  EXPECT_EQ(dt.DebugString(), "2001-01-01 11:12:13");
}

GTEST(TestLocalTime) {
  DateTime dt(2001, Month::DECEMBER, 1, 11, 12, 13);
  auto r = RawDateTime::FromLocalTime(dt);
  EXPECT_EQ(dt.DebugString(), r.ToLocalTime().DebugString());
}


GTEST(TestIntParsing) {
  DateTime dt(2001, Month::DECEMBER, 1, 11, 12, 13);
  auto r = RawDateTime::FromLocalTime(dt);
  EXPECT_EQ(dt.DebugString(), Int64ToDateTime(DateTimeToInt64(r)).ToLocalTime().DebugString());
}
