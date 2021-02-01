#include "TeaCommon/conversions.h"
#include <vector>

/*
A Wave generator for tests. This is only meant for testing 
so it doesn't have to be fast nor efficient. Just enough to make waves for analysis and self-check
*/

namespace TTModules {

  class TeaGenerator {

    public:
    TeaGenerator();
    TeaGenerator(float afs);
    ~TeaGenerator();

    float getFs();
    void setFs(float afs);

    std::vector<float> * getPulse();

    // Populates pulse, optionally can be a chirp
    void genPulse(float freq, float length, float chirpmask, bool print);
    // Mix another pulse with the current one, can also be a chirp
    void convolvePulse(float freq, float chirpmask,  bool print);

    private:
    //sample rate
    float fs;
    //Pulse generated
    std::vector<float>* pulse;
    //Last pulse settings, [freq,length,chirp]
    std::vector<float*> * oldpulses;
    
  };
};