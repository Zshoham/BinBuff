using BinBuff;

namespace BinBuffTest
{
    class Game : BinBuff.ISerializable
    {
        

        public override bool Equals(object obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (obj.GetType() != this.GetType()) return false;
            return Equals((Game) obj);
        }

        public override int GetHashCode()
        {
            unchecked
            {
                var hashCode = width;
                hashCode = (hashCode * 397) ^ height;
                hashCode = (hashCode * 397) ^ (player != null ? player.GetHashCode() : 0);
                hashCode = (hashCode * 397) ^ (enemies != null ? enemies.GetHashCode() : 0);
                return hashCode;
            }
        }

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

        public static bool operator ==(Game g1, Game g2)
        {
            bool areEqual = g1.width == g2.width && g1.height == g2.height;
            if (!areEqual) return false;
            for (int i = 0; i < g1.enemies.Length; i++)
            {
                areEqual = areEqual && g1.enemies[i] == g2.enemies[i];
            }

            return areEqual && g1.player == g2.player;
        }
        protected bool Equals(Game other)
        {
            return this == other;
        }

        public static bool operator !=(Game g1, Game g2)
        {
            return !(g1 == g2);
        }

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
}