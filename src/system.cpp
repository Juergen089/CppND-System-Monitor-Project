#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
vector<Process>& System::Processes() { 

  processes_.clear();
  vector<int> allProcessIds = LinuxParser::Pids();
  for (auto procID : allProcessIds) //walk through the processes
  {
    processes_.emplace_back(procID); //create a container for each PID. Emplace_back also calls the constructor
  }
  std::sort(processes_.rbegin(), processes_.rend());
  return processes_; 
  }

// Return the system's kernel identifier (string)
std::string System::Kernel() { 
    string kernel = LinuxParser::Kernel();
    return kernel;
}

// Return the system's memory utilization
float System::MemoryUtilization() { 
    float memUtil = LinuxParser::MemoryUtilization();
    return memUtil;}

// Return the operating system name
std::string System::OperatingSystem() { 
    string os = LinuxParser::OperatingSystem();
    return os;
}

// Return the number of processes actively running on the system
int System::RunningProcesses() { 
    int runningProcs = LinuxParser::RunningProcesses();
    return runningProcs; 
}

// Return the total number of processes on the system
int System::TotalProcesses() { 
    int totalProcs = LinuxParser::TotalProcesses();
    return totalProcs; 
}

//Return the number of seconds since the system started running
long int System::UpTime() { 
    long sysUpTime = LinuxParser::UpTime();
    return sysUpTime; 
}