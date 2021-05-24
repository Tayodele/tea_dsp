#include <AudioBuffer.h>

using namespace TTModules;

AudioBuffer::AudioBuffer(int fs, int blocksize,float* in = NULL, float* outp = NULL){
  this->fs = fs;
  this->blocksize = blocksize;
  this->sampin = in == NULL ? new float[blocksize] : in;
  this->sampout = outp == NULL ? new float[blocksize] : outp;

}

AudioBuffer::~AudioBuffer(){
  delete sampin;
  delete sampout;
}

AudioBuffer AudioBuffer::deepCopy(){
  return AudioBuffer(this->fs,this->blocksize,this->sampin,this->sampout);
}