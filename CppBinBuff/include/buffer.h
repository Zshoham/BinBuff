#ifndef BUFFER_H
#define BUFFER_H

#include "serializable.h"
#include "binbuff_traits.h"

#include <iostream>
#include <cstring>
#include <array>
#include <stack>
#include <queue>
#include <fstream>
#include <forward_list>
#include <functional>
#include <stdexcept>

#define DEFAULT_BUFFER_SIZE 32

namespace binbuff
{

class Buffer
{
public:
	enum TYPE { DYNAMIC, STATIC };
	enum MODE { READ, WRITE };

private:
	void *buffer_data;
	std::size_t buffer_size;
	std::size_t next_pointer;
	TYPE buffer_type;
	MODE buffer_mode;

	const int endianness_test = 1;
	bool is_big_endian = (*((char *)&endianness_test)) != 1;

	void alloc_buffer(const std::size_t &size);

	// void internal_write(const void *data, const size_t &length, const bool &is_atomic = true, const size_t &width = 1);
	// void internal_read(void *dest, const size_t &length, const bool &is_atomic = true, const size_t & width = 1);

	static void *rev_memcpy(void *dest, const void *src, size_t length);

	/*
	 * The following methods are used internally to decide how to data should be serialized.
	 * All this is done in order to have the simplest possible interface of one write and read function
	 * with many overloads.
	 * The function's form a type deduction tree using the std::bool_constant type passed to all of them.
	 * The tree looks something like this:
	 */

	 /*
		 The leafs in the tree represent states from which there is no need for further deduction and
		 it is possible to serialize the type, if the type fails to reach one of the leafs then it cannot be
		 serialized.


												Write Tree

											 +----------------+
									+--------+ Generic Type T +-----+
									|        +----------------+     |
									|                               |
									v                               v
							+-------+------------+         +--------+-------+
					 +------+ T is not primitive +---+     | T is primitive |
					 |      +--------------------+   |     +----------------+
					 |                               |
					 |                               |
					 v                               v
		 +-----------+-----------+        +----------+--------+
		 | T is not serializable |        | T is serializable |
		 +-----------+-----------+        +-------------------+
					 |
					 |
					 v
		 +-----------+------------+
		 | T is a valid container |
		 +------------------------+

														  Read Tree

													   +----------------+
												+------+ Generic Type T +-------------------+
												|      +----------------+                   |
												|                                           |
												|                                           |
									  +---------+----------+                         +------+---------+
					+-----------------+ T is not primitive +---------------+         | T is primitive |
					|                 +---------+----------+               |         +----------------+
					|                           |                          |
					|                           |                          |
					v                           v                          v
		 +----------+------------+ +------------+--------------+ +---------+------------+
		 | T readable sequential | | T is readable associative | | T is not a container |
		 +-----------------------+ +---------------------------+ +----------------------+


		 Note that in the read tree if a type is not primitive and it wasn't passed with length variable, it must be deserializable
		 because in order to read a container, the length must be provided.

	  */


	template <class T>
	void write(const T &data, std::true_type) // T is primitive
	{
		static_assert(std::is_arithmetic<T>::value, "trying to write non primitive non serializable types.");
		write_generic(static_cast<const void *>(&data), sizeof(data));
	}

	template<class T>
	void write(const T &data, std::false_type) // T is not primitive
	{
		this->write_n_primitive(data, is_serializable<T>());
	}

	template<class T>
	void write_n_primitive(const T &data, std::true_type) // T is serializable
	{
		static_assert(is_serializable<T>::value, "trying to write unserializable type.");
		data.serialize(*this);
	}

	template <class T>
	void write_n_primitive(const T &data, std::false_type) // T is not serializable, meaning it must be a container.
	{
		static_assert(is_writable_container<T>::value, "trying to write an un-serializable object or an un-writable container into buffer.");
		this->write(data.begin(), data.end());
	}

	// the array methods follow a similar logic, though they are a bit different

