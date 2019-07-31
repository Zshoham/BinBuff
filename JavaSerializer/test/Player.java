import java.util.Objects;

public class Player implements ISerializable, Comparable<Player> {

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

    @Override
    public boolean equals(Object o) {
        if(!(o instanceof Player)) return false;
        return this.playerNum == ((Player) o).playerNum
                && this.isAlive == ((Player) o).isAlive
                && this.health == ((Player) o).health
                && this.strength == ((Player) o).strength;
    }

    @Override
    public int compareTo(Player player) {
        return this.playerNum - player.playerNum;
    }

    @Override
    public int hashCode() {
        return Objects.hash(this.playerNum, this.isAlive, this.health, this.strength);
    }
}
