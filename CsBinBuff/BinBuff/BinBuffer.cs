using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Reflection;


namespace BinBuff
{
    public class BinBuffer
    {
        public enum Type { DYNAMIC, STATIC }
        public enum Mode { READ, WRITE }

        /// <summary>
        /// Writes the given data into the given buffer.
        /// </summary>
        /// <typeparam name="T"> The type of data that can be written.</typeparam>
        /// <param name="data"> The data to be written.</param>
        /// <param name="buffer"> The buffer to write the data to.</param>
        public delegate void Serializer<in T>(T data, BinBuffer buffer);

        /// <summary>
        /// Reads the data from the buffer into the given object 'dest'.
        /// </summary>
        /// <typeparam name="T"> The type of object that can be read into.</typeparam>
        /// <param name="dest"> The object to read into.</param>
        /// <param name="buffer"> The buffer to read from.</param>
        public delegate void Deserializer<in T>(T dest, BinBuffer buffer);


        private byte[] bufferData;
        private uint nextPointer;
        private Type type;
        private Mode mode;

        /// <summary>
        /// Buffer copy constructor, creates new buffer with the same attributes of the given buffer.
        /// </summary>
        /// <param name="other"></param>
        public BinBuffer(BinBuffer other)
        {
            this.nextPointer = other.nextPointer;
            this.type = other.type;
            this.mode = other.mode;
            this.bufferData = new byte[other.bufferData.Length];
            Array.Copy(other.bufferData, 0, this.bufferData, 0, this.nextPointer);
        }

        /// <summary>
        /// Create an empty buffer in write mode.
        /// </summary>
        /// <param name="type"> The type of the buffer. </param>
        /// <param name="size"> The initial size of the buffer. </param>
        public BinBuffer(Type type, uint size = 32)
        {
            this.bufferData = new byte[size];
            this.type = type;
            this.mode = Mode.WRITE;
        }

        /// <summary>
        /// Creates a buffer with the contents of the given stream, the buffer is created in read mode.
        /// </summary>
        /// <param name="stream"> The stream to create a buffer from.</param>
        /// <param name="count"> Number of bytes to read from the Stream.</param>
        public BinBuffer(Stream stream, int count = -1)
        {
            this.mode = Mode.READ;
            this.type = Type.STATIC;
            if (count == -1) count = (int)stream.Length - (int)stream.Position;
            this.bufferData = new byte[count];
            stream.Read(bufferData, 0, count);
            this.nextPointer = (uint) count;
        }

        /// <summary>
        /// Creates buffer containing the provided data, the buffer is created in read mode.
        /// </summary>
        /// <param name="data"> the data the buffer will contain. </param>
        /// <param name="offset"> offset from which to start writing the data into the buffer. (default is 0)</param>
        /// <param name="count"> number of bytes that will be written to the buffer or -1 to write all bytes. (default is -1) </param>
        public BinBuffer(byte[] data, int offset = 0, int count = -1)
        {
            this.mode = Mode.READ;
            this.type = Type.STATIC;
            if (count == -1) count = data.Length;
            this.bufferData = new byte[count];
            Array.Copy(data, offset, this.bufferData, 0, count);
            this.nextPointer = (uint) count;

        }

        /// <summary>
        /// Sets the buffer into read mode, resizing the buffer to fit the used space and making it static.
        /// </summary>
        public void SetRead()
        {
            if (this.mode == Mode.READ) return;
            if (nextPointer != this.bufferData.Length) Array.Resize(ref this.bufferData, (int)nextPointer);
            this.mode = Mode.READ;
            this.type = Type.STATIC;
            this.nextPointer = 0;
        }

        /// <summary>
        /// Sets the buffer into write mode, 
        /// </summary>
        /// <param name="newType"> The desired type of the buffer once in read mode.</param>
        /// <param name="extraSize"> The additional size that will be allocated for the buffer once in read mode.</param>
        public void SetWrite(Type newType, int extraSize = 32)
        {
            if (this.mode == Mode.WRITE) return;
            this.mode = Mode.WRITE;
            this.type = newType;
            this.nextPointer = (uint)this.bufferData.Length;
            Array.Resize(ref this.bufferData, this.bufferData.Length + extraSize);
        }