	template <class T>
	void write(const T *data, const std::size_t &length, std::true_type) // T is primitive, hence 'data' is a primitive array.
	{
		static_assert(std::is_arithmetic<T>::value, "trying to write non primitive non serializable types.");
		if (!data) throw std::runtime_error("trying to write nullptr");
		if (sizeof(T) == 1 || is_big_endian)
			write_generic(static_cast<const void *>(data), sizeof(*data) * length, false);
		else
		{
			constexpr size_t size = sizeof(T);
			for (int i = 0; i < length; ++i)
				write_generic(static_cast<const void *>(&data[i]), size);
		}
	}

	template<class T>
	void write(const T *data, const std::size_t &length, std::false_type); // T is not primitive, hence 'data' must be an array of serializable types.


	template<class T>
	void read(T &dest, std::true_type) // T is primitive
	{
		static_assert(std::is_arithmetic<T>::value, "trying to read into primitive non serializable types.");
		read_generic(static_cast<void *>(&dest), sizeof(dest));
	}

	template<class T>
	void read(T &dest, std::false_type) // T is not primitive, hence it must be deserializable.
	{
		static_assert(std::is_base_of<Serializable, T>::value, "trying to read into primitive non serializable types.");
		static_assert(!is_readable_container<T>::value, "trying to read into container without specifying number of elements.");
		dest.deserialize(*this);
	}

	template<class T>
	void read_container(T &dest, const size_t &length, std::true_type) // T is readable associative
	{
		static_assert(is_readable_container<T>::value, "trying to read into unreadable container.");
		typedef typename remove_inner_const<typename T::value_type>::type value_type;
		for (size_t i = 0; i < length; ++i)
		{
			value_type tmp = default_construct<value_type>::get_instance();
			this->read(tmp);
			dest.insert(tmp);
		}
	}

	template<class T>
	void read_container(T &dest, const size_t &length, std::false_type) // T is readable sequential
	{
		typedef typename remove_inner_const<typename T::value_type>::type value_type;
		for (size_t i = 0; i < length; ++i)
		{
			value_type tmp = default_construct<value_type>::get_instance();
			this->read(tmp);
			dest.insert(dest.end(), tmp);
		}
	}

	// again the array methods have very similar logic 

	template<class T>
	void read(T *dest, const std::size_t &length, std::true_type) // T is primitive, hence 'data' is a primitive array.
	{
		static_assert(std::is_arithmetic<T>::value, "trying to read into primitive non serializable types.");
		if (!dest) throw std::runtime_error("trying to read into nullptr");
		if (sizeof(T) == 1 || is_big_endian)
			read_generic(static_cast<void *>(dest), sizeof(*dest) * length, false);
		else
		{
			constexpr size_t size = sizeof(T);
			for (int i = 0; i < length; ++i)
				read_generic(static_cast<void *>(&dest[i]), size);
		}

	}

	template<class T>
	void read(T *dest, const std::size_t &length, std::false_type); // T is not primitive, hence 'data' must be an array of serializable types.

public:

	/**
	 * \brief creates a buffer in read mode with initial size 'size'
	 * \param type the type of the buffer that will be created {DYNAMIC, STATIC}
	 * \param size the initial size of the buffer that will be created.
	 */
	explicit Buffer(const TYPE &type, const std::size_t &size = DEFAULT_BUFFER_SIZE);

	Buffer(const Buffer &other);
	Buffer(Buffer &&other) noexcept;
	~Buffer();

	Buffer &operator=(const Buffer &other);
	Buffer &operator=(Buffer &&other) noexcept;

	//sets the buffer to read mode.
	void set_mode_read();

	//sets the buffer to write mode, and sets the type of the buffer.
	void set_mode_write(TYPE type, size_t extra_size = DEFAULT_BUFFER_SIZE);

	bool operator==(const Buffer &other) const;
	bool operator!=(const Buffer &other) const;

	//jump 'jmp' bytes forward in the buffer.
	Buffer &operator+=(const std::size_t &jmp);

	//jump one byte forward in the buffer.
	Buffer &operator++();

