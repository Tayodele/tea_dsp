/*** 
Basic Chain FIR, different filter formats like a basic IIR and Biquad will be separate objects, 
but all of them will be able to implement windows and have some form of Parks-M design when I learn how to do it.

Todo: more filters
***/

#include <vector>
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

    void filter(float* input,float* output);

    void enableFilter();
    void passthrough();

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
    float fresp[64];
    const int RESP_SIZE = 64;
    
    //turn filter on/off
    bool enable;

  };
};