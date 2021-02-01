#include "TeaGenerator.h"
#include <iostream>
#include <fstream>
#include <string>
#include "math.h"

using namespace TTModules;

TeaGenerator::TeaGenerator() {
 fs = 44100.; 
 oldpulses = new std::vector<float*>;
}

TeaGenerator::TeaGenerator(float afs) {
 fs = afs; 
 oldpulses = new std::vector<float*>;
}

TeaGenerator::~TeaGenerator(){
  delete pulse;
  delete oldpulses;
}

std::vector<float> * TeaGenerator::getPulse() {
  return pulse;
}

// Populates pulse, optionally can be a chirp
void TeaGenerator::genPulse(float freq, float length, float chirpmask = 0., bool print = false) {
  float w = 2*PI*(freq/fs);
  int pulsesize = (int)ceil(length*fs);
  pulse = new std::vector<float>;
  for(int i = 0; i < pulsesize; i++) {
    if(chirpmask > 0) {
      w=2*PI*((freq/fs) + ((chirpmask/pulsesize)/fs));
    }
    pulse->push_back((float) cos(w*i));
  }
  oldpulses->clear();
  float pulse_hist[3] = {freq,length,chirpmask}; 
  oldpulses->push_back(pulse_hist);
  if(print) {
    std::ofstream * pulsedata = new std::ofstream("pulsedata.txt");
    for(int i = 0; i < pulse->size(); i++) {
      *pulsedata << std::to_string(pulse->at(i));
    }
    delete pulsedata;
  }
}
// Mix another pulse with the current one, can also be a chirp
void TeaGenerator::convolvePulse(float freq, float chirpmask = 0., bool print = false){
  float w = 2*PI*(freq/fs);
  int pulsesize = (int)ceil((pulse->size()*sizeof(float))*fs);
  for(int i = 0; i < pulse->size(); i++) {
    if(chirpmask > 0) w=2*PI*((freq/fs) + ((chirpmask/pulsesize)/fs));
    pulse->at(i) *= (float) cos(w*i);
  }
  float pulse_hist[3] = {freq,(float)pulse->size()/fs,chirpmask};
  oldpulses->push_back(pulse_hist);
  if(print) {
    std::ofstream * pulsedata = new std::ofstream("pulsedata.txt");
    for(int i = 0; i < pulse->size(); i++) {
      *pulsedata << std::to_string(pulse->at(i));
    }
    delete pulsedata;
  }
}

float TeaGenerator::getFs(){
  return fs;
}
void TeaGenerator::setFs(float afs) {
  fs = afs;
  //old pulse is trash now, regenerate
  delete pulse;
  if(oldpulses->size() > 0) {
    genPulse(oldpulses->at(0)[0],oldpulses->at(0)[1],oldpulses->at(0)[2]);
    for(int i = 1; i < oldpulses->size(); i++) {
      convolvePulse(oldpulses->at(i)[0],oldpulses->at(i)[2]);
    }
  }
}