        /// <summary>
        /// Reads data from the given stream into the buffer.
        /// </summary>
        /// <param name="stream"> The stream to read from.</param>
        /// <param name="count"> The number of bytes that should be read from the stream into the buffer, if count = -1 the stream will be read to its end.</param>
        /// <param name="concat"> Boolean value indicating whether or not the data read from the stream should be concatenated to the buffer or override the data already in the buffer.</param>
        public void ReadStream(Stream stream, int count = -1, bool concat = false)
        {
            if (!concat) this.nextPointer = 0;
            if (count == -1) count = (int)stream.Length - (int)stream.Position;
            stream.Read(this.bufferData, (int)nextPointer, count);
        }

        /// <summary>
        /// Writes data from the buffer into the given stream.
        /// </summary>
        /// <param name="stream"> he stream to write to.</param>
        /// <param name="begin"> The starting position in the buffer from which to begin writing.</param>
        /// <param name="count"> The number of bytes that should be written to the stream, if count = -1 all the data in the buffer after 'begin' will be written.</param>
        public void WriteStream(Stream stream, int begin = 0, int count = -1)
        {
            if (count == -1) count = this.bufferData.Length - begin;
            stream.Write(this.bufferData, begin, count);
        }

        /// <summary>
        /// Get byte array containing the serialized data contained in the buffer.
        /// Note: the returned byte array acts as a pointer to the data that the buffer manages,
        /// and as such it could be dangerous to modify the returned array.
        /// (use CloneSerialized for a safer version)
        /// </summary>
        /// <returns> Byte array that the buffer uses to manage the serialized data. </returns>
        public byte[] GetSerialized() { return bufferData; }

        /// <summary>
        /// Get MemoryStream backed by the buffers serialized data.
        /// Note: the returned memory stream is not writable, it acts as a view of the
        /// buffer's data.
        /// </summary>
        /// <returns> MemoryStream backed by the buffers serialized data. </returns>
        public MemoryStream AsMemoryStream() { return new MemoryStream(this.bufferData, false); }
         
        /// <summary>
        /// Get byte array describing the current state of the buffer.
        /// Note: this operation is relatively costly since it creates a copy of the data in the buffer
        /// in order to return only the relevant data.
        /// </summary>
        /// <returns> Byte array containing all the data written to the buffer as bytes.</returns>
        public byte[] CloneSerialized()
        {
            byte[] res = new byte[this.nextPointer + 1];
            Array.Copy(this.bufferData, res, res.Length);
            return res;
        }

        /// <summary>
        /// Advances the read/write pointer one byte forward without preforming a read or a write.
        /// </summary>
        /// <param name="buffer"></param>
        /// <returns></returns>
        public static BinBuffer operator ++(BinBuffer buffer)
        {
            buffer.nextPointer++;
            return buffer;
        }

        /// <summary>
        /// Takes the read/write pointer one byte backwards without preforming a read or a write.
        /// </summary>
        /// <param name="buffer"></param>
        /// <returns></returns>
        public static BinBuffer operator --(BinBuffer buffer)
        {
            buffer.nextPointer--;
            return buffer;
        }

        /// <summary>
        /// Advances the read/write pointer forward without preforming a read or a write.
        /// </summary>
        /// <param name="buffer"></param>
        /// <param name="amount"> The amount of bytes to jump forward.</param>
        /// <returns></returns>
        public static BinBuffer operator +(BinBuffer buffer, uint amount)
        {
            if (buffer.bufferData.Length < buffer.nextPointer + amount) throw new IndexOutOfRangeException("trying to jump outside of buffer bounds.");
            buffer.nextPointer += amount;
            return buffer;
        }

