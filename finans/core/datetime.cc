// Copyright (2015) Gustav

#include "finans/core/datetime.h"

#include <vector>
#include <cassert>

RawDateTime::RawDateTime(time_t time) : time_(time) {
}

RawDateTime RawDateTime::FromLocalTime(const DateTime& dt) {
  struct tm tt = dt.time();
  return RawDateTime(mktime(&tt));
}

RawDateTime RawDateTime::FromGmt(const DateTime& dt) {
#ifdef FINANS_WINDOWS
  struct tm tt = dt.time();
  return RawDateTime(_mkgmtime(&tt));
#else
#error "undefined platform"
#endif
}

RawDateTime RawDateTime::CurrentTime() {
  return RawDateTime(time(NULL));
}

double RawDateTime::Difference(const RawDateTime& start, const RawDateTime& end) {
  return difftime(end.time_, start.time_);
}

DateTime RawDateTime::ToLocalTime() const {
  return DateTime(*localtime(&time_));
}

DateTime RawDateTime::ToGmt() const {
  return DateTime(*gmtime(&time_));
}

//////////////////////////////////////////////////////////////////////////

DateTime::DateTime(struct tm time) : time_(time) {
}

struct tm DateTime::time() const {
  return time_;
}

DateTime::DateTime(int year, int month, int day) {
  memset(&time_, 0, sizeof(struct tm));
  set_year(year);
  set_month(month);
  set_day_of_moth(day);
  set_hour(12);
  set_minutes(0);
  set_seconds(0);
}

DateTime::DateTime(int year, int month, int day, int hour, int minute, int second, bool dst) {
  memset(&time_, 0, sizeof(struct tm));
  set_year(year);
  set_month(month);
  set_day_of_moth(day);
  set_hour(hour);
  set_minutes(minute);
  set_seconds(second);
  set_dst(dst ? DstInfo::IN_EFFECT : DstInfo::NOT_IN_EFFECT);
}

void DateTime::set_seconds(int seconds) { time_.tm_sec = seconds; }

void DateTime::set_minutes(int minutes) { time_.tm_min = minutes; }

void DateTime::set_hour(int hour) { time_.tm_hour = hour; }

void DateTime::set_day_of_moth(int day_of_moth) { time_.tm_mday = day_of_moth; }

void DateTime::set_month(int month) { time_.tm_mon = month; }

void DateTime::set_year(int year) { time_.tm_year = year - 1900; }

void DateTime::set_dst(DstInfo dst) {
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

int DateTime::seconds() const { return time_.tm_sec; }

int DateTime::minutes() const { return time_.tm_min; }

int DateTime::hour() const { return time_.tm_hour; }

int DateTime::day_of_moth() const { return time_.tm_mday; }

int DateTime::month() const { return time_.tm_mon; }

int DateTime::year() const { return time_.tm_year + 1900; }

DstInfo DateTime::dst() const {
  if (time_.tm_isdst == 0) return DstInfo::NOT_IN_EFFECT;
  if (time_.tm_isdst > 0) return DstInfo::IN_EFFECT;
  else return DstInfo::INFO_UNAVAILABLE;
}

std::string DateTime::ToString(const std::string& format) const {
  std::vector<char> ret;
  const size_t base_size = 100;
  for (int i = 1; i < 100; ++i) {
    const size_t string_length = base_size * i;
    ret.reserve(string_length);
    const auto characters_written = strftime(&ret[0], string_length, format.c_str(), &time_);
    if (characters_written != 0) return &ret[0];
  }

  return "";
}

std::string DateTime::DebugString() const {
  return asctime(&time_);
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

uint64_t DateTimeToInt64(const RawDateTime& dt) {
  const auto diff = RawDateTime::Difference(RawDateTime::FromGmt(DateTime(1970, 0 ,1, 0, 0, 0)), dt);
  return static_cast<uint64_t>(diff);
}

RawDateTime Int64ToDateTime(uint64_t i) {
  const uint64_t minutes = (i - (i % 60)) / 60;
  const int actual_seconds = i % 60;
  const uint64_t hours = (minutes - (minutes % 60)) / 60;
  const int acutal_minutes = minutes % 60;
  return RawDateTime::FromGmt(DateTime(1970, 0, 1, hours, acutal_minutes, actual_seconds));
}
