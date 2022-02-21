#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// Generic function for finding a Value in a file indentified by key
template <typename T>
T LinuxParser::findValueByKey(std::string const &keyFilter, std::string const &filename)
{
    string line, key;
    T value;

    std::ifstream filestream(kProcDirectory + filename);
    if (filestream.fail())
    {
        std::cerr << "Error opening " << kProcDirectory + filename << std::endl;
        exit(1);
    }
    while (getline(filestream, line))
    {
        std::istringstream linestream(line);
        while (linestream >> key >> value)
        {
            if (key == keyFilter)
            {
                filestream.close();
                return value;
            }
        }
    }
    filestream.close();
    return value;
}

// Generic function for getting the first Value in a file or the first line if no whitespace
template <typename T>
T LinuxParser::getValueOfFile(string const &filename)
{
    string line;
    T value;

    std::ifstream filestream(kProcDirectory + filename);
    if (filestream.fail())
    {
        std::cerr << "Error opening " << kProcDirectory + filename << std::endl;
        exit(1);
    }
    getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> value;
    filestream.close();
    return value;
}

// Helper function to store the content of a line from a file in a vector
std::vector<std::string> LinuxParser::getVectorOfLine (std::string const &filename)
{
    string line;
    std::ifstream filestream(kProcDirectory + filename);
    if (filestream.fail())
    {
        std::cerr << "Error opening " << kProcDirectory + filename << std::endl;
        exit(1);
    }
    getline(filestream,line);
    std::istringstream buffer(line);
    // more elegant way than using for loop to fill the vector using interator as seen in the book
    //"C++ mit Visual Studio 2017 und Windows Forms-Anwendungen", page 730:
    // iterator to the beginning and end of the container stream and using the
    //constructor of the vector to fill the vector
    std::istream_iterator<string> start(buffer), end;
    std::vector<std::string> values(start, end);
    return values;
}

// Read and return the operating system name from the filesystem
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

// Read and return the kernel name from the filesystem
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

// Read and return the system memory utilization
// Formaula from hTop to calculate the utilization used:
// Used Memory = MemTotal - MemFree
float LinuxParser::MemoryUtilization()
{
    float memoryTotal = LinuxParser::findValueByKey<float>(filterMemTotal, kMeminfoFilename);
    float memoryFree = LinuxParser::findValueByKey<float>(filterMemFree, kMeminfoFilename);
    return float((memoryTotal - memoryFree) / memoryTotal);
}

// Read and return the system uptime
long LinuxParser::UpTime()
{
    long upTime = getValueOfFile<long>(kUptimeFilename);
    return upTime;
}

// Read and return the number of jiffies for the system
/* Number of jiffies are the number of ticks the system made since the last boot. 
To calculate it, two ingradients are needed: 
    - System uptime from LinuxParser::UpTime)
    - System frequency which is the number of ticks per second, from function sysconf(_SC_CLK_TCK)
Multiplying them returns the total jiffies.
*/
long LinuxParser::Jiffies()
{
    long uptime = LinuxParser::UpTime();
    long freq = sysconf(_SC_CLK_TCK);
    return uptime * freq;
}

// Read and return the number of active jiffies for a PID
/* we need from /proc/[pid]/stat:
(14) utime
(15) stime
(16) cutime
(17) cstime, measured in clock ticks (divide by sysconf(_SC_CLK_TCK).
 And sum them up
 */

long LinuxParser::ActiveJiffies(int pid)
{
    long activjif = 0;
    vector<string> times = getVectorOfLine(to_string(pid) + kStatFilename);
    // get the values #14 - # 17 (index is less one because vector start with 0)
    for (int i = 13; i <= 16; i++)
    {
        activjif += atol(times[i].c_str());
    }
    return activjif;
}

