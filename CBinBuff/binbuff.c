#include "binbuff.h"

typedef enum {FALSE = 0, TRUE = 1} bool;

struct  s_buffer
{
	void *data;
	size_t size;
	size_t next_pointer;
	type type;
	mode mode;
	bool big_endian;
};

#pragma region UTIL

//checks if the system is using big endian representation or not.
static bool is_big_endian()
{
    int num = 1;
    if(*(char *)&num == 1) return FALSE;
    return TRUE;
}

/*
reverse memory copy - copies length bytes from the memory pointed to
 by src into dest but in reverse order, meaning for the following
 src -> [b1, b2, b3, ..., bn] source data the n bytes following the location
 pointed to by dest will look like this:
 dest -> [bn, b(n-1), ..., b3, b2, b1]
 */
static void* rev_memcpy(void *dest, const void *src, size_t length)
{
    char *d = (char*)dest + length - 1;
    const char *s = src;
    while (length--)
        *d-- = *s++;
    return dest;
}

/*
If the buffer is dynamic ensures the buffer has at least `size` free memory to write to.
Otherwise will return status - BUFFER OVERFLOW.
buffer - buffer for which  to allocate the memory.
size - the size of the memory that needs to be available.

returns status { BUFFER_OVERFLOW, FAILURE, SUCCESS } indicating the completion status of the function.
*/
static status alloc_buffer(buffer *buffer, const size_t size)
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
static status serialize_data(buffer *buffer, void *data, const size_t size)
{
	if (!buffer) return FAILURE;
	if (!data) return FAILURE;
	if (buffer->mode == READ) return ILLEGAL_WRITE;
	status s;
	if ((s = alloc_buffer(buffer, size) != SUCCESS)) return s;
	char *dest = (char*)buffer->data;

	if (buffer->big_endian)
        dest = memcpy(dest + buffer->next_pointer, data, size);
    else 
		dest = rev_memcpy(dest + buffer->next_pointer, data, size);

    if (!dest) return FAILURE;
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
static status deserialize_data(buffer *buffer, void *dest, const size_t size)
{
	if (!buffer) return FAILURE;
	if (!dest) return FAILURE;
	if (buffer->mode == WRITE) return ILLEGAL_READ;
	if (buffer->next_pointer + buffer->size < size) return BUFFER_OVERFLOW;
	const char *src = buffer->data;

	if (buffer->big_endian)
        dest = memcpy(dest, src + buffer->next_pointer, size);
    else
        dest = rev_memcpy(dest, src + buffer->next_pointer, size);

    if (!dest) return FAILURE;
    buffer->next_pointer += size;
	return SUCCESS;
}

#pragma endregion UTIL

#pragma region BUFFER

buffer* create_dynamic_buffer(const size_t initial_size, status *status)
{
	if (initial_size < 1)
	{
		*status = BUFFER_UNDERFLOW;
		return NULL;
	}

	buffer *res = (buffer*)malloc(sizeof(struct s_buffer));
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
    res->big_endian = is_big_endian();


    return res;
}

buffer* create_buffer(const type type, status *status)
{
	buffer *res = (buffer*)malloc(sizeof(struct s_buffer));
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
    res->big_endian = is_big_endian();

	return res;
}

buffer* create_static_buffer(const size_t size, status *status)
{
	if (size < 1)
	{
		*status = BUFFER_UNDERFLOW;
		return NULL;
	}

	buffer *res = (buffer*)malloc(sizeof(struct s_buffer));
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
    res->big_endian = is_big_endian();

    return res;
}

buffer* create_read_buffer(void* data, const size_t size, status* status)
{
	if (size < 1)
	{
		*status = BUFFER_UNDERFLOW;
		return NULL;
	}

	buffer* res = (buffer*)malloc(sizeof(struct s_buffer));
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
	memcpy(res->data, data, size);
	
	res->size = size;
	res->next_pointer = 0;
	res->type = STATIC;
	res->mode = READ;
	*status = SUCCESS;
	res->big_endian = is_big_endian();

	return res;
}

buffer* create_read_buffer_unsafe(void* data, size_t size, status* status)
{
	if (size < 1)
	{
		*status = BUFFER_UNDERFLOW;
		return NULL;
	}

	buffer* res = (buffer*)malloc(sizeof(struct s_buffer));
	if (!res)
	{
		*status = FAILURE;
		return NULL;
	}

	res->data = data;
	res->size = size;
	res->next_pointer = 0;
	res->type = STATIC;
	res->mode = READ;
	*status = SUCCESS;
	res->big_endian = is_big_endian();

	return res;
}


status set_mode_read(buffer *buffer)
{
	void *tmp = realloc(buffer->data, buffer->size);
	if (!tmp) return FAILURE;
	buffer->data = tmp;
	buffer->mode = READ;
	buffer->next_pointer = 0;
	return SUCCESS;
}

status set_mode_write(buffer *buffer, const type type)
{
	if(type == DYNAMIC) 
	{
		void *tmp = realloc(buffer->data, buffer->size + DEFAULT_BUFFER_SIZE);
		if (!tmp) return FAILURE;
		buffer->data = tmp;
	}
	buffer->mode = WRITE;
	buffer->type = type;
	buffer->next_pointer = buffer->size;
	buffer->size = buffer->size + DEFAULT_BUFFER_SIZE;
	return SUCCESS;
}

status seek(buffer* buffer, const int amount)
{
	if (buffer->size < buffer->next_pointer + amount)
		return BUFFER_OVERFLOW;

	if (amount < 0 && ((buffer->next_pointer + amount) > buffer->next_pointer))
		return BUFFER_UNDERFLOW;

	buffer->next_pointer += amount;
	return SUCCESS;
}

void buffer_rewind(buffer* buffer)
{
	buffer->next_pointer = 0;
}

void* get_serialized(buffer *buffer, status *status)
{
	return buffer->data;
}

void* clone_serialized(buffer *buffer, status *status)
{
	void* clone = malloc(buffer->next_pointer);
	if (!clone)
	{
		*status = FAILURE;
		return NULL;
	}
	memcpy(clone, buffer->data, buffer->next_pointer);
	if (!clone)
	{
		*status = FAILURE;
		return NULL;
	}
	
	return clone;
}

void close_buffer(buffer **buffer)
{
	free((*buffer)->data);
	free(*buffer);
	buffer = NULL;
}

#pragma endregion BUFFER

#pragma region WRITE

#pragma region INTEGER

status write_char(buffer* buffer, char data)
{
	return serialize_data(buffer, &data, sizeof(data));
}

status write_char_array(buffer* buffer, char* data, const size_t length)
{
	return serialize_data(buffer, data, sizeof(*data) * length);
}

status write_short(buffer* buffer, short data)
{
	return serialize_data(buffer, &data, sizeof(data));
}

status write_short_array(buffer* buffer, short* data, const size_t length)
{
	return serialize_data(buffer, data, sizeof(*data) * length);
}

status write_int(buffer* buffer, int data)
{
	return serialize_data(buffer, &data, sizeof(data));
}

status write_int_array(buffer* buffer, int* data, const size_t length)
{
	return serialize_data(buffer, data, sizeof(*data) * length);
}

status write_long(buffer* buffer, long data)
{
	return serialize_data(buffer, &data, sizeof(data));
}

status write_long_array(buffer* buffer, long* data, const size_t length)
{
	return serialize_data(buffer, data, sizeof(*data) * length);
}

#pragma endregion INTEGER

#pragma region FLOATING POINT

status write_float(buffer* buffer, float data)
{
	return serialize_data(buffer, &data, sizeof(data));
}

status write_float_array(buffer* buffer, float* data, const size_t length)
{
	return serialize_data(buffer, data, sizeof(*data) * length);
}

status write_double(buffer* buffer, double data)
{
	return serialize_data(buffer, &data, sizeof(data));
}

status write_double_array(buffer* buffer, double* data, const size_t length)
{
	return serialize_data(buffer, data, sizeof(*data) * length);
}

#pragma endregion FLOATING POINT

#pragma region GENERIC

status write_data(buffer* buffer, void* data, const size_t size)
{
	return serialize_data(buffer, data, size);
}

status write_generic_data(buffer* buffer, serializable data, const serialize serializer)
{
	return serializer(buffer, data);
}

status write_generic_data_array(buffer* buffer, serializable *data, const size_t length, const serialize serializer)
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

status read_char(buffer *buffer, char *dest)
{
	return deserialize_data(buffer, dest, sizeof(*dest));
}

status read_char_array(buffer *buffer, char *dest, const size_t length)
{
	return deserialize_data(buffer, dest, sizeof(*dest) * length);
}

status read_short(buffer *buffer, short *dest)
{
	return deserialize_data(buffer, dest, sizeof(*dest));
}

status read_short_array(buffer *buffer, short *dest, const size_t length)
{
	return deserialize_data(buffer, dest, sizeof(*dest) * length);
}

status read_int(buffer *buffer, int *dest)
{
	return deserialize_data(buffer, dest, sizeof(*dest));
}

status read_int_array(buffer *buffer, int *dest, const size_t length)
{
	return deserialize_data(buffer, dest, sizeof(*dest) * length);
}

status read_long(buffer *buffer, long *dest)
{
	return deserialize_data(buffer, dest, sizeof(*dest));
}

status read_long_array(buffer *buffer, long *dest, const size_t length)
{
	return deserialize_data(buffer, dest, sizeof(*dest) * length);
}

#pragma endregion INTEGER

#pragma region FLOATING POINT

status read_float(buffer *buffer, float *dest)
{
	return deserialize_data(buffer, dest, sizeof(*dest));
}

status read_float_array(buffer *buffer, float *dest, const size_t length)
{
	return deserialize_data(buffer, dest, sizeof(*dest) * length);
}

status read_double(buffer *buffer, double *dest)
{
	return deserialize_data(buffer, dest, sizeof(*dest));
}

status read_double_array(buffer *buffer, double *dest, const size_t length)
{
	return deserialize_data(buffer, dest, sizeof(*dest) * length);
}

#pragma endregion FLOATING POINT

#pragma region GENERIC

status read_data(buffer* buffer, void* dest, const size_t size)
{
	return deserialize_data(buffer, dest, size);
}

status read_generic_data(buffer *buffer, serializable dest, const deserialize deserializer)
{
	return deserializer(buffer, dest);
}

status read_generic_data_array(buffer *buffer, serializable *dest, const size_t length, const deserialize deserializer)
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


