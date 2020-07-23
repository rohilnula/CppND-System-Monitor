#include "processor.h"
#include "linux_parser.h"

Processor::Processor() {
    prev_active_jiffies = LinuxParser::ActiveJiffies();
    prev_jiffies = LinuxParser::Jiffies();
}

// Return the aggregate CPU utilization
float Processor::Utilization() { 
    float curr_jiffies = LinuxParser::Jiffies();
    float curr_active_jiffies = LinuxParser::ActiveJiffies();
    if (curr_jiffies - prev_jiffies == 0) {
        return 0;
    }
    float util = (curr_active_jiffies - prev_active_jiffies) / (curr_jiffies - prev_jiffies);
    prev_active_jiffies = curr_active_jiffies;
    prev_jiffies =  curr_jiffies;
    return util; 
}