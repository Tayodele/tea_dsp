#include "gtest/gtest.h"
#include "TeaCommon/conversions.h"
#include "TeaGenerator/TeaGenerator.h"
#include <array>
#include <vector>

namespace {

  TEST(GeneratorTest,SimpleWaveTest) {
    TeaGenerator aGen;
    std::array<float,44100> testwave;
    std::vector<float>* genout;

    ASSERT_EQ(aGen.getFs(),44100.);

    aGen.gePulse(10,1,0,false);
    for(int i: testwave) {
      testwave[i] = (float)cos(2*PI*((1./44100.))*i);
    }
    genout = aGen.getPulse();
    for(int i: testwave) {
      ASSERT_FLOAT_EQ(testwave[i],genout->at(i));
    }
  }
  
  // TEST(GeneratorTest,ConvWaveTest) {
    
  // }
}