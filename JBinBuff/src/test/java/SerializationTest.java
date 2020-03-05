import com.binbuff.Buffer;
import org.junit.Test;

import java.util.*;

public class SerializationTest {

    @Test
    public void testPrimitive() {
        byte b = 127;
        char c = '~';
        boolean bl = true;
        float f = 3.1f;
        double d = 3.1d;
        short s = 6874;
        int i = -568;
        long l = 77;

        Buffer dbuf = new Buffer(Buffer.TYPE.DYNAMIC);
        Buffer sbuf = new Buffer(Buffer.TYPE.STATIC, 64);

        dbuf.write(b, c, bl, f, d, s, i, l);
        sbuf.write(b, c, bl, f, d, s, i, l);

        byte dnb, snb;
        char dnc, snc;
        boolean dnbl, snbl;
        float dnf, snf;
        double dnd, snd;
        short dns, sns;
        int dni, sni;
        long dnl, snl;

        dbuf.setRead();
        sbuf.setRead();

        dnb = dbuf.readByte(); snb = sbuf.readByte();
        dnc = dbuf.readChar(); snc = sbuf.readChar();
        dnbl = dbuf.readBoolean(); snbl = sbuf.readBoolean();
        dnf = dbuf.readFloat(); snf = sbuf.readFloat();
        dnd = dbuf.readDouble(); snd = sbuf.readDouble();
        dns = dbuf.readShort(); sns = sbuf.readShort();
        dni = dbuf.readInt(); sni = sbuf.readInt();
        dnl = dbuf.readLong(); snl = sbuf.readLong();

        assert  b == dnb && b == snb;
        assert  c == dnc && c == snc;
        assert  dnbl && snbl;
        assert  f == dnf && f == snf;
        assert  d == dnd && d == snd;
        assert  s == dns && s == sns;
        assert  i == dni && i == sni;
        assert  l == dnl && l == snl;
    }

    @Test
    public void testPrimitiveArray() {
        byte[] b = {1, 2, 3, 4};
        boolean[] bl = {true, false, true, false};
        char[] c = {'a', 'b', 'c', 'd'};
        short[] s = {100, 200, 300, 400};
        int[] i = {100, 200, 300, 400};
        float[] f = {1.1f, 2.2f, 3.3f, 4.4f};
        long[] l = {100, 200, 300, 400};
        double[] d = {1.1d, 2.2d, 3.3d, 4.4d};

        Buffer dbuf = new Buffer(Buffer.TYPE.DYNAMIC);
        Buffer sbuf = new Buffer(Buffer.TYPE.STATIC, 256);

        dbuf.write(b, bl, c, s, i, f, l, d);
        sbuf.write(b, bl, c, s, i, f, l, d);

        dbuf.setRead();
        sbuf.setRead();

        byte[] dnb = new byte[4], snb = new byte[4];
        boolean[] dnbl = new boolean[4], snbl = new boolean[4];
        char[] dnc = new char[4], snc = new char[4];
        short[] dns = new short[4], sns = new short[4];
        int[] dni = new int[4], sni = new int[4];
        float[] dnf = new float[4], snf = new float[4];
        long[] dnl = new long[4], snl = new long[4];
        double[] dnd = new double[4], snd = new double[4];

        dbuf.read(dnb, dnbl, dnc, dns, dni, dnf, dnl, dnd);
        sbuf.read(snb, snbl, snc, sns, sni, snf, snl, snd);

        for (int j = 0; j < 4; j++)
        {
            assert b[j] == dnb[j] && b[j] == snb[j];
            assert bl[j] == dnbl[j] && bl[j] == snbl[j];
            assert c[j] == dnc[j] && c[j] == snc[j];
            assert s[j] == dns[j] && s[j] == sns[j];
            assert i[j] == dni[j] && i[j] == sni[j];
            assert f[j] == dnf[j] && f[j] == snf[j];
            assert l[j] == dnl[j] && l[j] == snl[j];
            assert d[j] == dnd[j] && d[j] == snd[j];
        }
    }

    @Test
    public void testSerializable() {
        Player player = new Player(1);
        Game game = new Game(1200, 720, 2);

        Buffer dbuf = new Buffer(Buffer.TYPE.DYNAMIC);
        Buffer sbuf = new Buffer(Buffer.TYPE.STATIC, 1024);

        dbuf.write(player, game);
        sbuf.write(player, game);

        Player dnp = new Player(), snp = new Player();
        Game dng = new Game(), sng = new Game();

        dbuf.setRead();
        sbuf.setRead();

        dbuf.read(dnp, dng);
        sbuf.read(snp, sng);

        assert  player.equals(dnp) && player.equals(snp);
        assert  game.equals(dng) && game.equals(sng);

    }

