import exceptions.*;
import sun.misc.Unsafe;

import java.lang.reflect.Field;
import java.util.*;

public class Buffer {

    public enum TYPE { DYNAMIC, STATIC }
    public enum MODE { READ, WRITE }

    private byte[] data;
    private int pointer;

    private TYPE type;
    private MODE mode;

    private Unsafe unsafe;

    /**
     * Allocate 'size' free bytes for the buffer.
     * if there are 'size' free bytes then nothing is changed.
     * otherwise the needed number of bytes will be added.
     * @param size number of bytes that need to be allocated.
     * @throws BufferIllegalOperationException if additional allocation is attempted on a STATIC buffer.
     */
    private void alloc_buffer(int size) throws BufferIllegalOperationException {
        if (this.data.length - this.pointer < size) {
            if (this.type == TYPE.STATIC)
                throw new BufferIllegalOperationException("cannot change size of static buffer.");
            byte[] arr = null;
            int length = 0;
            if (size > this.data.length * 2) length = this.pointer + size;
            else length = this.data.length * 2;
            this.data = Arrays.copyOf(this.data, length);
        }
    }

    /**
     * Constructs a copy of a given Buffer.
     * @param other the Buffer to copy.
     */
    public Buffer(Buffer other) {
        this.pointer = other.pointer;
        this.type = other.type;
        this.mode = other.mode;
        this.data = new byte[other.data.length];
        this.data = Arrays.copyOf(other.data, other.data.length);
    }

    /**
     * Constructs a Buffer of a given type and size.
     * @param type the type of the buffer.
     * @param size the size of the buffer.
     */
    public Buffer(TYPE type, int size) {
        this.data = new byte[size];
        this.type = type;
        this.mode = MODE.WRITE;
    }

    /**
     * Constructs Buffer with default size of 32
     * @see Buffer#Buffer(TYPE, int)
     */
    public Buffer(TYPE type) {
        this(type, 32);
    }

    /**
     * Sets the Buffer to READ mode, this changes the Buffer's type to STATIC. <br><br>
     * * While in READ mode the buffer wont be able to change its size or its contents. <br><br>
     * * Changing the buffer to READ mode sets its pointer to point to the first element. <br><br>
     * * Changing the buffer to READ mode resize's the buffer to fit the data it holds,
     */
    public void setRead() {
        if (this.mode == MODE.READ) return;
        if (pointer + 1 !=  this.data.length)
            this.data = Arrays.copyOf(this.data, this.pointer);
        this.mode = MODE.READ;
        this.type = TYPE.STATIC;
        this.pointer = 0;
    }

    /**
     * Sets the Buffer to WRITE mode, enabling changing the contents of the buffer.
     * @param type the type of the newly writable Buffer.
     * @param extraSize additional size to add to the Buffer.
     */
    public void setWrite(TYPE type, int extraSize) {
        if (this.mode == MODE.WRITE) return;
        this.mode = MODE.WRITE;
        this.type = type;
        this.pointer = this.data.length;
        this.data = Arrays.copyOf(this.data, this.pointer + extraSize);
    }

    public void setWrite(TYPE type) {
        this.setWrite(type, 32);
    }

    public void setWrite() {
        this.setWrite(TYPE.DYNAMIC, 32);
    }

    /**
     * Creates a byte array containing all the buffers data and exactly
     * all the buffers data, meaning the last byte of the array will be the last byte that was written
     * to the buffer.
     * note: this is a copy operation, meaning all the buffers data will be copied to a new location.
     * @return byte array containing all the data currently in the buffer.
     */
    public byte[] getBytes() {
        return Arrays.copyOf(this.data, this.pointer + 1);
    }

    public void increment(int amount) throws BufferException {
        if (this.data.length < this.pointer + amount)
            throw new BufferOverflowException("trying to jump outside of buffer bounds.");
        this.pointer += amount;
    }

    public void increment() throws BufferException { increment(1); }

    public void decrement(int amount) throws BufferException {
        if (this.pointer - amount < 0)
            throw new BufferUnderflowException("trying to jump outside of buffer bounds.");
        this.pointer -= amount;
    }

    public void decrement() throws BufferException { decrement(1); }



    //region Write

    public <T> void write(T... data) throws BufferException {
        for (T elem : data) {
            write(elem);
        }
    }

