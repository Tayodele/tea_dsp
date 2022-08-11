#define _USE_MATH_DEFINES
#include "TeaLFO.h"
#include <math.h>
#include <stdlib.h>

using namespace TTModules;

TeaLFO::TeaLFO() {
  samptime = 0;
  fs = 44100;
  freq = 1;
  initphi = 0;
  T_size = int(floor(fs/freq));
  if(T_size < 1) T_size = 1;
  lfobuf = (float*) malloc(sizeof(float)*T_size);
  shape = SINE;
  populateLFO();
}

TeaLFO::TeaLFO(float aphi) {
  samptime = 0;
  fs = 44100;
  freq = 1;
  initphi = aphi;
  lfobuf = (float*) malloc(sizeof(float)*fs*10);
  shape = SINE;
  populateLFO();
}

TeaLFO::~TeaLFO() {
  free(lfobuf);
}

void TeaLFO::setShape(wavetype ashape) {
  shape = ashape;
  populateLFO();
}

void TeaLFO::setFs(float afs) {
  fs = afs;
  T_size = int(floor(fs/freq));
  if(T_size < 1) T_size = 1;
  lfobuf = (float*) realloc(lfobuf,sizeof(float)*T_size);
  populateLFO();
}

void TeaLFO::setFreq(float afreq) {
  freq = afreq;
  T_size = int(floor(fs/freq));
  if(T_size < 1) T_size = 1;
  lfobuf = (float*) realloc(lfobuf,sizeof(float)*T_size);
  populateLFO();
}

void TeaLFO::setPhi(float aphi) {
  initphi = aphi;
}

void TeaLFO::populateLFO() {
  float w;
  float wave = 0;
  w = 2*M_PI*freq/fs;
  
  for(int i = 0; i < T_size; i++){
    wave = cos(w*i + initphi);
    switch(shape) {

      case SINE:
        lfobuf[i] = wave;
        break;

      case SQUARE:
        if(wave > 0) {
          lfobuf[i] = 1;
        } else if(wave < 0) {
          lfobuf[i] = -1; 
        } else {
          lfobuf[i] = 0; 
        }
        break;

      default:
        break;
    }
  }
  samptime = 0;
}

void TeaLFO::modulate(float *sample, bool pos){
  float mod;
  if(pos) mod = fabs(lfobuf[samptime]);
  else mod = lfobuf[samptime];
  *sample = mod * (*sample);
  samptime = (samptime + 1) % T_size;
}
  
void TeaLFO::add(float *sample, bool pos){
  float mod;
  if(pos) mod = fabs(lfobuf[samptime]);
  else mod = lfobuf[samptime];
  *sample = mod + *sample;
  samptime = (samptime + 1) % T_size;
}
  
void TeaLFO::subtract(float *sample, bool pos){
  float mod;
  if(pos) mod = fabs(lfobuf[samptime]);
  else mod = lfobuf[samptime];
  *sample = mod - *sample;
  samptime = (samptime + 1) % T_size;
}

int TeaLFO::getBufSize(){
  return T_size;
}

void TeaLFO::print(float* wave, int size){
  int readsize = 0;
  float data;
  if(size > T_size) readsize = T_size;
  else readsize = size;
  for(int i=0; i < readsize; i++){
    data = lfobuf[i];
    wave[i] = lfobuf[i];
  }
}