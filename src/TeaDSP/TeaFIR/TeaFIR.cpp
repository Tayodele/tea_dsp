#include "TeaFIR.h"
#include "TeaCommon/conversions.h"

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
  // Create filter response
  // IFFT
  // Window
  setCoeffs();
}

void TeaFIR::filter(float* input, float* output) {
  int inp = 0;
  int outp = 0;
  //do something
}

void TeaFIR::setCoeffs() {
  int half  = RESP_SIZE/2 - csize/2;
  for(int i = 0; i < csize; i++){
    coeffs[i] = fresp[half+i];
  }
}