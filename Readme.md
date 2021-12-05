# Tea Time SSL

### Frontend builds - Each branch is a different framework/build of the SSL

* ***JUCE***: I know I need to learn how to make GUI interfaces, but for now JUCE is nice for GIU and the buffer. All the DSP is hand made because if I ever want to have my own codebase for homebrew plugins it would involve me writing my own GUI and buffer.

* ***IPlug2***: Free GUI Framework to wrap dsp into a VST and run on a DAW. UPDATE (12/4/2021): this will probably be the main framework. JUCE is chunky and this is mostly for testing now. DSP in this repo is destined for the embedded space. Though this has interesting WAM integrations... 

### Tools used
* VS 2020
* Bazel
* Cargo

### TESTING
* C++ Tests are google tests executed with bazel tests. Most of em might be broken since the cpp is getting ported (12/4/2021)

* Rust tests soon
