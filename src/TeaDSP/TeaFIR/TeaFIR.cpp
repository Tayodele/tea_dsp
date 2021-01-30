#include "TeaFIR.h"
#include <stdlib.h>

using namespace TTModules;

//FIR
TeaFIR::TeaFIR() {
  fs = 44100.;
  cutoff = 0.;
  order = 2;
  bufq = new std::queue<float>;
  coeffs = new std::vector<float>;
}

TeaFIR::TeaFIR(float afs) {
  fs = afs;
  cutoff = 0.;
  order = 2;
  bufq = new std::queue<float>;
  coeffs = new std::vector<float>;
}


TeaFIR::~TeaFIR() {
  delete bufq;
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
  delete bufq;
  bufq = new std::queue<float>;
  for(int i = 0; i < order; i++) bufq->push(0.);
  coeffs->resize(order);
  buildFilter();
}

TeaFIR::filtertype TeaFIR::getType(){
  return type;
}
void TeaFIR::setType(filtertype val){
  type = val;
  buildFilter();
}

void TeaFIR::buildFilter()  {
  for(int i = 1; i <= order; i++) {
    coeffs[i-1] = (float)(sin(2*pi*(cutoff/fs)*(i-order/2)) / (pi*(i-order/2)));
    coeffs[i-1] *= (float)(0.54f + 0.46f*cos(2*pi*(i-order/2) / order));
  }
}

void TeaFIR::filter(float* input) {
  float output = 0;
  for(int i=0;i < bufq->size(); i++) {
    output += bufq[i] * coeffs[i];
  }
  *input += output;
}