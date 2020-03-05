# BinBuff Java Implementation

The library is build using Gradle, to learn more about how to use Gradle visit the [official site](https://gradle.org/)

the latest release is available [here](https://github.com/Zshoham/BinBuff/releases).

## Working With Sources 

To build the library run `./gradlew build` in the JBinBuff directory.
you will find that Gradle created a build directory, inside the libs directory you will find two jars have been built. The library jar and a [benchmark jar](#Running The Benchmark).

The build command automatically runs the test and will fail and tell you if some of the tests failed, but if you wish to independently run the tests use - `./gradlew test`.
In addition, if you wish to run the benchmarks through Gradle you can use - `./gradle benchmark`,
this will use the 0 log level and run only the in-memory write benchmark, to use a different configuration for the benchmark use - `./gradle benchmark -Parg=[Log],[Benchmark]`.

*note: all the commands assume that you are using bash like CLI that recognizes the "./" instruction, if you are using the normal windows CMD you can simply omit the "./" from all the commands and it should work fine*

## Benchmarks Results

* **Speed:** The Results of the benchmarks in terms of speed are not very conclusive because they were only tested on a small number of machines and cannot be generalized, but after running the benchmarks on my PC and a couple of laptops it seems that BinBuff is at least as fast as the JDK Object Serialization in most cases.
* **Size:** In terms of size the results are more conclusive since the size of the serialized object does not change depending on the system, and as expected BinBuff is as good as the JDK Serialization when primitive data types are concerned, but when Objects and collections of Objects are serialized BinBuff is much more efficient in terms of size, this is fairly obvious when you see that java stores all kinds of metadata with every object it serializes, while BinBuff stores only the data.


## Running The Benchmark

In the java release of the library available [here](https://github.com/Zshoham/BinBuff/releases), you can find the library jar itself in addition to a test jar.
to run the test use the command 
`java -jar BinBuff-[version]-Benchmark.jar [Log] [Benchmarks]` 
this will run the test with all the benchmarks and all log messages, to use a different log level use
`java -jar BinBuff-[version]-Benchmark [0|1}2]`

* 0 - if you wish to see all the available information about all the benchmarks.
* 1 - if you wish to see only the final statistics for each benchmark.
* 2 - if you wish to see only the overall statistics from running all the benchmarks.

To specify which tests are run use -
`java -jar BinBuff-[version]-Benchmark.jar [0|1}2] [wm-wd-ram-rad-rcm-rcd-rmm-rmd]`

* wm - is write in memory
* wd - is write in disk
* ram - is read array in memory
* rad - is read array in disk
* rcm - is read collection in memory
* rcd - is read collection in disk
* rmm - is read map in memory
* rmd - is read map in disk

to activate one of the benchmarks set the appropriate value to 1, for example to activate all the benchmarks use 11111111, or to activate only the two write benchmarks use 11000000.


# Getting Started

The Buffer class is the core of the library, to serialize data you must write it into the Buffer and later to deserialize it you must use the buffer and read the data from it.

The buffer can automatically serialize many standard types, but to make a custom type serializable you must implement the ISerializable interface, or implement a Serializer/Deserializer for the type.

The Buffer constructor accepts TYPE and size (default is 32), there are two buffer types:

* Dynamic - will resize the buffer to fit incoming data.
* Static - buffer size will remain constant and trying to write too much data will cause an error.

## Basic Operation

Let's begin by creating a buffer.

```Java
Buffer buff = new Buffer(Buffer.TYPE.DYNAMIC); // this create a buffer with initial size of 32.

buff = new Buffer(Buffer.TYPE.STATIC, 128); // this creates a buffer with intial size of 128.
```

Now we have a buffer we can start writing to it, we can call the write method with many arguments, this will write each argument to the buffer in the order they were passed to the method from left to right.

```C#
buff.write((byte)5); // you can use the Write method with a single argument
buff.write((byte)25, (byte)'a'); // or with many

boolean[] bools = { true, true, false, true };
buff.write(bools); 

// this array now contains all the data written to the buffer, 
// and will look something like - [5, 25, 97, 1, 1, 0, 1]
byte[] ser = buff.cloneSerialized();
```

Until now the buffer was in WRITE mode, now that we finished writing to it we would do something with the serialized data and later return it to a buffer and from which we would like to read it, to do that the buffer must be in READ mode, for this example lets just use the same buffer and convert it to read mode.

```Java
buff.setRead();

// here we read the first three numbers we wrote, into an array.
byte[] first = new byte[3];
buff.read(first);

// next we skip 2 bytes forward bringing us to the 2 index in the boolean array.
buff.seekByte(2);
boolean b = buff.readBoolean(); // meaning b is now false.
```

## Complex Data

All data structures implemented in the java collections library are supported, the library tries to provide an interface for serializing data as generically as possible, for example, any class that is a child of Iterable can be serialized as a container.

That being said it is also possible to create serializable types by implementing the ISerializable interface or otherwise implementing a Serializer for existing classes.
The tests can provide examples of both behaviors, let's see how it works. (for more details take a look at the tests)

```Java
public class Player implements ISerializable {

    public int playerNum;

    private boolean isAlive;
    private float health;
    private int strength;

    public Player()
    {
        this.playerNum = 0;
        this.isAlive = false;
        this.health = 0;
        this.strength = 0;
    }

    public Player(int playerNum)
    {
        this.playerNum = playerNum;
        this.isAlive = true;
        this.health = 100 * playerNum;
        this.strength = 10 * playerNum;
    }

     @Override
    public void serialize(Buffer buffer) { ... }

    @Override
    public void deserialize(Buffer buffer) { ... }
}

public class Game  implements ISerializable {

    private int width, height;
    private Player player;
    private Player[] enemies;

    public Game()
    {
        width = 0;
        height = 0;
        player = new Player();
        enemies = null;
    }

    public Game(int width, int height, int numEnemies)
    {
        this.width = width;
        this.height = height;
        this.enemies = new Player[numEnemies];
        this.player = new Player(1);
        for (int i = 0; i < numEnemies; i++)
        {
            this.enemies[i] = new Player(i + 2);
        }
    }

    @Override
    public void serialize(Buffer buffer) { ... }

    @Override
    public void deserialize(Buffer buffer) { ... }

```

We have two classes, Player, relatively simple with only primitive data in it, and Game which contains an instance of player, and an array of players.
Both classes implement ISerializable and override serialize and deserialize methods.

Next, we need to implement the serialization and deserialization methods for both classes:

```Java

Player {

    @Override
    public void serialize(Buffer buffer) {
        buffer.write(playerNum, isAlive, health, strength);
    }

    @Override
    public void deserialize(Buffer buffer) {
        this.playerNum = buffer.readInt();
        this.isAlive = buffer.readBoolean();
        this.health = buffer.readFloat();
        this.strength = buffer.readInt();
    }
}

Game {

    @Override
    public void serialize(Buffer buffer) {
        // notice how we tell the buffer to serialize player here,
        // the buffer will use the serialize method defined for the player.
        buffer.write(width, height, player);
        buffer.write((short) enemies.length, enemies);
    }

    @Override
    public void deserialize(Buffer buffer) {
        this.width = buffer.readInt();
        this.height = buffer.readInt();
        // the same is true of the reading of player.
        buffer.read(player);
        short numEnemies = buffer.readShort();
        enemies = new Player[numEnemies];
        buffer.read(enemies);
    }
}

```

Now we have everything we need to start working with these classes.

```Java

buff = new Buffer(Buffer.TYPE.DYNAMIC);

buff.write(players[0]); // we can write the individual games/players.
buff.write(players[1]);

buff.write(players); // we can write objects as an array

// and we can do both in one line.
buff.write(games[0], games[1], games);

buff.setRead();

Player fPlayer = new Player();
Player sPlayer = new Player();
Game nGame = new Game();

Player[] playerArr = new Player[2];
List<Game> gameList = new ArrayList<>();

buff.read(fPlayer, sPlayer);
buff.read(playerArr);
// note you must provide the class of the generic type in order to read into a collection.
buff.read(gameList, Game.class, 4);

```