	//jump 'jmp' bytes backwards in the buffer.
	Buffer &operator-=(const std::size_t &jmp);

	//jump one byte backwards in the buffer.
	Buffer &operator--();


	/**
	 * \brief	Moves the read/write pointer to the start of the buffer (index 0).
	 *			Writing after a rewind will overwrite the data in the buffer,
	 *			this makes the rewind equivalent to clear operation.
	 */
	Buffer &rewind() { this->next_pointer = 0; return *this; }

	/**
	 * \brief	Returns a pointer to the underlying buffer holding the data, note that this is a pointer
	 *			to the same memory managed by the buffer, it could be dangerous to use, in addition
	 *			note that the allocated size is most likely larger then the number of bytes written to the buffer
	 *			as the buffer preallocate's memory.
	 */
	void *get_serialized() const { return this->buffer_data; }

	/**
	 * \brief	Returns a pointer to memory holding the serialized data, similar to the above get method
	 *			only this method clones the memory held by the buffer, and cuts it to fit the serialized data exactly
	 *			making the pointer safer to use, but costing the additional memory and time for the cloning.
	 */
	void *clone_serialized() const;

	/**
	 * \brief	Returns vector containing the serialized data represented as unsigned chars (uint8/byte)
	 *			this method also copies all the data from the buffer meaning only the necessary data will
	 *			be put in the vector and it will be safe to change the vector, though again at the cost of
	 *			the memory and time of cloning.
	 */
	std::vector<unsigned char> as_vec() const;

	//writes the buffer into the file-stream.
	friend std::ostream &operator<<(std::ostream &stream, const Buffer &buffer);

	//reads the buffer_data in the file-stream into the buffer.
	friend std::istream &operator>>(std::istream &stream, Buffer &buffer);


#pragma region write


	/**
	 * \brief	writes 'size' bytes starting from where 'data' points into the buffer.
	 *			note that this method assumes the pointer is for a single object, if the
	 *			contents of the pointer will later be read independently the behavior is
	 *			undefined. These is caused by the fact that the buffer needs to decide the
	 *			endianess of the data, and if it is different from the system's if will write
	 *			the data in reverse order, meaning that if the data is read in pieces those pieces
	 *			might be written in reverse order depending on the system that serialized them.
	 *
	 * \param	data pointer to the data to be written into the buffer.
	 * \param	size the size in bytes of the data to be written.
	 * \param	check_endian if set to false the method will not try to change the endianess of the
	 *			data to fit with the rest of the library, this might lead to errors when transporting
	 *			data between two systems that use different endian representation.
	 */
	void write_generic(const void *data, const std::size_t &size, const bool &check_endian = true);


	/**
	 * \brief writes 'data' into the buffer using the gives serializer.
	 * \tparam T a serializable data type or a container of a serializable data type.
	 * \param data the data to be written.
	 * \param serializer a function to preform the serialization.
	 */
	template <typename T>
	void write(const T &data, std::function<void(Buffer &, const T &)> serializer) { serializer(*this, data); }


	/**
	 * \brief writes 'data' into the buffer.
	 * \tparam T a serializable data type or a container of a serializable data type.
	 * \param data the data to be written.
	 */
	template <class T>
	void write(const T &data) { this->write(data, std::is_arithmetic<T>()); }


	/**
	 * \brief writes data pointed to by the given pointer 'data'.
	 * \tparam T a serializable data type or a container of a serializable data type.
	 * \param data pointer to the data to be written.
	 */
	template<class T>
	void write(T *data)
	{
		if (!data) throw std::runtime_error("trying to write nullptr");
		this->write(*data);
	}


	/**
	 * \brief writes data contained in the shared pointer 'data'.
	 * \tparam T a serializable data type or a container of a serializable data type.
	 * \param data
	 */
	template<class T>
	void write(const std::shared_ptr<T> &data) { this->write(*data); }

