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
using std::stof;

// Constructor
Process::Process (int processId) {
    processId_ = processId;
}

// Return this process's ID
int Process::Pid() const { return processId_; }

// Return this process's CPU utilization
float Process::CpuUtilization() const { 
    vector<string> cpu_util = LinuxParser::CpuUtilization(Pid());
    if (cpu_util.size() < 5) {
        return 0;
    }
    float up_time = LinuxParser::UpTime() - stof(cpu_util[4]);
    // (float) ((LinuxParser::UpTime() - LinuxParser::UpTime(Pid())) / sysconf(_SC_CLK_TCK));
    float tot_time = 0;
    for (int i = 0; i < 4; i++) {
        tot_time += stof(cpu_util[i]);
    }
    tot_time /= sysconf(_SC_CLK_TCK);
    return std::abs(tot_time/up_time); 
}

// Return the command that generated this process
string Process::Command() const { return LinuxParser::Command(Pid()); }

// Return this process's memory utilization
string Process::Ram() const { return LinuxParser::Ram(Pid()); }

// Return the user (name) that generated this process
string Process::User() const { return LinuxParser::User(Pid()); }

// Return the age of this process (in seconds)
long int Process::UpTime() const { 
    vector<string> cpu_util = LinuxParser::CpuUtilization(Pid());
    if (cpu_util.size() < 5) {
        return 0;
    }
    long up_time = LinuxParser::UpTime() - stof(cpu_util[4]);
    return up_time;
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
    return CpuUtilization() > a.CpuUtilization(); 
}