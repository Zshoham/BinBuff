import com.binbuff.Buffer;
import com.binbuff.ISerializable;

import java.io.Serializable;
import java.util.Arrays;
import java.util.Objects;

public class Game  implements ISerializable, Comparable<Game>, Serializable {

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
    public void serialize(Buffer buffer) {
        buffer.write(width);
        buffer.write(height);
        buffer.write(player);
        buffer.write((short) enemies.length);
        buffer.write(enemies);
    }

    @Override
    public void deserialize(Buffer buffer) {
        this.width = buffer.readInt();
        this.height = buffer.readInt();
        buffer.read(player);
        short numEnemies = buffer.readShort();
        enemies = new Player[numEnemies];
        buffer.read(enemies);
    }

    @Override
    public boolean equals(Object o) {
        if(!(o instanceof Game)) return false;
        boolean res = true;
        Game other = (Game)o;
        res = this.width == other.width;
        res = res && this.height == other.height;
        res = res && this.player.equals(other.player);
        if (this.enemies.length != other.enemies.length) return false;
        for (int i = 0; i < this.enemies.length; i++) {
            res = res && this.enemies[i].equals(other.enemies[i]);
        }

        return res;
    }

    @Override
    public int compareTo(Game game) {
        return this.enemies.length - game.enemies.length;
    }


    @Override
    public int hashCode() {
        return Objects.hash(width, height, player, Arrays.hashCode(enemies));
    }
}
