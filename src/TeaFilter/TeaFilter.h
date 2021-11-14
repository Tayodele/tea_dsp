#pragma once

#include "IPlug_include_in_plug_hdr.h"
#include "TeaFIR.h"

const int kNumPresets = 1;

enum EParams
{
  kGain = 0,
  tFs = 1,
  kNumParams
};

using namespace iplug;
using namespace igraphics;

class TeaFilter final : public Plugin
{
public:
  TeaFilter(const InstanceInfo &info);

#if IPLUG_DSP // http://bit.ly/2S64BDd
  void ProcessBlock(sample **inputs, sample **outputs, int nFrames) override;
  TTModules::TeaFIR FIR();
#endif
};
