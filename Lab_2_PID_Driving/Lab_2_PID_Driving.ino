#include <Zumo32U4Motors.h>
#include <Zumo32U4Encoders.h>

#include "button.h"       //include your button class from last week
#include "eventTimer.h"  //include your shiny, new event timer class
#include "Segment.h"

#include "params.h"
#include "serial_comm.h"

Button buttonA(14); //button A is pin 14 on the Zumo
eventTimer timer;   //assumes you named your class EventTimer

//use the Pololu libraries for motors and encoders
Zumo32U4Motors motors;
Zumo32U4Encoders encoders; //(we're not acutally using this in this code, but we will soon)

//declare the robot states here
enum MOTOR_STATE  {IDLE, DRIVING};
MOTOR_STATE state = IDLE;

eventTimer myTimer;//Declare timer

volatile uint8_t readyToPID = 0;

volatile int16_t countsLeft = 0;
volatile int16_t countsRight = 0;

static int16_t sumLeft = 0;
static int16_t sumRight = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("Hello.");

  iSeg = 0;
  countLimit += segments[iSeg].dist * 65;

  noInterrupts(); //disable interupts while we mess with the Timer4 registers

  //sets up timer 4
  TCCR4A = 0x00; //disable some functionality -- no need to worry about this
  TCCR4B = 0x0C; //sets the prescaler -- look in the handout for values
  TCCR4C = 0x04; //toggles pin 6 at one-half the timer frequency
  TCCR4D = 0x00; //normal mode

  OCR4C = 132;   //TOP goes in OCR4C
  TIMSK4 = 0x04; //enable overflow interrupt

  interrupts(); //re-enable interrupts


  buttonA.Init(); //don't forget to call Init()!
}

void loop()
{
  switch (state) {
    case IDLE:
      motors.setSpeeds(0, 0);
      if (buttonA.CheckButtonPress()) {
        state = DRIVING;
        Serial.println("idling");

        delay(200); //delay so i can move my finger off of the zumo
      }
      break;
    case DRIVING:

      targetLeft = segments[iSeg].leftTarget;
      targetRight = segments[iSeg].rightTarget;
      if (readyToPID) //timer flag set
      {
        //clear the timer flag
        readyToPID = 0;

        //for tracking previous counts
        static int16_t prevLeft = 0;
        static int16_t prevRight = 0;

        //error sum
        sumLeft = 0;
        sumRight = 0;

        /*
           Do PID stuffs here. Note that we turn off interupts while we read countsLeft/Right
           so that it won't get accidentally updated (in the ISR) while we're reading it.
        */
        noInterrupts();
        int16_t speedLeft = countsLeft - prevLeft;
        int16_t speedRight = countsRight - prevRight;

        prevLeft = countsLeft;
        prevRight = countsRight;
        interrupts();

        int16_t errorLeft = targetLeft - speedLeft;
        int16_t errorRight = targetRight - speedRight;



        if (bufferIndexRight < 99) {
          bufferCountRight[bufferIndexRight] = errorLeft;
          bufferIndexRight++;
        } else bufferIndexRight = 0;

        for (int i = 0; i < 99; i++) {
          sumRight += bufferCountRight[i];
        }

        if (sumRight > 170) {
          sumRight = 170;
        }
        else if (sumRight < -135) {
          sumRight = -135;
        }


        if (bufferIndexLeft < 99) {
          bufferCountLeft[bufferIndexLeft] = errorLeft;
          bufferIndexLeft++;
        } else bufferIndexLeft = 0;


        for (int i = 0; i < 99; i++) {
          sumLeft += bufferCountLeft[i];
        }

        if (sumLeft > 170) {
          sumLeft = 170;
        }
        else if (sumLeft < -135) {
          sumLeft = -135;
        }


        float effortLeft = Kp * errorLeft + Ki * sumLeft;
        float effortRight = Kp * errorRight + Ki * sumRight;


        motors.setSpeeds(effortLeft, effortRight);

        if (countsRight > countLimit) {
          if (iSeg < 2) {
            Serial.println("Updating iSeg");
            Serial.println(iSeg);
            iSeg++;
            countLimit += segments[iSeg].dist * 65;
          }
          else {
            iSeg = 0;
            motors.setSpeeds(0, 0);
            state = IDLE;
          }
        }

        break;
      }
  }
}

ISR(TIMER4_OVF_vect)
{
  //Capture a "snapshot" of the encoder counts for later processing
  countsLeft = encoders.getCountsLeft();
  countsRight = encoders.getCountsRight();

  readyToPID = 1;
}
