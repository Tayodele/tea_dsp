#include "TTEnvGen.h"

using namespace TTModules;

TTEnvGen::TTEnvGen() {
  statetypes[0] = NONE;
  statetypes[1] = NONE;
  statetypes[2] = NONE;
  statetypes[3] = NONE;
}

TTEnvGen::~TTEnvGen() {
  free(samples);
}

void TTEnvGen::SetAttack(double start,double length,envtype curve){
  attkStart = start;
  attkTime = length;
  statetypes[0] = curve;
}
void TTEnvGen::SetSustain(double length){
  susTime = length;
  statetypes[1] = NONE;
}
void TTEnvGen::SetDecay(double length,double final,envtype curve){
  decayFinal = final;
  decayTime = length;
  statetypes[2] = curve;
}
void TTEnvGen::SetRelease(double length,envtype curve){
  releaseTime = length;
  statetypes[3] = curve;
}

void TTEnvGen::NoAttack() {
  attkStart = 0;
  attkTime = 0;
}
void TTEnvGen::NoSustain(){
  susTime = 0;
}
void TTEnvGen::NoDecay(){
  decayFinal = 1;
  decayTime = 0;
}
void TTEnvGen::NoRelease(){
  releaseTime = 0;
}

//since I'm trying to apply and envelope to an existing waveform that I'm reading and writing, 
//env should cut it's size to prevent hella silence
void TTEnvGen::ApplyEnvelope(AmpValue * samplesIn,int length){
  for(int i = 0; i < length; i++) {
    samplesIn[i] = samples[i] * samplesIn[i];
  }
}

void TTEnvGen::GenerateEnvelope(){
  //based on total env time allocate env buffer
  envlength = releaseTime + decayTime + susTime + attkTime + attkStart;
  int numSamples = SynthCommon::synthParams.sampleRate * envlength;
  samples = (AmpValue *) malloc(numSamples * sizeof(AmpValue));
  envSize = numSamples;
  EnvSM();
}

void TTEnvGen::EnvSM() {
  int sIndex = 0;
  BuildAttack(sIndex);
  BuildSustain(sIndex);
  BuildDecay(sIndex);
  BuildRelease(sIndex);
}

void TTEnvGen::BuildAttack(int &index){
  int sStartSamp = attkStart * SynthCommon::synthParams.sampleRate;
  for(int i = index; i < sStartSamp; i++) samples[i] = 0;
  index += sStartSamp;
  int sAttSamp = attkTime * SynthCommon::synthParams.sampleRate;
  switch(statetypes[0]) {
    case LINEAR:
      xLinear(0,1,sAttSamp,index);
    break;
    case EXP:
      xExp(0,1,sAttSamp,index);
    break;
    case LOG:
      xLog(0,1,sAttSamp,index);
    break;
    default:
    break;
  }
  samples[index - 1] = 1;
}

void TTEnvGen::BuildSustain(int &index){
  int sSusSamp = susTime * SynthCommon::synthParams.sampleRate;
  int atime  = index + sSusSamp;
  for(int i = index; i < atime; i++) samples[i] = 1;
  index += sSusSamp;
}

void TTEnvGen::BuildDecay(int &index){
  int sDecSamp = decayTime * SynthCommon::synthParams.sampleRate;
  switch(statetypes[2]) {
    case LINEAR:
      xLinear(samples[index-1],decayFinal,decayTime,index);
    break;
    case EXP:
      xExp(samples[index-1],decayFinal,decayTime,index);
    break;
    case LOG:
      xLog(samples[index-1],decayFinal,decayTime,index);
    break;
    default:
    break;
  }
}

void TTEnvGen::BuildRelease(int &index){
  int sRelSamp = releaseTime * SynthCommon::synthParams.sampleRate;
  switch(statetypes[3]) {
    case LINEAR:
      xLinear(samples[index-1],0,sRelSamp,index);
    break;
    case EXP:
      xExp(samples[index-1],0,sRelSamp,index);
    break;
    case LOG:
      xLog(samples[index-1],0,sRelSamp,index);
    break;
    default:
    break;
  }
  samples[index - 1] = 0;
}

void TTEnvGen::xLinear(double initial,double final,int time,int &index){
  double volume = 0;
  int atime = time + index;
  int initialInd = index;
  double coeff = (final - initial)/time;
  for(int i = index;i < atime; i++) {
    volume = coeff * (i - initialInd) + initial;
    samples[i] = volume;
    index++;
  }
}
void TTEnvGen::xExp(double initial,double final,int time,int &index){
  double volume = 0;
  int atime = time + index;
  int initialInd = index;
  double coeff = (final - initial)/(pow(time,2));
  for(int i = index;i < atime; i++) {
    volume = coeff * pow((i-initialInd),2) + initial;
    samples[i] = volume;
    index++;
  }
}
void TTEnvGen::xLog(double initial,double final,int time,int &index){
  double volume = 0;
  int atime = time + index;
  int initialInd = index;
  double coeff = (final - initial)/(log(time+1));
  for(int i = index;i < atime; i++) {
    volume = coeff * log((i-initialInd+1)) + initial;
    samples[i] = volume;
    index++;
  }
}

void TTEnvGen::printData() {
  std::ofstream testfile;
  char data[50];
  testfile.open("C:\\Users\\TJ\\Documents\\AdventuresInAudioDesign\\TeaTimeSSL\\TestTones\\test.csv");
  for (int i = 0; i < envSize; i++) {
      sprintf(data, "%f\n", samples[i]);
      testfile << data;
  }
  testfile.close();
}