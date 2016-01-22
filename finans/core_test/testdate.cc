// Copyright (2015) Gustav

#include "finans/core/datetime.h"

#include "gtest/gtest.h"

#define GTEST(x) GTEST_TEST(datetime, x)

GTEST(TestBasic) {
  StructTmWrapper dt(2001, Month::JANUARY, 1, 11, 12, 13);
  EXPECT_EQ(dt.DebugString(), "2001-01-01 11:12:13");
}

GTEST(TestLocalTime) {
  StructTmWrapper dt(2001, Month::DECEMBER, 1, 11, 12, 13);
  auto r = TimetWrapper::FromLocalTime(dt);
  EXPECT_EQ(dt.DebugString(), r.ToLocalTime().DebugString());
}


GTEST(TestIntParsing) {
  StructTmWrapper dt(2001, Month::DECEMBER, 1, 11, 12, 13);
  auto r = TimetWrapper::FromLocalTime(dt);
  EXPECT_EQ(dt.DebugString(), Int64ToDateTime(DateTimeToInt64(r)).ToLocalTime().DebugString());
}

//////////////////////////////////////////////////////////////////////////

GTEST(TestConstructorGmt) {
  const auto dt = DateTime::FromDate(2010, Month::FEBRUARY, 2, TimeZone::GMT);
  EXPECT_EQ(2010, dt.year());
  EXPECT_EQ(Month::FEBRUARY, dt.month());
  EXPECT_EQ(2, dt.day_of_moth());
  EXPECT_EQ(12, dt.hour());
}

GTEST(TestConstructorLocal) {
  const auto dt = DateTime::FromDate(2010, Month::FEBRUARY, 2, TimeZone::LOCAL);
  EXPECT_EQ(2010, dt.year());
  EXPECT_EQ(Month::FEBRUARY, dt.month());
  EXPECT_EQ(2, dt.day_of_moth());
  EXPECT_EQ(12, dt.hour());
}