        /// <summary>
        /// Takes the read/write pointer one byte backwards without preforming a read or a write.
        /// </summary>
        /// <param name="buffer"></param>
        /// <param name="amount"> The amount of bytes to jump back.</param>
        /// <returns></returns>
        public static BinBuffer operator -(BinBuffer buffer, uint amount)
        {
            if ((int)buffer.nextPointer - (int)amount < 0) throw new IndexOutOfRangeException("trying to jump outside of buffer bounds.");
            buffer.nextPointer -= amount;
            return buffer;
        }

         #region Write


        /// <summary>
        /// Write the given objects into the buffer in the sequence they are passed to the function from left to right.
        /// </summary>
        /// <param name="sData">The array of the data objects to be written.</param>
        public void Write(params object[] sData)
        {
            if (this.mode != Mode.WRITE) throw new InvalidOperationException("cannot write to buffer when not in WRITE mode.");
            foreach (var t in sData)
                WriteSingle(t);
            
        }

        private void WriteSingle<T>(T sData)
        {
            if (sData is ISerializable serializable) WriteSerializable(serializable);
            else if (sData is IEnumerable enumerable) WriteEnumerable(enumerable);
            else if (sData.GetType().GetTypeInfo().IsGenericType)
            {
                System.Type valueType = sData.GetType();
                if (valueType.GetGenericTypeDefinition() != typeof(KeyValuePair<,>)) return;

                object key = valueType.GetTypeInfo().GetProperties()[0].GetValue(sData, null);
                object value = valueType.GetTypeInfo().GetProperties()[1].GetValue(sData, null);
                Write(key, value);
            }
            else if (sData is int) { WritePrimitive((int)(object)sData); }
            else if (sData is uint) WritePrimitive((uint)(object)sData);
            else if (sData is bool) WritePrimitive((bool)(object)sData);
            else if (sData is sbyte) WritePrimitive((sbyte)(object)sData);
            else if (sData is byte) WritePrimitive((byte)(object)sData);
            else if (sData is char) WritePrimitive((char)(object)sData);
            else if (sData is decimal) WritePrimitive((decimal)(object)sData);
            else if (sData is double) WritePrimitive((double)(object)sData);
            else if (sData is float) WritePrimitive((float)(object)sData);
            else if (sData is long) WritePrimitive((long)(object)sData);
            else if (sData is ulong) WritePrimitive((ulong)(object)sData);
            else if (sData is short) WritePrimitive((short)(object)sData);
            else if (sData is ushort) WritePrimitive((ushort)(object)sData);
            else throw new ArgumentException(nameof(sData) + " is of type - " + typeof(T).Name + ", this type cannot be written without a ");
        }


        /// <summary>
        /// Writes the given objects into the buffer using the provided Serializer.
        /// </summary>
        /// <typeparam name="T"> The type of the objects that should be written.</typeparam>
        /// <param name="serializer"> The Serializer that will be used to write the objects.</param>
        /// <param name="sData"> The list of the objects to be written.</param>
        public void Write<T>(Serializer<T> serializer, params T[] sData)
        {
            if (sData == null) throw new ArgumentNullException(nameof(sData));
            if (serializer == null) throw new ArgumentNullException(nameof(serializer));
            if (this.mode != Mode.WRITE) throw new InvalidOperationException("cannot write to buffer when not in WRITE mode.");
            foreach (T data in sData)
            {
                serializer(data, this);
            }
        }

        private void AllocBuffer(int size)
        {
            if (bufferData.Length - this.nextPointer < size)
            {
                if (this.type == Type.STATIC) throw new InvalidOperationException("reached the end of the allocated space of the STATIC buffer.");
                Array.Resize(ref bufferData, this.bufferData.Length * 2);
            }
        }

