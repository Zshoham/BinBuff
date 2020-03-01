#include "buffer.h"

namespace binbuff
{

void Buffer::alloc_buffer(const std::size_t &size)
{
	if (this->buffer_size - this->next_pointer < size)
	{
		if (this->buffer_type == STATIC)
			throw std::length_error("reached the end of the allocated buffer_size of a static buffer.");

		void *tmp = nullptr;
		if (size > this->buffer_size * 2 - this->next_pointer)
		{
			tmp = std::realloc(this->buffer_data, this->next_pointer + size);
			this->buffer_size = this->next_pointer + size;
		}
		else
		{
			tmp = std::realloc(this->buffer_data, this->buffer_size * 2);
			this->buffer_size *= 2;
		}
		if (!tmp) throw std::runtime_error("failed to allocate additional buffer space.");
		this->buffer_data = tmp;
	}
}

void *Buffer::rev_memcpy(void *dest, const void *src, size_t length)
{
	char *d = static_cast<char *>(dest) + length - 1;
	const char *s = static_cast<const char *>(src);
	while (length--)
		*d-- = *s++;
	return dest;
}

Buffer::Buffer(const TYPE &type, const std::size_t &size)
{
	if (size < 1) throw std::length_error("buffer buffer_size must be greater than 1.");
	this->buffer_data = std::malloc(size);
	if (!buffer_data) throw std::runtime_error("filed to allocate buffer.");
	this->buffer_size = size;
	this->buffer_type = type;
	this->buffer_mode = WRITE;
	this->next_pointer = 0;

}

Buffer::Buffer(const Buffer &other)
{
	this->buffer_data = std::malloc(other.buffer_size);
	if (!buffer_data) throw std::runtime_error("filed to allocate buffer.");
	if (!std::memcpy(this->buffer_data, other.buffer_data, other.next_pointer))
		throw std::runtime_error("failed to write buffer_data into buffer.");

	this->buffer_size = other.buffer_size;
	this->next_pointer = other.next_pointer;
	this->buffer_mode = other.buffer_mode;
	this->buffer_type = other.buffer_type;
}

Buffer::Buffer(Buffer &&other) noexcept
{
	this->buffer_data = other.buffer_data;
	this->buffer_size = other.buffer_size;
	this->next_pointer = other.next_pointer;
	this->buffer_mode = other.buffer_mode;
	this->buffer_type = other.buffer_type;

	other.buffer_data = nullptr;
	other.buffer_size = 0;
	other.next_pointer = 0;
	other.buffer_mode = READ;
	other.buffer_type = DYNAMIC;
}

Buffer::~Buffer()
{
	free(this->buffer_data);
}


Buffer &Buffer::operator=(const Buffer &other)
{
	if (*this != other)
	{
		//allocate the new place for the buffer data.
		void *tmp = std::malloc(other.buffer_size);
		if (!tmp) throw std::runtime_error("filed to allocate buffer.");
		if (!std::memcpy(tmp, other.buffer_data, other.buffer_size))
			throw std::runtime_error("failed to write buffer_data into buffer.");

		//if there were no problems point buffer_data to the newly allocated data.
		free(this->buffer_data);
		this->buffer_data = tmp;

		this->buffer_size = other.buffer_size;
		this->next_pointer = other.next_pointer;
		this->buffer_mode = other.buffer_mode;
		this->buffer_type = other.buffer_type;
	}


	return *this;
}

Buffer &Buffer::operator=(Buffer &&other) noexcept
{
	if (*this == other) return *this;
	free(this->buffer_data);

	this->buffer_data = other.buffer_data;
	this->buffer_size = other.buffer_size;
	this->next_pointer = other.next_pointer;
	this->buffer_mode = other.buffer_mode;
	this->buffer_type = other.buffer_type;

	other.buffer_data = nullptr;
	other.buffer_size = 0;
	other.next_pointer = 0;
	other.buffer_mode = READ;
	other.buffer_type = DYNAMIC;

	return *this;
}


void Buffer::set_mode_read()
{
	if (this->buffer_mode == READ) return;
	void *tmp = std::realloc(this->buffer_data, this->next_pointer);
	if (!tmp) throw std::runtime_error("failed to reallocate buffer space.");
	this->buffer_data = tmp;
	this->buffer_mode = READ;
	this->next_pointer = 0;
}

void Buffer::set_mode_write(const TYPE type, const size_t extra_size)
{
	if (this->buffer_mode == WRITE) return;
	if (type == DYNAMIC)
	{
		void *tmp = std::realloc(this->buffer_data, this->buffer_size + extra_size);
		if (!tmp) throw std::runtime_error("failed to reallocate buffer space.");
		this->buffer_data = tmp;
	}
	this->buffer_mode = WRITE;
	this->buffer_type = type;
	this->next_pointer = this->buffer_size;
	this->buffer_size = buffer_size + extra_size;
}


bool Buffer::operator!=(const Buffer &other) const
{
	return !(*this == other);
}

Buffer &Buffer::operator+=(const std::size_t &jmp)
{
	if (this->buffer_mode == WRITE) throw std::logic_error("cannot jump forward in buffer while in write mode.");
	if (this->next_pointer + jmp > this->buffer_size)
		throw std::length_error("trying to move outside of the buffer bounds.");
	this->next_pointer += jmp;
	return *this;
}

Buffer &Buffer::operator++()
{
	if (this->buffer_mode == WRITE)
		throw std::logic_error("cannot jump forward in buffer while in write mode.");

	if (this->next_pointer + 1 > this->buffer_size)
		throw std::length_error("trying to move outside of the buffer bounds.");

	this->next_pointer++;

	return *this;
}

Buffer &Buffer::operator-=(const std::size_t &jmp)
{
	if (static_cast<int>(this->next_pointer) - static_cast<int>(jmp) < 0)
		throw std::logic_error("trying to move to negative position in the buffer.");

	this->next_pointer -= jmp;
	return *this;
}

Buffer &Buffer::operator--()
{
	if (static_cast<int>(this->next_pointer) - 1 < 0)
		throw std::logic_error("trying to move to negative position in the buffer.");

	this->next_pointer--;
	return *this;
}

void *Buffer::clone_serialized() const
{
	void *res = std::malloc(this->next_pointer);
	std::memcpy(res, this->buffer_data, this->next_pointer);
	return res;
}

std::vector<unsigned char> Buffer::as_vec() const
{
	std::vector<unsigned char> res;
	const unsigned char *data = static_cast<const unsigned char *>(buffer_data);

	for(size_t i = 0; i < this->next_pointer; ++i)
		res.push_back(data[i]);

	return res;
}

bool Buffer::operator==(const Buffer &other) const
{
	if (this->next_pointer != other.next_pointer)
		return false;

	const char *this_data = static_cast<const char *>(this->buffer_data);
	const char *other_data = static_cast<const char *>(other.buffer_data);

	for (size_t i = 0; i < this->next_pointer; ++i)
	{
		if (this_data[i] != other_data[i])
			return false;
	}

	return true;
}

void Buffer::write_generic(const void *data, const std::size_t &size, const bool &check_endian)
{
	if (!data) throw std::runtime_error("trying to write buffer_data that from null pointer.");
	if (this->buffer_mode == READ)
		throw std::logic_error("cannot write to buffer when in read mode.");

	alloc_buffer(size);
	char *dest = static_cast<char *>(this->buffer_data);

	if (is_big_endian || !check_endian)
		dest = static_cast<char *>(std::memcpy(dest + this->next_pointer, data, size));
	else
		dest = static_cast<char *>(rev_memcpy(dest + this->next_pointer, data, size));

	if (!dest) throw std::runtime_error("failed to write buffer_data into buffer.");
	this->next_pointer += size;
}

void Buffer::read_generic(void *dest, const std::size_t &size, const bool &check_endian)
{
	if (!dest) throw std::runtime_error("trying to read data into a null pointer.");
	if (this->buffer_mode == WRITE)
		throw std::logic_error("cannot read from buffer when in write mode.");

	if (this->next_pointer + size > this->buffer_size)
		throw std::length_error("reached end of buffer.");

	const char *src = static_cast<char *>(this->buffer_data);

	if (is_big_endian || !check_endian)
		dest = std::memcpy(dest, src + this->next_pointer, size);
	else
		dest = rev_memcpy(dest, src + this->next_pointer, size);

	if (!dest) throw std::runtime_error("failed to read data due to memory copying error.");
	this->next_pointer += size;
}


std::ostream &operator<<(std::ostream &stream, const Buffer &buffer)
{
	stream.write(static_cast<char *>(buffer.buffer_data), buffer.next_pointer);
	return stream;
}

std::istream &operator>>(std::istream &stream, Buffer &buffer)
{
	if (buffer.buffer_mode == Buffer::READ)
		throw std::logic_error("cannot write to buffer when in read mode.");

	stream.seekg(0, std::istream::end);
	const size_t stream_length = stream.tellg();
	stream.seekg(0, std::istream::beg);

	buffer.alloc_buffer(stream_length);

	stream.read(static_cast<char *>(buffer.buffer_data) + buffer.next_pointer, buffer.buffer_size);
	buffer.next_pointer += stream_length;

	return stream;
}

}
