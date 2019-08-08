import com.sun.jdi.IntegerType;

import java.io.*;
import java.lang.ref.Reference;
import java.util.*;
import java.util.concurrent.atomic.AtomicReference;

public class Test {
    public static void main(String[] args) {
        if (!testPrimitive()) System.out.println("failed primitive test.");
        else if (!testPrimitiveArray()) System.out.println("failed primitive array test.");
        else if (!testSerializable()) System.out.println("failed serializable test.");
        else if (!testCollections()) System.out.println("failed collections test.");
        else if (!testMaps()) System.out.println("failed maps test.");
        else System.out.println("all tests passed.");

        benchMark();
    }

    private static boolean testPrimitive() {
        byte b = 127;
        char c = '~';
        boolean bl = true;
        float f = 3.1f;
        double d = 3.1d;
        short s = 6874;
        int i = -568;
        long l = 77;

        boolean res = true;
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

        res = b == dnb && b == snb;
        res = res && c == dnc && c == snc;
        res = res && bl == dnbl && bl == snbl;
        res = res && f == dnf && f == snf;
        res = res && d == dnd && d == snd;
        res = res && s == dns && s == sns;
        res = res && i == dni && i == sni;
        res = res && l == dnl && l == snl;


        return res;
    }

    private static boolean testPrimitiveArray() {

        byte[] b = {1, 2, 3, 4};
        boolean[] bl = {true, false, true, false};
        char[] c = {'a', 'b', 'c', 'd'};
        short[] s = {100, 200, 300, 400};
        int[] i = {100, 200, 300, 400};
        float[] f = {1.1f, 2.2f, 3.3f, 4.4f};
        long[] l = {100, 200, 300, 400};
        double[] d = {1.1d, 2.2d, 3.3d, 4.4d};

        boolean res = true;
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
            res = res && b[j] == dnb[j] && b[j] == snb[j];
            res = res && bl[j] == dnbl[j] && bl[j] == snbl[j];
            res = res && c[j] == dnc[j] && c[j] == snc[j];
            res = res && s[j] == dns[j] && s[j] == sns[j];
            res = res && i[j] == dni[j] && i[j] == sni[j];
            res = res && f[j] == dnf[j] && f[j] == snf[j];
            res = res && l[j] == dnl[j] && l[j] == snl[j];
            res = res && d[j] == dnd[j] && d[j] == snd[j];
        }

        return res;
    }

    private static boolean testSerializable() {
        Player player = new Player(1);
        Game game = new Game(1200, 720, 2);

        boolean res = true;
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

        res = player.equals(dnp) && player.equals(snp);
        res = res && game.equals(dng) && game.equals(sng);

        return res;
    }

    private static boolean testCollections() {
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

        boolean res = true;
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

        res = arrlist.equals(narrlist);
        res = res && llist.equals(nllist);
        res = res && vector.equals(nvector);
        res = res && stack.equals(nstack);
        for (int i = 0; i < 2; i++) {
            res = res && pqueue.poll().equals(npqueue.poll());
            res = res && adeque.poll().equals(nadeque.poll());
        }

        res = res && nhset.contains(p1) && nhset.contains(p2);
        res = res && lhset.equals(nlhset);
        res = res && ntset.contains(p1) && ntset.contains(p2);

        return res;
    }

    private static boolean testMaps() {
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

        boolean res = true;
        Buffer dbuf = new Buffer(Buffer.TYPE.DYNAMIC);

        dbuf.write(hmap, lhmap, tmap);

        dbuf.setRead();

        dbuf.read(nhmap, Integer.class, Player.class, 2);
        dbuf.read(nlhmap, Integer.class, Game.class, 2);
        dbuf.read(ntmap, Player.class, Game.class, 2);

        res = hmap.equals(nhmap);
        res = res && lhmap.equals(nlhmap);
        res = res && tmap.equals(ntmap);

        return res;
    }

    private static void benchMark() {
        int length = 1000000;

        double[] p = new double[length];
        double[] g = new double[length];

        Random r = new Random(System.nanoTime());

        for (int i = 0; i < length; i++) {
            p[i] = (r.nextInt(Integer.MAX_VALUE));
            g[i] = (r.nextInt(Integer.MAX_VALUE));
        }

        FileOutputStream ofs = null;
        FileOutputStream bfs = null;

        Timer timer = new Timer();
        timer.start();
        try {

            ofs = new FileOutputStream("oos.test");
            bfs = new FileOutputStream("buf.test");

            ObjectOutputStream oos = new ObjectOutputStream(ofs);
            oos.writeObject(p);
            oos.writeObject(g); 
            System.out.println("oos: " + timer.stop());

            timer.start();
            Buffer buf = new Buffer(Buffer.TYPE.DYNAMIC);
            buf.write(p);
            buf.write(g);
            bfs.write(buf.getBytes());
            System.out.println("buf: " + timer.stop());

        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    private static class Timer {

        private long startTime = System.nanoTime();

        public void start() { startTime = System.nanoTime(); }

        public double stop() { return (double)(System.nanoTime() - startTime) / 1000000; }

    }

}