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

GTEST(TestChangeMembers) {
  auto dt = DateTime::FromDate(2010, Month::FEBRUARY, 2, TimeZone::LOCAL);
  dt.set_year(2012);
  dt.set_month(Month::JUNE);
  dt.set_day_of_moth(10);
  dt.set_hour(8);
  dt.set_minutes(33);
  dt.set_seconds(42);
  EXPECT_EQ(2012, dt.year());
  EXPECT_EQ(Month::JUNE, dt.month());
  EXPECT_EQ(10, dt.day_of_moth());
  EXPECT_EQ(8, dt.hour());
  EXPECT_EQ(33, dt.minutes());
  EXPECT_EQ(42, dt.seconds());
}
