package com.binbuff;

/**
 * Represents a function that is used to serialize some data type 'T'.
 * @param <T> the data type that the deserializer operates on.
 */
public interface ISerializer<T> {

    void serialize(T data, Buffer buffer);
}