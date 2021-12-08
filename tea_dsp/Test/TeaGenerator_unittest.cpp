#include "gtest/gtest.h"
#include "TeaTest/TeaGenerator.h"
#include "TeaTest/DSPAsserts.h"
#include <vector>
#ifndef COMMONTEA
#define COMMONTEA
#include "TeaCommon/conversions.h"
#endif

using namespace TTModules;
using namespace TTTest;

namespace {

  TEST(GeneratorTest,SimpleWaveTest) {
    TeaGenerator aGen;
    std::vector<float> genout;
    std::vector<float> tester;

    ASSERT_EQ(aGen.getFs(),44100.);

    aGen.genPulse(1.,1,0,false);
    genout = aGen.getPulse();
    for(int i: genout) {
      tester.push_back((float)cos(2.*PI*((1./44100.))*i));
    }
    ASSERT_TRUE(checkWave(genout,tester,8));
  }
  
  TEST(GeneratorTest,ConvWaveTest) {
    TeaGenerator aGen;
    std::vector<float> genout;
    std::vector<float> tester;

    ASSERT_EQ(aGen.getFs(),44100.);

    aGen.genPulse(1.,5,0,false);
    aGen.convolvePulse(10.,0,false);
    genout = aGen.getPulse();
    for(int i: genout) {
      tester.push_back(cos(2.*PI*((1./44100.))*i) * cos(2.*PI*((10./44100.))*i));
    }
    ASSERT_TRUE(checkWave(genout,tester,6));
  }

  TEST(GeneratorTest,PrintChirp) {
    TeaGenerator aGen;

    ASSERT_EQ(aGen.getFs(),44100.);

    aGen.genPulse(1.,2.,1.,true);
  }
}