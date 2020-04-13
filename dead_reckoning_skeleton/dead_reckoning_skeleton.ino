#include <Zumo32U4Motors.h>
#include <Zumo32U4Encoders.h>

#include "button.h"       //include your button class from last week
#include "eventTimer.h"  //include your shiny, new event timer class
#include "Segment.h"

Button buttonA(14); //button A is pin 14 on the Zumo
eventTimer timer;   //assumes you named your class EventTimer

//use the Pololu libraries for motors and encoders
Zumo32U4Motors motors;
Zumo32U4Encoders encoders; //(we're not acutally using this in this code, but we will soon)

//declare the robot states here
enum MOTOR_STATE  {IDLE, DRIVING};
MOTOR_STATE state = IDLE;

eventTimer myTimer;//Declare timer

void setup()
{
  Serial.begin(115200);
  Serial.println("Hello.");

  buttonA.Init(); //don't forget to call Init()!
}

void loop()
{
  switch (state) {
    case IDLE:
      Serial.println("idling");
      delay(100);
      motors.setSpeeds(0, 0);
      if (handleButtonPress(buttonA)) state = DRIVING; //for some reason using a handler causes that function to break :/
      if (buttonA.CheckButtonPress()) {
        state = DRIVING;
        delay(200); //delay so i can move my finger off of the zumo
      }
      break;
    case DRIVING:

      myTimer.start(segments[0].duration);
      Drive(0);

      myTimer.start(segments[1].duration);
      Drive(1);

      myTimer.start(segments[2].duration);
      Drive(2);

      state = IDLE;
      break;
  }
}

//add you handlers here:
//handle the button press
bool handleButtonPress(Button button) {
  return button.CheckButtonPress();
}

//handle the timer expiring
bool handleTimerExp(eventTimer timer) {
  return timer.checkExpired();
}

bool handleTimerTransition(eventTimer timer) {
  Serial.println(timer.getRunning());
  return timer.getRunning();
}

//you'll want a function called Drive() to actually implement the motor commands and start the timer
void Drive(int iSeg) //pass it the number of the Segment you wish to execute
{
  while (!(myTimer.checkExpired())) {
    motors.setSpeeds(segments[iSeg].rMotorVal, segments[iSeg].lMotorVal);
  }



}
