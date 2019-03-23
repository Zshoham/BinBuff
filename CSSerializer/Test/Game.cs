using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Buffer = CSSerializer.Buffer;

namespace Test
{
	class Game : CSSerializer.ISerializable
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

		public static bool operator !=(Game g1, Game g2)
		{
			return !(g1 == g2);
		}

		public void Serialize(Buffer buffer)
		{
			buffer.Write(width, height);
			buffer.Write(player);
			buffer.Write((short)enemies.Length);
			buffer.Write(enemies);
		}

		public void Deserialize(Buffer buffer)
		{
			buffer.Read(out width);
			buffer.Read(out height);
			buffer.Read(out player);
			short numEnemies;
			buffer.Read(out numEnemies);
			enemies = new Player[numEnemies];
			buffer.Read(enemies, numEnemies);
		}
	}
}
