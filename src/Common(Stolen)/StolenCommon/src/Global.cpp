#include <stdlib.h>
#include <math.h>
#include "SynthDefs.h"
#include "SynthFile.h"

namespace SynthCommon {

    SynthConfig synthParams;

    int InitSynthesizer(bsInt32 sr, bsInt32 wtlen, bsInt32 wtusr)
    {
        synthParams.Init(sr, wtlen);
        return 0;
    }
}