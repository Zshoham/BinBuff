using System;
using System.Collections.Generic;
using System.IO;
using Xunit;

namespace BinBuff.Test
{
    public class Test
    {
        [Fact]
        public void TestPrimitive()
        {
            byte ub = 255;
            sbyte sb = -1;
            char c = 'a';
            bool b = true;
            float f = 3.1f;
            double d = 3.2;
            decimal dc = 3.3m;
            short s = 1;
            int i = 2;
            long l = 3;
            ushort us = 1;
            uint ui = 2;
            ulong ul = 3;

            BinBuffer dbuf = new BinBuffer(BinBuffer.Type.DYNAMIC);
            BinBuffer sbuf = new BinBuffer(BinBuffer.Type.STATIC, 128);

            dbuf.Write(ub, sb, c, b, f, d, dc, s, i, l, us, ui, ul);
            sbuf.Write(ub, sb, c, b, f, d, dc, s, i, l, us, ui, ul);

            dbuf.SetRead();
            sbuf.SetRead();

            byte dnub, snub;
            sbyte dnsb, snsb;
            char dnc, snc;
            bool dnb, snb;
            short dns, sns;
            int dni, sni;
            long dnl, snl;
            float dnf, snf;
            double dnd, snd;
            decimal dndc, sndc;
            ushort dnus, snus;
            uint dnui, snui;
            ulong dnul, snul;

            dbuf.Read(out dnub); sbuf.Read(out snub);
            dbuf.Read(out dnsb); sbuf.Read(out snsb);
            dbuf.Read(out dnc); sbuf.Read(out snc);
            dbuf.Read(out dnb); sbuf.Read(out snb);
            dbuf.Read(out dnf); sbuf.Read(out snf);
            dbuf.Read(out dnd); sbuf.Read(out snd);
            dbuf.Read(out dndc); sbuf.Read(out sndc);
            dbuf.Read(out dns); sbuf.Read(out sns);
            dbuf.Read(out dni); sbuf.Read(out sni);
            dbuf.Read(out dnl); sbuf.Read(out snl);
            dbuf.Read(out dnus); sbuf.Read(out snus);
            dbuf.Read(out dnui); sbuf.Read(out snui);
            dbuf.Read(out dnul); sbuf.Read(out snul);

            Assert.Equal(ub, dnub);  
            Assert.Equal(ub, snub);

            Assert.Equal(sb ,dnsb);
            Assert.Equal(sb, snsb);

            Assert.Equal(c, dnc); 
            Assert.Equal(c, snc);
            
            Assert.Equal(b, dnb);
            Assert.Equal(b, snb);
            
            Assert.Equal(f, dnf);
            Assert.Equal(f, snf);
            
            Assert.Equal(d ,dnd);
            Assert.Equal(d, snd);
            
            Assert.Equal(dc, dndc);
            Assert.Equal(dc, sndc);
            
            Assert.Equal(s, dns); 
            Assert.Equal(s, sns);

            Assert.Equal(i ,dni);
            Assert.Equal(i ,sni);
            
            Assert.Equal(l ,dnl);
            Assert.Equal(l ,snl);

            Assert.Equal(us, dnus);
            Assert.Equal(us, snus);
            
            Assert.Equal(ui, dnui);
            Assert.Equal(ui, snui);
            
            Assert.Equal(ul, dnul);
            Assert.Equal(ul, snul);

        }

