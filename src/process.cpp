#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// own constructor
Process::Process(int pid) {
  pid_ = pid; // pid_ as identifier for the container
  CpuUtilization(); /* initialize cpuutilization needed 
  for overloading the < operator. In the instructions the cpu utilization 
  seams to be the criteria for sorting the prosesses
  */
}

//Return this process's ID
int Process::Pid() { return pid_;} 

// Return this process's CPU utilization
float Process::CpuUtilization() { 
  cpuutilization_ = LinuxParser::ProcCpuUtilization(pid_);
  return cpuutilization_;
  }

// Return the command that generated this process
string Process::Command()  { return LinuxParser::Command(pid_); }

//Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// Overloading the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a)  { 
  return (this->cpuutilization_ < a.cpuutilization_); 
  }
  // compare if the cpu util of this container is smaller than 
  // of the container in the argument