#ifndef BINBUFF_H
#define BINBUFF_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DEFAULT_BUFFER_SIZE 32

typedef struct  s_buffer Buffer;

typedef void* Serializable;

typedef enum e_status {FAILURE, SUCCESS, BUFFER_OVERFLOW, BUFFER_UNDERFLOW, ILLEGAL_WRITE, ILLEGAL_READ} status;

typedef enum e_mode {READ, WRITE} mode;

typedef enum e_type {DYNAMIC, STATIC} type;

typedef  status (*serialize)(Buffer *buffer, Serializable data);

typedef status(*deserialize)(Buffer *buffer, Serializable dest);

/*
Creates a new dynamic buffer with initial size - `size`.
Note that buffer size less then 1 is not allowed.
status - pointer to status that will hold the status of the function after its completion.

returns the created buffer, also the status { BUFFER_UNDERFLOW, FAILURE, SUCCESS }
*/
Buffer* create_dynamic_buffer(size_t initial_size, status *status);

/*
Creates a new buffer with default size and type - `type`.
status - pointer to status that will hold the status of the function after its completion.

returns the created buffer, also the status { BUFFER_UNDERFLOW, FAILURE, SUCCESS }.
*/
Buffer *create_buffer(type type, status *status);


/*
Creates a new static buffer of size - `size`.
Note that buffer size less then 1 is not allowed.
status - pointer to status that will hold the status of the function after its competition.

returns the created buffer, also the status { BUFFER_UNDERFLOW, FAILURE, SUCCESS }.
*/
Buffer *create_static_buffer(size_t size, status *status);

/*
Changes the mode of `buffer` to read.
Note that reading after calling this function will result in a read beginning from the start of the buffer.

returns status { FAILURE, SUCCESS } indicating the completion status of the function.
 */
status set_mode_read(Buffer *buffer);

/*
Changes the mode of `buffer` to write setting its type to be `type`.

returns status { FAILURE, SUCCESS } indicating the completion status of the function.
 */
status set_mode_write(Buffer* buffer, type type);

//Closes the buffer that `buffer` points to and releases all the memory associated with it.
void close_buffer(Buffer **buffer);

/*
 * the WRITE and WRITE_ARRAY macros use _Generic which was introduced in C11,
 * thus we need to make sure the C11 standard is implemented, this if statement
 * should be true only if the C11 standard and _Generic are implemented and used
 * by the compiler.
 */
#if __STDC__==1 && __STDC_VERSION__ >= 201112L

#define uchar unsigned char
#define schar signed char
#define ushort unsigned short
#define uint unsigned int
#define ulong unsigned long


//writes X into the Buffer BUF, X must be a primitive type.
#define WRITE(BUF,X) _Generic((BUF),\
                            Buffer*: _Generic((X),\
                            char:       write_char,\
                            uchar:      write_char,\
                            schar:      write_char,\
                            short:      write_short,\
                            ushort:     write_short,\
                            int:        write_int,\
                            uint:       write_int,\
                            long:       write_long,\
                            ulong:      write_long,\
                            float:      write_float,\
                            double:     write_double))(BUF,X)

//writes the array pointed to by X into the buffer BUF, only writes L elements of the array.
#define WRITE_ARRAY(BUF,X,L) _Generic((BUF),\
                                    Buffer*: _Generic((L),\
                                    size_t: _Generic((X),\
                                    char*:      write_char_array,\
                                    uchar*:     write_char_array,\
                                    schar*:     write_char_array,\
                                    short*:     write_short_array,\
                                    ushort*:    write_short_array,\
                                    int*:       write_int_array,\
                                    uint*:      write_int_array,\
                                    long*:      write_long_array,\
                                    ulong*:     write_long_array,\
                                    float*:     write_float_array,\
                                    double*:    write_double_array,\
                                    default:    write_data)))(BUF,X,L)
#endif

