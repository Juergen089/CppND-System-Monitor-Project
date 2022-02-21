#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  Processor();
  float Utilization();  // TODO: See src/processor.cpp

    // TODO: Declare any necessary private members
    private:
      long user_;
      long nice_;
      long system_;
      long idle_;
      long io_wait_;
      long irq_;
      long soft_irq_;
      long steal_;
  // previous times
      long previdle_ ;
      long prevtotal_;
};

#endif