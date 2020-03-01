# BinBuff C Implementation

This is the C implementation of BinBuff.
This library is built using CMake and is cross-platform, to learn more about CMake and download it visit the [official site](https://cmake.org/).

the latest release is available [here](https://github.com/Zshoham/BinBuff/releases).

## Working With Sources 

To generate the build files for your System create a folder in the CppBinBuff directory called build
then inside the build folder run `cmake ..` and all the appropriate build files will be generated in the build folder.

* **on Windows** - if you are on windows a Visual Studio solution will be created with projects for the test and the library itself.
* **on Linux** - if you are on Linux system a Makefile will be generated, running `make` in the build directory will create a `libbinbuff.a` file and inside the test directory there will be a `binbuff_test` executable.
* **other** - the project is not very complex and only consists of three files - `binbuff.c` , `binbuff.h`, and the `test.c` file, there is no weird code or compiler intrinsics used, and all the code is C99 compliant except for optional macros that are only available if the current compiler defines \_\_STDC_VERSION\_\_ >= 201112L and \_\_STDC\_\_==1.

# Getting Started

The most important structure in the library is the buffer, to serialize the data you need to write the data into the buffer, later you can extract the serialized data from the buffer or deserialize it.

## Basic Operation

There are a few ways to create a buffer, we'll start by creating a dynamic buffer - meaning that it will grow in size to fit new incoming data.

```c
status status;
buffer *buff = create_dynamic_buffer(16, &status);
```

Writing to the buffer follows a similar pattern for all data write_type, for example, let's write a few things to the buffer we created.

```C
write_char(buff, 5); // writes the char 5 (usually 1 byte)
write_short(buff, 5); // writes the short 5 (usually 2 bytes)
//now lets try arrays
int ints[4] = { 2, 3, 4, 5 };
// lets assume an int is 32 bits (4 bytes), this array will be 3 * 4 = 12 bytes
write_int_array(buff, ints, 4); // writes the first 4 elements of the array (12 bytes)
void *buffer_data = get_serialized(buff, &status); 
// buffer_data points to the serialized data that we can now use however we want
```

Until now the buffer was in Write mode, now if we want to read from a buffe r to deserialize the data we need to move to Read mode or create a buffer in READ mode using `create_read_buffer` - this is useful if, for example, we chose to write the serialized data to a file and later want to read the file and deserialize it. 
In this example let's convert the same buffer into READ mode.

Notice that the buffer's pointer resets to 0 when changing to read mode - this is the reason why we started reading from the beginning.

```C
set_mode_read(buff); // converts the buffer to read mode.
// the buffers pointer now points to the first element (5).
char c;
read_char(buff, &c); // reads the first char into c (c now equals 5)
seek(buff, 6); // skips 6 bytes in the buffer, into the second int in the array
int i;
read_int(buff, &i); // i now equals 3
int arr[2];
read_int_array(buff, arr, 2); // reads two ints from the buffer into the array
// arr now contains two elements - {4, 5}
```

## Complex Data

The C language standard does not implement any data structures, and thus this implementation will not implicitly support any complex data structures, meaning that if you want to serialize your structs you must write a serializer function for them, a relatively simple example can be found in the test under "generic tests".

```C
typedef  struct s_player
{
	int health;
	int attack;
	int defense;

} Player;

typedef struct s_game
{
	int width;
	int height;
	Player *player;

} Game;
```
First, we define two structs, Player has just basic values describing its state. Game has more primitives describing width and height, but it also has a player. To serialize these structs we need to write a serializer as mentioned above.

```C
status serialize_player(buffer *buffer, serializable player)
{
	Player *p = (Player *)player;
	status status = SUCCESS;
	if ((status = write_int(buffer, p->health)) != SUCCESS) return status;
	if ((status = write_int(buffer, p->attack)) != SUCCESS) return status;
	if ((status = write_int(buffer, p->defense)) != SUCCESS) return status;

	return status;
}

status serialize_game(buffer *buffer, serializable game)
{
	Game *g = (Game *)game;
	status status = SUCCESS;
	if ((status = write_int(buffer, g->width)) != SUCCESS) return status;
	if ((status = write_int(buffer, g->height)) != SUCCESS) return status;

    // the game uses the already defined serializer for player.
	if ((status = write_generic_data(buffer, g->player, serialize_player)) != SUCCESS) return status;

	return status;
}
```
Secondly, we'll write a deserializer that will later retrieve the state of the structs from the buffer.

```C
status deserialize_player(buffer *buffer, serializable player)
{
	Player *p = (Player *)player;
	status status = SUCCESS;
	if ((status = read_int(buffer, &p->health)) != SUCCESS) return status;
	if ((status = read_int(buffer, &p->attack)) != SUCCESS) return status;
	if ((status = read_int(buffer, &p->defense)) != SUCCESS) return status;

	return status;
}

status deserialize_game(buffer *buffer, serializable game)
{
	Game *g = (Game *)game;
	status status = SUCCESS;
	if ((status = read_int(buffer, &g->width)) != SUCCESS) return status;
	if ((status = read_int(buffer, &g->height)) != SUCCESS) return status;
	g->player = (Player*)malloc(sizeof(Player));

    // again we use the already defiend deserializer for player.
	if ((status = read_generic_data(buffer, g->player, deserialize_player)) != SUCCESS) return status;

	return status;
}
```
Now we have everything we need to serialize and deserialize games and players.

```C

// lets first create a player and game to use
Player p;
p.health = 100;
p.attack = 10;
p.defense = 10;

Game g;
g.width = 1280;
g.height = 720;
g.player = &p;

// now initialize the buffer.
status status;
buffer *buff = create_dynamic_buffer(16, &status);

// now we can serialize the player and game however we please.
write_generic_data(buff, &p, serialize_player);
write_generic_data(buff, &g, serialize_game);

// now lets read the data into new instances of the structs;
Player newP;
Game newG;

set_mode_read(buff);

// we skip over the player and then two ints over the size of the game
seek(buff, sizeof(Player) + sizeof(int) * 2);
// and now we read the player that was written as part of the game
read_generic_data(buff, &newP, deserialize_player);

// then we skip back over the entire size of the game
seek(buff, -1 * (sizeof(Player) + sizeof(int) * 2));
// and read the whole game again.
read_generic_data(buff, &newG, deserialize_game);	

```

Please try and look at the `test.c` file for more examples, and to understand how to library works more in-depth please look at the sources, they are pretty well-documented :D.