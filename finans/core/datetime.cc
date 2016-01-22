// Copyright (2015) Gustav

#include "finans/core/datetime.h"

#include <vector>
#include <cassert>

int MonthToInt(Month month) {
  return static_cast<int>(month);
}

Month IntToMonth(int m) {
  assert(m >= MonthToInt(Month::JANUARY) && m <= MonthToInt(Month::DECEMBER));
  return static_cast<Month>(m);
}

//////////////////////////////////////////////////////////////////////////

TimetWrapper::TimetWrapper(time_t time) : time_(time) {
}

TimetWrapper TimetWrapper::FromLocalTime(const StructTmWrapper& dt) {
  struct tm tt = dt.time();
  return TimetWrapper(mktime(&tt));
}

TimetWrapper TimetWrapper::FromGmt(const StructTmWrapper& dt) {
  // http://stackoverflow.com/questions/283166/easy-way-to-convert-a-struct-tm-expressed-in-utc-to-time-t-type
#ifdef FINANS_WINDOWS
  struct tm tt = dt.time();
  return TimetWrapper(_mkgmtime(&tt));
#else
#error "undefined platform"
#endif
}

TimetWrapper TimetWrapper::CurrentTime() {
  return TimetWrapper(time(NULL));
}

double TimetWrapper::Difference(const TimetWrapper& start, const TimetWrapper& end) {
  return difftime(end.time_, start.time_);
}

StructTmWrapper TimetWrapper::ToLocalTime() const {
  return StructTmWrapper(*localtime(&time_));
}

StructTmWrapper TimetWrapper::ToGmt() const {
  return StructTmWrapper(*gmtime(&time_));
}

//////////////////////////////////////////////////////////////////////////

StructTmWrapper::StructTmWrapper(struct tm time) : time_(time) {
}

struct tm StructTmWrapper::time() const {
  return time_;
}

StructTmWrapper::StructTmWrapper(int year, Month month, int day) {
  memset(&time_, 0, sizeof(struct tm));
  set_year(year);
  set_month(month);
  set_day_of_moth(day);
  set_hour(12);
  set_minutes(0);
  set_seconds(0);
}

StructTmWrapper::StructTmWrapper(int year, Month month, int day, int hour, int minute, int second, bool dst) {
  memset(&time_, 0, sizeof(struct tm));
  set_year(year);
  set_month(month);
  set_day_of_moth(day);
  set_hour(hour);
  set_minutes(minute);
  set_seconds(second);
  set_dst(dst ? DstInfo::IN_EFFECT : DstInfo::NOT_IN_EFFECT);
}

void StructTmWrapper::set_seconds(int seconds) { time_.tm_sec = seconds; }

void StructTmWrapper::set_minutes(int minutes) { time_.tm_min = minutes; }

void StructTmWrapper::set_hour(int hour) { time_.tm_hour = hour; }

void StructTmWrapper::set_day_of_moth(int day_of_moth) { time_.tm_mday = day_of_moth; }

void StructTmWrapper::set_month(Month month) { time_.tm_mon = MonthToInt(month); }

void StructTmWrapper::set_year(int year) { time_.tm_year = year - 1900; }

void StructTmWrapper::set_dst(DstInfo dst) {
  switch (dst) {
  case DstInfo::IN_EFFECT:
    time_.tm_isdst = 1;
    return;
  case DstInfo::NOT_IN_EFFECT:
    time_.tm_isdst = 0;
    return;
  case DstInfo::INFO_UNAVAILABLE:
    time_.tm_isdst = -1;
    return;
  }
  assert(0 && "unhandled dst value switch");
}

int StructTmWrapper::seconds() const { return time_.tm_sec; }

int StructTmWrapper::minutes() const { return time_.tm_min; }

int StructTmWrapper::hour() const { return time_.tm_hour; }

int StructTmWrapper::day_of_moth() const { return time_.tm_mday; }

Month StructTmWrapper::month() const { return IntToMonth(time_.tm_mon); }

int StructTmWrapper::year() const { return time_.tm_year + 1900; }

DstInfo StructTmWrapper::dst() const {
  if (time_.tm_isdst == 0) return DstInfo::NOT_IN_EFFECT;
  if (time_.tm_isdst > 0) return DstInfo::IN_EFFECT;
  else return DstInfo::INFO_UNAVAILABLE;
}

std::string StructTmWrapper::ToString(const std::string& format) const {
  std::vector<char> ret;
  const size_t base_size = 100;
  for (int i = 1; i < 100; ++i) {
    const size_t string_length = base_size * i;
    ret.resize(string_length);
    const auto characters_written = strftime(&ret[0], string_length, format.c_str(), &time_);
    if (characters_written != 0) return &ret[0];
  }

  return "";
}

std::string StructTmWrapper::DebugString() const {
  return ToString("%Y-%m-%d %H:%M:%S");
}

/*
tm_sec	int	seconds after the minute	0 - 60 *
tm_min	int	minutes after the hour	0 - 59
tm_hour	int	hours since midnight	0 - 23
tm_mday	int	day of the month	1 - 31
tm_mon	int	months since January	0 - 11
tm_year	int	years since 1900
tm_isdst	int	Daylight Saving Time flag

The values of the members tm_wday and tm_yday of timeptr are ignored, and the values
of the other members are interpreted even if out of their valid ranges(see struct tm).

tm_wday	int	days since Sunday	0 - 6
tm_yday	int	days since January 1	0 - 365
*/

uint64_t DateTimeToInt64(const TimetWrapper& dt) {
  const auto diff = TimetWrapper::Difference(TimetWrapper::FromGmt(StructTmWrapper(1970, Month::JANUARY ,1, 0, 0, 0)), dt);
  return static_cast<uint64_t>(diff);
}

TimetWrapper Int64ToDateTime(uint64_t i) {
  const uint64_t minutes = (i - (i % 60)) / 60;
  const int actual_seconds = i % 60;
  const uint64_t hours = (minutes - (minutes % 60)) / 60;
  const int acutal_minutes = minutes % 60;
  return TimetWrapper::FromGmt(StructTmWrapper(1970, Month::JANUARY, 1, hours, acutal_minutes, actual_seconds));
}
