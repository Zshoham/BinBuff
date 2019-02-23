#ifndef CPPSERIALIZEREXCEPTION_H
#define CPPSERIALIZEREXCEPTION_H

#include <exception>
#include <string>

class CPPSerializerException : std::exception
{
	const char *error_msg;

public:
	CPPSerializerException(const char *error_msg) noexcept : error_msg(error_msg) {}

	virtual const char *what() const noexcept override { return error_msg; }
};

class BufferOverflowException : public CPPSerializerException
{
public:
	BufferOverflowException(const char *error_msg) noexcept : CPPSerializerException(error_msg) {}

};

class BufferUnderflowException : public CPPSerializerException
{
public:
	BufferUnderflowException(const char *error_msg) noexcept : CPPSerializerException(error_msg) {}
};

class BufferAllocationException : public CPPSerializerException
{
public:
	BufferAllocationException(const char *error_msg) noexcept : CPPSerializerException(error_msg) {}
};

class BufferIllegalReadException : public CPPSerializerException
{
public:
	BufferIllegalReadException(const char *error_msg) noexcept : CPPSerializerException(error_msg) {}
};

class BufferIllegalWriteException : public CPPSerializerException
{
public:
	BufferIllegalWriteException(const char *error_msg) noexcept : CPPSerializerException(error_msg) {}
};

class BufferNullPointerException : public CPPSerializerException
{
public:
	BufferNullPointerException(const char *error_msg) noexcept : CPPSerializerException(error_msg) {}
};


#endif
