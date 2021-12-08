#include "TeaFilter.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

TeaFilter::TeaFilter(const InstanceInfo &info)
    : Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  // Instance Params
  GetParam(kGain)->InitDouble("Cutoff", 0., 0., 12000.0, 0, "Hz");
  GetParam(tFs)->InitDouble("Sampling Freq", 44100., 0., 96000., 1., "Hz");

  // Instance DSP objects
#if IPLUG_DSP
  FIR->setOrder(2)
#endif

  // Instance Graphics
#if IPLUG_EDITOR
      mMakeGraphicsFunc = [&]()
  {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };

  mLayoutFunc = [&](IGraphics *pGraphics)
  {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_GRAY);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
    pGraphics->AttachControl(new ITextControl(b.GetMidVPadded(50), "Hello iPlug 2!", IText(50)));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(-100), kGain));
  };
#endif
}

#if IPLUG_DSP
void TeaFilter::ProcessBlock(sample **inputs, sample **outputs, int nFrames)
{
  const double cutoff = GetParam(kGain)->Value() / 100.;
  const double sfreq = GetParam(tFs)->Value(); // TODO get this from Plugin Instance. Shouldn't be dynamic
  const int nChans = NOutChansConnected();

  FIR->setFs(sfreq);
  FIR->setCutoff(cutoff);

  for (int s = 0; s < nFrames; s++)
  {
    for (int c = 0; c < nChans; c++)
    {
      FIR->filter(inputs[c][s], outputs[c][s])
    }
  }
}
#endif
