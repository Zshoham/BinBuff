#include "CSerializer.h"

struct  s_buffer
{
	void *data;
	size_t size;
	size_t next_pointer;
	type type;
	mode mode;
};

#pragma region UTIL

/*
If the buffer is dynamic ensures the buffer has at least `size` free memory to write to.
Otherwise will return status - BUFFER OVERFLOW.
buffer - buffer for which  to allocate the memory.
size - the size of the memory that needs to be available.

returns status { BUFFER_OVERFLOW, FAILURE, SUCCESS } indicating the completion status of the function.
*/
status alloc_buffer(Buffer *buffer, size_t size)
{
	if(buffer->size - buffer->next_pointer < size)
	{
		if (buffer->type == STATIC) return BUFFER_OVERFLOW;
		void *tmp = realloc(buffer->data, buffer->size * 2);
		if (!tmp) return FAILURE;
		buffer->data = tmp;
		buffer->size *= 2;
		alloc_buffer(buffer, size);
	}
	return SUCCESS;
}


/*
Writes `size` bits starting from where `data` points to into the buffer.
If the buffer is not in WRITE mode returns ILLEGAL WRITE status.
buffer - buffer to write to.
data - pointer to the beginning of the data to be written.
size - size of the data to be written.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
*/
status serialize_data(Buffer *buffer, void *data, size_t size)
{
	if (!buffer) return FAILURE;
	if (buffer->mode == READ) return ILLEGAL_WRITE;
	status s;
	if ((s = alloc_buffer(buffer, size) != SUCCESS)) return s;
	char *dest = (char*)buffer->data;
	if (!memcpy(dest + buffer->next_pointer, data, size)) return FAILURE;
	buffer->next_pointer += size;
	return SUCCESS;
}


/*
Reads `size` bits starting from the buffer into `dest`.
If the buffer is not in READ mode returns ILLEGAL READ status.
buffer - buffer to read from.
dest - pointer to where the data should be read into.
size - size of the data to be read.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
*/
status deserialize_data(Buffer *buffer, void *dest, size_t size)
{
	if (!buffer) return FAILURE;
	if (buffer->mode == WRITE) return ILLEGAL_READ;
	if (buffer->next_pointer + buffer->size < size) return BUFFER_OVERFLOW;
	const char *src = buffer->data;
	if (!memcpy(dest, src + buffer->next_pointer, size)) return FAILURE;
	buffer->next_pointer += size;
	return SUCCESS;
}

#pragma endregion UTIL

#pragma region BUFFER

/*
Creates a new dynamic buffer with initial size - `size`.
Note that buffer size less then 1 is not allowed.
status - pointer to status that will hold the status of the function after its completion.

returns the created buffer, also the status { BUFFER_UNDERFLOW, FAILURE, SUCCESS }
*/
Buffer* create_dynamic_buffer(size_t initial_size, status *status)
{
	if (initial_size < 1)
	{
		*status = BUFFER_UNDERFLOW;
		return NULL;
	}

	Buffer *res = (Buffer*)malloc(sizeof(struct s_buffer));
	if(!res)
	{
		*status = FAILURE;
		return NULL;
	}

	res->data = malloc(initial_size);
	if(!res->data)
	{
		*status = FAILURE;
		free(res);
		return NULL;
	}
	res->size = initial_size;
	res->next_pointer = 0;
	res->type = DYNAMIC;
	res->mode = WRITE;
	*status = SUCCESS;

	return res;
}

/*
Creates a new buffer with default size and type - `type`.
status - pointer to status that will hold the status of the function after its completion.

returns the created buffer, also the status { BUFFER_UNDERFLOW, FAILURE, SUCCESS }.
*/
Buffer* create_buffer(type type, status *status)
{
	Buffer *res = (Buffer*)malloc(sizeof(struct s_buffer));
	if (!res)
	{
		*status = FAILURE;
		return NULL;
	}

	res->data = malloc(DEFAULT_BUFFER_SIZE);
	if (!res->data)
	{
		*status = FAILURE;
		free(res);
		return NULL;
	}
	res->size = DEFAULT_BUFFER_SIZE;
	res->next_pointer = 0;
	res->type = type;
	res->mode = WRITE;
	*status = SUCCESS;

	return res;
}

