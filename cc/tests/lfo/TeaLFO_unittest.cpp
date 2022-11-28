#include "gtest/gtest.h"
#include "TeaLFO/TeaLFO.h"
#include "TeaCommon/conversions.h"
#include <iostream>
#include <fstream>

using namespace TTModules;
using namespace std;

namespace {
  TEST(LFOTest, PeeknPoke) {
    TeaLFO* ALFO = new TeaLFO();
    ALFO->setFreq(1.);
    ASSERT_EQ(ALFO->getBufSize(),44100.);
    delete ALFO;
  }

  float testwave(int idx) {
      float w = 2 * PI * (500.f / 44100.f);
      return cos(w*idx);
  }

  TEST(LFOTest,SignalPrintTest) {
    TeaLFO alfo;
    TeaLFO alfo2(PI/2.);
    float freq = 1.f;
    float fs = 44100;
    alfo.setFreq(freq);
    alfo2.setFreq(freq);
    float* buffer;
    float* buffer2;
    int len = fs * 20;

    buffer = (float*)malloc(sizeof(float) * len);
    buffer2 = (float*)malloc(sizeof(float) * len);

    ofstream datafile("dataout.txt");
    ofstream datafile2("dataout2.txt");
    for (int i = 0; i < len; i++) {
        *buffer = 10*testwave(i);
        *buffer2 = 10*testwave(i);
        alfo.modulate(buffer, true);
        alfo2.modulate(buffer2, true);
        datafile << std::to_string(*buffer);
        datafile << ",";
        datafile2 << std::to_string(*buffer2);
        datafile2 << ",";
        buffer++;
        buffer2++;
    }
  }
}