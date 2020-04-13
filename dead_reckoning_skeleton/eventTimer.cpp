#include "eventTimer.h"
/*
 * Constructor
 */
eventTimer::eventTimer() {

}

/*
 * Starts the timer
 * @param dur - sets the duration of the timer
 */
void eventTimer::start(uint32_t dur) {
  isRunning = true;
  startTime = millis();
  duration = dur;
}

/*
 * checks the timer to see if it the set amount of time has passed
 */
bool eventTimer::checkExpired() {
  if (isRunning) { //sees if the timer is even running
    if (millis() > (startTime + duration)) { //checks if the specified amount of time has passed
      isRunning = false;
      return true; //returns true and sets the timer to not running
    } else return false;
  }
  else return false;
}

bool eventTimer::getRunning(){
  return isRunning;
}

/*
 * ends the timer by setting isRunning to false
 */
void eventTimer::cancel() { 
  isRunning = false;
}
