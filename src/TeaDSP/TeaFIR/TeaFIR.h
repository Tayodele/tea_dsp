/*** 
Filters! 
Filtering is so cool, but I don't want to spend a long time churning out cool ones,
so for now a flexible dynamic FIR will do.

Todo: more filters
***/

#include <queue>
#include <stdlib.h>

namespace TTModules {

  class TeaFIR {

    public:

    TeaFIR();

    TeaFIR(float afs);

    ~TeaFIR();

    float getCutoff();
    void  setCutoff(float val);
    
    int getOrder();
    void  setOrder(int val);
    
    enum filtertype {
      LOW,
      HIGH,
      BAND,
      STOP,
      BELL,
    };
    
    filtertype getType();
    void  setType(filtertype val);

    void buildFilter();

    void filter(float* input);

    private:

    void setCoeffs();

    filtertype type;

    //cutoff freq
    float cutoff;
    float fs;
    
    //queue used for buffer 
    std::queue<float>* bufq;
    //array for coeffs
    float* coeffs;
    int qin;
    int qout;
    //length of queue
    int order;
    //turn filter on/off
    bool enable;

  };
};