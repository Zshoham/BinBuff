# BinBuff C# Implementation

This is the C# implementation of BinBuff, the full documentation is available [here](https://github.com/Zshoham/BinBuff/docs).
This library is built using netstandard2.0 and is cross platform, to learn more about netstandrad2.0 and where it can run see the [official documentation](https://docs.microsoft.com/en-us/dotnet/standard/net-standard) .

the latest release is available [here](https://github.com/Zshoham/BinBuff/releases).

## Working With Sources 

This is a solution based project if you are on Windows or macOS it can be opened easily with Visual Studio or Jetbrains Rider or any other c# IDE.

If you have dotnet installed on your machine but no other IDE you can use dotnet tools to build the library and run the test, in the BinBuff.Test directory run `dotnet test` - this will build the project and run the tests.

The Buffer class is the core of the library, to serialize data you must write it into the Buffer and later to deserialize it you must use the buffer and read the data from it.

# Getting Started

The Buffer class is the core of the library, to serialize data you must write it into the Buffer and later to deserialize it you must use the buffer and read the data from it.

The buffer can automaticllay serialize many standard types, but to make a custom type serializable you must inherit the ISerializable interface.

The Buffer constructor accepts TYPE and size (default is 32), there are two buffer types:

* Dynamic - will resize the buffer to fit incoming data.
* Static - buffer size will remain constant and trying to write too much data will cause an error.

## Basic Operation

Let's begin by creating a buffer.

```C#
 BinBuffer buff = new BinBuffer(BinBuffer.Type.DYNAMIC); // this create a buffer with initial size of 32.

buff = new BinBuffer(BinBuffer.Type.STATIC, 128); // this creates a buffer with intial size of 128.
```

Now we have a buffer we can start writing to it, we can call the write method with many arguments, this will write each argument to the buffer in the order they were passed to the method from left to right.

```C#
buff.Write((byte)5); // you can use the write method
buff.Write((byte)25, (byte)'a'); // or the shift left operator

// for raw arrays the size must be specified thus << operator cannot be used.
bool[] bools = { true, true, false, true };
buff.Write(bools); 

// this array now contains all the data written to the buffer, 
// and will look something like - [5, 25, 97, 1, 1, 0, 1]
byte[] ser = buff.CloneSerialized();
```

Until now the buffer was in WRITE mode, now that we finished writing to it we would do something with the serialized data and later return it to a buffer and then we would like to read it, to do that the buffer must be in READ mode, for this example lets just use the same buffer and convert it to read mode.

```C#
buff.SetRead();

// here we read the first three numbers we wrote into an array.
byte[] first = new byte[3];
buff.Read(first, 3);

// next we skip 2 bytes forward bringing us to the 2 index in the boolean array.
buff += 2;
bool b;
buff.Read(out b); // meaning b is now false.
```

## Complex Data

All data structures implemented in the c# collections library are supported, the library tries to provide an interface for serializing data as generically as possible, for example, any class that is a child of IEnumerable can be serialized as a container.

That being said it is also possible to create serializable types by deriving the Serializable class or otherwise implementing a serializer for existing classes.
The tests can provide examples of both behaviors, let's see how it works. (for more details take a look at the tests)

```C#
class Player : ISerializable
{
    public int playerNum;

    private bool isAlive;
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

    ...

    public void Serialize(BinBuffer binBuffer) { ... }

    public void Deserialize(BinBuffer binBuffer) { ... }
}

class Game : BinBuff.ISerializable
{

    private int width, height;
    private Player player;
    private Player[] enemies;

    public Game()
    {
        width = 0;
        height = 0;
        player = null;
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

    ...

    public void Serialize(BinBuffer binBuffer) { ... }

    public void Deserialize(BinBuffer binBuffer) { ... }
}

```

We have two classes, Player, relatively simple with only primitive data in it, and Game which contains an instance of player, and an array of players.
Both classes inherit Serializable and redefine serialize and deserialize methods.

Next, we need to implement the serialization and deserialization methods for both classes:

```C#
Player
{
    public void Serialize(BinBuffer binBuffer)
    {
        binBuffer.Write(playerNum, isAlive, health, strength);
    }

    public void Deserialize(BinBuffer binBuffer)
    {
        binBuffer.Read(out playerNum);
        binBuffer.Read(out isAlive);
        binBuffer.Read(out health);
        binBuffer.Read(out strength);
    }
}

Game 
{
    public void Serialize(BinBuffer binBuffer)
    {
        binBuffer.Write(width, height);
        binBuffer.Write(player);
        binBuffer.Write((short)enemies.Length);
        binBuffer.Write(enemies);
    }

    public void Deserialize(BinBuffer binBuffer)
    {
        binBuffer.Read(out width);
        binBuffer.Read(out height);
        binBuffer.Read(out player);
        short numEnemies;
        binBuffer.Read(out numEnemies);
        enemies = new Player[numEnemies];
        binBuffer.Read(enemies, numEnemies);
    }
}

```

Now we have everything we need to start working with these classes.


```C#
Player[] players = { new Player(1), new Player(2) };
Game[] games = { new Game(1280, 720, 1), new Game(1280, 720, 2) };

buff = new BinBuffer(BinBuffer.Type.DYNAMIC);

buff.Write(players[0]); // we can write the individual games/players.
buff.Write(players[1]);

buff.Write(players); // we can write objects as an array

// and we can do both in one line.
buff.Write(games[0], games[1], games);

buff.SetRead();

Player fPlayer;
Player sPlayer;
Game nGame;

Player[] playerArr = new Player[2];
List<Game> gameList = new List<Game>();

buff.Read(out fPlayer);
buff.Read(out sPlayer);
buff.Read(playerArr, 2);
buff.Read(gameList, 4);
```