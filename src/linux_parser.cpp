#include <dirent.h>
#include <unistd.h>
#include <sstream>
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
  string os, kernel, version;
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  	string line;
	float freemem;
  	float totmem;
  	float usedmem;
  	string value;
  	string key;
  	string memunit;
  	std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  	if (filestream.is_open()) {
    	while (std::getline(filestream, line)) {
      		std::istringstream linestream(line);
      		while(linestream >> key >> value >> memunit){
      			if(key == "MemTotal:") {
        			totmem = std::stof(value);
      			} 
      			if (key == "MemFree:"){
        			freemem = std::stof(value);
      			}
            }
         usedmem = (totmem-freemem)/totmem;
      	}
      
  	}
  	return usedmem;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
	string line;
  	long uptime;
  	std::ifstream filestream(kProcDirectory + kUptimeFilename);
  	if (filestream.is_open()) {
    	std::getline(filestream, line);
        std::istringstream linestream(line);
        linestream >> uptime;
  	}
  	return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return ActiveJiffies()+IdleJiffies(); }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  string line;
  long utime;
  long stime;
  long cutime;
  long cstime;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for(int i=0;i<14; ++i) {
      linestream >> utime >> stime>> cutime >> cstime;
    }
  }
  return (utime + stime + cutime + cstime) ;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
	string cpu;
  	string line;
  	long kuser;
  	long knice;
  	long ksystem;
  	std::ifstream stream(kProcDirectory + kVersionFilename);
  	if (stream.is_open()) {
      	std::getline(stream, line);
      	std::istringstream linestream(line);
      	linestream >> cpu >>kuser >> knice >> ksystem; 
    }
  	return kuser + knice + ksystem; 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  	string cpu;
  	string line;
  	long kuser;
  	long knice;
  	long ksystem;
  	long kidle;
  	long kiowait;
  	std::ifstream stream(kProcDirectory + kVersionFilename);
  	if (stream.is_open()) {
      	std::getline(stream, line);
      	std::istringstream linestream(line);
      	linestream >> cpu >>kuser >> knice >> ksystem >> kidle >> kiowait; 
    }
  	return kidle + kiowait; 
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  	vector<string> cpu_val;
  	string val1, val2, val3, val4, val5, val6, val7, val8, val9, val10;
  	string key;
  	string ncpu;
  	string line;
  	std::ifstream stream(kProcDirectory + kVersionFilename);
  	if (stream.is_open()) {
      	std::getline(stream, line);
      	std::istringstream linestream(line);
        while(linestream >> key >> val1>> val2>> val3>> val4>> val5>> val6>> val7>> val8>> val9>> val10){
          	if(key == "cpu"){
          		cpu_val.push_back(val1); cpu_val.push_back(val2); cpu_val.push_back(val3); cpu_val.push_back(val4); cpu_val.push_back(val5);
              	cpu_val.push_back(val6); cpu_val.push_back(val7); cpu_val.push_back(val8); cpu_val.push_back(val9); cpu_val.push_back(val10);
            }
    	}
    }
  	return cpu_val; 
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return std::stoi(value);
        }
      }
    }
  }
  return std::stoi(value);
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return stoi(value);
        }
      }
    }
  }
  return stoi(value);
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string cmdl;
  string line;

  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> cmdl; 
    }
  return cmdl;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          return value;
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }
  return value;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string line;
  string usernm;
  string userid;
  string uid = Uid(pid);
  string x;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> usernm >> x >> userid) {
        if (userid == uid) {
          return usernm;
        }
      }
    }
  }
  return usernm;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  string line;
  long pid_start;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for(int i=0;i<22; i++) {
      linestream >> pid_start;
    }
    //std::cout<< pid_start<<" ";
  }
  return pid_start / sysconf(_SC_CLK_TCK);
}

