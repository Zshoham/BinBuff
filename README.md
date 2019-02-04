# Serializer

Simple and minimalistic library that provides binary serialization in `C` , `C++` `C#` and `java`.

All implementations support the same basic API decrined here.

## Supported Functionality
---

The library uses a `Buffer` to serilaize and desirealize data, the buffer holds the data written into it and can be set to write or read mode. </br>

* once in read mode the buffer cannot be written into and only read operations may be preformed. </br>

* in write mode write operations will be possible **as well as read operations**. </br>
  When in write mode the buffer may also be either dynamic or static.

  * Dynamic Buffer - will adapt its size to the data being inserted into it.
  
  * Static Buffer - has a set size that cannot change and trying to write into it over its size limit will result in an error.

Note that **trying to write into a buffer set to read mode or reading from a buffer set to write mode will result in an error**. </br>
Lastly the buffers data may be converted at any time to a `"byte array"` implementation varies between langueges. 



* Creating a buffer :
  
  * create a dynamic buffer in write mode.
  
  * create a static buffer in write mode.
  
  * create buffer from byte array in write mode.

* Write into buffer : 
  
  * Write primitive data types into the buffer.

  * Write array types into the buffer.

  * Write generic data types into the buffer. </br>
    Note that in `C` generic data type refers simply to a `void*` type that may refer to any data type. </br> In other 
    languages the abiity to write objects is supported.

* Read from buffer :
  
  * Read primitive data types from buffer.
  
  * Read array types from buffer, given array to read to and desired length.
  
  * Read generic data types from buffer.

  Reading from a buffer will return the data that was read and remove it from the buffer.

Documentation and examples for the different implementations of the library may be found in the following links :

* [*C implementation*]()

* [*C++ implementation*]()

* [*C# implementation*]()

* [*Java implementation*]()