        private unsafe void WritePrimitive(void* data, int size)
        {
            if (data == null) throw new NullReferenceException();
            if (this.mode != Mode.WRITE) throw new InvalidOperationException("cannot write to buffer when not in WRITE mode.");
            AllocBuffer(size);

            if (BitConverter.IsLittleEndian)
            {
                // reverse copy.
                byte* sData = (byte*)data + size - 1;
                while (size > 0)
                {
                    this.bufferData[this.nextPointer++] = *sData--;
                    size--;
                }

            }
            else
            {
                // normal copy.
                byte* sData = (byte*)data;
                for (int i = 0; i < size; i++)
                    this.bufferData[this.nextPointer++] = *sData++;
            }
        }

        private void WriteSerializable(ISerializable sData)
        {
            if (sData == null) throw new ArgumentNullException(nameof(sData));
            sData.Serialize(this);
        }

        private void WriteEnumerable(IEnumerable sData)
        {
            if (sData == null) throw new ArgumentNullException(nameof(sData));
            IEnumerator itr = sData.GetEnumerator();
            while (itr.MoveNext())
                this.WriteSingle(itr.Current);
            
        }


        #region Write Primitive

        private unsafe void WritePrimitive(int data)
        {
            WritePrimitive(&data, sizeof(int));
        }

        private unsafe void WritePrimitive(uint data)
        {
            WritePrimitive(&data, sizeof(uint));
        }

        private void WritePrimitive(bool data)
        {
            AllocBuffer(1);
            this.bufferData[nextPointer++] = data ? (byte)1 : (byte)0;
        }

        private unsafe void WritePrimitive(sbyte data)
        {
            AllocBuffer(1);
            this.bufferData[nextPointer++] = *((byte*)&data);
        }

        private void WritePrimitive(byte data)
        {
            AllocBuffer(1);
            this.bufferData[nextPointer++] = data;
        }

        private unsafe void WritePrimitive(char data)
        {
            WritePrimitive(&data, sizeof(char));
        }

        private unsafe void WritePrimitive(decimal data)
        {
            WritePrimitive(&data, sizeof(decimal));
        }

        private unsafe void WritePrimitive(double data)
        {
            WritePrimitive(&data, sizeof(double));
        }

        private unsafe void WritePrimitive(float data)
        {
            WritePrimitive(&data, sizeof(float));
        }

        private unsafe void WritePrimitive(long data)
        {
            WritePrimitive(&data, sizeof(long));
        }

        private unsafe void WritePrimitive(ulong data)
        {
            WritePrimitive(&data, sizeof(ulong));
        }

        private unsafe void WritePrimitive(short data)
        {
            WritePrimitive(&data, sizeof(short));
        }

        private unsafe void WritePrimitive(ushort data)
        {
            WritePrimitive(&data, sizeof(ushort));
        }

        #endregion

        #endregion


        #region Read

        /// <summary>
        /// Read from the buffer into the given object 'dest'.
        /// note that this will delete any data already stored in the object.
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="dest"></param>
        public void Read<T>(out T dest) where T : new()
        {
            if (this.mode != Mode.READ) throw new InvalidOperationException("cannot read from buffer when not in READ mode.");
            dest = new T();
            ReadRef(ref dest);
        }

