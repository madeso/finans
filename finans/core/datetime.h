// Copyright (2015) Gustav

#ifndef CORE_DATETIME_H_
#define CORE_DATETIME_H_

#include <ctime>
#include <string>
#include <cstdint>

class RawDateTime;
class DateTime;

class RawDateTime {
protected:
  friend class DateTime;
  explicit RawDateTime(time_t time);
public:
  static RawDateTime FromLocalTime(const DateTime& dt);
  static RawDateTime FromGmt(const DateTime& dt);
  static RawDateTime CurrentTime();

  static double Difference(const RawDateTime& start, const RawDateTime& end);

  DateTime ToLocalTime() const;
  DateTime ToGmt() const;
private:
  time_t time_;
};

enum class DstInfo {
  IN_EFFECT, NOT_IN_EFFECT, INFO_UNAVAILABLE
};

class DateTime {
protected:
  friend class RawDateTime;
  explicit DateTime(struct tm time);
  struct tm time() const;

public:
  DateTime(int year, int month, int day);
  DateTime(int year, int month, int day, int hour, int minute, int second, bool dst=false);

  void set_seconds(int seconds);
  void set_minutes(int minutes);
  void set_hour(int hour);
  void set_day_of_moth(int day_of_moth);
  void set_month(int month);
  void set_year(int year);
  void set_dst(DstInfo dst);

  int seconds() const;
  int minutes() const;
  int hour() const;
  int day_of_moth() const;
  int month() const;
  int year() const;
  DstInfo dst() const;

  // format: http://www.cplusplus.com/reference/ctime/strftime/
  std::string ToString(const std::string& format) const;
  std::string DebugString() const;

private:
  struct tm time_;
};

// unix date time format, 64 bit
// todo: test 2038 problem
uint64_t DateTimeToInt64(const RawDateTime& dt);
RawDateTime Int64ToDateTime(uint64_t i);

#endif  // CORE_PROTO_H_
