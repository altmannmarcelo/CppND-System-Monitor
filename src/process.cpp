#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "iostream"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {
  /* calculate cpu usage */
  long activeJiffies = LinuxParser::ActiveJiffies(Pid());
  long pUptime = LinuxParser::UpTime(Pid());
  long systemUptime = LinuxParser::UpTime();
  long timeDiff = systemUptime - (pUptime / sysconf(_SC_CLK_TCK));
  if (timeDiff <= 0) {
    cpu_usage_ = 0.0;
  } else {
    cpu_usage_ = (1.0 * (activeJiffies / sysconf(_SC_CLK_TCK)) / timeDiff);
  }
}

int Process::Pid() { return pid_; }

float Process::CpuUtilization() { return cpu_usage_; }

string Process::Command() { return LinuxParser::Command(Pid()); }

string Process::Ram() { return LinuxParser::Ram(Pid()); }

string Process::User() { return LinuxParser::User(Pid()); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  return cpu_usage_ > a.cpu_usage_;
}