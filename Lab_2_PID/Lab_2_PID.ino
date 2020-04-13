/*
   Code for using TCC4 for precision PID timing.
   You'll need to set TOP to set the interval

   This code adds the ability to tune the gains and change the targets
*/

#include <Zumo32U4Motors.h>
#include <Zumo32U4Encoders.h>

#include "params.h"
#include "serial_comm.h"

#include "button.h"

volatile uint8_t readyToPID = 0;   //a flag that is set when the PID timer overflows

const uint8_t buttonPin = 14; //button A on the Romi

Button buttonA(buttonPin, 10);

Zumo32U4Motors motors;

Zumo32U4Encoders encoders;
volatile int16_t countsLeft = 0;
volatile int16_t countsRight = 0;

static int16_t sumLeft = 0;


void setup()
{
  Serial.begin(115200);
  while (!Serial) {} //IF YOU DON'T COMMENT THIS OUT, YOU MUST OPEN THE SERIAL MONITOR TO START
  Serial.println("Hi.");

  noInterrupts(); //disable interupts while we mess with the Timer4 registers

  //sets up timer 4
  TCCR4A = 0x00; //disable some functionality -- no need to worry about this
  TCCR4B = 0x0C; //sets the prescaler -- look in the handout for values
  TCCR4C = 0x04; //toggles pin 6 at one-half the timer frequency
  TCCR4D = 0x00; //normal mode

  OCR4C = 132;   //TOP goes in OCR4C 2048
  TIMSK4 = 0x04; //enable overflow interrupt

  interrupts(); //re-enable interrupts

  buttonA.Init();


  //pinMode(6, OUTPUT); //COMMENT THIS OUT TO SHUT UP THE PIEZO!!!
}

void loop()
{

  if (buttonA.CheckButtonPress())
  {
    if (targetLeft == 50){
      targetLeft = 30;
    }
    else if (targetLeft = 30){
      targetLeft = 50;
    }
    else targetLeft = 30;
  }


  if (readyToPID) //timer flag set
  {
    //clear the timer flag
    readyToPID = 0;

    //for tracking previous counts
    static int16_t prevLeft = 0;
    static int16_t prevRight = 0;

    //error sum
    sumLeft = 0;


    /*
       Do PID stuffs here. Note that we turn off interupts while we read countsLeft/Right
       so that it won't get accidentally updated (in the ISR) while we're reading it.
    */
    noInterrupts();
    int16_t speedLeft = countsLeft - prevLeft; //speeds in encoder ticks
    int16_t speedRight = countsRight - prevRight;

  
    prevLeft = countsLeft;
    prevRight = countsRight;
    interrupts();

    int16_t errorLeft = targetLeft - speedLeft;

    if (bufferIndex < 99) {
      bufferCount[bufferIndex] = errorLeft;
      bufferIndex++;
    } else bufferIndex = 0;

    for (int i = 0; i < 99; i++) {
      sumLeft += bufferCount[i];
    }

    if(sumLeft > 170){
      sumLeft = 170;
    }
    else if(sumLeft < -135){
      sumLeft = -135;
    }


    float effortLeft = Kp * errorLeft + Ki * sumLeft;


    motors.setSpeeds(effortLeft, 0); //up to you to add the right motor

     //TESTING
    /*Serial.print('\t');
    Serial.print(Kp);
    Serial.print('\t');
    Serial.print(Ki);
    Serial.print('\t');
    Serial.print(speedLeft);
    Serial.print('\t');
    Serial.print(targetLeft);
    Serial.print('\t');
    Serial.print(errorLeft);
    Serial.print('\t');
    Serial.print(effortLeft);
    Serial.print('\t');
    Serial.print(sumLeft);
    Serial.print('\t');
    Serial.print('\n');*/
    //DEMO
    Serial.print(0);
    Serial.print('\t');
    Serial.print(speedLeft);
    Serial.print('\t');
    Serial.print(targetLeft);
    Serial.print('\n');
  }

  /* for reading in gain settings
     CheckSerialInput() returns true when it gets a complete string, which is
     denoted by a newline character ('\n'). Be sure to set your Serial Monitor to
     append a newline
  */
  if (CheckSerialInput()) {
    ParseSerialInput();
  }
}


float calcVel(int lastCounts) {
  return (((float)countsLeft - (float)lastCounts) / .017) * (0.019);
}

/*
   ISR for timing. Basically, raise a flag on overflow. Timer4 is set up to run with a pre-scaler
   of 1024 and TOP is set to 249. Clock is 16 MHz, so interval is dT = (1024 * 250) / 16 MHz = 16 ms.
*/
ISR(TIMER4_OVF_vect)
{
  //Capture a "snapshot" of the encoder counts for later processing
  countsLeft = encoders.getCountsLeft();
  countsRight = encoders.getCountsRight();

  readyToPID = 1;
}
