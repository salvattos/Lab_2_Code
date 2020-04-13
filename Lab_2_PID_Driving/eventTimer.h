#include <Arduino.h>

#ifndef __EVENTTIMER_H
#define __EVENTTIMER_H

class eventTimer {
  private: //variables to be used later
    
    uint32_t startTime;
    
    
  public: //fuction definitions
    uint32_t duration;
    bool isRunning = false;
    eventTimer(void);
    void start(uint32_t timeMS = 500); //default timer is .5 seconds
    bool checkExpired(void);
    void cancel(void);
    bool getRunning(void);
};
#endif
