#include <math.h>
#include <stdlib.h>

namespace TTModules {
  class AudioBuffer {
    public:

    AudioBuffer(int fs, int blocksize,float* in, float* outp);

    ~AudioBuffer();

    int blocksize;
    int fs;
    float* sampin;
    float* sampout;

    AudioBuffer deepCopy();
  };
};