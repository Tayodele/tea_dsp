#include <math.h>
#include <stdlib.h>

namespace TeaDSP {

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

  //increases/decreases volume in decibels.
  inline float volume(float sample, float decibels)
  {
    float step1 = pow(10, decibels / 10);
    return sample * step1;
  }

  /******** Time Conversions ************/

  // lil step divisor function
  
  inline float step2Div(bpmstep step)
  {
    float div = 0;
    if (step == DBL)
      return 2;
    if (step == WHOLE)
      return 1;
    else
      div = 1. / (2. * ((float)step - 1.));
    return div;
  }

  //assumes whole step = 1 period

  inline float bpm2Freq(float bpm,bpmstep step)
  {
    float bps = 0.;
    float div = 0.;

    bps = bpm / 60;
    div = step2Div(step);
    return bps / div;
  }

  inline float freq2BPM(float Hz) { return Hz / 60; }

  /******** Other Algorithms ************/

  //gcd using euclidean
  inline int gcd(int x,int y) 
  {
    int r;
    if (x == 0)
      return y;
    if (y == 0)
      return x;
    if (x > y) {
      r = x % y;
      return gcd(y, r);
    } else {
      r = y % x;
      return gcd(x, r);
    }
  }

  //rounding for floats
  inline float roundfs(float x,unsigned int n)
  {
    int scale = pow(10, n - 1);
    unsigned int overdot = n - 1;
    float res = x;
    if (n == 0)
      return 0;
    if (x > scale) {
      res /= scale;
      return round(res) * scale;
    } else {
      for (int i = n - 2; i >= 0; i--) {
        if (x > pow(10, i)) {
          overdot = 1;
          break;
        }
      }
      scale = pow(10, overdot);
      res *= scale;
      return round(res) / scale;
    }
  }

  /********* Some Constants ************/
  const float PI = 2.*acos(0);

};