	/**
	 * \brief writes a pair 'data' in sequence such that the 'data.first' is written first and 'data.second' is written second.
	 * \tparam F a serializable data type or a container of a serializable data type.
	 * \tparam S a serializable data type or a container of a serializable data type.
	 * \param data the pair to be written.
	 */
	template<class F, class S>
	void write(const std::pair<F, S> &data)
	{
		this->write(data.first);
		this->write(data.second);
	}


	/**
	 * \brief writes an array 'data' into the buffer.
	 * \tparam T a serializable data type or a container of a serializable data type.
	 * \param data pointer to the start of the array.
	 * \param length the length of the array.
	 */
	template<class T>
	void write(const T *data, const std::size_t &length)
	{
		if (!data) throw std::runtime_error("trying to write nullptr");
		this->write(data, length, std::is_arithmetic<T>());
	}

	/**
	 * \brief writes array of pointers into the buffer.
	 * \tparam T a serializable data type or a container of a serializable data type.
	 * \param data pointer to the start of the array of pointers.
	 * \param length the length of the array.
	 */
	template<class T>
	void write(T **data, const std::size_t &length);

	/**
	 * \brief writes 'data' into the buffer.
	 * \tparam T a serializable data type or a container of a serializable data type.
	 * \param data the data to be written.
	 */
	template<class T>
	Buffer &operator<<(const T &data) { this->write(data); return *this; }

#pragma endregion

#pragma region container write


	/**
	 * \brief writes a container of serializable data using an iterator 'begin' and 'end' to denote where the writing should start and where it should end respectively.
	 * the container will be written in the order of iteration defined by the container.
	 * \tparam Itr iterator that points to a serializable data type.
	 * \param begin iterator pointing to the beginning of the region of the container that should be written.
	 * \param end iterator pointing to the end of the region of the container that should be written.
	 */
	template<class Itr>
	void write(Itr begin, Itr end);


	/**
	 * \brief writes the entire stack into the buffer.
	 * \tparam T a serializable data type or a container of a serializable data type.
	 * \param data the stack containing the data.
	 */
	template<class T>
	void write(std::stack<T> &data);

	/**
	 * \brief writes the entire queue into the buffer.
	 * \tparam T a serializable data type or a container of a serializable data type.
	 * \param data the queue containing the data.
	 */
	template<class T>
	void write(std::queue<T> &data);

	/**
	 * \brief writes the entire stack into the buffer.
	 * \tparam T a serializable data type or a container of a serializable data type.
	 * \param data the stack containing the data.
	 */
	template<class T>
	Buffer &operator<<(std::stack<T> &data) { this->write(data); return *this; }

	/**
	 * \brief writes the entire queue into the buffer.
	 * \tparam T a serializable data type or a container of a serializable data type.
	 * \param data the queue containing the data.
	 */
	template<class T>
	Buffer &operator<<(std::queue<T> &data) { this->write(data); return *this; }

#pragma endregion


#pragma region read

	/**
	 * \brief reads 'size' bytes from the buffer into the memory 'dest' points to.
	 * 
	 * \param dest			pointer to the beginning of the memory to be read into.
	 * \param size			the size in bytes of the data to be read.
	 * \param check_endian	check_endian if set to false the method will not try to change the endianess of the
	 *						data to fit with the rest of the library, this might lead to errors when transporting
	 *						data between two systems that use different endian representation.
	 */
	void read_generic(void *dest, const std::size_t &size, const bool &check_endian = true);

	/**
	 * \brief reads data from the buffer into a container of deserializable data 'dest'
	 * using the provided deserializer.
	 * \tparam T the type to be deserialized.
	 * \param dest reference to where the data should be read into.
	 * \param deserializer a function to preform the deserialization.
	 */
	template <class T>
	void read(T &dest, std::function<void(Buffer &, T &)> deserializer) {
		deserializer(*this, dest);
	}

	/**
	 * \brief reads data from the buffer into 'dest'.
	 * \tparam T a deserializable data type.
	 * \param dest reference to where the data should be read into.
	 */
	template<class T>
	void read(T &dest) { this->read(dest, std::is_arithmetic<T>()); }

