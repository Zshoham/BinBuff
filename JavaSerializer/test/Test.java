import java.io.*;
import java.util.*;

public class Test {

    private static final String ANSI_RESET = "\u001B[0m";
    private static final String ANSI_BLACK = "\u001B[30m";
    private static final String ANSI_RED = "\u001B[31m";
    private static final String ANSI_GREEN = "\u001B[32m";
    private static final String ANSI_YELLOW = "\u001B[33m";
    private static final String ANSI_BLUE = "\u001B[34m";
    private static final String ANSI_PURPLE = "\u001B[35m";
    private static final String ANSI_CYAN = "\u001B[36m";
    private static final String ANSI_WHITE = "\u001B[37m";

   private static final byte BENCHMARK_OPTION = 0;    // all
                                                //1;      // final
                                                //2;    //overall
    public static void main(String[] args) {
        if (!testPrimitive()) System.out.println(ANSI_RED + "failed primitive test." + ANSI_RESET);
        else if (!testPrimitiveArray()) System.out.println(ANSI_RED + "failed primitive array test." + ANSI_RESET);
        else if (!testSerializable()) System.out.println(ANSI_RED + "failed serializable test." + ANSI_RESET);
        else if (!testCollections()) System.out.println(ANSI_RED + "failed collections test." + ANSI_RESET);
        else if (!testMaps()) System.out.println(ANSI_RED + "failed maps test." + ANSI_RESET);
        else System.out.println(ANSI_GREEN + "all tests passed." + ANSI_RESET);

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
        Random r;
        LinkedHashMap<String, Double> memoryResults = new LinkedHashMap<>();
        LinkedHashMap<String, Double> diskResults = new LinkedHashMap<>();

        double res = 0;
        String description = "";
        double overallAvg = 0;
        int resCounter = 0;

        {
            System.out.println(ANSI_CYAN + "running in memory write benchmarks..." + ANSI_RESET);

            description = "int[] with 1000000 elements";
            int[] ints = new int[1000000];
            r = new Random();
            for (int i = 0; i < ints.length; i++) ints[i] = r.nextInt(Integer.MAX_VALUE);
            res = benchmarkWriteInMemory(ints, 100, description);
            memoryResults.put(description, res);


            description = "double[] with 1000000 elements";
            double[] doubles = new double[1000000];
            r = new Random();
            for (int i = 0; i < doubles.length; i++) doubles[i] = r.nextDouble();
            res = benchmarkWriteInMemory(doubles, 100, description);
            memoryResults.put(description, res);

            description = "int[] with 10000000 elements";
            int[] lInts = new int[10000000];
            r = new Random();
            for (int i = 0; i < lInts.length; i++) lInts[i] = r.nextInt(Integer.MAX_VALUE);
            res = benchmarkWriteInMemory(lInts, 100, description);
            memoryResults.put(description, res);

            description = "double[] with 10000000 elements";
            double[] ldoubles = new double[10000000];
            r = new Random();
            for (int i = 0; i < ldoubles.length; i++) ldoubles[i] = r.nextDouble();
            res = benchmarkWriteInMemory(ldoubles, 100, description);
            memoryResults.put(description, res);

            description = "Player[] with 1000 elements";
            Player[] players = new Player[1000];
            r = new Random();
            for (int i = 0; i < players.length; i++) players[i] = new Player(r.nextInt(Integer.MAX_VALUE));
            res = benchmarkWriteInMemory(players, 100, description);
            memoryResults.put(description, res);

            description = "Player[] with 100000 elements";
            Player[] lplayers = new Player[100000];
            r = new Random();
            for (int i = 0; i < lplayers.length; i++) lplayers[i] = new Player(r.nextInt(Integer.MAX_VALUE));
            res = benchmarkWriteInMemory(lplayers, 100, description);
            memoryResults.put(description, res);

            description = "ArrayList<Integer> with 1000 elements";
            ArrayList<Integer> Aintegers = new ArrayList<>();
            r = new Random();
            for (int i = 0; i < 1000; i++) Aintegers.add(r.nextInt(Integer.MAX_VALUE));
            res = benchmarkWriteInMemory(Aintegers, 100, description);
            memoryResults.put(description, res);

            description = "ArrayList<Player> with 1000 elements";
            ArrayList<Player> Aplayers = new ArrayList<>();
            r = new Random();
            for (int i = 0; i < 1000; i++) Aplayers.add(new Player(r.nextInt(Integer.MAX_VALUE)));
            res = benchmarkWriteInMemory(Aplayers, 100, description);
            memoryResults.put(description, res);

            description = "ArrayList<Integer> with 100000 elements";
            ArrayList<Integer> ALintegers = new ArrayList<>();
            r = new Random();
            for (int i = 0; i < 100000; i++) ALintegers.add(r.nextInt(Integer.MAX_VALUE));
            res = benchmarkWriteInMemory(ALintegers, 100, description);
            memoryResults.put(description, res);

            description = "ArrayList<Player> with 100000 elements";
            ArrayList<Player> ALplayers = new ArrayList<>();
            r = new Random();
            for (int i = 0; i < 100000; i++) ALplayers.add(new Player(r.nextInt(Integer.MAX_VALUE)));
            res = benchmarkWriteInMemory(ALplayers, 100, description);
            memoryResults.put(description, res);

            description = "HashMap<Player, Game> with 1000 mappings";
            HashMap<Player, Game> pgMap = new HashMap<>();
            r = new Random();
            for (int i = 0; i < 1000; i++) pgMap.put(new Player(r.nextInt(Integer.MAX_VALUE)), new Game(1280, 1080, r.nextInt(100)));
            res = benchmarkWriteInMemory(pgMap, 100, description);
            memoryResults.put(description, res);

            description = "HashMap<Player, Game> with 10000 mappings";
            HashMap<Player, Game> PGmap = new HashMap<>();
            r = new Random();
            for (int i = 0; i < 10000; i++) PGmap.put(new Player(r.nextInt(Integer.MAX_VALUE)), new Game(1280, 1080, r.nextInt(100)));
            res = benchmarkWriteInMemory(PGmap, 100, description);
            memoryResults.put(description, res);

            description = "HashMap<Integer, Player> with 100000 mappings";
            HashMap<Integer, Player> IPmap = new HashMap<>();
            r = new Random();
            for (int i = 0; i < 100000; i++) IPmap.put(r.nextInt(Integer.MAX_VALUE), new Player(r.nextInt(Integer.MAX_VALUE)));
            res = benchmarkWriteInMemory(IPmap, 100, description);
            memoryResults.put(description, res);

            System.out.println(ANSI_CYAN + "final results:" + ANSI_RESET);

            for (HashMap.Entry<String, Double> e : memoryResults.entrySet()) {
                if (BENCHMARK_OPTION <= 1) System.out.println(ANSI_YELLOW + e.getKey() + ANSI_RESET + " : " + ANSI_BLUE + e.getValue() + "%" + ANSI_RESET);
                overallAvg = (overallAvg * resCounter + e.getValue()) / (resCounter + 1);
            }

            if(BENCHMARK_OPTION <= 2) System.out.println(ANSI_RED + "overall average improvement - " + overallAvg + "%" + ANSI_RESET);
        }

        {
            System.out.println(ANSI_CYAN + "running on disk write benchmarks..." + ANSI_RESET);

            description = "int[] with 1000000 elements";
            int[] ints = new int[1000000];
            r = new Random();
            for (int i = 0; i < ints.length; i++) ints[i] = r.nextInt(Integer.MAX_VALUE);
            res = benchmarkWriteOnDisk(ints, 100, description);
            diskResults.put(description, res);

            description = "double[] with 1000000 elements";
            double[] doubles = new double[1000000];
            r = new Random();
            for (int i = 0; i < doubles.length; i++) doubles[i] = r.nextDouble();
            res = benchmarkWriteOnDisk(doubles, 100, description);
            diskResults.put(description, res);

            description = "int[] with 10000000 elements";
            int[] lInts = new int[10000000];
            r = new Random();
            for (int i = 0; i < lInts.length; i++) lInts[i] = r.nextInt(Integer.MAX_VALUE);
            res = benchmarkWriteOnDisk(lInts, 100, description);
            diskResults.put(description, res);

            description = "double[] with 10000000 elements";
            double[] ldoubles = new double[10000000];
            r = new Random();
            for (int i = 0; i < ldoubles.length; i++) ldoubles[i] = r.nextDouble();
            res = benchmarkWriteOnDisk(ldoubles, 100, description);
            diskResults.put(description, res);

            description = "Player[] with 1000 elements";
            Player[] players = new Player[1000];
            r = new Random();
            for (int i = 0; i < players.length; i++) players[i] = new Player(r.nextInt(Integer.MAX_VALUE));
            res = benchmarkWriteOnDisk(players, 100, description);
            diskResults.put(description, res);

            description = "Player[] with 100000 elements";
            Player[] lplayers = new Player[100000];
            r = new Random();
            for (int i = 0; i < lplayers.length; i++) lplayers[i] = new Player(r.nextInt(Integer.MAX_VALUE));
            res = benchmarkWriteOnDisk(lplayers, 100, description);
            diskResults.put(description, res);

            description = "ArrayList<Integer> with 1000 elements";
            ArrayList<Integer> Aintegers = new ArrayList<>();
            r = new Random();
            for (int i = 0; i < 1000; i++) Aintegers.add(r.nextInt(Integer.MAX_VALUE));
            res = benchmarkWriteOnDisk(Aintegers, 100, description);
            diskResults.put(description, res);

            description = "ArrayList<Player> with 1000 elements";
            ArrayList<Player> Aplayers = new ArrayList<>();
            r = new Random();
            for (int i = 0; i < 1000; i++) Aplayers.add(new Player(r.nextInt(Integer.MAX_VALUE)));
            res = benchmarkWriteOnDisk(Aplayers, 100, description);
            diskResults.put(description, res);

            description = "ArrayList<Integer> with 100000 elements";
            ArrayList<Integer> ALintegers = new ArrayList<>();
            r = new Random();
            for (int i = 0; i < 100000; i++) ALintegers.add(r.nextInt(Integer.MAX_VALUE));
            res = benchmarkWriteOnDisk(ALintegers, 100, description);
            diskResults.put(description, res);

            description = "ArrayList<Player> with 100000 elements";
            ArrayList<Player> ALplayers = new ArrayList<>();
            r = new Random();
            for (int i = 0; i < 100000; i++) ALplayers.add(new Player(r.nextInt(Integer.MAX_VALUE)));
            res = benchmarkWriteOnDisk(ALplayers, 100, description);
            diskResults.put(description, res);

            description = "HashMap<Player, Game> with 1000 mappings";
            HashMap<Player, Game> pgMap = new HashMap<>();
            r = new Random();
            for (int i = 0; i < 1000; i++) pgMap.put(new Player(r.nextInt(Integer.MAX_VALUE)), new Game(1280, 1080, r.nextInt(100)));
            res = benchmarkWriteOnDisk(pgMap, 100, description);
            diskResults.put(description, res);

            description = "HashMap<Player, Game> with 10000 mappings";
            HashMap<Player, Game> PGmap = new HashMap<>();
            r = new Random();
            for (int i = 0; i < 10000; i++) PGmap.put(new Player(r.nextInt(Integer.MAX_VALUE)), new Game(1280, 1080, r.nextInt(100)));
            res = benchmarkWriteOnDisk(PGmap, 100, description);
            diskResults.put(description, res);

            description = "HashMap<Integer, Player> with 100000 mappings";
            HashMap<Integer, Player> IPmap = new HashMap<>();
            r = new Random();
            for (int i = 0; i < 100000; i++) IPmap.put(r.nextInt(Integer.MAX_VALUE), new Player(r.nextInt(Integer.MAX_VALUE)));
            res = benchmarkWriteOnDisk(IPmap, 100, description);
            diskResults.put(description, res);

            System.out.println(ANSI_CYAN + "final results:" + ANSI_RESET);

            for (HashMap.Entry<String, Double> e : diskResults.entrySet()) {
                if(BENCHMARK_OPTION <= 1) System.out.println(ANSI_YELLOW + e.getKey() + ANSI_RESET + " : " + ANSI_BLUE + e.getValue() + "%" + ANSI_RESET);
                overallAvg = (overallAvg * resCounter + e.getValue()) / (resCounter + 1);
            }

            if(BENCHMARK_OPTION <= 2) System.out.println(ANSI_RED + "overall average improvement - " + overallAvg + "%" + ANSI_RESET);
        }


    }

