#include <Arduino.h>

#ifndef __SEGMENT_H
#define __SEGMENT_H

struct Segment{
  int lMotorVal;
  int rMotorVal;
  int duration;
};


const int numberOfSegments = 3;
Segment seg1 = {200,200,1000};
Segment seg2 = {0,200,550};
Segment seg3 = {200,200,1000};

Segment segments[numberOfSegments] = {seg1,seg2,seg3};


#endif