    public <T> void write(T data) throws BufferException {
        if (data instanceof Byte) write((byte)(Byte) data);
        else if (data instanceof Boolean) write((boolean)(Boolean) data);
        else if (data instanceof Short) write((short)(Short) data);
        else if (data instanceof Character) write((char)(Character) data);
        else if (data instanceof Integer) write((int)(Integer) data);
        else if (data instanceof Float) write((float)(Float) data);
        else if (data instanceof Long) write((long)(Long) data);
        else if (data instanceof Double) write((double)(Double) data);
        else if (data instanceof byte[]) write((byte[]) data);
        else if (data instanceof boolean[]) write((boolean[]) data);
        else if (data instanceof short[]) write((short[]) data);
        else if (data instanceof char[]) write((char[]) data);
        else if (data instanceof int[]) write((int[]) data);
        else if (data instanceof long[]) write((long[]) data);
        else if (data instanceof float[]) write((float[]) data);
        else if (data instanceof double[]) write((double[]) data);
        else if (data instanceof Iterable) writeIterable((Iterable) data);
        else if (data instanceof Map) writeMap((Map) data);
        else if (data instanceof ISerializable) write((ISerializable) data);
        else throw new BufferException("trying to write an unsupported type - " + data.getClass().getSimpleName() + ".");
    }

    public void write(ISerializable... data) {
        for (ISerializable serializable : data) {
            serializable.serialize(this);
        }
    }

    public void write(ISerializable data) { data.serialize(this); }

    public <T> void write(ISerializer<T> serializer, T... data) {
        for (T elem : data) {
            serializer.serialize(elem, this);
        }
    }

    public <T> void write(ISerializer<T> serializer, T data) { serializer.serialize(data, this); }

    private void writeIterable(Iterable data) throws BufferException {
        for (Object datum : data) {
            write(datum);
        }
    }

    public void writeMap(Map data) {
        for (Object entry : data.entrySet()) {
            write(((Map.Entry)entry).getKey());
            write(((Map.Entry)entry).getValue());
        }
    }

    //region Write Primitive

    public void write(byte data) throws BufferException {
        this.alloc_buffer(1);
        this.data[pointer++] = data;
    }

    public void write(boolean data) throws BufferException {
        if (data) write((byte) 1);
        else write((byte) 0);
    }

    public void write(short data) throws BufferException {
        this.alloc_buffer(2);
        this.data[pointer++] = (byte) (data >> 8);
        this.data[pointer++] = (byte) (data & 0xff);
    }

    public void write(char data) throws BufferException {
        this.alloc_buffer(2);
        this.data[pointer++] = (byte) (data >> 8);
        this.data[pointer++] = (byte) (data & 0xff);
    }

    public void write(int data) throws BufferException {
        this.alloc_buffer(4);
        this.data[pointer++] = (byte) (data >> 24);
        this.data[pointer++] = (byte) (data >> 16);
        this.data[pointer++] = (byte) (data >> 8);
        this.data[pointer++] = (byte) (data & 0xff);
    }

    public void write(long data) throws BufferException {
        this.alloc_buffer(8);
        this.data[pointer++] = (byte) (data >> 56);
        this.data[pointer++] = (byte) (data >> 48);
        this.data[pointer++] = (byte) (data >> 40);
        this.data[pointer++] = (byte) (data >> 32);
        this.data[pointer++] = (byte) (data >> 24);
        this.data[pointer++] = (byte) (data >> 16);
        this.data[pointer++] = (byte) (data >> 8);
        this.data[pointer++] = (byte) (data & 0xff);
    }

    public void write(float data) throws BufferException {
        this.alloc_buffer(4);
        int intVal = Float.floatToIntBits(data);
        this.write(intVal);
    }

    public void write(double data) throws BufferException {
        this.alloc_buffer(8);
        long longVal = Double.doubleToLongBits(data);
        this.write(longVal);
    }


    //region Write Primitive Array

    public void write(byte[] data) throws BufferException {
        for (byte b : data) {
            write(b);
        }
    }

    public void write(boolean[] data) throws BufferException {
        for(boolean b: data) {
            write(b);
        }
    }

    public void write(short[] data) throws BufferException {
        for (short s : data) {
            write(s);
        }
    }

    public void write(char[] data) throws BufferException {
        for (char c : data) {
            write(c);
        }
    }

