# BinBuff Java Implementation

This is the java implementation of BinBuff, the full documentation is available [here](https://github.com/Zshoham/BinBuff/docs).
This project comes with test you can run on your machine along with benchmark 
comparing the BinBuff library to the JDK Object Serialization.

the latest release is available [here](https://github.com/Zshoham/BinBuff/releases).

## Benchmarks Results

* **Speed:** The Results of the benchmarks in terms of speed are obviously not very conclusive because they were only tested on a small number of machines and cannot be generalized, but after running the benchmarks on my PC and a couple laptops it seems that BinBuff is at least as fast as the JDK Object Serialization in most cases.
* **Size:** In terms of size the results are more conclusive since the size of the serialized object does not change depending on the system, and as expected BinBuff is as good as the JDK Serialization when primitive data types are concerned, but when Objects and collections of Objects are serialized BinBuff is much more efficient in terms of size, this is fairly obvious when you see that java stores all kinds of meta data with every object it serializes, while BinBuff stores only the data.

## Working With Sources 

This project is intellij idea based, if you have intellij then all you need to do is open this folder through intellij and everything should be set up, otherwise there should be some option in your IDE to import project from sources.

## Running The Tests

In the java release of the library available [here](https://github.com/Zshoham/BinBuff/releases) you can find the library jar itself in addition to a test jar.
in order to run the test use the command `java -jar BinBuff_Test.jar` this will run the test with all the benchmarks and all log messages, in order to use a different log level use `java -jar BinBuff_Test.jar[0|1}2]`:

* 0 - if you wish to see all the available information about all the benchmarks.
* 1 - if you wish to see only the final statistics for each benchmark.
* 2 - if you wish to see only the overall statistics from running all the benchmarks.

In order to specify which tests are run use -
`java -jar BinBuff_Test.jar[0|1}2] [wm-wd-ram-rad-rcm-rcd-rmm-rmd]`:

* wm - is write in memory
* wd - is write in disk
* ram - is read array in memory
* rad - is read array in disk
* rcm - is read collection in memory
* rcd - is read collection in disk
* rmm - is read map in memory
* rmd - is read map in disk

in order to activate on of the benchmarks set the appropriate value to 1, for example to activate all the benchmarks use 11111111, or to activate only the two write benchmarks use 11000000.