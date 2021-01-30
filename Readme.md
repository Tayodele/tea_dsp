# Tea Time SSL

### Specs
* Originally Built In VS 2019 (Hence the VS Solutions)
* I don't use Mac so don't expect any of the things to work with it

### Current Algorithms
* Amp Envelope
* Auto Panner
* Filter (In progress)

### Builds - Each branch is a different framework/build of the SSL
* ***Stolen***: Made with some base classes I stole from a Audio Design book repo. I'd post credits but methinks if they ever saw the butchery that I did to their code it would bring only dishonor to my clan.

* ***JUCE***: I know I need to learn how to make GUI interfaces, but for now JUCE is nice for GIU and the buffer. All the DSP is hand made because if I ever want to have my own codebase for homebrew plugins it would involve me writing my own GUI and buffer.

### Tools used
* VS 2019
* Bazel
* GoogleTest

### Test Targets Available
TeaDSP has Google tests for some useful unit testing. Here is what's there so far

* **test-common**: test conversion function and other common functions and abstracts

* **test-lfo**: Basic tests of LFO functionality

Some of these Tests generate data files that can analyzed using Octave scripts in analysis