    public void write(int[] data) throws BufferException {
        for (int i : data) {
            write(i);
        }
    }

    public void write(long[] data) throws BufferException {
        for (long l : data) {
            write(l);
        }
    }

    public void write(float[] data) throws BufferException {
        for (float f : data) {
            write(f);
        }
    }

    public void write(double[] data) throws BufferException {
        for (double d : data) {
            write(d);
        }
    }

    //endregion
    //endregion

    //endregion


    //region Read

    public <T> void read(T... dest) {
        for (T elem : dest) {
            read(elem);
        }
    }

    public <T> void read(T dest) throws BufferException {
        if (dest instanceof Byte)           unsafeReadByte((Byte)       dest);
        else if (dest instanceof Boolean)   unsafeReadBoolean((Boolean) dest);
        else if (dest instanceof Short)     unsafeReadShort((Short)     dest);
        else if (dest instanceof Character) unsafeReadChar((Character)  dest);
        else if (dest instanceof Integer)   unsafeReadInt((Integer)     dest);
        else if (dest instanceof Float)     unsafeReadFloat((Float)     dest);
        else if (dest instanceof Long)      unsafeReadLong((Long)       dest);
        else if (dest instanceof Double)    unsafeReadDouble((Double)   dest);
        else if (dest instanceof byte[])    read((byte[])   dest);
        else if (dest instanceof boolean[]) read((boolean[])dest);
        else if (dest instanceof short[])   read((short[])  dest);
        else if (dest instanceof char[])    read((char[])   dest);
        else if (dest instanceof int[])     read((int[])    dest);
        else if (dest instanceof long[])    read((long[])   dest);
        else if (dest instanceof float[])   read((float[])  dest);
        else if (dest instanceof double[])  read((double[]) dest);
        else if (dest instanceof ISerializable) read((ISerializable) dest);
        else throw new BufferException("trying to read an unsupported type - " + dest.getClass().getSimpleName() + ".");
    }

    private Unsafe getUnsafe() {
        if (this.unsafe != null) return this.unsafe;
        try {
            Field field = Unsafe.class.getDeclaredField("theUnsafe");
            field.setAccessible(true);
            this.unsafe = (Unsafe)field.get(null);
        } catch (NoSuchFieldException | IllegalAccessException e) {
            e.printStackTrace();
        }
        return this.unsafe;
    }

    public void read(ISerializable... data) {
        for (int i = 0; i < data.length; i++) {
            if (data[i] == null) {
                try {
                    data[i] = (ISerializable) data.getClass().getComponentType().newInstance();
                } catch (InstantiationException | IllegalAccessException e) {
                    e.printStackTrace();
                }
            }
            data[i].deserialize(this);
        }
    }

    public void read(ISerializable data) {
        data.deserialize(this);
    }

    public <T> void read(IDeserializer<T> deserializer, T... dest) {
        for (T elem : dest) {
            deserializer.deserialize(elem, this);
        }
    }

    public <T> void read(IDeserializer<T> deserializer, T dest) {
        if (dest == null) {
            Class<T> type = deserializer.getTypeClass();
            try { dest = type.newInstance(); }
            catch (InstantiationException | IllegalAccessException e) {
                throw new BufferIllegalReadException("Cannot read object of type - " + type.getSimpleName() +
                        ", all readable classes must have an empty constructor");
            }
        }
        deserializer.deserialize(dest, this);
    }

    private static <T> T instantiate(Class<T> c) throws IllegalAccessException, InstantiationException {
        if (c.getSimpleName().equals("Byte")) return (T) new Byte((byte)0);
        else if (c.getSimpleName().equals("Boolean")) return (T) new Boolean(false);
        else if (c.getSimpleName().equals("Short")) return (T) new Short((short)0);
        else if (c.getSimpleName().equals("Character")) return (T) new Character((char)0);
        else if (c.getSimpleName().equals("Integer")) return (T) new Integer(0);
        else if (c.getSimpleName().equals("Float")) return (T) new Float(0f);
        else if (c.getSimpleName().equals("Long")) return (T) new Long(0l);
        else if (c.getSimpleName().equals("Double")) return (T) new Double(0d);
        return c.newInstance();
    }

