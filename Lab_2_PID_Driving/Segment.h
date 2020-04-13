#include <Arduino.h>

#ifndef __SEGMENT_H
#define __SEGMENT_H

struct Segment{
  int leftTarget;
  int rightTarget;
  int dist;
};


int iSeg;
int countLimit;

const int numberOfSegments = 3;
Segment seg1 = {30,30,55};
Segment seg2 = {0,50,15};
Segment seg3 = {30,30,35};

Segment segments[numberOfSegments] = {seg1,seg2,seg3};


#endif
