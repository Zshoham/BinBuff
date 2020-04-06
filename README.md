# BinBuff 

[![Build Status](https://travis-ci.org/Zshoham/BinBuff.svg?branch=master)](https://travis-ci.org/Zshoham/BinBuff)
[![codecov](https://codecov.io/gh/Zshoham/BinBuff/branch/master/graph/badge.svg)](https://codecov.io/gh/Zshoham/BinBuff)

Simple and minimalistic library that provides binary serialization in  [`C`](https://github.com/Zshoham/BinBuff/tree/master/CBinBuff
)  , [`C++`](https://github.com/Zshoham/BinBuff/tree/master/CppBinBuff) [`C#`](https://github.com/Zshoham/BinBuff/tree/master/CsBinBuff) and [`Java`](https://github.com/Zshoham/BinBuff/tree/master/JBinBuff).

All implementations support the same basic API described here, more information and documentation is provided for each implementation in its respective README file.

## Abstract API

The library uses a `Buffer` to serialize and deserialize data, the buffer holds the data written into it and can be set to write or read mode.

- once in read mode the buffer cannot be written into and only read operations may be performed.
- in write mode, only write operations will be possible. When in write mode the buffer may also be either dynamic or static.
- Dynamic Buffer - will adapt its size to the data being inserted into it.
- Static Buffer - has a set size that cannot change and trying to write into it over its size limit will result in an error.
- Creating a buffer:
  - create a dynamic buffer in write mode.
  - create a static buffer in write mode.
  - create a buffer from byte array in write mode.
- Write into the buffer :
  - Write primitive data types into the buffer.
  - Write array types into the buffer.
  - Write generic data types into the buffer, this means that any type can be serialized using a custom serializer, though some types are supported 'out of the box'.
- Read from buffer :
  - Read primitive data types from a buffer.
  - Read array types from a buffer, given array to read to and desired length.
  - Read generic data types from a buffer. Note that usually a type being deserializable is a stronger constraint than being serializable and so a type might be serializable but not deserializable.
- Reading from a buffer will return the data that was read and will remove it from the buffer.

## Pseudocode Usage

this is an example usage that uses pseudocode to demonstrate how the library is to be used:

```pseudocode
Person {
	id;
	age;
	// some more fiels..
}

person_serializer(person, buffer):
	buffer.write(id);
	buffer.write(age);
	// write all the fields..

person_deserializer(person, buffer):
	buffer.read(id);
	buffer.read(age);
	// read all the fields

...
// program that creates some data for a person object
Person person = aquire_person()
// now we want to save this person's data
Buffer buffer = make_buffer(type = dynamic)
buffer.write(person, person_serializer)
save_to_file(buffer.get_bytes())
...
// later we want to retrive the data and create a person
Buffer buffer = make_buffer(load_data())
buffer.set_read()
Person new_person() // empty person
buffer.read(new_person, person_deserializer)
//now new_person has the same state that person had before the serialization.
```

## Building The library

Information on how to build each implementation is provided in the respective README files, but for quick building the `pack.py` script is provided to help build, test and consolidate all the binaries quickly and from one place, run `./pack.py -b` to rebuild all the implementations, this will create a `release` folder containing folders with binaries for each implementation and a log folder with logs of the build and packaging.