    public <T> void read(Collection<T> dest, Class<T> type, int amount) throws BufferException {
        for (int i = 0; i < amount; i++) {
            T elem = null;
            try { elem = instantiate(type); }
            catch (IllegalAccessException | InstantiationException e) {
                throw new BufferIllegalReadException("Cannot read object of type - " + type.getSimpleName() +
                        ", all readable classes must have an empty constructor");
            }
            read(elem);
            dest.add(elem);
        }
    }

    public <T> void read(Collection<T> dest, IDeserializer<T> deserializer, int amount) throws BufferException {
        for (int i = 0; i < amount; i++) {
            T elem = null;
            Class<T> type = deserializer.getTypeClass();
            try { elem = instantiate(type); }
            catch (InstantiationException | IllegalAccessException e) {
                throw new BufferIllegalReadException("Cannot read object of type - " + type.getSimpleName() +
                        ", all readable classes must have an empty constructor");
            }
            read(elem);
            dest.add(elem);
        }
    }

    public <K, V> void read(Map<K, V> dest, Class<K> keyType, Class<V> valueType, int amount) throws BufferException {
        for (int i = 0; i < amount; i++) {
            K key = null;
            V value = null;
            try {
                key = instantiate(keyType);
                value = instantiate(valueType);
            } catch (IllegalAccessException | InstantiationException e) {
                throw new BufferIllegalReadException("Cannot read object of type - Map<" + keyType.getSimpleName() +
                       ", " + valueType + "> , all readable classes must have an empty constructor");
            }
            read(key);
            read(value);
            dest.put(key, value);
        }
    }

    public <K, V> void read(Map<K, V> dest, IDeserializer<K> keyDe, IDeserializer<V> valueDe, int amount) throws BufferException {
        K key = null;
        V value = null;
        Class<K> keyType = keyDe.getTypeClass();
        Class<V> valueType = valueDe.getTypeClass();
        try {
            key = instantiate(keyType);
            value = instantiate(valueType);
        } catch (IllegalAccessException | InstantiationException e) {
            throw new BufferIllegalReadException("Cannot read object of type - Map<" + keyType.getSimpleName() +
                    ", " + valueType + "> , all readable classes must have an empty constructor");
        }
        keyDe.deserialize(key, this);
        valueDe.deserialize(value, this);
        dest.put(key, value);
    }

    //region Read Primitive

    public byte readByte() {
        return this.data[pointer++];
    }

    private void unsafeReadByte(Byte dest) {
        Unsafe unsafe = getUnsafe();
        Field value;
        try {
            value = dest.getClass().getDeclaredField("value");
            unsafe.putByte(dest, unsafe.objectFieldOffset(value), readByte());
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        }
    }

    public boolean readBoolean() {
        return this.data[pointer++] == 1;
    }

    private void unsafeReadBoolean(Boolean dest) {
        Unsafe unsafe = getUnsafe();
        Field value;
        try {
            value = dest.getClass().getDeclaredField("value");
            unsafe.putBoolean(dest, unsafe.objectFieldOffset(value), readBoolean());
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        }
    }

    public short readShort() {
        byte b1 = data[pointer++];
        byte b0 = data[pointer++];

        return (short) (((b1 & 0xff) << 8) | (b0 & 0xff));
    }

    private void unsafeReadShort(Short dest) {
        Unsafe unsafe = getUnsafe();
        Field value;
        try {
            value = dest.getClass().getDeclaredField("value");
            unsafe.putShort(dest, unsafe.objectFieldOffset(value), readShort());
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        }
    }

    public char readChar() {
        byte b1 = data[pointer++];
        byte b0 = data[pointer++];

        return (char) (((b1 & 0xff) << 8) | (b0 & 0xff));
    }

    private void unsafeReadChar(Character dest) {
        Unsafe unsafe = getUnsafe();
        Field value;
        try {
            value = dest.getClass().getDeclaredField("value");
            unsafe.putChar(dest, unsafe.objectFieldOffset(value), readChar());
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        }
    }

    public int readInt() {
        byte b3 = data[pointer++];
        byte b2 = data[pointer++];
        byte b1 = data[pointer++];
        byte b0 = data[pointer++];

        return (((b3 & 0xff) << 24) | ((b2 & 0xff) << 16) |((b1 & 0xff) << 8) | (b0 & 0xff));
    }