/*
Creates a new static buffer of size - `size`.
Note that buffer size less then 1 is not allowed.
status - pointer to status that will hold the status of the function after its competition.

returns the created buffer, also the status { BUFFER_UNDERFLOW, FAILURE, SUCCESS }.
*/
Buffer* create_static_buffer(size_t size, status *status)
{
	if (size < 1)
	{
		*status = BUFFER_UNDERFLOW;
		return NULL;
	}

	Buffer *res = (Buffer*)malloc(sizeof(struct s_buffer));
	if (!res)
	{
		*status = FAILURE;
		return NULL;
	}

	res->data = malloc(size);
	if (!res->data)
	{
		*status = FAILURE;
		free(res);
		return NULL;
	}
	res->size = size;
	res->next_pointer = 0;
	res->type = STATIC;
	res->mode = WRITE;
	*status = SUCCESS;

	return res;
}

/*
Changes the mode of `buffer` to read.
Note that reading after calling this function will result in a read beginning from the start of the buffer.

returns status { FAILURE, SUCCESS } indicating the completion status of the function.
 */
status set_mode_read(Buffer *buffer)
{
	void *tmp = realloc(buffer->data, buffer->size);
	if (!tmp) return FAILURE;
	buffer->data = tmp;
	buffer->mode = READ;
	buffer->next_pointer = 0;
	return SUCCESS;
}

/*
Changes the mode of `buffer` to write setting its type to be `type`.

returns status { FAILURE, SUCCESS } indicating the completion status of the function.
 */
status set_mode_write(Buffer* buffer, type type)
{
	void *tmp = realloc(buffer->data, buffer->size + DEFAULT_BUFFER_SIZE);
	if (!tmp) return FAILURE;
	buffer->data = tmp;
	buffer->mode = WRITE;
	buffer->type = type;
	buffer->next_pointer = buffer->size;
	return SUCCESS;
}


//Closes the buffer that `buffer` points to and releases all the memory assosiated with it.
void close_buffer(Buffer **buffer)
{
	free((*buffer)->data);
	free(*buffer);
	buffer = NULL;
}

#pragma endregion BUFFER

#pragma region WRITE

#pragma region INTEGER

/*
Write a char into the buffer - `buffer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status write_char(Buffer* buffer, char data)
{
	return serialize_data(buffer, &data, sizeof(data));
}

/*
Write char array of length - `length` into the buffer - `buffer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status write_char_array(Buffer* buffer, char* data, size_t length)
{
	return serialize_data(buffer, data, sizeof(*data) * length);
}

/*
Write a short into the buffer - `buffer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status write_short(Buffer* buffer, short data)
{
	return serialize_data(buffer, &data, sizeof(data));
}

/*
Write short array of length - `length` into the buffer - `buffer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status write_short_array(Buffer* buffer, short* data, size_t length)
{
	return serialize_data(buffer, data, sizeof(*data) * length);
}

/*
Write an int into the buffer - `buffer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status write_int(Buffer* buffer, int data)
{
	return serialize_data(buffer, &data, sizeof(data));
}

/*
Write int array of length - `length` into the buffer - `buffer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status write_int_array(Buffer* buffer, int* data, size_t length)
{
	return serialize_data(buffer, data, sizeof(*data) * length);
}

/*
Write a long into the buffer - `buffer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status write_long(Buffer* buffer, long data)
{
	return serialize_data(buffer, &data, sizeof(data));
}

/*
Write long array of length - `length` into the buffer - `buffer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status write_long_array(Buffer* buffer, long* data, size_t length)
{
	return serialize_data(buffer, data, sizeof(*data) * length);
}

#pragma endregion INTEGER

#pragma region FLOATING POINT

/*
Write a float into the buffer - `buffer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status write_float(Buffer* buffer, float data)
{
	return serialize_data(buffer, &data, sizeof(data));
}

/*
Write float array of length - `length` into the buffer - `buffer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status write_float_array(Buffer* buffer, float* data, size_t length)
{
	return serialize_data(buffer, data, sizeof(*data) * length);
}

/*
Write a double into the buffer - `buffer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status write_double(Buffer* buffer, double data)
{
	return serialize_data(buffer, &data, sizeof(data));
}

/*
Write double array of length - `length` into the buffer - `buffer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status write_double_array(Buffer* buffer, double* data, size_t length)
{
	return serialize_data(buffer, data, sizeof(*data) * length);
}

#pragma endregion FLOATING POINT

#pragma region GENERIC

/*
Writes `size` bytes starting from where `data` points to into the buffer - `buffer`.
Use this function carefully and only when you know the data pointed to by `data`
is continues in memory.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
*/
status write_data(Buffer* buffer, void* data, size_t size)
{
	return serialize_data(buffer, data, size);
}

/*
Writes `data` into the buffer using the provided serializer - `serializer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
*/
status write_generic_data(Buffer* buffer, Serializable data, serialize serializer)
{
	return serializer(buffer, data);
}

