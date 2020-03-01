# BinBuff C++ Implementation

This is the C++ implementation of BinBuff.
This library is built using CMake and is cross-platform, to learn more about CMake and download it visit the   [official site](https://cmake.org/).

The latest release of the library is available [here](https://github.com/Zshoham/BinBuff/releases).

## Working With Sources 

To generate the build files for your System create a folder in the CppBinBuff directory called build
then inside the build folder run `cmake ..` and all the appropriate build files will be generated in the build folder.

* **on Windows** - if you are on windows a Visual Studio solution will be created with projects for the test and the library itself.
* **on Linux** - if you are on a Linux system a Makefile will be generated, running `make` in the build directory will create a libbinbuff.a file and inside the test directory there will be a binbuff_test executable.
* **other** - the project is c++ 11 compliant and very complicated - it only consists of 5 files - 4 .h files in the include folder and one source file in the src folder.

A `configure.py` script is provided to help configure the project correctly and later build and test it, it is recommended to use this file for the initial configuration at the very least as CMake can make some things a bit annoying to do by hand, also the CMake install command should be implemented in your desired build system, but it might not be possible to make the distinction between the install components (binbuff and googletest) by running the `configure.py` only the binbuff install will be performed to the bin folder.

# Getting Started

The Buffer class is the core of the library, to serialize data you must write it into the Buffer and later to deserialize it you must use the buffer and read the data from it.

There are a few more files in the library though, `serializable.h` defines the serializable class, inherit from this class to create new serializable data types.
The `binbuff_traits.h` file defines a lot of useful traits for the library like what is a serializable type, what counts as a container and so on, this is used internally mostly but might be useful in some other cases.
Lastly the `binbuff.h` file simply includes all you need to use the library so that all you need to do to start using the library is include it.

The Buffer constructor accepts TYPE and size (default is 32), there are two buffer types:

* Dynamic - will resize the buffer to fit incoming data.
* Static - buffer size will remain constant and trying to write too much data will cause an error.

## Basic Operation

Let's begin by creating a buffer.

```C++
Buffer buff(Buffer::DYNAMIC); // this create a buffer with initial size of 32.

buff = Buffer(Buffer::DYNAMIC, 128); // this creates a buffer with intial size of 128.
```

Now we have a buffer we can start writing to it, there are two main ways to write to the buffer we can use the write method or use the << operator (the << does not work for raw arrays since the array length must be provided).
Let's try and write a few things into the buffer.

```C++
buff.write((char)5); // you can use the write method
buff << (char)25 << 'a'; // or the shift left operator

// for raw arrays the size must be specified thus << operator cannot be used.
bool bools[4] = { true, true, false, true };
buff.write(bools, 4); 

// this vector now contains all the data written to the buffer, 
// and will look something like - [5, 25, 97, 1, 1, 0, 1]
std::vector<unsigned char> ser = buff.as_vec();
```

Until now the buffer was in WRITE mode, now that we finished writing to it we would do something with the serialized data and later return it to a buffer and then we would like to read it, to do that the buffer must be in READ mode, for this example lets just use the same buffer and convert it to read mode.

```C++
buff.set_mode_read();

// here we read the first three numbers we wrote into an array.
unsigned char first[3];
buff.read(first, 3);

// next we skip 2 bytes forward bringing us to the 2 index in the boolean array.
buff += 2;
bool b;
buff >> b; // meaning b is now false.

```

## Complex Data

All data structures implemented in the c++ standard library are supported, the library tries to provide an interface for serializing data as generically as possible, for example, to serialize a container all you need is to provide iterators to the first element and the last (very similar to many std algorithms).

That being said it is also possible to create serializable types by deriving the Serializable class or otherwise implementing a serializer for existing classes.
The tests can provide examples of both behaviors, let's see how it works.

```C++

class Player : public bbf::Serializable
{
	bool isAlive;
    double health;
	int strength;

public:

	int player_num;

    explicit Player(int player_num);

    // the empry constructor is very important, if there isn't one reading 
    // into from the buffer into this class might be impossible.
	Player() : isAlive(false), health(0), strength(0), player_num(0) {}

	...
    ...    
    ...

	void serialize(bbf::Buffer& buffer) const override;
	void deserialize(bbf::Buffer& buffer) override;
};

class Game : public bbf::Serializable
{
	int width, height;
	Player *player;
	int num_enemies;
	Player **enemies;

public:


    Game(int width, int height, int num_enemies);
	Game() : width(0), height(0), player(nullptr), num_enemies(0), enemies(nullptr)  {}
    ...
    ...
    ...

	void serialize(bbf::Buffer& buffer) const override;
	void deserialize(bbf::Buffer& buffer) override;
};
```

We have two classes, Player, relatively simple with only primitive data in it, and Game which contains an instance of player, and an array of players.
Both classes inherit Serializable and redefine serialize and deserialize methods.

Next, we need to implement the serialization and deserialization methods for both classes:

```C++

void Player::serialize(bbf::Buffer& buffer) const
{
	buffer << this->player_num << this->isAlive << this->health << this->strength;
}

void Player::deserialize(bbf::Buffer& buffer)
{
	buffer >> this->player_num >> this->isAlive >> this->health >> this->strength;
}

void Game::serialize(bbf::Buffer& buffer) const
{
	buffer << this->width << this->height;
	buffer << *this->player; // because player inherits Serializable its serialize method will be used here.
	buffer << this->num_enemies;
	buffer.write(this->enemies, this->num_enemies); // same here for all players.
}

void Game::deserialize(bbf::Buffer& buffer)
{
	buffer >> this->width >> this->height;
	this->player = new Player;
	buffer >> *this->player; // again the players deserialize method will be called here.
	buffer >> this->num_enemies;
	this->enemies = new Player*[this->num_enemies];
	buffer.read(this->enemies, this->num_enemies);  // and here.
}

```

Now we have everything we need to start working with these classes.

```C++
Player players[2];
players[0] = Player(1);
players[1] = Player(2);
Game games[2];
games[0] = Game(1280, 720, 1); // assume that the constructor creates players for the enemies.
games[1] = Game(1280, 720, 2);

Buffer buff(Buffer::DYNAMIC); 

// we can write the individual games/players.
buff << players[0];
buff << games[0];

buff.write(players, 2); // we can write objects as an array

// or we can use iterator syntax - `games` is a pointer to the first element or the
// begin iterator, and games + 2 is the end iterator.  
buff.write(games, games + 2);

buff.set_mode_read();

Player nPlayer;
Game nGame;

std::array<Player, 2> playerArr();
std::vector<Game> gameVec();

buff >> nPlayer;
buff >> nGame;
buff >> playerArr; // we can use the >> operator because std::array does not require size.

buff.read(gameVec, 2);

```