        private void ReadRef<T>(ref T dest)
        {
            if (dest is ISerializable serializable) ReadSerializable(serializable);
            else if (dest.GetType().GetTypeInfo().IsGenericType)
            {
                System.Type valueType = dest.GetType();
                if (valueType.GetGenericTypeDefinition() == typeof(KeyValuePair<,>))
                {
                    object key = Activator.CreateInstance(valueType.GetTypeInfo().GetGenericArguments()[0]);
                    object value = Activator.CreateInstance(valueType.GetTypeInfo().GetGenericArguments()[1]);
                    ReadRef(ref key);
                    ReadRef(ref value);
                    dest = (T)Activator.CreateInstance(typeof(T), key, value);
                }
            }
            else if (dest is int)
            {
                ReadPrimitive(out int i);
                dest = (T)(object)i;
            }
            else if (dest is uint)
            {
                ReadPrimitive(out uint ui);
                dest = (T)(object)ui;
            }
            else if (dest is bool)
            {
                ReadPrimitive(out bool b);
                dest = (T)(object)b;
            }
            else if (dest is sbyte)
            {
                ReadPrimitive(out sbyte sb);
                dest = (T)(object)sb;
            }
            else if (dest is byte)
            {
                ReadPrimitive(out byte b);
                dest = (T)(object)b;
            }
            else if (dest is char)
            {
                ReadPrimitive(out char c);
                dest = (T)(object)c;
            }
            else if (dest is decimal)
            {
                ReadPrimitive(out decimal d);
                dest = (T)(object)d;
            }
            else if (dest is double)
            {
                ReadPrimitive(out double d);
                dest = (T)(object)d;
            }
            else if (dest is float)
            {
                ReadPrimitive(out float f);
                dest = (T)(object)f;
            }
            else if (dest is long)
            {
                ReadPrimitive(out long l);
                dest = (T)(object)l;
            }
            else if (dest is ulong)
            {
                ReadPrimitive(out ulong ul);
                dest = (T)(object)ul;
            }
            else if (dest is short)
            {
                ReadPrimitive(out short s);
                dest = (T)(object)s;
            }
            else if (dest is ushort)
            {
                ReadPrimitive(out ushort us);
                dest = (T)(object)us;
            }
            else throw new ArgumentException(nameof(dest) + " is of type - " + typeof(T).Name + ", this type cannot be written without a ");
        }

        /// <summary>
        /// Reads into the given objects from the buffer using the provided Deserializer.
        /// </summary>
        /// <typeparam name="T"> The type of the objects that should be read.</typeparam>
        /// <param name="deserializer"> The Deserializer that will be used to read the objects.</param>
        /// <param name="dest"> The list of the objects to be written.</param>
        public void Read<T>(Deserializer<T> deserializer, params T[] dest)
        {
            if (dest == null) throw new ArgumentNullException(nameof(dest));
            if (deserializer == null) throw new ArgumentNullException(nameof(deserializer));
            if (this.mode != Mode.READ) throw new InvalidOperationException("cannot read from buffer when not in READ mode.");
            foreach (T obj in dest)
            {
                deserializer(obj, this);
            }
        }

        private unsafe void ReadPrimitive(void* dest, int size)
        {
            if (dest == null) throw new NullReferenceException();
            if (this.mode != Mode.READ) throw new InvalidOperationException("cannot read from buffer when not in READ mode.");
            if (this.bufferData.Length - this.nextPointer < size) throw new OverflowException("reached the end of the buffer.");

            if (BitConverter.IsLittleEndian)
            {
                // reverse copy.
                byte* sData = (byte*)dest + size - 1;
                while (size > 0)
                {
                    *sData-- = this.bufferData[this.nextPointer++];
                    size--;
                }
            }
            else
            {
                // normal copy.
                byte* sData = (byte*)dest;
                for (int i = 0; i < size; i++)
                    sData[i] = this.bufferData[this.nextPointer++];

            }
        }

        /// <summary>
        /// Reads data into a collection.
        /// </summary>
        /// <typeparam name="T"> The type of data stored in the collection.</typeparam>
        /// <param name="dest"> The collection to be read into.</param>
        /// <param name="size"> The number of objects to be read into the collection.</param>
        public void Read<T>(ICollection<T> dest, int size) where T : new()
        {
            if (this.mode != Mode.READ) throw new InvalidOperationException("cannot read from buffer when not in READ mode.");
            if (dest is IList list && list.IsFixedSize)
            {
                ReadFixed((IList<T>)list, size);
                return;
            }

            for (int i = 0; i < size; i++)
            {
                Read(out T tmp);
                dest.Add(tmp);
            }
        }

        private void ReadFixed<T>(IList<T> dest, int size) where T : new()
        {
            for (int i = 0; i < size; i++)
            {
                Read(out T tmp);
                dest[i] = tmp;
            }
        }