        [Fact]
        public void TestPrimitiveArray()
        {
            sbyte[] sb = { -3, -2, -1, 0 };
            char[] c = { 'a', 'b', 'c', 'd' };
            bool[] b = { true, true, false, true };
            short[] s = { 1, 2, 3, 4 };
            int[] i = { 2, 3, 4, 5 };
            long[] l = { 3, 4, 5, 6 };
            float[] f = { 3.1f, 3.2f, 3.3f, 3.4f };
            double[] d = { 3.2, 3.3, 3.4, 3.5 };

            BinBuffer dbuf = new BinBuffer(BinBuffer.Type.DYNAMIC);
            BinBuffer sbuf = new BinBuffer(BinBuffer.Type.STATIC, 256);

            dbuf.Write(sb, c, b, s, i, l, f, d);
            sbuf.Write(sb, c, b, s, i, l, f, d);

            dbuf.SetRead();
            sbuf.SetRead();


            sbyte[] dnsb = new sbyte[4], snsb = new sbyte[4];
            char[] dnc = new char[4], snc = new char[4];
            bool[] dnb = new bool[4], snb = new bool[4];
            short[] dns = new short[4], sns = new short[4];
            int[] dni = new int[4], sni = new int[4];
            long[] dnl = new long[4], snl = new long[4];
            float[] dnf = new float[4], snf = new float[4];
            double[] dnd = new double[4], snd = new double[4];

            dbuf.Read(dnsb, 4); sbuf.Read(snsb, 4);
            dbuf.Read(dnc, 4); sbuf.Read(snc, 4);
            dbuf.Read(dnb, 4); sbuf.Read(snb, 4);
            dbuf.Read(dns, 4); sbuf.Read(sns, 4);
            dbuf.Read(dni, 4); sbuf.Read(sni, 4);
            dbuf.Read(dnl, 4); sbuf.Read(snl, 4);
            dbuf.Read(dnf, 4); sbuf.Read(snf, 4);
            dbuf.Read(dnd, 4); sbuf.Read(snd, 4);

            for (int j = 0; j < 4; j++)
            {
                Assert.Equal(sb[j] ,dnsb[j]);
                Assert.Equal(sb[j] ,snsb[j]);
                
                Assert.Equal(c[j], dnc[j]);
                Assert.Equal(c[j] ,snc[j]);
                
                Assert.Equal(b[j] ,dnb[j]);
                Assert.Equal(b[j] ,snb[j]);
                
                Assert.Equal(s[j] ,dns[j]);
                Assert.Equal(s[j] ,sns[j]);
                
                Assert.Equal(i[j] ,dni[j]);
                Assert.Equal(i[j] ,sni[j]);
                
                Assert.Equal(l[j] ,dnl[j]);
                Assert.Equal(l[j] ,snl[j]);
                
                Assert.Equal(f[j] ,dnf[j]);
                Assert.Equal(f[j] ,snf[j]);
                
                Assert.Equal(d[j] ,dnd[j]);
                Assert.Equal(d[j] ,snd[j]);
            }
        }

        [Fact]
        public void TestSerializable()
        {
            Player player = new Player(5);
            Game game = new Game(720, 1280, 2);

            BinBuffer dbuf = new BinBuffer(BinBuffer.Type.DYNAMIC);
            BinBuffer sbuf = new BinBuffer(BinBuffer.Type.STATIC, 1024);

            dbuf.Write(player, game);
            sbuf.Write(player, game);

            Player dnp, snp;
            Game dng, sng;

            dbuf.SetRead();
            sbuf.SetRead();

            dbuf.Read(out dnp);
            dbuf.Read(out dng);
            sbuf.Read(out snp);
            sbuf.Read(out sng);

            Assert.Equal(player ,dnp); 
            Assert.Equal(player ,snp);
            
            Assert.Equal(game ,dng);
            Assert.Equal(game ,sng);
        }

        [Fact]
        public void TestSerializableArray()
        {
            Player[] players = { new Player(1), new Player(2) };
            Game[] games = { new Game(1280, 720, 1), new Game(1280, 720, 2) };

            BinBuffer dbuf = new BinBuffer(BinBuffer.Type.DYNAMIC);
            BinBuffer sbuf = new BinBuffer(BinBuffer.Type.STATIC, 1024);

            dbuf.Write(players, games);
            sbuf.Write(players, games);

            Player[] dnp = new Player[2], snp = new Player[2];
            Game[] dng = new Game[2], sng = new Game[2];

            dbuf.SetRead();
            sbuf.SetRead();

            dbuf.Read(dnp, 2);
            dbuf.Read(dng, 2);
            sbuf.Read(snp, 2);
            sbuf.Read(sng, 2);

            for (int i = 0; i < 2; i++)
            {
                Assert.Equal(players[i] ,dnp[i]);
                Assert.Equal(players[i] ,snp[i]);

                Assert.Equal(games[i] ,dng[i]);
                Assert.Equal(games[i] ,sng[i]);
            }
        }

