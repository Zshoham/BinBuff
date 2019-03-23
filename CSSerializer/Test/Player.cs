using System;
using System.CodeDom;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Buffer = CSSerializer.Buffer;

namespace Test
{
	class Player : CSSerializer.ISerializable
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

		public static bool operator ==(Player p1, Player p2)
		{
			return p1.playerNum == p2.playerNum && p1.isAlive == p2.isAlive && p1.health == p2.health &&
			       p1.strength == p2.strength;
		}

		public static bool operator !=(Player p1, Player p2)
		{
			return !(p1 == p2); }

		public void Serialize(Buffer buffer)
		{
			buffer.Write(playerNum, isAlive, health, strength);
		}

		public void Deserialize(Buffer buffer)
		{
			buffer.Read(out playerNum);
			buffer.Read(out isAlive);
			buffer.Read(out health);
			buffer.Read(out strength);
		}
	}
}
