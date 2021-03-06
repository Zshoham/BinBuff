package com.binbuff;

/**
 * The interface implements serialization and deserialization into and from a BinBuff.com.BinBuff.Buffer.
 * Classes that implement this interface will always use the provided method when written
 * into a BinBuff.com.BinBuff.Buffer.
 * @see Buffer
 */
public interface ISerializable {

    void serialize(Buffer buffer);

    void deserialize(Buffer buffer);
}