    private static <T> double benchmarkWriteOnDisk(T data, int iterations, String description) {
        System.out.println("running Write benchmark on " + ANSI_YELLOW +  description + ANSI_RESET + " (" + iterations + " iterations)...");
        double minTimeOOS = Double.MAX_VALUE, maxTimeOOS = 0, avgTimeOOS = 0;
        double minTimeBUF = Double.MAX_VALUE, maxTimeBUF = 0, avgTimeBUF = 0;
        long avgSizeBUF = 0, avgSizeOOS = 0;

        for (int i = 0; i < iterations; i++) {
            try {
                File bufFile = new File("buf.test");
                if (!bufFile.createNewFile()) throw new Exception("cannot create new File");
                File oosFile = new File("oos.test");
                if (!oosFile.createNewFile()) throw new Exception("cannot create new File");
                Timer timer = new Timer();
                double res;

                FileOutputStream bufFOS = new FileOutputStream(bufFile);
                FileOutputStream oosFOS = new FileOutputStream(oosFile);
                Buffer buf = new Buffer(Buffer.TYPE.DYNAMIC);
                ByteArrayOutputStream byteStream = new ByteArrayOutputStream();
                ObjectOutputStream oos = new ObjectOutputStream(byteStream);
                byte[] oosBytes;

                long bufSize = 0, oosSize = 0;

                //write buffer
                timer.start();
                buf.write(data);
                bufFOS.write(buf.getBytes());
                bufFOS.flush();
                bufFOS.close();
                res = timer.stop();
                minTimeBUF = Math.min(res, minTimeBUF);
                maxTimeBUF = Math.max(res, maxTimeBUF);
                avgTimeBUF = (avgTimeBUF * i + res) / (i + 1);
                bufSize = bufFile.length();
                avgSizeBUF = (avgSizeBUF * i + bufSize) / (i + 1);



                // write object stream
                timer.start();
                oos.writeObject(data);
                oosBytes = byteStream.toByteArray();
                oosFOS.write(oosBytes);
                oosFOS.flush();
                oos.close();
                oosFOS.close();
                res = timer.stop();
                minTimeOOS = Math.min(res, minTimeOOS);
                maxTimeOOS = Math.max(res, maxTimeOOS);
                avgTimeOOS = (avgTimeOOS * i + res) / (i + 1);
                oosSize = oosFile.length();
                avgSizeOOS = (avgSizeOOS * i + oosSize) / (i + 1);



                if (!bufFile.delete())
                    throw new Exception("cannot delete new File");
                if (!oosFile.delete())
                    throw new Exception("cannot delete new File");
            }
            catch (Exception e) { e.printStackTrace(); }
        }

        double runTimeImprove = ((avgTimeOOS / avgTimeBUF) * 100) - 100;
        double sizeImprove = (((double)avgSizeOOS / (double)avgSizeBUF) * 100) - 100;

        if (BENCHMARK_OPTION <= 0) System.out.println("Buffer runtime - min =" + minTimeBUF + ", max = " + maxTimeBUF + ", average = " + avgTimeBUF + "\n" +
                "ObjectOutputStream runtime - min =" + minTimeOOS + ", max = " + maxTimeOOS + ", average = " + avgTimeOOS + "\n" +
                ANSI_BLUE + "average runtime improvement - " + runTimeImprove + "%" + "\n" + ANSI_RESET +
                "Buffer file size = " + avgSizeBUF + "B, " + (avgSizeBUF / 1024) + "Kb, " + (avgSizeBUF / (1024 * 1024)) + "Mb" + "\n" +
                "ObjectOutputStream file size = " + avgSizeOOS + "B, " + (avgSizeOOS / 1024) + "Kb, " + (avgSizeOOS / (1024 * 1024)) + "Mb" + "\n" +
                ANSI_BLUE + "average file size improvement - " + sizeImprove + "%" + ANSI_RESET);

        return 0.8 * runTimeImprove + 0.2 * sizeImprove;
    }

