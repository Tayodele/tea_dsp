/*** 
Basic Chain FIR, different filter formats like a basic IIR and Biquad will be separate objects, 
but all of them will be able to implement windows and have some form of Parks-M design when I learn how to do it.

Todo: more filters

Some Specs: 

Resolution is 1 Hz, so allocated at least fs*sizeof(double)
***/

#include <stdlib.h>
#include <fftw3.h>
#include "TeaCommon/conversions.h"

namespace TeaFIR
{

  class FIR
  {

  public:
    FIR()
    {
      init();
    }

    FIR(double afs = 44100, double ord = 2.)
    {
      init(afs, ord);
    }

    ~FIR()
    {
      delete coeffs;
    }

    double getCutoff()
    {
      return cutoff;
    }
    void setCutoff(double val)
    {
      cutoff = val;
      buildFilter();
    }


    int getOrder();

    // Doubles the input value to enusure order is always even
    void setOrder(int val);

    enum filtertype
    {
      LOW,
      HIGH,
      BAND,
      STOP,
      // I don't know how to do this one yet
      // BELL, 
    } type;

    void filter(double &input, double &output)
    {
      for (auto i = order - 1; i > 0; i--)
      {
        buffer[i] = buffer[i - 1];
      }
      buffer[0] = input;
      auto temp = 0.;
      for (auto j = 0; j < order; j++)
      {
        temp += buffer[j] * coeffs[order - j - 1];
      }
      output = temp;
    }

    // Return Response of filter
    double *getResponse();
    int getResponseSize();


  private:
    void setCoeffs()
    {
      int half = resp_size / 2 - order / 2;
      for (int i = 0; i < order; i++)
      {
        coeffs[i] = fresp[half + i];
      }
    }

    void buildFilter()
    {
      // Rectangle Response
      auto resp_delta = (fs) / resp_size;
      double *rect_resp = new double[resp_size];
      double *fresp = new double[resp_size];
      auto kind = fftw_r2r_kind(1);
      unsigned flags = 0;
      fftw_plan plan;
      plan = fftw_plan_r2r_1d(resp_size, rect_resp, fresp, kind, flags);
      auto mirror_len = -1;
      for (int i = 0; i < resp_size; i++)
      {
        rect_resp[i] = 0;
        if (i * resp_delta <= cutoff)
          rect_resp[i] = 1;
        else
        {
          if (mirror_len < 0)
            mirror_len = i;
          if (i >= resp_size - mirror_len)
          {
            rect_resp[i] = 1;
            mirror_len--;
          }
        }
      }
      // IFFT
      fftw_execute(plan);
      // Window

      setCoeffs();
      fftw_destroy_plan(plan);
    }

    void init(double afs = 44100., double ord = 2.)
    {
      fs = afs;
      cutoff = 0.;
      order = ord;
      coeffs = new double[order];
      buffer = new double[order];
      //using constant for now, want to test the speed of this
      resp_size = int(1 << 6);
      buildFilter();
    }

    //cutoff freq
    double cutoff;
    double fs;
    //length of queue, even number for symmetry (taken from a 64 point DFT)
    int order;

    //array for coeffs and filter buffer
    double *coeffs;
    double *buffer;

    //Coeffs pulled from a 64 point DFT for now. Maybe build a higher end filter later?
    double *fresp;
    int resp_size;

    //turn filter on/off
    bool enable;
  };
};