# BinBuff C++ Implementation

This is the C++ implementation of BinBuff, the full documentation is available [here](https://github.com/Zshoham/BinBuff/docs).
This library is built using CMake and is cross platform, to learn more about CMake and download it visit the   [official site](https://cmake.org/)  .

the latest release is available [here](https://github.com/Zshoham/BinBuff/releases).

## Working With Sources 

In order to generate the build files for your System create a folder in the CppBinBuff directory called build
then inside the build folder run `cmake ..` and all the appropriate build files will be generated in the build folder.

*  **on Windows** - if you are on windows a Visual Studio solution will be created with projects for the test and the library itself.
* **on Linux** - if you are on a Linux system a Makefile will be generated, running `make` in the build directory will create a libbinbuff.a file and inside the test directory there will be a binbuff_test executable.

A `configure.py` script is provided to help configure the project correctly and later build and test it, it is recommended to use this file for the initial configuration at the vary least as cmake can make some things a bit annoying to do by hand, in addition the cmake install command should be implemented in your desired build system, but it might not be possible to make the distinction between the install components (binbuff and googletest) by running the `configure.py` only the binbuff install will be preformed to the bin folder.