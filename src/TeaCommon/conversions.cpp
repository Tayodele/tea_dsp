#define _USE_MATH_DEFINES
#include "conversions.h"
#include <math.h>
#include <cmath>
#include <stdlib.h>


using namespace TTModules;
using namespace std;

void TTModules::volume(float *sample, float decibels){
  float step1 = pow(10,decibels/10);
  *sample *= step1;
}

float TTModules::step2Div(bpmstep step) {
  float div = 0;
  if(step == DBL) return 2;
  if(step == WHOLE) return 1;
  else div = 1./(2.*((float)step-1.));
  return div;
}

float TTModules::bpm2Freq(float bpm,bpmstep step) {
  float bps = 0.;
  float div = 0.;

  bps = bpm/60;
  div = step2Div(step);
  return bps/div;
}

float TTModules::freq2BPM(float Hz) {
  return Hz/60;
}

int TTModules::gcd(int x, int y) {
  int r;
  if(x = 0) return y;
  if(y = 0) return x;
  if(x > y) {
    r = x % y;
    gcd(y,r);
  }
  else {
    r = y % x;
    gcd(x,r);
  }
}