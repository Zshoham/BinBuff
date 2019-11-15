using System;
using System.Collections.Generic;
using BinBuff;


namespace BinBuffTest
{
    static class Program
    {
        private static void Main(string[] args)
        {
            if (!TestPrimitive()) Console.WriteLine("failed primitive test.");
            else if (!TestPrimitiveArray()) Console.WriteLine("failed primitive array test.");
            else if (!TestISerializable()) Console.WriteLine("failed ISerializable test.");
            else if (!TestISerializableArray()) Console.WriteLine("failed ISerializable array test.");
            else if (!TestCollections()) Console.WriteLine("failed Collections test.");
            else Console.WriteLine("all tests passed.");


            Console.Read();
        }

        private static bool TestPrimitive()
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

            bool res = true;
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

            res = ub == dnub && ub == snub;
            res = res && sb == dnsb && sb == snsb;
            res = res && c == dnc && c == snc;
            res = res && b == dnb && b == snb;
            res = res && f == dnf && f == snf;
            res = res && d == dnd && d == snd;
            res = res && dc == dndc && dc == sndc;
            res = res && s == dns && s == sns;
            res = res && i == dni && i == sni;
            res = res && l == dnl && l == snl;
            res = res && us == dnus && us == snus;
            res = res && ui == dnui && ui == snui;
            res = res && ul == dnul && ul == snul;

            return res;
        }

        private static bool TestPrimitiveArray()
        {
            sbyte[] sb = { -3, -2, -1, 0 };
            char[] c = { 'a', 'b', 'c', 'd' };
            bool[] b = { true, true, false, true };
            short[] s = { 1, 2, 3, 4 };
            int[] i = { 2, 3, 4, 5 };
            long[] l = { 3, 4, 5, 6 };
            float[] f = { 3.1f, 3.2f, 3.3f, 3.4f };
            double[] d = { 3.2, 3.3, 3.4, 3.5 };

            bool res = true;
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
                res = res && sb[j] == dnsb[j] && sb[j] == snsb[j];
                res = res && c[j] == dnc[j] && c[j] == snc[j];
                res = res && b[j] == dnb[j] && b[j] == snb[j];
                res = res && s[j] == dns[j] && s[j] == sns[j];
                res = res && i[j] == dni[j] && i[j] == sni[j];
                res = res && l[j] == dnl[j] && l[j] == snl[j];
                res = res && f[j] == dnf[j] && f[j] == snf[j];
                res = res && d[j] == dnd[j] && d[j] == snd[j];
            }

            return res;
        }

        private static bool TestISerializable()
        {
            Player player = new Player(5);
            Game game = new Game(720, 1280, 2);

            bool res = true;
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

            res = res && player == dnp && player == snp;
            res = res && game == dng && game == sng;

            return res;
        }

        private static bool TestISerializableArray()
        {
            Player[] players = { new Player(1), new Player(2) };
            Game[] games = { new Game(1280, 720, 1), new Game(1280, 720, 2) };

            bool res = true;
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
                res = res && players[i] == dnp[i] && players[i] == snp[i];
                res = res && games[i] == dng[i] && games[i] == sng[i];
            }

            return res;
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

        private static bool TestCollections()
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

            bool res = true;
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
                res = res && list[i] == nl[i];
                res = res && dict[i + 1] == nd[i + 1];
                res = res && slist[i + 1] == nsl[i + 1];
                res = res && sdict[i + 1] == nsd[i + 1];
                res = res && queue.Dequeue() == nq.Dequeue();
                res = res && stack.Pop() == ns.Pop();
            }

            res = res && nhs.Contains(p1) && nhs.Contains(p2);
            res = res && nss.Contains(p1) && nss.Contains(p2);

            return res;
        }
    }
}

