import sun.misc.Unsafe;

import java.lang.reflect.Field;
import java.nio.BufferOverflowException;
import java.nio.BufferUnderflowException;
import java.util.*;

/**
 * The buffer stores serialized data,
 * it contains a byte array with all the serialized data that was written to it.
 * A buffer may be in one of two modes: <br>
 * * WRITE - allows only writes to the buffer. <br>
 * * READ - allows only reads from the buffer. <br>
 * note: switching between WRITE and READ modes is relatively costly
 * thus it is recommended to structure the use of the buffer so that reading and writing do not overlap,
 * for example the code should look something like:     <br>
 *      Buffer buf = new Buffer();                      <br>
 *      ------some other operations-----                <br>
 *      buf.write(some_data);                           <br>
 *      buf.write(some_more_data);                      <br>
 *      ------some more operations------                <br>
 *      buf.setRead();                                  <br>
 *      buf.read(read_into_this);                       <br>
 *      ------end of use for buf--------                <br>
 *
 * As well as modes a buffer may also have two types:
 * * DYNAMIC - will allocate new space to the buffer if needed.
 * * STATIC - will not allocate new space, and throw an exception if a write is attempted
 *   when there isn't enough space.
 */
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
	 * @throws IllegalStateException if additional allocation is attempted on a STATIC buffer.
	 */
	private void alloc_buffer(int size) throws IllegalStateException {
		if (this.data.length - this.pointer < size) {
			if (this.type == TYPE.STATIC)
				throw new IllegalStateException("cannot change size of STATIC buffer.");
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
		this.unsafe = other.unsafe;
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

	/**
	 * Sets the Buffer to WRITE mode, enabling changing the contents of the buffer.
	 * @param type the type of the newly writable Buffer.
	 * @see Buffer#setWrite(TYPE, int)
	 */
	public void setWrite(TYPE type) {
		this.setWrite(type, 0);
	}

	/**
	 * Sets the Buffer to WRITE mode, enabling changing the contents of the buffer.
	 * The TYPE of the buffer will be changed to DYNAMIC.
	 * @see Buffer#setWrite(TYPE, int)
	 */
	public void setWrite() {
		this.setWrite(TYPE.DYNAMIC, 0);
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

	/**
	 * Moves the read/write pointer of the buffer by adding 'amount' to the current pointer.
	 * In order to move the pointer backwards set amount to a negative value.
	 * note: that the pointer could be moved past the last byte that was written into
	 * space that was allocated but is not written, because of this it is not recommended
	 * changing the pointer while in WRITE mode.
	 * @param amount signed amount of integers to skip.
	 * @throws BufferOverflowException if the pointer is moved past the end of the buffer.
	 * @throws BufferUnderflowException if the pointer is moved to a position smaller than 0.
	 */
	public void seekByte(int amount) throws BufferOverflowException, BufferUnderflowException {
		if (this.data.length < this.pointer + amount)
			throw new BufferOverflowException();
		if (this.pointer + amount < 0)
			throw new BufferUnderflowException();
		this.pointer += amount;
	}

	/**
	 * Moves the read/write pointer to the next byte.
	 * @throws BufferOverflowException if the pointer is currently pointing to the last allocated byte.
	 * @see Buffer#seekByte(int)
	 */
	public void nextByte() throws BufferOverflowException { seekByte(1); }

	/**
	 * Moves the read/write pointer to the previous byte.
	 * @throws BufferUnderflowException if the pointer is currently pointing to the first byte in the buffer.
	 * @see Buffer#seekByte(int)
	 */
	public void prevByte() throws BufferUnderflowException { seekByte(-1); }

	/**
	 * Moves the read/write pointer to the start of the buffer (index 0).
	 * Writing after a rewind will overwrite the data in the buffer,
	 * this makes the rewind equivalent to clear operation.
	 */
	public void rewind() { this.pointer = 0; }

	//region Write

	/**
	 * Write all the data into the buffer in order from left to right,
	 * that is for a call write(D1, D2, D3, ... ,Dn)
	 * the data will be written into the buffer in the order of D1 -> D2 -> D3 ... -> Dn.
	 * @param data the data to be written to the buffer.
	 * @param <T> the type of the data that will be written.
	 * @throws IllegalArgumentException if T is not a supported type.
	 * @throws IllegalStateException if the buffer is in READ mode or it is STATIC and there is not enough space.
	 */
	public <T> void write(T... data) throws IllegalArgumentException, IllegalStateException {
		for (T elem : data) {
			write(elem);
		}
	}

	/**
	 * Writes the given data into the buffer.
	 * @param data the data to be written to the buffer.
	 * @param <T> the type of the data that will be written.
	 * @throws IllegalArgumentException if T is not a supported type.
	 * @throws IllegalStateException if the buffer is in READ mode or it is STATIC and there is not enough space.
	 */
	public <T> void write(T data) throws IllegalArgumentException, IllegalStateException {
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
		else throw new IllegalArgumentException("trying to write an unsupported type - " + data.getClass().getSimpleName() + ".");
	}

	/**
	 * Write all the data into the buffer in order from left to right,
	 * that is for a call write(D1, D2, D3, ... ,Dn)
	 * the data will be written into the buffer in the order of D1 -> D2 -> D3 ... -> Dn.
	 * @param data the data to be written into the buffer.
	 * @throws IllegalStateException if the buffer is in READ mode or it is STATIC and there is not enough space.
	 */
	public void write(ISerializable... data) throws IllegalStateException {
		if (this.mode == MODE.READ) throw new IllegalStateException("Cannot write to buffer while in READ mode.");
		for (ISerializable serializable : data) {
			serializable.serialize(this);
		}
	}

	/**
	 * Writes the given data into the buffer.
	 * @param data the data to be written to the buffer.
	 * @throws IllegalStateException if the buffer is in READ mode or it is STATIC and there is not enough space.
	 */
	public void write(ISerializable data) throws IllegalStateException {
		if (this.mode == MODE.READ) throw new IllegalStateException("Cannot write to buffer while in READ mode.");
		data.serialize(this);
	}

	/**
	 * Write all the data into the buffer using the given serializer.
	 * the writing is done in order from left to right, that is for a call write(serializer, D1, D2, D3, ... ,Dn)
	 * the data will be written into the buffer in the order of D1 -> D2 -> D3 ... -> Dn.
	 * @param serializer the serializer that should be used to write the data.
	 * @param data the data to be written into the buffer.
	 * @param <T> the type of the data that will be written.
	 * @throws IllegalStateException if the buffer is in READ mode or it is STATIC and there is not enough space.
	 */
	public <T> void write(ISerializer<T> serializer, T... data) throws IllegalStateException {
		if (this.mode == MODE.READ) throw new IllegalStateException("Cannot write to buffer while in READ mode.");
		for (T elem : data) {
			serializer.serialize(elem, this);
		}
	}

	/**
	 * Writes the given data into the buffer.
	 * @param serializer the serializer that should be used to write the data.
	 * @param data the data to be written to the buffer.
	 * @param <T> the type of the data that will be written.
	 * @throws IllegalStateException if the buffer is in READ mode or it is STATIC and there is not enough space.
	 */
	public <T> void write(ISerializer<T> serializer, T data) throws IllegalStateException {
		if (this.mode == MODE.READ) throw new IllegalStateException("Cannot write to buffer while in READ mode.");
		serializer.serialize(data, this);
	}

    /*
    These two functions are used internally to deconstruct Iterable and Map types into their elements,
    and then return the elements to be written through the generic write method.
     */
	private void writeIterable(Iterable data) throws IllegalStateException {
		for (Object itr : data) {
			write(itr);
		}
	}

	private void writeMap(Map data) throws IllegalStateException {
		for (Object entry : data.entrySet()) {
			write(((Map.Entry)entry).getKey());
			write(((Map.Entry)entry).getValue());
		}
	}

	/**
	 * Write all the data into the buffer using the given serializer.
	 * the writing is done in order from left to right, that is for a call write(serializer, IT1, IT2, IT3, ... ,ITn)
	 * the data will be written into the buffer in the order of IT1 -> IT2 -> IT3 ... -> ITn.
	 * note: the serializer should be for the type contained in each Iterable not for the Iterable itself.
	 * @param serializer the serializer that should be used to write the data.
	 * @param data the data to be written to the buffer.
	 * @param <T> the type of the data that will be written.
	 * @throws IllegalStateException if the buffer is in READ mode or it is STATIC and there is not enough space.
	 */
	public <T> void write(ISerializer<T> serializer, Iterable<T>... data) throws IllegalStateException{
		if (this.mode == MODE.READ) throw new IllegalStateException("Cannot write to buffer while in READ mode.");
		for (Iterable<T> iterable : data) {
			for (T itr : iterable) {
				serializer.serialize(itr, this);
			}
		}
	}

	/**
	 * Write all the data into the buffer using the given serializer.
	 * the writing is done in order from left to right, that is for a call write(keySer, valueSer, M1, M2, M3, ... ,Mn)
	 * the data will be written into the buffer in the order of M1 -> M2 -> M3 ... -> Mn.
	 * @param keySer the serializer that should be used for the key type
	 * @param valueSer the serializer that should be used for the value type.
	 * @param data the data to be written.
	 * @param <K> the key type of the Maps.
	 * @param <V> the value type of the Maps.
	 */
	public <K, V> void write(ISerializer<K> keySer, ISerializer<V> valueSer, Map<K, V>... data) {
		if (this.mode == MODE.READ) throw new IllegalStateException("Cannot write to buffer while in READ mode.");
		for (Map<K, V> map : data) {
			for (Map.Entry<K, V> entry : map.entrySet()) {
				keySer.serialize(entry.getKey(), this);
				valueSer.serialize(entry.getValue(), this);
			}
		}
	}

	//region Write Primitive

	public void write(byte data) throws IllegalStateException {
		if (this.mode == MODE.READ) throw new IllegalStateException("Cannot write to buffer while in READ mode.");
		this.alloc_buffer(1);
		this.data[pointer++] = data;
	}

	public void write(boolean data) throws IllegalStateException {
		if (this.mode == MODE.READ) throw new IllegalStateException("Cannot write to buffer while in READ mode.");
		if (data) write((byte) 1);
		else write((byte) 0);
	}

	public void write(short data) throws IllegalStateException {
		if (this.mode == MODE.READ) throw new IllegalStateException("Cannot write to buffer while in READ mode.");
		this.alloc_buffer(2);
		this.data[pointer++] = (byte) (data >> 8);
		this.data[pointer++] = (byte) (data & 0xff);
	}

	public void write(char data) throws IllegalStateException {
		if (this.mode == MODE.READ) throw new IllegalStateException("Cannot write to buffer while in READ mode.");
		this.alloc_buffer(2);
		this.data[pointer++] = (byte) (data >> 8);
		this.data[pointer++] = (byte) (data & 0xff);
	}

	public void write(int data) throws IllegalStateException {
		if (this.mode == MODE.READ) throw new IllegalStateException("Cannot write to buffer while in READ mode.");
		this.alloc_buffer(4);
		this.data[pointer++] = (byte) (data >> 24);
		this.data[pointer++] = (byte) (data >> 16);
		this.data[pointer++] = (byte) (data >> 8);
		this.data[pointer++] = (byte) (data & 0xff);
	}

	public void write(long data) throws IllegalStateException {
		if (this.mode == MODE.READ) throw new IllegalStateException("Cannot write to buffer while in READ mode.");
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

	public void write(float data) throws IllegalStateException {
		if (this.mode == MODE.READ) throw new IllegalStateException("Cannot write to buffer while in READ mode.");
		this.alloc_buffer(4);
		int intVal = Float.floatToIntBits(data);
		this.write(intVal);
	}

	public void write(double data) throws IllegalStateException {
		if (this.mode == MODE.READ) throw new IllegalStateException("Cannot write to buffer while in READ mode.");
		this.alloc_buffer(8);
		long longVal = Double.doubleToLongBits(data);
		this.write(longVal);
	}


	//region Write Primitive Array

	public void write(byte[] data) throws IllegalStateException {
		for (byte b : data) {
			write(b);
		}
	}

	public void write(boolean[] data) throws IllegalStateException {
		for(boolean b: data) {
			write(b);
		}
	}

	public void write(short[] data) throws IllegalStateException {
		for (short s : data) {
			write(s);
		}
	}

	public void write(char[] data) throws IllegalStateException {
		for (char c : data) {
			write(c);
		}
	}

	public void write(int[] data) throws IllegalStateException {
		for (int i : data) {
			write(i);
		}
	}

	public void write(long[] data) throws IllegalStateException {
		for (long l : data) {
			write(l);
		}
	}

	public void write(float[] data) throws IllegalStateException {
		for (float f : data) {
			write(f);
		}
	}

	public void write(double[] data) throws IllegalStateException {
		for (double d : data) {
			write(d);
		}
	}

	//endregion

	//endregion

	//endregion

	//region Read

	/*
	This function is used in order to instantiate an element in a collection,
	it defines special behavior for the java Boxed primitives who do not define an empty constructor,
	instead they are instantiated with the value 0 (char is counted among the numeric values) for numeric types and false for booleans.
	 */
	private static <T> T instantiate(Class<T> c) throws IllegalAccessException, InstantiationException {
		if (c.equals(Byte.class)) return (T) new Byte((byte)0);
		else if (c.equals(Boolean.class)) return (T) Boolean.FALSE;
		else if (c.equals(Short.class)) return (T) new Short((short)0);
		else if (c.equals(Character.class)) return (T) new Character((char)0);
		else if (c.equals(Integer.class)) return (T) new Integer(0);
		else if (c.equals(Float.class)) return (T) new Float(0f);
		else if (c.equals(Long.class)) return (T) new Long(0L);
		else if (c.equals(Double.class)) return (T) new Double(0d);
		return c.newInstance();
	}

	/**
	 * 	Function used to obtain instance of the Unsafe Object, naturally this should be used unless
	 * 	you know what you are doing.
	 *
	 * 	Unsafe is used in this library *only* to change the value of Boxed primitive types
	 * 	that is necessary in order to be able to read collections of primitive types.
	 * 	For example consider an ArrayList< Integer >, writing this object is very simple using an iterator,
	 * 	reading on the other hand is much more complicated.
	 * 	You might think well all i need to do is read each int from the buffer and then add it to the list,
	 * 	but during runtime you do not know the type of the object stored in the list and thus cannot call
	 * 	the readInt() method, and so similarly to the other implementations you must use a "pointer" to an unknown type
	 * 	that is passed to the read method and then the method reads the data into the memory the pointer points to.
	 * 	Since pointers are not supported in java, we use Boxed types, but boxed types are immutable that is once
	 * 	an Integer is created its value cannot change, that is were Unsafe comes in, it used to change the value
	 * 	of the private field holding the boxed value of the Integer.
	 * 	If you take the metaphor of a boxed type being a pointer the use of Unsafe allows the dereference operation
	 * 	that is if we have int* ip = 0; we can say *ip = 4; and change the value being pointed to.
	 * @return instance of the Unsafe Object.
	 */
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

	/**
	 * Reads data form the buffer into the provided objects.
	 * The reading is done in order from left to right, meaning that given the call read(D1, D2, ..., Dn)
	 * the data will be read from the buffer in the order - D1 -> D2 -> ... -> Dn.
	 * @param dest objects to read the data into.
	 * @param <T> the type of data that will be read.
	 * @throws IllegalArgumentException if the data type is unsupported.
	 * @throws IllegalStateException if the buffer is in write mode.
	 */
	public <T> void read(T... dest) throws IllegalArgumentException, IllegalStateException {
		for (T elem : dest) {
			read(elem);
		}
	}

	/**
	 * Reads data from the buffer into the provided 'dest'.
	 * @param dest an object to read the data into.
	 * @param <T> the type of the data that will be read.
	 * @throws IllegalArgumentException if the data type is unsupported.
	 * @throws IllegalStateException if the buffer is in write mode.
	 */
	public <T> void read(T dest) throws IllegalArgumentException, IllegalStateException {
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
		else throw new IllegalArgumentException("trying to read an unsupported type - " + dest.getClass().getSimpleName() + ".");
	}

	/**
	 * Reads data form the buffer into the provided objects.
	 * The reading is done in order from left to right, meaning that given the call read(D1, D2, ..., Dn)
	 * the data will be read from the buffer in the order - D1 -> D2 -> ... -> Dn.
	 * @param dest objects to read the data into.
	 * @throws IllegalStateException if the buffer is in write mode.
	 */
	public void read(ISerializable... dest) throws IllegalStateException {
		if (this.mode == MODE.WRITE) throw new IllegalStateException("Cannot read from buffer while in WRITE mode.");
		for (int i = 0; i < dest.length; i++) {
			if (dest[i] == null) {
				try {
					dest[i] = (ISerializable) dest.getClass().getComponentType().newInstance();
				} catch (InstantiationException | IllegalAccessException e) {
					e.printStackTrace();
				}
			}
			dest[i].deserialize(this);
		}
	}

	/**
	 * Reads data from the buffer into the provided 'dest'.
	 * @param dest an object to read the data into.
	 * @throws IllegalStateException if the buffer is in write mode.
	 */
	public void read(ISerializable dest) throws IllegalStateException {
		if (this.mode == MODE.WRITE) throw new IllegalStateException("Cannot read from buffer while in WRITE mode.");
		dest.deserialize(this);
	}

	/**
	 * Reads data form the buffer into the provided objects using the 'deserializer'.
	 * The reading is done in order from left to right, meaning that given the call read(D1, D2, ..., Dn)
	 * the data will be read from the buffer in the order - D1 -> D2 -> ... -> Dn.
	 * @param deserializer the deserializer that should be used to read the data.
	 * @param dest objects to read the data into.
	 * @throws IllegalStateException if the buffer is in write mode.
	 */
	public <T> void read(IDeserializer<T> deserializer, T... dest) throws IllegalStateException {
		if (this.mode == MODE.WRITE) throw new IllegalStateException("Cannot read from buffer while in WRITE mode.");
		for (T elem : dest) {
			deserializer.deserialize(elem, this);
		}
	}

	/**
	 * Reads data from the buffer into the provided 'dest' using the 'deserializer'.
	 * @param deserializer deserializer that should be used to read the data.
	 * @param dest an object to read the data into.
	 * @throws IllegalStateException if the buffer is in write mode.
	 */
	public <T> void read(IDeserializer<T> deserializer, T dest) throws IllegalStateException {
		if (this.mode == MODE.WRITE) throw new IllegalStateException("Cannot read from buffer while in WRITE mode.");
		deserializer.deserialize(dest, this);
	}

    /*
    The two following methods ask for a Class object of the type that is being read,
    that is if you want to read 10 elements into ArrayList<Integer> the call should
    look something like - read(myArrayList, Integer.class, 10).
    What in the world is that Integer.class you might be wondering why should do i need to provide the Class
    object myself cant that be solved using generics ?
    Well it seems not, in order to read into the Collection or the Map we need to construct new instances
    of a generic type T (or K, V for Map) the only way to do such a thing with java would be using the
    Class.newInstance() method but there is no way to get the Class object of T from the Collection object,
    that's why the user must provide the Class object himself.
    (In other languages like C# or C++ we would simply be able to say T elem = new T();
    though such syntax requires verifying that the type T has an empty constructor)
     */

	/**
	 * Reads 'amount' objects of type T from the buffer into the collection.
	 * note: to provide the Class object for some type T simply write T.class,
	 * for example - in order to read 5 elements into ArrayList< Integer> the method call should look
	 * something like this, read(myList, Integer.class, 5);
	 * @param dest the collection to read the data into.
	 * @param type the type of data that will be read.
	 * @param amount the number of objects of type T that will be read.
	 * @param <T> the type of the data that will be read.
	 * @throws IllegalStateException if the buffer is in write mode.
	 */
	public <T> void read(Collection<T> dest, Class<T> type, int amount) throws IllegalStateException {
		for (int i = 0; i < amount; i++) {
			T elem;
			try { elem = instantiate(type); }
			catch (IllegalAccessException | InstantiationException e) {
				throw new IllegalArgumentException("Cannot read object of type - " + type.getSimpleName() +
						", all readable classes must have an empty constructor");
			}
			read(elem);
			dest.add(elem);
		}
	}

	/**
	 * Reads 'amount' key value pairs of type < K, V > from the buffer into the collection.
	 * note: to provide the Class object for some type T simply write T.class,
	 * for example - in order to read 5 mappings into Map< Integer, Integer > the method call should
	 * look something like this, read(myMap, Integer.class, Integer.class, 5);
	 * @param dest the map to read the data into.
	 * @param keyType the Class of the keys in the map.
	 * @param valueType the Class of the values in the map.
	 * @param amount the number of objects of type T that will be read.
	 * @param <K> the type of the values in the map.
	 * @param <V> the type of the keys in the map.
	 * @throws IllegalStateException if the buffer is in write mode.
	 */
	public <K, V> void read(Map<K, V> dest, Class<K> keyType, Class<V> valueType, int amount) throws IllegalStateException {
		for (int i = 0; i < amount; i++) {
			K key;
			V value;
			try {
				key = instantiate(keyType);
				value = instantiate(valueType);
			} catch (IllegalAccessException | InstantiationException e) {
				throw new IllegalArgumentException("Cannot read object of type - Map<" + keyType.getSimpleName() +
						", " + valueType + "> , all readable classes must have an empty constructor");
			}
			read(key);
			read(value);
			dest.put(key, value);
		}
	}

	/**
	 * Reads 'amount' objects of type T from the buffer into the collection using the provided 'deserializer'.
	 * note: to provide the Class object for some type T simply write T.class,
	 * for example - in order to read 5 elements into ArrayList< Integer> the method call should look
	 * something like this, read(myList, Integer.class, 5);
	 * @param dest the collection to read the data into.
	 * @param deserializer the deserializer that should be used to read the data.
	 * @param amount the number of objects of type T that will be read.
	 * @param <T> the type of the data that will be read.
	 * @throws IllegalStateException if the buffer is in write mode.
	 */
	public <T> void read(Collection<T> dest, IDeserializer<T> deserializer, int amount) throws IllegalStateException {
		if (this.mode == MODE.WRITE) throw new IllegalStateException("Cannot read from buffer while in WRITE mode.");
		for (int i = 0; i < amount; i++) {
			T elem;
			Class<T> type = deserializer.getTypeClass();
			try { elem = instantiate(type); }
			catch (InstantiationException | IllegalAccessException e) {
				throw new IllegalArgumentException("Cannot read object of type - " + type.getSimpleName() +
						", all readable classes must have an empty constructor");
			}
			read(elem);
			dest.add(elem);
		}
	}

	/**
	 * Reads 'amount' key value pairs of type < K, V > from the buffer into the collection.
	 * @param dest the map to read the data into.
	 * @param keyDe the deserializer that should be used to read keys.
	 * @param valueDe the deserializer that should be used to read values.
	 * @param amount the number of objects of type T that will be read.
	 * @param <K> the type of the values in the map.
	 * @param <V> the type of the keys in the map.
	 * @throws IllegalStateException if the buffer is in write mode.
	 */
	public <K, V> void read(Map<K, V> dest, IDeserializer<K> keyDe, IDeserializer<V> valueDe, int amount) throws IllegalStateException {
		if (this.mode == MODE.WRITE) throw new IllegalStateException("Cannot read from buffer while in WRITE mode.");
		K key;
		V value;
		Class<K> keyType = keyDe.getTypeClass();
		Class<V> valueType = valueDe.getTypeClass();
		try {
			key = instantiate(keyType);
			value = instantiate(valueType);
		} catch (IllegalAccessException | InstantiationException e) {
			throw new IllegalArgumentException("Cannot read object of type - Map<" + keyType.getSimpleName() +
					", " + valueType + "> , all readable classes must have an empty constructor");
		}
		keyDe.deserialize(key, this);
		valueDe.deserialize(value, this);
		dest.put(key, value);
	}

	//region Read Primitive

	/*
	The methods in this section read primitive types,
	in order to read type T (assuming T is primitive) use:
	T myPrim = readT();
	in order to read into an array of type T (again assuming T is primitive)
	use:
	T myArray = new T[length];
	read(myArray);
	 */

	public byte readByte() throws IllegalStateException {
		if (this.mode == MODE.WRITE) throw new IllegalStateException("Cannot read from buffer while in WRITE mode.");
		return this.data[pointer++];
	}

	private void unsafeReadByte(Byte dest) throws IllegalStateException {
		Unsafe unsafe = getUnsafe();
		Field value;
		try {
			value = dest.getClass().getDeclaredField("value");
			unsafe.putByte(dest, unsafe.objectFieldOffset(value), readByte());
		} catch (NoSuchFieldException e) {
			e.printStackTrace();
		}
	}

	public boolean readBoolean() throws IllegalStateException {
		if (this.mode == MODE.WRITE) throw new IllegalStateException("Cannot read from buffer while in WRITE mode.");
		return this.data[pointer++] == 1;
	}

	private void unsafeReadBoolean(Boolean dest) throws IllegalStateException {
		Unsafe unsafe = getUnsafe();
		Field value;
		try {
			value = dest.getClass().getDeclaredField("value");
			unsafe.putBoolean(dest, unsafe.objectFieldOffset(value), readBoolean());
		} catch (NoSuchFieldException e) {
			e.printStackTrace();
		}
	}

	public short readShort() throws IllegalStateException {
		if (this.mode == MODE.WRITE) throw new IllegalStateException("Cannot read from buffer while in WRITE mode.");
		byte b1 = data[pointer++];
		byte b0 = data[pointer++];

		return (short) (((b1 & 0xff) << 8) | (b0 & 0xff));
	}

	private void unsafeReadShort(Short dest) throws IllegalStateException {
		Unsafe unsafe = getUnsafe();
		Field value;
		try {
			value = dest.getClass().getDeclaredField("value");
			unsafe.putShort(dest, unsafe.objectFieldOffset(value), readShort());
		} catch (NoSuchFieldException e) {
			e.printStackTrace();
		}
	}

	public char readChar() throws IllegalStateException {
		if (this.mode == MODE.WRITE) throw new IllegalStateException("Cannot read from buffer while in WRITE mode.");
		byte b1 = data[pointer++];
		byte b0 = data[pointer++];

		return (char) (((b1 & 0xff) << 8) | (b0 & 0xff));
	}

	private void unsafeReadChar(Character dest) throws IllegalStateException {
		Unsafe unsafe = getUnsafe();
		Field value;
		try {
			value = dest.getClass().getDeclaredField("value");
			unsafe.putChar(dest, unsafe.objectFieldOffset(value), readChar());
		} catch (NoSuchFieldException e) {
			e.printStackTrace();
		}
	}

	public int readInt() throws IllegalStateException {
		if (this.mode == MODE.WRITE) throw new IllegalStateException("Cannot read from buffer while in WRITE mode.");
		byte b3 = data[pointer++];
		byte b2 = data[pointer++];
		byte b1 = data[pointer++];
		byte b0 = data[pointer++];

		return (((b3 & 0xff) << 24) | ((b2 & 0xff) << 16) |((b1 & 0xff) << 8) | (b0 & 0xff));
	}

	private void unsafeReadInt(Integer dest) throws IllegalStateException {
		Unsafe unsafe = getUnsafe();
		Field value;
		try {
			value = dest.getClass().getDeclaredField("value");
			unsafe.putInt(dest, unsafe.objectFieldOffset(value), readInt());
		} catch (NoSuchFieldException e) {
			e.printStackTrace();
		}
	}

	public long readLong() throws IllegalStateException {
		if (this.mode == MODE.WRITE) throw new IllegalStateException("Cannot read from buffer while in WRITE mode.");
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

	private void unsafeReadLong(Long dest) throws IllegalStateException {
		Unsafe unsafe = getUnsafe();
		Field value;
		try {
			value = dest.getClass().getDeclaredField("value");
			unsafe.putLong(dest, unsafe.objectFieldOffset(value), readLong());
		} catch (NoSuchFieldException e) {
			e.printStackTrace();
		}
	}

	public float readFloat() throws IllegalStateException {
		return Float.intBitsToFloat(readInt());
	}

	private void unsafeReadFloat(Float dest) throws IllegalStateException {
		Unsafe unsafe = getUnsafe();
		Field value;
		try {
			value = dest.getClass().getDeclaredField("value");
			unsafe.putFloat(dest, unsafe.objectFieldOffset(value), readFloat());
		} catch (NoSuchFieldException e) {
			e.printStackTrace();
		}
	}

	public double readDouble() throws IllegalStateException {
		return Double.longBitsToDouble(readLong());
	}

	private void unsafeReadDouble(Double dest) throws IllegalStateException  {
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

	public void read(byte[] dest) throws BufferOverflowException, IllegalStateException {
		if (this.mode == MODE.WRITE) throw new IllegalStateException("Cannot read from buffer while in WRITE mode.");
		if (this.data.length < this.pointer + dest.length)
			throw new BufferOverflowException();
		for (int i = 0; i < dest.length; i++) {
			dest[i] = this.data[pointer++];
		}
	}

	public void read(boolean[] dest) throws BufferOverflowException, IllegalStateException {
		if (this.mode == MODE.WRITE) throw new IllegalStateException("Cannot read from buffer while in WRITE mode.");
		if(this.data.length < this.pointer + dest.length)
			throw new BufferOverflowException();
		for (int i = 0; i < dest.length; i++) {
			dest[i] = this.data[pointer++] == 1;
		}
	}

	public void read(short[] dest) throws BufferOverflowException, IllegalStateException {
		if (this.data.length < this.pointer + dest.length)
			throw new BufferOverflowException();
		for (int i = 0; i < dest.length; i++) {
			dest[i] = readShort();
		}
	}

	public void read(char[] dest) throws BufferOverflowException, IllegalStateException {
		if (this.data.length < this.pointer + dest.length)
			throw new BufferOverflowException();
		for (int i = 0; i < dest.length; i++) {
			dest[i] = readChar();
		}
	}

	public void read(int[] dest) throws BufferOverflowException, IllegalStateException {
		if (this.data.length < this.pointer + dest.length)
			throw new BufferOverflowException();
		for (int i = 0; i < dest.length; i++) {
			dest[i] = readInt();
		}
	}

	public void read(long[] dest) throws BufferOverflowException, IllegalStateException {
		if (this.data.length < this.pointer + dest.length)
			throw new BufferOverflowException();
		for (int i = 0; i < dest.length; i++) {
			dest[i] = readLong();
		}
	}

	public void read(float[] dest) throws BufferOverflowException, IllegalStateException {
		if (this.data.length < this.pointer + dest.length)
			throw new BufferOverflowException();
		for (int i = 0; i < dest.length; i++) {
			dest[i] = readFloat();
		}
	}

	public void read(double[] dest) throws BufferOverflowException, IllegalStateException {
		if (this.data.length < this.pointer + dest.length)
			throw new BufferOverflowException();
		for (int i = 0; i < dest.length; i++) {
			dest[i] = readDouble();
		}
	}

	//endregion

	//endregion

	//endregion
}
