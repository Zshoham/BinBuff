#ifndef CSERIALIZER_H
#define CSERIALIZER_H

#include <stdlib.h>
#include <string.h>

#define DEFAULT_BUFFER_SIZE 32


typedef struct  s_buffer Buffer;

typedef void* Serializable;

typedef enum e_status {FAILURE, SUCCESS, BUFFER_OVERFLOW, BUFFER_UNDERFLOW, ILLEGAL_WRITE, ILLEGAL_READ} status;

typedef enum e_mode {READ, WRITE} mode;

typedef enum e_type {DYNAMIC, STATIC} type;

typedef  status (*serialize)(Buffer *buffer, Serializable data);

typedef status(*deserialize)(Buffer *buffer, Serializable dest);


Buffer* create_dynamic_buffer(size_t initial_size, status *status);

Buffer *create_buffer(type type, status *status);

Buffer *create_static_buffer(size_t size, status *status);


status set_mode_read(Buffer *buffer);

status set_mode_write(Buffer* buffer, type type);

void close_buffer(Buffer **buffer);



status write_char(Buffer *buffer, char data);

status write_char_array(Buffer *buffer, char *data, size_t length);

status write_short(Buffer *buffer, short data);

status write_short_array(Buffer *buffer, short *data, size_t length);

status write_int(Buffer *buffer, int data);

status write_int_array(Buffer *buffer, int *data, size_t length);

status write_long(Buffer *buffer, long data);

status write_long_array(Buffer *buffer, long *data, size_t length);

status write_float(Buffer *buffer, float data);

status write_float_array(Buffer *buffer, float *data, size_t length);

status write_double(Buffer *buffer, double data);

status write_double_array(Buffer *buffer, double *data, size_t length);

status write_data(Buffer *buffer, void *data, size_t size);

status write_generic_data(Buffer *buffer, Serializable data, serialize serializer);

status write_generic_data_array(Buffer *buffer, Serializable *data, size_t length, serialize serializer);



status read_char(Buffer *buffer, char *dest);

status read_char_array(Buffer *buffer, char *dest, size_t length);

status read_short(Buffer *buffer, short *dest);

status read_short_array(Buffer *buffer, short *dest, size_t length);

status read_int(Buffer *buffer, int *dest);

status read_int_array(Buffer *buffer, int *dest, size_t length);

status read_long(Buffer *buffer, long *dest);

status read_long_array(Buffer *buffer, long *dest, size_t length);

status read_float(Buffer *buffer, float *dest);

status read_float_array(Buffer *buffer, float *dest, size_t length);

status read_double(Buffer *buffer, double *dest);

status read_double_array(Buffer *buffer, double *dest, size_t length);

status read_data(Buffer *buffer, void *dest, size_t size);

status read_generic_data(Buffer *buffer, Serializable dest, deserialize deserializer);

status read_generic_data_array(Buffer *buffer, Serializable *dest, size_t length, deserialize deserializer);



#endif
