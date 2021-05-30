#include "TeaFIR.h"
#include "TeaCommon/conversions.h"
#include <fftw3.h>

using namespace TTModules;

//FIR
TeaFIR::TeaFIR() {
  init();
}

TeaFIR::TeaFIR(float afs = 44100,float ord = 2.) {
  init(afs,ord);
}

void TeaFIR::init(float afs = 44100.,float ord = 2.) {
  fs = afs;
  cutoff = 0.;
  order = ord;
  coeffs = new float[order];
  buff = AudioBuffer(fs,256);
  //using constant for now, want to test the speed of this
  fresp = new float[1<<16];
  resp_size = int(1<<16);
  buildFilter();
}

TeaFIR::~TeaFIR() {
  delete coeffs;
}

float TeaFIR::getCutoff(){
  return cutoff;
}

void TeaFIR::setCutoff(float val){
  cutoff = val;
  buildFilter();
}

int TeaFIR::getOrder(){
  return order;
}
void TeaFIR::setOrder(int val){
  if(val < 2) order = 2;
  order = val*2;
  setCoeffs();
}

TeaFIR::filtertype TeaFIR::getType(){
  return type;
}
void TeaFIR::setType(filtertype val){
  type = val;
  buildFilter();
}

void TeaFIR::buildFilter()  {
  // Rectangle Response
  double* rect_resp = new double[resp_size];
  double* rect_wave = new double[resp_size];
  plan = fftw_plan_r2r_1d(resp_size, rect_resp, rect_wave, fftw_r2r_kind(1), 0);    
  for(int i = 0; i < resp_size; i++) {
    if(i <= int(cutoff)) rect_resp[i] = 1;
    else if(i >= int(resp_size) - int(cutoff)) rect_resp[i] = 1;
    else rect_resp[i] = 0;
  }
  // IFFT
  // Window
  setCoeffs();
  fftw_destroy_plan(plan);
}

// Passes one sample block thorugh the filter
// Maybe put the convolution as a general function?
int TeaFIR::filter() {
  int inp = buff.blocksize - 1;
  int outp = 0;
  while(inp >= 0) {
    auto temp = 0.;
    for(auto j = 0; j < csize; j++) {
      temp += buff.sampin[inp-j] * coeffs[csize-j-1];
    }
    buff.sampout[outp] = temp;
    outp++;
    inp--;
  }
  return 0;
}

void TeaFIR::setCoeffs() {
  int half  = resp_size/2 - csize/2;
  for(int i = 0; i < csize; i++){
    coeffs[i] = fresp[half+i];
  }
}