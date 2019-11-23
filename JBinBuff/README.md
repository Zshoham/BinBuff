# BinBuff Java Implementation

This is the java implementation of BinBuff, the full documentation is available [here](https://github.com/Zshoham/BinBuff/docs).
The library is build using gradle, to learn more about how to use gradle visit the [official site](https://gradle.org/)

the latest release is available [here](https://github.com/Zshoham/BinBuff/releases).

## Benchmarks Results

* **Speed:** The Results of the benchmarks in terms of speed are obviously not very conclusive because they were only tested on a small number of machines and cannot be generalized, but after running the benchmarks on my PC and a couple laptops it seems that BinBuff is at least as fast as the JDK Object Serialization in most cases.
* **Size:** In terms of size the results are more conclusive since the size of the serialized object does not change depending on the system, and as expected BinBuff is as good as the JDK Serialization when primitive data types are concerned, but when Objects and collections of Objects are serialized BinBuff is much more efficient in terms of size, this is fairly obvious when you see that java stores all kinds of meta data with every object it serializes, while BinBuff stores only the data.

## Working With Sources 

In order to build the library run `./gradlew build` in the JBinBuff directory.
you will find that gradle created a build directory, inside the libs directory you will find two jars have been built. The library jar and a [benchmark jar](#running-the-benchmark), as well as the test results.
The test results can be found in `build/test-results` as an XML, 
or in `build/reports/tests/test` as a web site, simply open the index.html file in your browser to view the results.

The build command automatically runs the test and will fail and tell you if some of the tests failed, but if you wish to independently run the tests use - `./gradlew test`.
In addition if you wish to run the benchmarks through gradle you can use - `./gradle benchmark`,
this will use the 0 log level and run only the in memory write benchmark, in order to use a different configuration for the benchmark use - `./gradle benchmark -Parg=[Log],[Benchmark]`.

*note: all the commands assume that you are using bash like CLI that recognizes the "./" instruction, if you are using the normal windows CMD you can simply omit the "./" from all the commands and it should work fine*

## Running The Benchmark

In the java release of the library available [here](https://github.com/Zshoham/BinBuff/releases) you can find the library jar itself in addition to a test jar.
in order to run the test use the command 
`java -jar BinBuff-[version]-Benchmark.jar [Log] [Benchmarks]` 
this will run the test with all the benchmarks and all log messages, in order to use a different log level use
`java -jar BinBuff-[version]-Benchmark [0|1}2]`

* 0 - if you wish to see all the available information about all the benchmarks.
* 1 - if you wish to see only the final statistics for each benchmark.
* 2 - if you wish to see only the overall statistics from running all the benchmarks.

In order to specify which tests are run use -
`java -jar BinBuff-[version]-Benchmark.jar [0|1}2] [wm-wd-ram-rad-rcm-rcd-rmm-rmd]`

* wm - is write in memory
* wd - is write in disk
* ram - is read array in memory
* rad - is read array in disk
* rcm - is read collection in memory
* rcd - is read collection in disk
* rmm - is read map in memory
* rmd - is read map in disk

in order to activate on of the benchmarks set the appropriate value to 1, for example to activate all the benchmarks use 11111111, or to activate only the two write benchmarks use 11000000.