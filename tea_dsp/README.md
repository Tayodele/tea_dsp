# Where's the Data? #

Data is in TeaDSP/bazel-bin/Test/\<testname exec file\>/.runfiles

## TODO

* Data Buffer type: need my own type, vectors are not the most efficient way to store in memory, since a sample block will always be the same time no need to allocate for growth.