    private static <T> double benchmarkWriteInMemory(T data, int iterations, String description) {
        System.out.println("running Write benchmark on " + ANSI_YELLOW +  description + ANSI_RESET + " (" + iterations + " iterations)...");
        double minTimeOOS = Double.MAX_VALUE, maxTimeOOS = 0, avgTimeOOS = 0;
        double minTimeBUF = Double.MAX_VALUE, maxTimeBUF = 0, avgTimeBUF = 0;
        long avgSizeBUF = 0, avgSizeOOS = 0;

        for (int i = 0; i < iterations; i++) {
            try {
                byte[] bufBytes, oosBytes;
                Timer timer = new Timer();
                double res;

                Buffer buf = new Buffer(Buffer.TYPE.DYNAMIC);
                ByteArrayOutputStream bytesStream = new ByteArrayOutputStream();
                ObjectOutputStream oos = new ObjectOutputStream(bytesStream);

                long bufSize = 0, oosSize = 0;

                //write buffer
                timer.start();
                buf.write(data);
                bufBytes = buf.getBytes();
                res = timer.stop();

                //buffer statistics
                minTimeBUF = Math.min(res, minTimeBUF);
                maxTimeBUF = Math.max(res, maxTimeBUF);
                avgTimeBUF = (avgTimeBUF * i + res) / (i + 1);
                bufSize = bufBytes.length;
                avgSizeBUF = (avgSizeBUF * i + bufSize) / (i + 1);



                // write object stream
                timer.start();
                oos.writeObject(data);
                oos.close();
                oosBytes = bytesStream.toByteArray();
                res = timer.stop();

                //oos statistics
                minTimeOOS = Math.min(res, minTimeOOS);
                maxTimeOOS = Math.max(res, maxTimeOOS);
                avgTimeOOS = (avgTimeOOS * i + res) / (i + 1);
                oosSize = oosBytes.length;
                avgSizeOOS = (avgSizeOOS * i + oosSize) / (i + 1);

            }
            catch (Exception e) { e.printStackTrace(); }
        }

        double runTimeImprove = ((avgTimeOOS / avgTimeBUF) * 100) - 100;
        double sizeImprove = (((double)avgSizeOOS / (double)avgSizeBUF) * 100) - 100;

        if (BENCHMARK_OPTION <= 0) System.out.println("Buffer runtime - min =" + minTimeBUF + ", max = " + maxTimeBUF + ", average = " + avgTimeBUF + "\n" +
                "ObjectOutputStream runtime - min =" + minTimeOOS + ", max = " + maxTimeOOS + ", average = " + avgTimeOOS + "\n" +
                ANSI_BLUE + "average runtime improvement - " + runTimeImprove + "%" + "\n" + ANSI_RESET +
                "Buffer file size = " + avgSizeBUF + "B, " + (avgSizeBUF / 1024) + "Kb, " + (avgSizeBUF / (1024 * 1024)) + "Mb" + "\n" +
                "ObjectOutputStream file size = " + avgSizeOOS + "B, " + (avgSizeOOS / 1024) + "Kb, " + (avgSizeOOS / (1024 * 1024)) + "Mb" + "\n" +
                ANSI_BLUE + "average file size improvement - " + sizeImprove + "%" + ANSI_RESET);

        return 0.5 * runTimeImprove + 0.5 * sizeImprove;
    }


    private static class Timer {

        private long startTime = System.nanoTime();

        public void start() { startTime = System.nanoTime(); }

        public double stop() { return (double)(System.nanoTime() - startTime) / 1000000; }

    }

}