    @Test
    public void testCollections() {
        Player p1 = new Player(1), p2 = new Player(2);
        Game g1 = new Game(1280, 720, 1), g2 = new Game(1280, 720, 2);

        ArrayList<Player> arrlist = new ArrayList<>(), narrlist = new ArrayList<>();
        arrlist.add(p1);
        arrlist.add(p2);

        LinkedList<Game> llist = new LinkedList<>(), nllist = new LinkedList<>();
        llist.add(g1);
        llist.add(g2);

        Vector<Player> vector = new Vector<>(), nvector = new Vector<>();
        vector.add(p1);
        vector.add(p2);

        Stack<Game> stack = new Stack<>(), nstack = new Stack<>();
        stack.push(g1);
        stack.push(g2);

        PriorityQueue<Player> pqueue = new PriorityQueue<>(), npqueue = new PriorityQueue<>();
        pqueue.add(p1);
        pqueue.add(p2);

        ArrayDeque<Game> adeque = new ArrayDeque<>(), nadeque = new ArrayDeque<>();
        adeque.add(g1);
        adeque.add(g2);

        HashSet<Player> hset = new HashSet<>(), nhset = new HashSet<>();
        hset.add(p1);
        hset.add(p2);

        LinkedHashSet<Game> lhset = new LinkedHashSet<>(), nlhset = new LinkedHashSet<>();
        lhset.add(g1);
        lhset.add(g2);

        TreeSet<Player> tset = new TreeSet<>(), ntset = new TreeSet<>();
        tset.add(p1);
        tset.add(p2);

        Buffer dbuf = new Buffer(Buffer.TYPE.DYNAMIC);

        dbuf.write(arrlist, llist, vector, stack, pqueue, adeque, hset, lhset, tset);

        dbuf.setRead();

        dbuf.read(narrlist, Player.class, 2);
        dbuf.read(nllist, Game.class, 2);
        dbuf.read(nvector, Player.class, 2);
        dbuf.read(nstack, Game.class, 2);
        dbuf.read(npqueue, Player.class, 2);
        dbuf.read(nadeque, Game.class, 2);
        dbuf.read(nhset, Player.class, 2);
        dbuf.read(nlhset, Game.class, 2);
        dbuf.read(ntset, Player.class, 2);

        assert arrlist.equals(narrlist);
        assert llist.equals(nllist);
        assert vector.equals(nvector);
        assert stack.equals(nstack);
        for (int i = 0; i < 2; i++) {
            assert pqueue.poll().equals(npqueue.poll());
            assert adeque.poll().equals(nadeque.poll());
        }

        assert nhset.contains(p1) && nhset.contains(p2);
        assert lhset.equals(nlhset);
        assert ntset.contains(p1) && ntset.contains(p2);

    }

    @Test
    public void testMaps() {
        Player p1 = new Player(1), p2 = new Player(2);
        Game g1 = new Game(1280, 720, 1), g2 = new Game(1280, 720, 2);

        HashMap<Integer, Player> hmap = new HashMap<>(), nhmap = new HashMap<>();
        hmap.put(p1.hashCode(), p1);
        hmap.put(p2.hashCode(), p2);

        LinkedHashMap<Integer, Game> lhmap = new LinkedHashMap<>(), nlhmap = new LinkedHashMap<>();
        lhmap.put(g1.hashCode(), g1);
        lhmap.put(g2.hashCode(), g2);

        TreeMap<Player, Game> tmap = new TreeMap<>(), ntmap = new TreeMap<>();
        tmap.put(p1, g1);
        tmap.put(p2, g2);

        Buffer dbuf = new Buffer(Buffer.TYPE.DYNAMIC);

        dbuf.write(hmap, lhmap, tmap);

        dbuf.setRead();

        dbuf.read(nhmap, Integer.class, Player.class, 2);
        dbuf.read(nlhmap, Integer.class, Game.class, 2);
        dbuf.read(ntmap, Player.class, Game.class, 2);

        assert hmap.equals(nhmap);
        assert lhmap.equals(nlhmap);
        assert tmap.equals(ntmap);
    }
}