/*
Write a char into the buffer - `buffer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status write_char(Buffer *buffer, char data);

/*
Write char array of length - `length` into the buffer - `buffer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status write_char_array(Buffer *buffer, char *data, size_t length);

/*
Write a short into the buffer - `buffer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status write_short(Buffer *buffer, short data);

/*
Write short array of length - `length` into the buffer - `buffer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status write_short_array(Buffer *buffer, short *data, size_t length);

/*
Write an int into the buffer - `buffer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status write_int(Buffer *buffer, int data);

/*
Write int array of length - `length` into the buffer - `buffer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status write_int_array(Buffer *buffer, int *data, size_t length);

/*
Write a long into the buffer - `buffer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status write_long(Buffer *buffer, long data);

/*
Write long array of length - `length` into the buffer - `buffer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status write_long_array(Buffer *buffer, long *data, size_t length);

/*
Write a float into the buffer - `buffer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status write_float(Buffer *buffer, float data);

/*
Write float array of length - `length` into the buffer - `buffer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status write_float_array(Buffer *buffer, float *data, size_t length);

/*
Write a double into the buffer - `buffer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status write_double(Buffer *buffer, double data);

/*
Write double array of length - `length` into the buffer - `buffer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status write_double_array(Buffer *buffer, double *data, size_t length);

/*
Writes `size` bytes starting from where `data` points to into the buffer - `buffer`.
Use this function carefully and only when you know the data pointed to by `data`
is continues in memory.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
*/
status write_data(Buffer *buffer, void *data, size_t size);

/*
Writes `data` into the buffer using the provided serializer - `serializer`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
*/
status write_generic_data(Buffer *buffer, Serializable data, serialize serializer);

/*
Writes array of serializable data into the buffer using the provided serializer - `serializer`.
data - array of pointers to the serializable structure.
length - length of the array - `data`.

returns status { ILLEGAL_WRITE, FAILURE, SUCCESS } indicating the completion status of the function.
*/
status write_generic_data_array(Buffer *buffer, Serializable *data, size_t length, serialize serializer);


/*
Reads a char from the buffer - `buffer` into the char pointed to by `dest`.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status read_char(Buffer *buffer, char *dest);

/*
Reads a char array from the buffer - `buffer` into the char array `dest`.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status read_char_array(Buffer *buffer, char *dest, size_t length);

/*
Reads a short from the buffer - `buffer` into the short pointed to by `dest`.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status read_short(Buffer *buffer, short *dest);

/*
Reads a short array from the buffer - `buffer` into the short array `dest`.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status read_short_array(Buffer *buffer, short *dest, size_t length);

/*
Reads a long from the buffer - `buffer` into the long pointed to by `dest`.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status read_int(Buffer *buffer, int *dest);

/*
Reads a int array from the buffer - `buffer` into the int array `dest`.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status read_int_array(Buffer *buffer, int *dest, size_t length);

/*
Reads a char from the buffer - `buffer` into the char pointed to by `dest`.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status read_long(Buffer *buffer, long *dest);

/*
Reads a long array from the buffer - `buffer` into the long array `dest`.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status read_long_array(Buffer *buffer, long *dest, size_t length);

/*
Reads a float from the buffer - `buffer` into the float pointed to by `dest`.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status read_float(Buffer *buffer, float *dest);

/*
Reads a float array from the buffer - `buffer` into the float array `dest`.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status read_float_array(Buffer *buffer, float *dest, size_t length);

/*
Reads a double from the buffer - `buffer` into the double pointed to by `dest`.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status read_double(Buffer *buffer, double *dest);

/*
Reads a double array from the buffer - `buffer` into the double array `dest`.

returns status { ILLEGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
 */
status read_double_array(Buffer *buffer, double *dest, size_t length);

/*
Reads `size` bytes starting from the buffer into the "array" `dest`.

returns status { ILLEAGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
*/
status read_data(Buffer *buffer, void *dest, size_t size);

/*
Reads a single structure of the type that `dest` points to from the buffer into `dest` using the provided deserializer - `deserializer`.

returns status { ILLEAGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
*/
status read_generic_data(Buffer *buffer, Serializable dest, deserialize deserializer);

/*
Reads an array of structures of the type that `dest` contains from the buffer into `dest` using the provided deserializer - `deserializer`.
dest - array of pointers to the serializable structure to be read into.
length - length of the array - `dest` and the amount of structures that will be read.

returns status { ILLEAGAL_READ, FAILURE, SUCCESS } indicating the completion status of the function.
*/
status read_generic_data_array(Buffer *buffer, Serializable *dest, size_t length, deserialize deserializer);



#endif
