#ifndef BUFFER_H
#define BUFFER_H

#include "Serializable.h"
#include "CPPSerializerException.h"
#include "serializable_traits.h"

#include <iostream>
#include <cstring>
#include <array>
#include <stack>
#include <queue>
#include <fstream>
#include <forward_list>
#define DEFAULT_BUFFER_SIZE 32

namespace Serializer
{

class Buffer
{
public:
	enum type { DYNAMIC, STATIC };
	enum mode { READ, WRITE };

private:
	void *data;
	std::size_t size;
	std::size_t next_pointer;
	type buffer_type;
	mode buffer_mode;


	void alloc_buffer(const std::size_t &size);

	template <class T>
	void write(const T &data, std::true_type)
	{
		static_assert(std::is_arithmetic<T>::value, "trying to write non primitive non serializable types.");
		write_generic(static_cast<const void*>(&data), sizeof(data));
	}

	template<class T>
	void write(const T &data, std::false_type)
	{
		this->write_n_primitive(data, is_serializable<T>());
	}

	template<class T>
	void write_n_primitive(const T&data, std::true_type)
	{
		static_assert(is_serializable<T>::value, "trying to write unserializable type.");
		data.serialize(*this);
	}

	template <class T>
	void write_n_primitive(const T& data, std::false_type)
	{
		static_assert(is_writable_container<T>::value, "trying to write an unserializable object or an unwritable container into buffer.");
		this->write(data.begin(), data.end());
	}

	template <class T>
	void write(const T* data, const std::size_t &length, std::true_type)
	{
		static_assert(std::is_arithmetic<T>::value, "trying to write non primirive non serializable types.");
		if (!data) throw BufferNullPointerException("trying to write nullptr");
		write_generic(static_cast<const void*>(data), sizeof(*data) * length);
	}

	template<class T>
	void write(const T *data, const std::size_t &length, std::false_type);

	template<class T>
	void read(T &dest, std::true_type)
	{
		static_assert(std::is_arithmetic<T>::value, "trying to read into primitive non serializable types.");
		read_generic(static_cast<void *>(&dest), sizeof(dest));
	}

	template<class T>
	void read(T &dest, std::false_type)
	{
		static_assert(std::is_base_of<Serializable, T>::value, "trying to read into primitive non serializable types.");
		static_assert(!is_readable_container<T>::value, "trying to read into container without specifying number of elements.");
		dest.deserialize(*this);
	}
	
	template<class T>
	void read_container(T &dest, size_t length, std::true_type)
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
	void read_container(T &dest, size_t length, std::false_type)
	{
		typedef typename remove_inner_const<typename T::value_type>::type value_type;
		for (size_t i = 0; i < length; ++i)
		{
			value_type tmp = default_construct<value_type>::get_instance();
			this->read(tmp);
			dest.insert(dest.end(), tmp);
		}
	}

	template<class T>
	void read(T *dest, const std::size_t &length, std::true_type)
	{
		static_assert(std::is_arithmetic<T>::value, "trying to read into primitive non serializable types.");
		read_generic(static_cast<void *>(dest), sizeof(*dest) * length);
	}

	template<class T>
	void read(T *dest, const std::size_t &length, std::false_type);

public:

	/**
	 * \brief creates a buffer in read mode with initial size 'size'
	 * \param type the type of the buffer that will be created {DYNAMIC, STATIC}
	 * \param size the initial size of the buffer that will be created.
	 */
	explicit Buffer(type type, const std::size_t &size = DEFAULT_BUFFER_SIZE);

	Buffer(const Buffer& other);
	Buffer(Buffer&& other) noexcept;
	~Buffer();

	Buffer& operator=(const Buffer& other);
	Buffer& operator=(Buffer&& other) noexcept;

	//sets the buffer to read mode.
	void set_mode_read();

	//sets the buffer to write mode, and sets the type of the buffer.
	void set_mode_write(type type);

	bool operator==(const Buffer& other) const;
	bool operator!=(const Buffer& other) const;

	//jump 'jmp' bytes forward in the buffer.
	Buffer& operator+=(const std::size_t &jmp);

	//jump one byte forward in the buffer.
	Buffer& operator++();

	//jump one byte forward in the buffer.
	Buffer operator++(int);

	//jump 'jmp' bytes backwards in the buffer.
	Buffer& operator-=(const std::size_t &jmp);

	//jump one byte backwards in the buffer.
	Buffer& operator--();

	//jump one byte backwards in the buffer.
	Buffer operator--(int);

	//writes the buffer into the file-stream.
	friend std::ofstream& operator<<(std::ofstream& stream, const Buffer& buffer);

	//reads the data in the file-stream into the buffer.
	friend std::ifstream& operator>>(std::ifstream& stream, Buffer& buffer);


	#pragma region write

	
	/**
	 * \brief writes 'size' bytes starting from where 'data' points into the buffer.
	 * \param data pointer to the data to be written into the buffer.
	 * \param size the size in bytes of the data to be written.
	 */
	void write_generic(const void *data, const std::size_t &size);


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
		if (!data) throw BufferNullPointerException("trying to write nullptr");
		this->write(*data);
	}


	/**
	 * \brief writes data contained in the shared pointer 'data'.
	 * \tparam T a serializable data type or a container of a serializable data type.
	 * \param data 
	 */
	template<class T>
	void write(const std::shared_ptr<T>& data) { this->write(*data); }

	/**
	 * \brief writes a pair 'data' in sequence such that the 'data.first' is written first and 'data.second' is written second.
	 * \tparam F a serializable data type or a container of a serializable data type.
	 * \tparam S a serializable data type or a container of a serializable data type.
	 * \param data the pair to be written.
	 */
	template<class F, class S>
	void write(const std::pair<F, S>& data)
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
		if (!data) throw BufferNullPointerException("trying to write nullptr");
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
	Buffer& operator<<(const T &data) { this->write(data); return *this; }

