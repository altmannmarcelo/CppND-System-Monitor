#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

#include "iostream"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}
float LinuxParser::Kernel_version(string kernel_string) {
  string majour, minor;
  string version;
  std::replace(kernel_string.begin(), kernel_string.end(), '.', ' ');
  std::istringstream linestream(kernel_string);
  linestream >> majour >> minor;
  version = majour + "." + minor;
  return std::stof(version);
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization(float kernel_version) {
  string line;
  string key;
  string value;
  int memTotal;
  int MemAvailable;
  float memUsed = 0.0f;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          memTotal = std::stoi(value);
        } else if (kernel_version >= 3.14) {
          /*
           * MemAvailable is only avaialable on kernel 3.14 onwards. Its more
           * acurate than just MemFree
           * https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=34e431b0a
           */
          if (key == "MemAvailable:") {
            MemAvailable = std::stoi(value);
            memUsed = (float)(memTotal - MemAvailable) * 100 / memTotal / 100;
            return memUsed;
          }
        } else {
          if (key == "MemFree:") {
            MemAvailable = std::stoi(value);
            memUsed = (float)(memTotal - MemAvailable) * 100 / memTotal / 100;
            return memUsed;
          }
        }
      }
    }
  }
  return memUsed;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  long uptime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string utime, stime, cutime, cstime, starttime;
  utime = parsePStat(14, pid);
  stime = parsePStat(15, pid);
  cutime = parsePStat(16, pid);
  cstime = parsePStat(17, pid);
  return (long)stol(utime) + stol(stime) + stol(cutime) + stol(cstime);
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// TODO: Read and return CPU utilization
vector<int> LinuxParser::CpuUtilization() {
  string cpu, user, nice, system, idle, iowait, irq, softirq, steal, guest,
      guest_nice;
  string line;
  vector<int> proc;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >>
        softirq >> steal >> guest >> guest_nice;
  }
  proc.push_back(stoi(user));
  proc.push_back(stoi(nice));
  proc.push_back(stoi(system));
  proc.push_back(stoi(idle));
  proc.push_back(stoi(iowait));
  proc.push_back(stoi(irq));
  proc.push_back(stoi(softirq));
  proc.push_back(stoi(steal));
  proc.push_back(stoi(guest));
  proc.push_back(stoi(guest_nice));
  return proc;
}

// TODO: Read and return the total number of processes
int LinuxParser::parseStat(string iKey) {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == iKey) {
          return stoi(value);
        }
      }
    }
  }
  return 0;
}
int LinuxParser::TotalProcesses() { return parseStat("processes"); }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { return parseStat("procs_running"); }

string LinuxParser::Command(int pid) {
  string line, cmd;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cmd;
  }
  return cmd;
}

string LinuxParser::parsePStatus(string iKey, int pid) {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == iKey) {
          return value;
        }
      }
    }
  }
  return "0";
}
string LinuxParser::parsePStat(int column, int pid) {
  string line;
  vector<std::string> values{};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    std::string val;
    while (linestream >> val) values.push_back(val);

    return values[column - 1];
  }
  return "0";
}
string LinuxParser::Ram(int pid) {
  char rambuf[20];
  string ret;
  string ram = parsePStatus("VmSize:", pid);
  float mb = stof(ram) * 0.001;
  snprintf(rambuf, sizeof(rambuf), "%.2f", mb);
  ret = rambuf;
  return ret;
}

// TODO: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { return parsePStatus("Uid:", pid); }

// TODO: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line;
  string user, mode, pwduid;
  string uid = Uid(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> mode >> pwduid) {
        if (uid == pwduid) {
          return user;
        }
      }
    }
  }
  return string();
}

// TODO: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string sUptime = parsePStat(22, pid);
  long systemUptime = UpTime();
  long uptime = systemUptime - (std::stol(sUptime) / sysconf(_SC_CLK_TCK));
  return uptime;
}