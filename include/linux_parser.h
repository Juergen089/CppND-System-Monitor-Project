#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

namespace LinuxParser {
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};

//Filters
const std::string filterProcesses("processes");
const std::string filterRunningProcesses("procs_running");
const std::string filterMemTotal("MemTotal:");
const std::string filterMemFree("MemFree:");
const std::string filterCpu("cpu");
const std::string filterUID("Uid:");
const std::string filterProcMem("VmRSS:"); // The string can be VmSize As well when using the Virtual Memory as used Mem for processes
  
//Display
const int kDisplayedCmdLength = 40;  

//Helper
template <typename T>
T findValueByKey(std::string const &keyFilter, std::string const &filename);
template <typename T>
T getValueOfFile(std::string const &filename);
std::vector<std::string> getVectorOfLine(std::string const &filename);
 
// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_ = 1,
  kSystem_ = 2,
  kIdle_ = 3,
  kIOwait_ = 4,
  kIRQ_ = 5,
  kSoftIRQ_ = 6,
  kSteal_ = 7,
  kGuest_ = 8,
  kGuestNice_ = 9
};

  std::vector<std::string> CpuUtilization();
long Jiffies();
long ActiveJiffies();
long ActiveJiffies(int pid);
long IdleJiffies();

// Processes
float ProcCpuUtilization(int pid); // function overloading does not work because of returning float
std::string Command(int pid);
std::string Ram(int pid);
std::string Uid(int pid);
std::string User(int pid);
long int UpTime(int pid);
  
// Memory
enum MemoryStates
{
  kMemoryTotal_ = 0,
  kMemoryFree_,
  kBuffers_,
  kCached_,
  kShmem_,
  kSReclaimable_,
};
};  // namespace LinuxParser

#endif