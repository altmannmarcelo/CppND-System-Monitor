#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <vector>
using std::vector;
class Processor {
 public:
  // Processor() {};
  float Utilization();
  void Update(std::vector<int>);

  // TODO: Declare any necessary private members
 private:
  int user_, nice_, system_, idle_, iowait_, irq_, softirq_, steal_, guest_,
      guest_nice_;
  int CPUTime();
  int CPUIDle();
};

#endif