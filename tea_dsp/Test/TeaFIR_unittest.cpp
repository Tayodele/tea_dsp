#include "gtest/gtest.h"
#include "TeaFIR/TeaFIRDynamic.h"
#include "TeaCommon/conversions.h"
#include <iostream>
#include <fstream>

using namespace TTModules;

namespace
{
  TEST(FIRTest, ResponseCheck)
  {
    std::ofstream datafile("../../../../analysis/data/dataout.txt");
    TeaFIRDynamic Fir(44100, 2);
    double *fire_response = Fir.getResponse();
    for (auto i = 0; i < Fir.getResponseSize(); i++)
    {
      datafile << std::to_string(fire_response[i]) << ",\n";
    }
  }
}