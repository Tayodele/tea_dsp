/*** 
Filters! 
Filtering is so cool, but I don't want to spend a long time churning out cool ones,
so for now a flexible dynamic FIR will do.

Todo: more filters
***/

namespace TTModules {

  class TeaFIR {

    public:

    TeaFIR();

    ~TeaFIR();

    float getCutoff();
    void  setCutoff(auto val);
    
    float getOrder();
    void  setOrder(auto val);
    
    float getType();
    void  setType(auto val);

    enum filtertype {
      LOW,
      HIGH,
      BAND,
      STOP,
      BELL,
    }

    void filter(float* input);

    private:

    void setCoeffs();

    filtertype type;

    //cutoff freq
    float cutoff;
    float fs;
    
    //queue used for buffer 
    float* bufq;
    float* coeffs;
    int qin;
    int qout;
    //length of queue
    int order;

  };
};