	/**
	 * \brief reads data from the buffer into 'dest'.
	 * \tparam T a deserializable data type.
	 * \param dest pointer to where the data should be read into.
	 */
	template<class T>
	void read(T *dest)
	{
		if (!dest) throw std::runtime_error("trying to read into nullptr");
		this->read(*dest);
	}

	/**
	 * \brief reads data from the buffer into 'dest'.
	 * \tparam T a deserializable data type.
	 * \param dest shared pointer to where the data should be read into.
	 */
	template<class T>
	void read(std::shared_ptr<T> &dest) { this->read(*dest); }


	/**
	 * \brief reads a pair from the buffer in sequence such that the 'dest.first' is read first and 'dest.second' is read second.
	 * \tparam F a deserializable data type.
	 * \tparam S a deserializable data type.
	 * \param dest the pair that should be read into.
	 */
	template<class F, class S>
	void read(std::pair<F, S> &dest)
	{
		this->read(dest.first);
		this->read(dest.second);
	}


	/**
	 * \brief reads an array from the buffer into the given array 'dest'.
	 * \tparam T a deserializable data type.
	 * \param dest pointer to the beginning of the array the data should be read into.
	 * \param length the length of the array that should be read.
	 */
	template<class T>
	void read(T *dest, std::size_t length)
	{
		static_assert(is_serializable<T>::value, "trying to read into primirive non serializable types.");
		this->read(dest, length, std::is_arithmetic<T>());
	}


	/**
	 * \brief reads an array from the buffer into the given array of pointers 'dest'.
	 * \tparam T a deserializable data type.
	 * \param dest pointer to the beginning of the array of pointers.
	 * \param length the length of the array that should be read.
	 */
	template<class T>
	void read(T **dest, std::size_t length);


	/**
	 * \brief reads data from the buffer into 'dest'.
	 * \tparam T a deserializable data type.
	 * \param dest reference to where the data should be read into.
	 */
	template<class T>
	Buffer &operator>>(T &dest) { read(dest); return *this; }


#pragma endregion


#pragma region container read


	/**
	 * \brief reads data from the buffer into a container of deserializable data 'dest'.
	 * \tparam T a container of deserializable data types.
	 * \param dest reference to the container to read into.
	 * \param length number of elements of the containers 'value_type' that should be read into it.
	 */
	template<class T>
	void read(T &dest, size_t length)
	{
		static_assert(is_readable_container<T>::value, "trying to read into unreadable container.");
		this->read_container(dest, length, is_readable_associative<T>());
	}


	/**
	 * \brief reads an array from the buffer into the given array 'dest'.
	 * \tparam T a deserializable data type.
	 * \tparam length length of the array.
	 * \param dest the array to be read into.
	 */
	template<class T, size_t length>
	void read(std::array<T, length> &dest);


	/**
	 * \brief reads an array from the buffer into the given forward list 'dest'.
	 * \tparam T a deserializable data type.
	 * \param dest the forward list to be read into.
	 * \param length the number of elements to be read into the forward list.
	 */
	template<class T>
	void read(std::forward_list<T> &dest, size_t length);


	/**
	 * \brief reads an array from the buffer into the given stack 'dest'.
	 * \tparam T a deserializable data type.
	 * \param dest the stack to be read into.
	 * \param length the number of elements to be read into the stack.
	 */
	template<class T>
	void read(std::stack<T> &dest, size_t length);

