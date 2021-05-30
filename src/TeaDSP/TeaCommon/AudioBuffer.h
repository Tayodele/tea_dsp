#include <math.h>
#include <stdlib.h>

namespace TTModules {
  class AudioBuffer {
    public:

    AudioBuffer();

    AudioBuffer(float fs, int blocksize,float* in = NULL, float* outp = NULL);

    ~AudioBuffer();

    int blocksize;
    float fs;
    float* sampin;
    float* sampout;

    AudioBuffer deepCopy();
  };
};