#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <string>


/*  Using the stackoverflow method for calculation the cpu usage in percent
    Idle Times:

    PrevIdle = previdle + previowait
    Idle = idle + iowait (kIdle + kIOwait)
    
    Non Idle Times:

    PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal
    NonIdle = user + nice + system + irq + softirq + steal
    (kUser_ + kNice_ +  kSystem_ + kIRQ_ + kSoftIRQ_ + kSteal_ )
    Total Times:   
    PrevTotal = PrevIdle + PrevNonIdle
    Total = Idle + NonIdle
    
    Difference from Act - Prev:
    
    totaldiff = Total - PrevTotal
    idlediff = Idle - PrevIdle 

    Calculating the percentage = (total-idle) / total
    CPU_Percentage = (totaldiff - idlediff)/totaldiff
    */

Processor::Processor() {prevtotal_ = 0; previdle_ = 0;}

// Return the aggregate CPU utilization
float Processor::Utilization()
{	
  std::vector<std::string> times = LinuxParser::CpuUtilization();   

  idle_ = stol(times[LinuxParser::kIdle_]);
  io_wait_ = stol(times[LinuxParser::kIOwait_]);
  user_ = stol(times[LinuxParser::kUser_]);  
  nice_ = stol(times[LinuxParser::kNice_]);
  system_ = stol(times[LinuxParser::kSystem_]); 
  irq_ = stol(times[LinuxParser::kIRQ_]);
  soft_irq_ = stol(times[LinuxParser::kSoftIRQ_]); 
  steal_ = stol(times[LinuxParser::kSteal_]);
// idle, no idle and total times
  long idle = idle_ + io_wait_;
  long noidle = user_ + nice_ + system_ + irq_ + soft_irq_ + steal_;
  long total = idle + noidle;
 // delta times 
  long deltaidle = idle - previdle_;                 
  long deltatotal = total - prevtotal_;
 // calculate cpu util in percentage       
  float cpuPercentage = float(deltatotal - deltaidle)/deltatotal;
// setting private values previous
  previdle_ = idle;
  prevtotal_ = total;
  return cpuPercentage;
}