	/**
	 * \brief reads an array from the buffer into the given queue 'dest'.
	 * \tparam T a deserializable data type.
	 * \param dest the queue to be read into.
	 * \param length the number of elements to be read into the queue.
	 */
	template<class T>
	void read(std::queue<T> &dest, size_t length);

#pragma endregion

};


#pragma region write implementation

template <class T>
void Buffer::write(const T *data, const std::size_t &length, std::false_type)
{
	static_assert(std::is_base_of<Serializable, T>::value, "trying to write non primitive non serializable types.");
	if (!data) throw std::runtime_error("trying to write nullptr");
	for (size_t i = 0; i < length; ++i)
	{
		this->write(data[i]);
	}
}

template <class T>
void Buffer::write(T **data, const std::size_t &length)
{
	static_assert(is_serializable<T>::value, "trying to write non primitive non serializable type into buffer.");
	if (!data) throw std::runtime_error("trying to write nullptr");
	for (std::size_t i = 0; i < length; i++)
	{
		this->write(*data[i]);
	}
}

#pragma endregion 


#pragma region container write implementation

template <class Itr>
void Buffer::write(Itr begin, Itr end)
{
	static_assert(is_valid_iterator<Itr>::value || is_valid_const_iterator<Itr>::value, "received invalid iterator");
	static_assert(is_serializable<decltype(*begin)>::value, "trying to write non primitive non serializable type into buffer.");
	for (Itr itr = begin; itr != end; ++itr)
		this->write(*itr);
}

template <class T>
void Buffer::write(std::stack<T> &data)
{
	static_assert(is_serializable<T>::value, "trying to write non primitive non serializable type into buffer.");
	std::stack<T> tmp;
	while (!data.empty())
	{
		tmp.push(data.top());
		data.pop();
	}
	while (!tmp.empty())
	{
		this->write(tmp.top());
		data.push(tmp.top());
		tmp.pop();
	}
}

template <class T>
void Buffer::write(std::queue<T> &data)
{
	static_assert(is_serializable<T>::value, "trying to write non primitive non serializable type into buffer.");
	while (!data.empty())
	{
		this->write(data.front());
		data.pop();
	}
}

#pragma endregion



#pragma region read implementation


template <class T>
void Buffer::read(T *dest, const std::size_t &length, std::false_type)
{
	static_assert(is_deserializable<T>::value, "trying to read into non deserializable type.");
	if (!dest) throw std::runtime_error("trying to read buffer_data into null");
	for (std::size_t i = 0; i < length; ++i)
	{
		T tmp;
		tmp.deserialize(*this);
		dest[i] = tmp;
	}
}

template <class T>
void Buffer::read(T **dest, const std::size_t length)
{
	static_assert(is_deserializable<T>::value, "trying to read into non deserializable type.");
	if (!dest) throw std::runtime_error("trying to read buffer_data into null");
	for (std::size_t i = 0; i < length; ++i)
	{
		dest[i] = new T;
		this->read(dest[i]);
	}
}

#pragma endregion 


#pragma region container read implementation

template <class T, size_t length>
void Buffer::read(std::array<T, length> &dest)
{
	for (size_t i = 0; i < length; ++i)
	{
		this->read(dest[i]);
	}
}

template <class T>
void Buffer::read(std::forward_list<T> &dest, const size_t length)
{
	static_assert(is_deserializable<T>::value, "trying to write non primitive non serializable type into buffer.");
	std::stack<T> s_tmp;
	for (size_t i = 0; i < length; ++i)
	{
		T v_tmp = default_construct<T>::get_instance();
		this->read(v_tmp);
		s_tmp.push(v_tmp);
	}
	for (size_t i = 0; i < length; ++i)
	{
		dest.push_front(s_tmp.top());
		s_tmp.pop();
	}
}

template <class T>
void Buffer::read(std::stack<T> &dest, const size_t length)
{
	static_assert(is_deserializable<T>::value, "trying to write non primitive non serializable type into buffer.");
	for (size_t i = 0; i < length; ++i)
	{
		T tmp = default_construct<T>::get_instance();
		this->read(tmp);
		dest.push(tmp);
	}
}

template <class T>
void Buffer::read(std::queue<T> &dest, const size_t length)
{
	static_assert(is_deserializable<T>::value, "trying to write non primitive non serializable type into buffer.");
	for (size_t i = 0; i < length; ++i)
	{
		T tmp = default_construct<T>::get_instance();
		this->read(tmp);
		dest.push(tmp);
	}
}


#pragma endregion 


}

#endif
