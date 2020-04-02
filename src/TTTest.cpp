#include <SynthDefs.h>
#include <SynthFile.h>
#include <WaveFile.h>
#include <TTEnvGen.h>
#include <stdio.h>
#include <iostream>

using namespace SynthCommon;
using namespace TTModules;
void ExitRoutine() {
  int exitcode;
  std::cout << "Enter any to Exit";
  std::cin >> exitcode;
}

//first audio thing -> make things quiet!
int main() {
  WaveFileIn testsound;
  WaveFile outputsound;
  AmpValue* samplesOut;
  TTEnvGen egen;
  int status;
  char message[30];
  status = testsound.LoadWaveFile("C:\\Users\\TJ\\Documents\\AdventuresInAudioDesign\\TeaTimeSSL\\TestTones\\LFETestToneOsc_LF01.2.M.wav",0);
  if(status != 0) {
    sprintf(message,"Wave Load Failed! %i\n",status);
    std::cout << message;
    ExitRoutine();
    return 1;
  }
  samplesOut = (AmpValue *)malloc(testsound.GetInputLength()*sizeof(AmpValue));
  memcpy(samplesOut,testsound.GetSampleBuffer(),testsound.GetInputLength()*sizeof(AmpValue));
  outputsound.OpenWaveFile("C:\\Users\\TJ\\Documents\\AdventuresInAudioDesign\\TeaTimeSSL\\TestTones\\TTOut.wav"); 


  std::cout << "Setting up Env Generator\n";
  egen.SetAttack(3,1,TTEnvGen::envtype::LOG);
  egen.SetSustain(3);
  egen.SetRelease(3,TTEnvGen::envtype::LINEAR);
  std::cout << "Generating Envelope\n";
  egen.GenerateEnvelope();
  std::cout << "Applying Envelope\n";
  if(egen.getSize() < testsound.GetInputLength()) {
    egen.ApplyEnvelope(samplesOut,egen.getSize());
    for(int i = 0; i < egen.getSize(); i++) {
      outputsound.Output(samplesOut[i]);
    }
  }
  else {
    egen.ApplyEnvelope(samplesOut,testsound.GetInputLength());
    for(int i = 0; i < testsound.GetInputLength(); i++) {
      outputsound.Output(samplesOut[i]);
    }
  }
  std::cout << "Sucess!\n";
  egen.printData();

  outputsound.CloseWaveFile();
  ExitRoutine();
  return 0;
}