/*
Writes array of serializable data into the buffer using the provided serializer - `serializer`.
data - array of pointers to the serializable structure.
length - length of the array - `data`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
*/
status write_generic_data_array(Buffer* buffer, Serializable *data, size_t length, serialize serializer)
{
	status s = SUCCESS;
	size_t i = 0;
	while (s == SUCCESS && i < length)
	{
		s = serializer(buffer, data[i]);
		i++;
	}

	return s;
}


#pragma endregion GENERIC

#pragma  endregion WRITE

#pragma region READ

#pragma region INTEGER

/*
Reads a char from the buffer - `buffer` into the char pointed to by `dest`.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status read_char(Buffer *buffer, char *dest)
{
	return deserialize_data(buffer, dest, sizeof(*dest));
}

/*
Reads a char array from the buffer - `buffer` into the char array `dest`.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status read_char_array(Buffer *buffer, char *dest, size_t length)
{
	return deserialize_data(buffer, dest, sizeof(*dest) * length);
}

/*
Reads a short from the buffer - `buffer` into the short pointed to by `dest`.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status read_short(Buffer *buffer, short *dest)
{
	return deserialize_data(buffer, dest, sizeof(*dest));
}

/*
Reads a short array from the buffer - `buffer` into the short array `dest`.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status read_short_array(Buffer *buffer, short *dest, size_t length)
{
	return deserialize_data(buffer, dest, sizeof(*dest) * length);
}

/*
Reads a long from the buffer - `buffer` into the long pointed to by `dest`.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status read_int(Buffer *buffer, int *dest)
{
	return deserialize_data(buffer, dest, sizeof(*dest));
}

/*
Reads a int array from the buffer - `buffer` into the int array `dest`.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status read_int_array(Buffer *buffer, int *dest, size_t length)
{
	return deserialize_data(buffer, dest, sizeof(*dest) * length);
}

/*
Reads a char from the buffer - `buffer` into the char pointed to by `dest`.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status read_long(Buffer *buffer, long *dest)
{
	return deserialize_data(buffer, dest, sizeof(*dest));
}

/*
Reads a long array from the buffer - `buffer` into the long array `dest`.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status read_long_array(Buffer *buffer, long *dest, size_t length)
{
	return deserialize_data(buffer, dest, sizeof(*dest) * length);
}

#pragma endregion INTEGER

#pragma region FLOATING POINT

/*
Reads a float from the buffer - `buffer` into the float pointed to by `dest`.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status read_float(Buffer *buffer, float *dest)
{
	return deserialize_data(buffer, dest, sizeof(*dest));
}

/*
Reads a float array from the buffer - `buffer` into the float array `dest`.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status read_float_array(Buffer *buffer, float *dest, size_t length)
{
	return deserialize_data(buffer, dest, sizeof(*dest) * length);
}

/*
Reads a double from the buffer - `buffer` into the double pointed to by `dest`.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status read_double(Buffer *buffer, double *dest)
{
	return deserialize_data(buffer, dest, sizeof(*dest));
}

/*
Reads a double array from the buffer - `buffer` into the double array `dest`.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status read_double_array(Buffer *buffer, double *dest, size_t length)
{
	return deserialize_data(buffer, dest, sizeof(*dest) * length);
}

#pragma endregion FLOATING POINT

#pragma region GENERIC

/*
Reads `size` bytes starting from the buffer into the "array" `dest`.

returns status { ILLEAGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
*/
status read_data(Buffer* buffer, void* dest, size_t size)
{
	return deserialize_data(buffer, dest, size);
}

/*
Reads a single structure of the type that `dest` points to from the buffer into `dest` using the provided deserializer - `deserializer`.

returns status { ILLEAGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
*/
status read_generic_data(Buffer *buffer, Serializable dest, deserialize deserializer)
{
	return deserializer(buffer, dest);
}

/*
Reads an array of structures of the type that `dest` contains from the buffer into `dest` using the provided deserializer - `deserializer`.
dest - array of pointers to the serializable structure to be read into.
length - length of the array - `dest` and the amount of structures that will be read.

returns status { ILLEAGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
*/
status read_generic_data_array(Buffer *buffer, Serializable *dest, size_t length, deserialize deserializer)
{
	status s = SUCCESS;
	unsigned int i = 0;
	while(s == SUCCESS && i < length)
	{
		s = deserializer(buffer, *(dest + i));
		i++;
	}

	return s;
}

#pragma endregion GENERIC

#pragma endregion READ


