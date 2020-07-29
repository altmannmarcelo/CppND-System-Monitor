#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  char timebuf[20];
  string formatted;
  int hour = seconds / 3600;
  seconds %= 3600;
  int minutes = seconds / 60;
  seconds %= 60;
  int second = seconds;
  snprintf(timebuf, sizeof(timebuf), "%.2d:%.2d:%.2d", hour, minutes, second);
  formatted = timebuf;
  return formatted;
}