#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  Processor();
  float Utilization();  // See src/processor.cpp

  // Declare any necessary private members
 private:
   float prev_active_jiffies = 0;
   float prev_jiffies = 0;
};

#endif