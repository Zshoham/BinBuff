# BinBuff Java Implementation

This is the java implementation of BinBuff, the full documentation is available [here](https://github.com/Zshoham/BinBuff/docs).
This project comes with test you can run on your machine along with benchmark 
comparing the BinBuff library to the JDK Object Serialization.

the latest release is available [here](https://github.com/Zshoham/BinBuff/releases).

## Benchmarks Results

* **Speed:** The Results of the benchmarks in terms of speed are obviously not very conclusive because they were only tested on a small number of machines and cannot be generalized, but after running the benchmarks on my PC and a couple laptops it seems that BinBuff is at least as fast as the JDK Object Serialization in most cases.
* **Size:** In terms of size the results are more conclusive since the size of the serialized object does not change depending on the system, and as expected BinBuff is as good as the JDK Serialization when primitive data types are concerned, but when Objects and collections of Objects are serialized BinBuff is much more efficient in terms of size, this is fairly obvious when you see that java stores all kinds of meta data with every object it serializes, while BinBuff stores only the data.

## Working With Sources 

This project is intellij idea based, if you have intellij then all you need to do is open this folder through the editor and everything should work out of the box, otherwise there should be some option in your editor to import project from sources.

In order to run the tests go to the test folder and run the main from Test class.
by default all the test will run, if the test are successful "all tests passed" will be printed,
then the benchmarks start running, it might take a while to run all of them, if you wish to run some of the benchmark, main function, simply uncomment the ones that you wish to run in the.
for different levels of benchmark information change the `BENCHMARK_OPTION `to: 

* 0 - if you wish to see all the available information about all the benchmarks.
* 1 - if you wish to see only the final statistics for each benchmark.
* 2 - if you wish to see only the overall statistics from running all the benchmarks.