// Read and return the number of active jiffies for the system
/* extract the first line from the proc/stat file beginning with cpu by calling LinuxParser::CpuUtilization
Summing up the following ten tokens lead to the numener of ActiveJiffies.
*/
long LinuxParser::ActiveJiffies()
{
    long activjif = 0;
    string buffer;
    vector<string> times = LinuxParser::CpuUtilization();
    for (auto it = times.begin(); it != times.end(); ++it)
    {
        buffer = *it;
        activjif += atol(buffer.c_str());
    }
    return activjif;
}

// Read and return the number of idle jiffies for the system => token #4 from line cpu of the stat file
long LinuxParser::IdleJiffies()
{
    string buffer;
    long idle = 0;
    vector<string> time = LinuxParser::CpuUtilization();
    buffer = time[3];
    idle += atol(buffer.c_str());
    return idle;
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization()
{
    vector<string> times = getVectorOfLine(kStatFilename);
    // delete the first value of the vector, which is "cpu"
    times.erase(times.begin());
    return times;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses()
{
    int totalProcs = LinuxParser::findValueByKey<int>(filterProcesses, kStatFilename);
    return totalProcs;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses()
{
    int runningProcs = LinuxParser::findValueByKey<int>(filterRunningProcesses, kStatFilename);
    return runningProcs;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid)
{
    string command = LinuxParser::getValueOfFile<string>(to_string(pid) + kCmdlineFilename);
    // cut the command if longer than Display Parameter
    if (command.size() > kDisplayedCmdLength)
    {
        command.erase(command.begin() + kDisplayedCmdLength, command.end());
        command += "...";
    }
    return command;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid)
{
    long value = LinuxParser::findValueByKey<long>(filterProcMem, to_string(pid) + kStatusFilename);
    return to_string(value / 1024); // value in the file in kB, we want MB
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid)
{
    string value = LinuxParser::findValueByKey<string>(filterUID, to_string(pid) + kStatusFilename);
    return value;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid)
{
    string uid = LinuxParser::Uid(pid);
    string line, usr, pwd, ui = "";
    std::ifstream filestream(kPasswordPath);
    if (filestream.fail())
    {
        std::cerr << "Error opening /passwd file" << std::endl;
        exit(1);
    }
    while (std::getline(filestream, line))
    {
        std::replace(line.begin(), line.end(), ':', ' '); // replace, so the content can be separated due to whitespace
        std::istringstream linestream(line);
        while (linestream >> usr >> pwd >> ui)
        {
            if (ui == uid)
            { //we found the line matching to the given user ID
                filestream.close();
                return usr; //return the name of the usr of the active line and exit
            }
        }
    }
    filestream.close();
    return usr; // normally this code will not be executed
}

// Read and return the uptime of a process
// Process Uptime = Starttime of the system - Starttime of the Process
long LinuxParser::UpTime(int pid)
{
    vector<string> times = LinuxParser::getVectorOfLine(to_string(pid)+kStatFilename);
    // get the needed value startime #22 from the vector as float
    float procStartTimeInSec = (stof(times[21]))/sysconf(_SC_CLK_TCK);
    float sysStartTimeInSec = LinuxParser::UpTime();
    return (sysStartTimeInSec - procStartTimeInSec);
}

/* Method to calcullate CPU usage of a process as discribed in
   https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat
   used content of the statfile: 14 utime, 15 stime, 16 cutime, 17 cstime, 22 starttime.
   Numbers must be -1 to be used as index for the vector
 */
float LinuxParser::ProcCpuUtilization(int pid)
{
    vector<string> times = LinuxParser::getVectorOfLine(to_string(pid) + kStatFilename);
    // get the needed values from the vectoras float
    float upTime = LinuxParser::UpTime();
    float uTime = stof(times[13]);
    float sTime = stof(times[14]);
    float cuTime = stof(times[15]);
    float csTime = stof(times[16]);
    float startTime = stof(times[21]);
    float hertz = sysconf(_SC_CLK_TCK);
    // calculation
    float totalTime = uTime + sTime + cuTime + csTime;
    float seconds = upTime - (startTime / hertz);
    float cpuUsage = (totalTime / hertz) / seconds;
    return cpuUsage;
}
