#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>

namespace TTModules {

  enum bpmstep {
    DBL,
    WHOLE,
    HALF,
    QUARTER,
    EIGTH,
    SIXT,
    THIRT,
    SIXTFOUR
  };
  /******** Volume ************/

  //increases/decreses volume in decibels.
  void volume(float *sample, float decibels);

  /******** Time Conversions ************/

  // lil step divisor function
  
  float step2Div(bpmstep step);

  //assumes whole step = 1 period

  float bpm2Freq(float bpm,bpmstep step);

  float freq2BPM(float Hz);


  /******** Other Algorithms ************/

  //gcd using euclidean
  int gcd(int x,int y);

};