#include "cc/common_conversions/conversions.h"
#include "gtest/gtest.h"

using namespace TeaDSP;

namespace {
TEST(AlgTests, RoundTest) {
  EXPECT_EQ(roundfs(10.532, 3), 10.5);
  EXPECT_EQ(roundfs(10500., 3), 10500.);
}

TEST(BPMTest, BPMConversions) {
  EXPECT_EQ(step2Div(DBL), 2);
  EXPECT_EQ(step2Div(WHOLE), 1);
  EXPECT_EQ(roundfs(step2Div(SIXTFOUR), 5), roundfs((1. / 12.), 5));

  EXPECT_EQ(roundfs(bpm2Freq(600, WHOLE), 5), roundfs(10., 5));
  EXPECT_EQ(roundfs(bpm2Freq(600, SIXTFOUR), 5), roundfs(120., 5));

  EXPECT_EQ(roundfs(freq2BPM(60), 5), roundfs(1., 5));
  EXPECT_EQ(roundfs(freq2BPM(30), 5), roundfs(0.5, 5));
}

TEST(VolumeTest, Decibels) {
  EXPECT_EQ(roundf(volume(50, -3)), 25.);
}
} // namespace