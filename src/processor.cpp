#include "processor.h"

#include "linux_parser.h"
void Processor::Update(std::vector<int> stats) {
  user_ = stats[0];
  nice_ = stats[1];
  system_ = stats[2];
  idle_ = stats[3];
  iowait_ = stats[4];
  irq_ = stats[5];
  softirq_ = stats[6];
  steal_ = stats[7];
  guest_ = stats[8];
  guest_nice_ = stats[9];
}
int Processor::CPUTime() {
  return user_ + nice_ + system_ + idle_ + iowait_ + irq_ + softirq_ + steal_ +
         guest_ + guest_nice_;
}
int Processor::CPUIDle() { return idle_; }
float Processor::Utilization() {
  float utilization = 0.0;
  int prev_CPUTime = CPUTime();
  int prev_CPUIdle = CPUIDle();
  Update(LinuxParser::CpuUtilization());
  int current_CPUTime = CPUTime();
  int current_CPUIdle = CPUIDle();
  int cpu_delta = current_CPUTime - prev_CPUTime;
  int cpu_idle = current_CPUIdle - prev_CPUIdle;
  if (cpu_delta > 0) {
    utilization = (100 * (cpu_delta - cpu_idle) / cpu_delta);
  }
  return utilization / 100;
}