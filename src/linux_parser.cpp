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
float LinuxParser::MemoryUtilization() {
  string line, key, value;
  float available_memory = -1.0, free_memory = -1.0;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          available_memory = stof(value);
        }
        else if (key == "MemFree:") {
          if (available_memory == 0) {
            return 0;
          }
          free_memory = stof(value);
          return ( (available_memory - free_memory) / available_memory);
        }
      }
    }
  }
  return 0.0; 
}

// Read and return the system uptime
long LinuxParser::UpTime() { 
  string line, uptime_string;
  float uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> uptime_string;
      uptime = stol(uptime_string);
      return uptime;
    }
  }
  return 0; 
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  string line, key, value;
  long total_jiffies = 0;
  std::ifstream stream(kProcDirectory + kStatFilename); 
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "cpu") {
          while (linestream >> value) {
            total_jiffies += stol(value);
          }
          return total_jiffies;
        }
      }
    }
  }
  return 0; 
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string line, key, value;
  long active_jiffies = 0;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename); 
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      for (int i = 0; i < 13; i++) {
        linestream >> key;
      }
      for (int i = 0; i < 4; i++) {
        linestream >> value;
        active_jiffies += stol(value);
      }
      return active_jiffies;
    }
  }
  return 0; 
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  return Jiffies() - IdleJiffies(); 
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  string line, key, value;
  long idle_jiffies = 0;
  std::ifstream stream(kProcDirectory + kStatFilename); 
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        if (key == "cpu") {
          int i = 0;
          while (linestream >> value) {
            i++;
            if (i == 4 || i == 5) {
              idle_jiffies += stol(value);
            }
          }
          return idle_jiffies;
        }
      }
    }
  }
  return 0;  
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization(int pid) {
  string line, key, value;
  vector<string> cpu_utilization;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename); 
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      int i = 0;
      while (linestream >> value) {
        i++;
        if (i == 14 || i == 15 || i == 16 || i == 17 || i == 22) {
          cpu_utilization.push_back(to_string(stol(value) / sysconf(_SC_CLK_TCK)));
        }
      }
      return cpu_utilization; 
    }
  }
  return {}; 
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line, key, value;
  std::ifstream stream(kProcDirectory + kStatFilename); 
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return stoi(value);
        }
      }
    }
  } 
  return 0; 
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line, key, value;
  std::ifstream stream(kProcDirectory + kStatFilename); 
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return stoi(value);
        }
      }
    }
  }
  return 0; 
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string line, key, value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename); 
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> value) {
        return value;
      }
    }
  }
  return string(); 
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string line, key, value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename); 
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize") {
          return to_string(stol(value)/1000);
        }
      }
    }
  }
  return string(); 
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string line, key, value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename); 
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid") {
          return value;
        }
      }
    }
  }
  return string(); 
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string line, user, uid, junk;
  string user_id = Uid(pid);
  std::ifstream stream(kPasswordPath); 
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> junk >> uid) {
        if (uid == user_id) {
          return user;
        }
      }
    }
  }
  return string(); 
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  string line, key, value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename); 
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      int i = 0;
      while (linestream >> value) {
        i++;
        if (i == 22) {
          return stol(value);
        }
      } 
    }
  }
  return 0;  
}