    /**
     * \brief writes the entire stack into the buffer.
     * \tparam T a serializable data type or a container of a serializable data type.
     * \param data the stack containing the data.
     */
	template<class T>
	Buffer& operator<<(std::stack<T> &data) { this->write(data); return *this; }

    /**
     * \brief writes the entire queue into the buffer.
     * \tparam T a serializable data type or a container of a serializable data type.
     * \param data the queue containing the data.
     */
	template<class T>
	Buffer& operator<<(std::queue<T> &data) { this->write(data); return *this; }

	#ifdef __GNUC__

    /**
     * \brief writes the entire array into the buffer.
     * \tparam T a serializable data type or a container of a serializable data type.
     * \tparam length a size_t representing the length of the array.
     * \param data the queue containing the data.
     */
	template<class T, size_t length>
	Buffer& operator<<(std::array<T, length> &data) { this->write(data); return *this; }

    #endif

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

    #ifdef __GNUC__

	/**
     * \brief writes the entire array into the buffer.
     * \tparam T a serializable data type or a container of a serializable data type.
     * \tparam length a size_t representing the length of the array.
     * \param data the queue containing the data.
     */
	template <class T, size_t length>
	void write(std::array<T, length> &data);

	#endif

	#pragma endregion


	#pragma region read

	/**
	 * \brief reads 'size' bytes from the buffer into the memory 'dest' points to.
	 * \param dest pointer to the beginning of the memory to be read into.
	 * \param size the size in bytes of the data to be read.
	 */
	void read_generic(void *dest, std::size_t size);

	/**
	 * \brief reads data from the buffer into 'dest'.
	 * \tparam T a deserializable data type.
	 * \param dest reference to where the data should be read into.
	 */
	template<class T>
	void read(T& dest) { this->read(dest, std::is_arithmetic<T>()); }

	/**
	 * \brief reads data from the buffer into 'dest'.
	 * \tparam T a deserializable data type.
	 * \param dest pointer to where the data should be read into.
	 */
	template<class T>
	void read(T *dest)
	{
		if (!dest) throw BufferNullPointerException("trying to read into nullptr");
		this->read(*dest);
	}

	/**
	 * \brief reads data from the buffer into 'dest'.
	 * \tparam T a deserializable data type.
	 * \param dest shared pointer to where the data should be read into.
	 */
	template<class T>
	void read(std::shared_ptr<T>& dest) { this->read(*dest); }


	/**
	 * \brief reads a pair from the buffer in sequence such that the 'dest.first' is read first and 'dest.second' is read second.
	 * \tparam F a deserializable data type.
	 * \tparam S a deserializable data type.
	 * \param dest the pair that should be read into.
	 */
	template<class F, class S>
	void read(std::pair<F, S>& dest)
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
	void read(T* dest, std::size_t length)
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
	Buffer& operator>>(T &dest) { read(dest); return *this; }


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
void Buffer::write(const T* data, const std::size_t &length, std::false_type)
{
	static_assert(std::is_base_of<Serializable, T>::value, "trying to write non primitive non serializable types.");
	if (!data) throw BufferNullPointerException("trying to write nullptr");
	for (size_t i = 0; i < length; ++i)
	{
		this->write(data[i]);
	}
}

template <class T>
void Buffer::write(T **data, const std::size_t &length)
{
	static_assert(is_serializable<T>::value, "trying to write non primitive non serializable type into buffer.");
	if (!data) throw BufferNullPointerException("trying to write nullptr");
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
	static_assert(is_serializable<typename Itr::value_type>::value, "trying to write non primitive non serializable type into buffer.");
	for (Itr itr = begin; itr != end; ++itr)
		this->write(*itr);
}

template <class T>
void Buffer::write(std::stack<T>& data)
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
void Buffer::write(std::queue<T>& data)
{
	static_assert(is_serializable<T>::value, "trying to write non primitive non serializable type into buffer.");
	while (!data.empty())
	{
		this->write(data.front());
		data.pop();
	}
}

#ifdef __GNUC__

template<class T, size_t length>
void Buffer::write(std::array<T, length> &data)
{
    static_assert(is_serializable<T>::value, "trying to write non primitive non serializable type into buffer.");
    for (int i = 0; i < data.size(); ++i) {
        this->write(data[i]);
    }
}

#endif

#pragma endregion



#pragma region read implementation


template <class T>
void Buffer::read(T* dest, const std::size_t &length, std::false_type)
{
	if (!dest) throw BufferNullPointerException("trying to read data into null");
	static_assert(is_deserializable<T>::value, "trying to read into non deserializable type.");
	for (std::size_t i = 0; i < length; ++i)
	{
		T tmp;
		tmp.deserialize(*this);
		dest[i] = tmp;
	}
}

template <class T>
void Buffer::read(T** dest, std::size_t length)
{
	if (!dest) throw BufferNullPointerException("trying to read data into null");
	static_assert(is_deserializable<T>::value, "trying to read into non deserializable type.");
	for (std::size_t i = 0; i < length; ++i)
	{
		dest[i] = new T;
		this->read(dest[i]);
	}
}

#pragma endregion 


#pragma region container read implementation

template <class T, size_t length>
void Buffer::read(std::array<T, length>& dest)
{
	for (size_t i = 0; i < length; ++i)
	{
		this->read(dest[i]);
	}
}

template <class T>
void Buffer::read(std::forward_list<T>& dest, size_t length)
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
void Buffer::read(std::stack<T>& dest, size_t length)
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
void Buffer::read(std::queue<T>& dest, size_t length)
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
