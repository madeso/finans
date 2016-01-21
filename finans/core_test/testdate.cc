// Copyright (2015) Gustav

#include "finans/core/datetime.h"

#include "gtest/gtest.h"

#define GTEST(x) GTEST_TEST(datetime, x)

GTEST(TestBasic) {
  DateTime dt(2001, 1, 1, 11, 12, 13);
  auto r = RawDateTime::FromLocalTime(dt);
  EXPECT_EQ(dt.DebugString(), r.ToLocalTime().DebugString());
  EXPECT_EQ(dt.DebugString(), Int64ToDateTime(DateTimeToInt64(r)).ToLocalTime().DebugString());
}