        /// <summary>
        /// Reads data ino a queue.
        /// </summary>
        /// <typeparam name="T"> The type of data stored in the queue.</typeparam>
        /// <param name="dest"> The queue to be read into.</param>
        /// <param name="size"> The number of objects to be read into the queue.</param>
        public void Read<T>(Queue<T> dest, int size) where T : new()
        {
            if (this.mode != Mode.READ) throw new InvalidOperationException("cannot read from buffer when not in READ mode.");
            for (int i = 0; i < size; i++)
            {
                Read(out T tmp);
                dest.Enqueue(tmp);
            }
        }

        /// <summary>
        /// Reads data ino a stack.
        /// </summary>
        /// <typeparam name="T"> The type of data stored in the stack.</typeparam>
        /// <param name="dest"> The stack to be read into.</param>
        /// <param name="size"> The number of objects to be read into the stack.</param>
        public void Read<T>(Stack<T> dest, int size) where T : new()
        {
            if (this.mode != Mode.READ) throw new InvalidOperationException("cannot read from buffer when not in READ mode.");
            Stack<T> tmpStack = new Stack<T>();
            for (int i = 0; i < size; i++)
            {
                Read(out T tmp);
                tmpStack.Push(tmp);
            }

            for (int i = 0; i < size; i++)
            {
                dest.Push(tmpStack.Pop());
            }
        }

        private void ReadSerializable(ISerializable dest)
        {
            dest.Deserialize(this);
        }


        #region Read Primitive

        private unsafe void ReadPrimitive(out int dest)
        {
            fixed (void* p = &dest)
                ReadPrimitive(p, sizeof(int));
        }

        private unsafe void ReadPrimitive(out uint dest)
        {
            fixed (void* p = &dest)
                ReadPrimitive(p, sizeof(uint));
        }

        private void ReadPrimitive(out bool dest)
        {
            if (this.bufferData.Length - this.nextPointer < 1) throw new OverflowException("reached the end of the buffer.");
            dest = this.bufferData[nextPointer++] == 1;
        }

        private unsafe void ReadPrimitive(out sbyte dest)
        {
            if (this.bufferData.Length - this.nextPointer < 1) throw new OverflowException("reached the end of the buffer.");
            byte b = this.bufferData[nextPointer++];
            dest = *((sbyte*)&b);
        }

        private void ReadPrimitive(out byte dest)
        {
            if (this.bufferData.Length - this.nextPointer < 1) throw new OverflowException("reached the end of the buffer.");
            dest = this.bufferData[nextPointer++];
        }

        private unsafe void ReadPrimitive(out char dest)
        {
            fixed (void* p = &dest)
                ReadPrimitive(p, sizeof(char));
        }

        private unsafe void ReadPrimitive(out decimal dest)
        {
            fixed (void* p = &dest)
                ReadPrimitive(p, sizeof(decimal));
        }

        private unsafe void ReadPrimitive(out double dest)
        {
            fixed (void* p = &dest)
                ReadPrimitive(p, sizeof(double));
        }

        private unsafe void ReadPrimitive(out float dest)
        {
            fixed (void* p = &dest)
                ReadPrimitive(p, sizeof(float));
        }

        private unsafe void ReadPrimitive(out long dest)
        {
            fixed (void* p = &dest)
                ReadPrimitive(p, sizeof(long));
        }

        private unsafe void ReadPrimitive(out ulong dest)
        {
            fixed (void* p = &dest)
                ReadPrimitive(p, sizeof(ulong));
        }

        private unsafe void ReadPrimitive(out short dest)
        {
            fixed (void* p = &dest)
                ReadPrimitive(p, sizeof(short));
        }

        private unsafe void ReadPrimitive(out ushort dest)
        {
            fixed (void* p = &dest)
                ReadPrimitive(p, sizeof(ushort));
        }


        #endregion

        #endregion

    }
}