        class PlayerComparator : IComparer<Player>
        {
            public int Compare(Player x, Player y)
            {
                return x.playerNum - y.playerNum;
            }
        }

        class PlayerEqualityComparator : IEqualityComparer<Player>
        {
            public bool Equals(Player x, Player y)
            {
                return x.playerNum == y.playerNum;
            }

            public int GetHashCode(Player obj)
            {
                return obj.playerNum;
            }
        }

        [Fact]
        public void TestCollections()
        {
            Player p1 = new Player(1), p2 = new Player(2);
            Game g1 = new Game(1280, 720, 1), g2 = new Game(1280, 720, 2);

            List<Player> list = new List<Player>(2) { p1, p2 };

            Dictionary<int, Game> dict = new Dictionary<int, Game> { { 1, g1 }, { 2, g2 } };

            HashSet<Player> hset = new HashSet<Player>(new PlayerEqualityComparator()) { p1, p2 };

            SortedList<int, Game> slist = new SortedList<int, Game> { { 1, g1 }, { 2, g2 } };

            SortedSet<Player> sset = new SortedSet<Player>(new PlayerComparator()) { p1, p2, };

            SortedDictionary<int, Game> sdict = new SortedDictionary<int, Game>() { { 1, g1 }, { 2, g2 } };

            Queue<Player> queue = new Queue<Player>();
            queue.Enqueue(p1);
            queue.Enqueue(p2);

            Stack<Game> stack = new Stack<Game>();
            stack.Push(g1);
            stack.Push(g2);

            BinBuffer buf = new BinBuffer(BinBuffer.Type.DYNAMIC);

            buf.Write(list, dict, hset, slist, sset, sdict, queue, stack);
            
            buf.SetRead();

            List<Player> nl = new List<Player>();
            Dictionary<int, Game> nd = new Dictionary<int, Game>();
            HashSet<Player> nhs = new HashSet<Player>(new PlayerEqualityComparator());
            SortedList<int, Game> nsl = new SortedList<int, Game>();
            SortedSet<Player> nss = new SortedSet<Player>(new PlayerComparator());
            SortedDictionary<int, Game> nsd = new SortedDictionary<int, Game>();
            Queue<Player> nq = new Queue<Player>();
            Stack<Game> ns = new Stack<Game>();

            buf.Read(nl, 2);
            buf.Read(nd, 2);
            buf.Read(nhs, 2);
            buf.Read(nsl, 2);
            buf.Read(nss, 2);
            buf.Read(nsd, 2);
            buf.Read(nq, 2);
            buf.Read(ns, 2);

            for (int i = 0; i < 2; i++)
            {
                Assert.Equal(list[i] ,nl[i]);
                Assert.Equal(dict[i + 1] ,nd[i + 1]);
                Assert.Equal(slist[i + 1] ,nsl[i + 1]);
                Assert.Equal(sdict[i + 1] ,nsd[i + 1]);
                Assert.Equal(queue.Dequeue() ,nq.Dequeue());
                Assert.Equal(stack.Pop() ,ns.Pop());
            }

            Assert.True(nhs.Contains(p1) && nhs.Contains(p2));
            Assert.True(nss.Contains(p1) && nss.Contains(p2));


            BinBuffer buff = new BinBuffer(BinBuffer.Type.DYNAMIC); // this create a buffer with initial size of 32.

            buff = new BinBuffer(BinBuffer.Type.STATIC, 128); // this creates a buffer with intial size of 128.

            buff.Write((byte)5); // you can use the write method
            buff.Write((byte)25, (byte)'a'); // or the shift left operator

            // for raw arrays the size must be specified thus << operator cannot be used.
            bool[] bools = { true, true, false, true };
            buff.Write(bools);

            // this array now contains all the data written to the buffer, 
            // and will look something like - [5, 25, 97, 1, 1, 0, 1]
            byte[] ser = buff.CloneSerialized();

            buff.SetRead();

            // here we read the first three numbers we wrote into an array.
            byte[] first = new byte[3];
            buff.Read(first, 3);

            // next we skip 2 bytes forward bringing us to the 2 index in the boolean array.
            buff += 2;
            bool b;
            buff.Read(out b); // meaning b is now false.



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
        }
    }
}
