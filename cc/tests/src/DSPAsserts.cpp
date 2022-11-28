#include "DSPAsserts.h"
#include "common/conversions.h"

using namespace TTTest;
using namespace TTModules;

testing::AssertionResult TTTest::checkWave(std::vector<float> testee,
                                           std::vector<float> checker,
                                           int sigfig) {
  for (int i : testee) {
    if (roundfs(testee[i], sigfig) != roundfs(checker[i], sigfig)) {
      return testing::AssertionFailure()
             << "point " << i << " does not match."
             << " Testee " << roundfs(testee[i], sigfig) << " Checker "
             << roundfs(checker[i], sigfig);
    }
  }
  return testing::AssertionSuccess();
}