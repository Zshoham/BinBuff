#include "Buffer.h"

namespace Serializer
{

void Buffer::alloc_buffer(const std::size_t &size)
{
	if(this->buffer_size - this->next_pointer < size)
	{
		if (this->buffer_type == STATIC) throw BufferOverflowException("reached the end of the allocated buffer_size of a static buffer.");
		void *tmp = nullptr;
		if (size > this->buffer_size * 2) tmp = realloc(this->buffer_data, this->next_pointer + size);
		else tmp = realloc(this->buffer_data, this->buffer_size * 2);
		if (!tmp) throw BufferAllocationException("failed to allocate additional buffer space.");
		this->buffer_data = tmp;
		this->buffer_size *= 2;
	}
}

Buffer::Buffer(type type, const std::size_t &size)
{
	if (size < 1) throw BufferUnderflowException("buffer buffer_size must be greater than 1.");
	this->buffer_data = malloc(size);
	if (!buffer_data) throw BufferAllocationException("filed to allocate buffer.");
	this->buffer_size = size;
	this->buffer_type = type;
	this->buffer_mode = WRITE;
	this->next_pointer = 0;

}

Buffer::Buffer(const Buffer& other)
{
	this->buffer_data = malloc(other.buffer_size);
    if (!buffer_data) throw BufferAllocationException("filed to allocate buffer.");
    if (!std::memcpy(this->buffer_data, other.buffer_data, other.buffer_size)) throw BufferNullPointerException("failed to write buffer_data into buffer.");
	this->buffer_size = other.buffer_size;
	this->next_pointer = other.next_pointer;
	this->buffer_mode = other.buffer_mode;
	this->buffer_type = other.buffer_type;
}

Buffer::Buffer(Buffer&& other) noexcept
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


Buffer& Buffer::operator=(const Buffer& other)
{
	if (*this != other)
	{
		//allocate the new place for the buffer data.
		void* tmp = malloc(other.buffer_size);
		if (!tmp) throw BufferAllocationException("filed to allocate buffer.");
		if (!std::memcpy(tmp, other.buffer_data, other.buffer_size)) throw BufferNullPointerException("failed to write buffer_data into buffer.");

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

Buffer& Buffer::operator=(Buffer&& other) noexcept
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
	void *tmp = realloc(this->buffer_data, this->buffer_size);
	if (!tmp) throw BufferAllocationException("failed to reallocate buffer space.");
	this->buffer_data = tmp;
	this->buffer_mode = READ;
	this->next_pointer = 0;
}

void Buffer::set_mode_write(type type)
{
	if (this->buffer_mode == WRITE) return;
	if (type == DYNAMIC)
	{
		void *tmp = realloc(this->buffer_data, this->buffer_size + DEFAULT_BUFFER_SIZE);
		if (!tmp) throw BufferAllocationException("failed to reallocate buffer space.");
		this->buffer_data = tmp;
	}
	this->buffer_mode = WRITE;
	this->buffer_type = type;
	this->next_pointer = this->buffer_size;
}


bool Buffer::operator!=(const Buffer& other) const
{
	return !(*this == other);
}

Buffer& Buffer::operator+=(const std::size_t &jmp)
{
	if (this->buffer_mode == WRITE) throw BufferOverflowException("cannot jump forward in buffer while in write mode.");
	if (this->next_pointer + jmp > this->buffer_size) throw BufferOverflowException("trying to move outside of the buffer bounds.");
	this->next_pointer += jmp;
	return *this;
}

Buffer& Buffer::operator++()
{
	if (this->buffer_mode == WRITE) throw BufferOverflowException("cannot jump forward in buffer while in write mode.");
	if (this->next_pointer + 1 > this->buffer_size) throw BufferOverflowException("trying to move outside of the buffer bounds.");	this->next_pointer++;
	return *this;
}

Buffer Buffer::operator++(int)
{
	if (this->buffer_mode == WRITE) throw BufferOverflowException("cannot jump forward in buffer while in write mode.");
	if (this->next_pointer + 1 > this->buffer_size) throw BufferOverflowException("trying to move outside of the buffer bounds.");
	Buffer res(*this);

	this->next_pointer++;

	return res;
}

Buffer& Buffer::operator-=(const std::size_t &jmp)
{
	if (static_cast<int>(this->next_pointer) - static_cast<int>(jmp) < 0) throw BufferUnderflowException("trying to move to negative position in the buffer.");
	this->next_pointer -= jmp;
	return *this;
}

Buffer& Buffer::operator--()
{
	if (static_cast<int>(this->next_pointer) - 1 < 0) throw BufferUnderflowException("trying to move to negative position in the buffer.");
	this->next_pointer--;
	return *this;
}

Buffer Buffer::operator--(int)
{
	if (static_cast<int>(this->next_pointer) - 1 < 0) throw BufferUnderflowException("trying to move to negative position in the buffer.");
	Buffer res(*this);

	this->next_pointer--;

	return res;
}


bool Buffer::operator==(const Buffer& other) const
{
	const bool are_equal = strcmp(static_cast<char *>(this->buffer_data), static_cast<char *>(other.buffer_data));
	return are_equal && this->buffer_type == other.buffer_type;
}

void Buffer::write_generic(const void* data, const std::size_t &size)
{
	if (!data) throw BufferNullPointerException("trying to write buffer_data that from null pointer.");
	if (this->buffer_mode == READ) throw BufferIllegalWriteException("cannot write to buffer when in read mode.");
	try
	{
		alloc_buffer(size);
		char *dest = static_cast<char*>(this->buffer_data);
		if (!std::memcpy(dest + this->next_pointer, data, size)) throw BufferNullPointerException("failed to write buffer_data into buffer.");
		this->next_pointer += size;
	}
	catch (BufferOverflowException& e) { std::clog << e.what() << std::endl; }
	catch (BufferAllocationException& e) { std::cerr << e.what() << std::endl; }
}


void Buffer::read_generic(void* dest, std::size_t size)
{
	if (!buffer_data) throw BufferNullPointerException("trying to read data into a null pointer.");
	if (this->buffer_mode == WRITE) throw BufferIllegalWriteException("cannot write to buffer when in read mode.");
	if (this->next_pointer + size > this->buffer_size) throw BufferOverflowException("reached end of buffer.");
	const char *src = static_cast<char *>(this->buffer_data);
	if (!std::memcpy(dest, src + this->next_pointer, size)) throw BufferAllocationException("failed to allocate additional buffer space.");
	this->next_pointer += size;
}


std::ofstream& operator<<(std::ofstream& stream, const Buffer& buffer)
{
	stream.write(static_cast<char *>(buffer.buffer_data), buffer.buffer_size);
	return stream;
}

std::ifstream& operator>>(std::ifstream& stream, Buffer& buffer)
{
	stream.seekg(0, std::ifstream::end);
	buffer.buffer_size = stream.tellg();
	stream.seekg(0, std::ifstream::beg);

	buffer.next_pointer = 0;
	const Buffer::type p_type = buffer.buffer_type;
	buffer.buffer_type = Buffer::DYNAMIC;
	buffer.alloc_buffer(buffer.buffer_size);

	stream.read(static_cast<char *>(buffer.buffer_data), buffer.buffer_size);
	buffer.buffer_type = p_type;

	return stream;
}

}
