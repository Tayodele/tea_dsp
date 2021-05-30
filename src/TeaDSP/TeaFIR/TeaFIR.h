/*** 
Basic Chain FIR, different filter formats like a basic IIR and Biquad will be separate objects, 
but all of them will be able to implement windows and have some form of Parks-M design when I learn how to do it.

Todo: more filters

Some Specs: 

Resolution is 1 Hz, so allocated at least fs*sizeof(float)
***/

#include "TeaCommon/AudioBuffer.h"
#include <stdlib.h>

namespace TTModules {

  class TeaFIR {

    public:

    TeaFIR();

    TeaFIR(float afs, float ord);

    ~TeaFIR();

    float getCutoff();
    void  setCutoff(float val);
    
    int getOrder();

    // Doubles the input value to enusure order is always even
    void  setOrder(int val);
    
    enum filtertype {
      LOW,
      HIGH,
      BAND,
      STOP,
      //BELL, I don't know how to do this one yet
    };
    
    filtertype getType();
    void  setType(filtertype val);

    int filter();

    void enableFilter();
    void passthrough();

    AudioBuffer buff;

    private:

    void setCoeffs();

    void buildFilter();

    void init(float afs, float ord);
 
    filtertype type;

    //cutoff freq
    float cutoff;
    float fs;
    //length of queue, even number for symmetry (taken from a 64 point DFT)
    int order;

    //array for coeffs
    float* coeffs;
    int csize;

    //Coeffs pulled from a 64 point DFT for now. Maybe build a higher end filter later?
    float* fresp;
    int resp_size;
    // For Window style design, might build my own fft algo? at the least wrapping fftw3
    fftw_plan plan;
    
    //turn filter on/off
    bool enable;

  };
};