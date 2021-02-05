o_sig = dlmread("../bazel-bin/Test/test-gen.exe.runfiles/teadsp/pulsedata.txt");

Lo = length(o_sig);

plot([1:Lo],o_sig(1:Lo));