    private void unsafeReadInt(Integer dest) {
        Unsafe unsafe = getUnsafe();
        Field value;
        try {
            value = dest.getClass().getDeclaredField("value");
            unsafe.putInt(dest, unsafe.objectFieldOffset(value), readInt());
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        }
    }

    public long readLong() {
        byte b7 = data[pointer++];
        byte b6 = data[pointer++];
        byte b5 = data[pointer++];
        byte b4 = data[pointer++];
        byte b3 = data[pointer++];
        byte b2 = data[pointer++];
        byte b1 = data[pointer++];
        byte b0 = data[pointer++];

        return  (((b7 & 0xffL) << 56) | ((b6 & 0xffL) << 48) | ((b5 & 0xffL) << 40) | ((b4 & 0xffL) << 32)
                | ((b3 & 0xffL) << 24) | ((b2 & 0xffL) << 16) | ((b1 & 0xffL) << 8) | (b0 & 0xffL));
    }

    private void unsafeReadLong(Long dest) {
        Unsafe unsafe = getUnsafe();
        Field value;
        try {
            value = dest.getClass().getDeclaredField("value");
            unsafe.putLong(dest, unsafe.objectFieldOffset(value), readLong());
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        }
    }

    public float readFloat() {
        return Float.intBitsToFloat(readInt());
    }

    private void unsafeReadFloat(Float dest) {
        Unsafe unsafe = getUnsafe();
        Field value;
        try {
            value = dest.getClass().getDeclaredField("value");
            unsafe.putFloat(dest, unsafe.objectFieldOffset(value), readFloat());
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        }
    }

    public double readDouble() {
        return Double.longBitsToDouble(readLong());
    }

    private void unsafeReadDouble(Double dest) {
        Unsafe unsafe = getUnsafe();
        Field value;
        try {
            value = dest.getClass().getDeclaredField("value");
            unsafe.putDouble(dest, unsafe.objectFieldOffset(value), readDouble());
        } catch (NoSuchFieldException e) {
            e.printStackTrace();
        }
    }

    //region Read Primitive Array

    public void read(byte[] dest) throws BufferException {
        if (this.data.length < this.pointer + dest.length)
            throw new BufferOverflowException("trying to read outside the buffer bounds.");
        for (int i = 0; i < dest.length; i++) {
            dest[i] = this.data[pointer++];
        }
    }

    public void read(boolean[] dest) throws BufferException {
        if(this.data.length < this.pointer + dest.length)
            throw new BufferOverflowException("trying to read outside the buffer bounds.");
        for (int i = 0; i < dest.length; i++) {
            dest[i] = this.data[pointer++] == 1;
        }
    }

    public void read(short[] dest) throws BufferException {
        if (this.data.length < this.pointer + dest.length)
            throw new BufferOverflowException("trying to read outside the buffer bounds.");
        for (int i = 0; i < dest.length; i++) {
            dest[i] = readShort();
        }
    }

    public void read(char[] dest) throws BufferException {
        if (this.data.length < this.pointer + dest.length)
            throw new BufferOverflowException("trying to read outside the buffer bounds.");
        for (int i = 0; i < dest.length; i++) {
            dest[i] = readChar();
        }
    }

    public void read(int[] dest) throws BufferException {
        if (this.data.length < this.pointer + dest.length)
            throw new BufferOverflowException("trying to read outside the buffer bounds.");
        for (int i = 0; i < dest.length; i++) {
            dest[i] = readInt();
        }
    }

    public void read(long[] dest) throws BufferException {
        if (this.data.length < this.pointer + dest.length)
            throw new BufferOverflowException("trying to read outside the buffer bounds.");
        for (int i = 0; i < dest.length; i++) {
            dest[i] = readLong();
        }
    }

    public void read(float[] dest) throws BufferException {
        if (this.data.length < this.pointer + dest.length)
            throw new BufferOverflowException("trying to read outside the buffer bounds.");
        for (int i = 0; i < dest.length; i++) {
            dest[i] = readFloat();
        }
    }

    public void read(double[] dest) throws BufferException {
        if (this.data.length < this.pointer + dest.length)
            throw new BufferOverflowException("trying to read outside the buffer bounds.");
        for (int i = 0; i < dest.length; i++) {
            dest[i] = readDouble();
        }
    }

    //endregion

    //endregion

    //endregion
}
