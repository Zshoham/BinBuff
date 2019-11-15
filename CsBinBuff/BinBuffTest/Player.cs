using BinBuff;

namespace BinBuffTest
{
    class Player : ISerializable
    {

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (obj.GetType() != this.GetType()) return false;
            return Equals((Player) obj);
        }

        public override int GetHashCode()
        {
            unchecked
            {
                var hashCode = playerNum;
                hashCode = (hashCode * 397) ^ isAlive.GetHashCode();
                hashCode = (hashCode * 397) ^ health.GetHashCode();
                hashCode = (hashCode * 397) ^ strength;
                return hashCode;
            }
        }

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

        public static bool operator ==(Player p1, Player p2)
        {
            return p1.playerNum == p2.playerNum && p1.isAlive == p2.isAlive && p1.health == p2.health &&
                   p1.strength == p2.strength;
        }

        protected bool Equals(Player other)
        {
            return this == other;
        }

        public static bool operator !=(Player p1, Player p2)
        {
            return !(p1 == p2);
        }

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
}