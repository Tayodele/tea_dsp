/*** 
Basic Chain FIR, different filter formats like a basic IIR and Biquad will be separate objects, 
but all of them will be able to implement windows and have some form of Parks-M design when I learn how to do it.

Todo: more filters

Some Specs: 

Resolution is 1 Hz, so allocated at least fs*sizeof(double)
***/

#include <stdlib.h>

namespace TTModules
{

  class TeaFIR
  {

  public:
    TeaFIR();

    TeaFIR(double afs = 44100, double ord = 2.);

    ~TeaFIR();

    double getCutoff();
    void setCutoff(double val);

    int getOrder();

    // Doubles the input value to enusure order is always even
    void setOrder(int val);

    enum filtertype
    {
      LOW,
      HIGH,
      BAND,
      STOP,
      //BELL, I don't know how to do this one yet
    };

    filtertype getType();
    void setType(filtertype val);

    void filter(double &input, double &output);

    // Return Response of filter
    double *getResponse();
    int getResponseSize();

  private:
    void setCoeffs();

    void buildFilter();

    void init(double afs = 44100., double ord = 2.);

    filtertype type;

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