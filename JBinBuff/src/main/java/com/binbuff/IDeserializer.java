package com.binbuff;

/**
 * Represents a function that is used to deserialize some data type 'T'.
 * @param <T> the data type that the deserializer operates on.
 */
public interface IDeserializer<T> {

    void deserialize(T dest, Buffer buffer);

    Class<T> getTypeClass();
}