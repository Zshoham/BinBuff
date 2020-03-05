import com.binbuff.Buffer;

import java.io.*;
import java.util.*;

public class Benchmark {

    private static final String ANSI_RESET = "\u001B[0m";
    private static final String ANSI_RED = "\u001B[31m";
    private static final String ANSI_YELLOW = "\u001B[33m";
    private static final String ANSI_BLUE = "\u001B[34m";
    private static final String ANSI_CYAN = "\u001B[36m";

    // set to on of the values {0 - all, 1 - final, 2 - overall} for different levels of information.
    private static int BENCHMARK_OPTION = 0;    // all
                                        //1;    // final
                                        //2;    //overall

    private static final String USAGE = "usage: [Log] [Benchmarks]\n" +
            "Log Level: \n" +
            "\t0 - all\n" +
            "\t1 - final\n" +
            "\t2 - overall\n" +
            "Benchmarks:\n" +
            "\tformat is wm-wd-ram-rad-rcm-rcd-rmm-rmd\n" +
            "\twm - is write in memory\n" +
            "\twd - is write in disk\n" +
            "\tram - is read array in memory\n" +
            "\trad - is read array in disk\n" +
            "\trcm - is read collection in memory\n" +
            "\trcd - is read collection in disk\n" +
            "\trmm - is read map in memory\n" +
            "\trmd - is read map in disk\n" +
            "\tin order to activate on of the benchmarks set the appropriate value to 1, " +
            "for example to activate all the benchmarks use [1, 1, 1, 1, 1, 1, 1, 1]\n" +
            "\tdefault value for log is 2 and for benchmark it is 1000000 (only wm)";

    public static void main(String[] args) {
        try {
            run_benchmark(args);
        } catch (Exception e) {
            System.out.println(USAGE);
        }
    }

    private static void run_benchmark(String[] args) {
        String benchmark = "10000000";
        int option = 2;
        if (args.length > 3){
            System.out.println(USAGE);
            System.exit(0);
        }
        else if (args.length == 1)
            option = Integer.parseInt(args[0]);
        else if (args.length == 2) {
            option = Integer.parseInt(args[0]);
            benchmark = args[1];
        }

        if (option < 0 || option > 2) {
            System.out.println("[ERROR]: invalid log level " + option + "\n" + USAGE);
            System.exit(0);
        }
        BENCHMARK_OPTION = option;

        if(benchmark.length() != 8) {
            System.out.println("[ERROR]: invalid benchmarks " + benchmark + "\n" + USAGE);
            System.exit(0);
        }

        System.out.println(ANSI_RED + "note that all time is measured in milliseconds" + ANSI_RESET);

        //comment out the benchmarks that you do not want to run.
        if (benchmark.charAt(0) == '1') benchmarkWrite("write in memory", Benchmark::benchmarkWriteInMemory);
        if (benchmark.charAt(1) == '1') benchmarkWrite("write on disk", Benchmark::benchmarkWriteOnDisk);
        if (benchmark.charAt(2) == '1') benchmarkReadArray("read array in memory", Benchmark::benchmarkReadInMemoryArray);
        if (benchmark.charAt(3) == '1') benchmarkReadArray("read array on disk", Benchmark::benchmarkReadOnDiskArray);
        if (benchmark.charAt(4) == '1') benchmarkReadCollection("read collection in memory", Benchmark::benchmarkReadInMemoryCollection);
        if (benchmark.charAt(5) == '1') benchmarkReadCollection("read collection on disk", Benchmark::benchmarkReadOnDiskCollection);
        if (benchmark.charAt(6) == '1') benchmarkReadMap("read map in memory", Benchmark::benchmarkReadInMemoryMap);
        if (benchmark.charAt(7) == '1') benchmarkReadMap("read map on disk", Benchmark::benchmarkReadOnDiskMap);

        System.out.println(ANSI_RED + "average time saved - " + overallTimeSaved / benchmarkCounter);
    }

    private static double overallTimeSaved = 0;

    private static int benchmarkCounter = 0;


    private interface writeBenchmark {
        <T> double run(T data, String description);
    }

    private interface readArrayBenchmark {
        <T> double run(T data, String description, T read);
    }

    private interface readCollectionBenchmark {
        <T> double run(Collection<T> data, String description, Collection<T> read, Class<T> type);
    }

    private interface readMapBenchmark {
        <K, V> double run(Map<K, V> data, String description, Map<K, V> read, Class<K> kType, Class<V> vType);
    }

    private static void benchmarkWrite(String benchTitle, writeBenchmark benchmark) {
        Random r;
        LinkedHashMap<String, Double> results = new LinkedHashMap<>();

        double res = 0;
        String description = "";
        double overallAvg = 0;
        int resCounter = 0;

        System.out.println(ANSI_CYAN + "running " + benchTitle + " benchmark..." + ANSI_RESET);

        description = "int[] with 1000000 elements";
        int[] ints = new int[1000000];
        r = new Random();
        for (int i = 0; i < ints.length; i++) ints[i] = r.nextInt(Integer.MAX_VALUE);
        res = benchmark.run(ints, description);
        results.put(description, res);

        description = "double[] with 1000000 elements";
        double[] doubles = new double[1000000];
        r = new Random();
        for (int i = 0; i < doubles.length; i++) doubles[i] = r.nextDouble();
        res = benchmark.run(doubles, description);
        results.put(description, res);

        description = "int[] with 10000000 elements";
        int[] lInts = new int[10000000];
        r = new Random();
        for (int i = 0; i < lInts.length; i++) lInts[i] = r.nextInt(Integer.MAX_VALUE);
        res = benchmark.run(lInts, description);
        results.put(description, res);

        description = "double[] with 10000000 elements";
        double[] ldoubles = new double[10000000];
        r = new Random();
        for (int i = 0; i < ldoubles.length; i++) ldoubles[i] = r.nextDouble();
        res = benchmark.run(ldoubles, description);
        results.put(description, res);

        description = "Player[] with 1000 elements";
        Player[] players = new Player[1000];
        r = new Random();
        for (int i = 0; i < players.length; i++) players[i] = new Player(r.nextInt(Integer.MAX_VALUE));
        res = benchmark.run(players, description);
        results.put(description, res);

        description = "Player[] with 100000 elements";
        Player[] lplayers = new Player[100000];
        r = new Random();
        for (int i = 0; i < lplayers.length; i++) lplayers[i] = new Player(r.nextInt(Integer.MAX_VALUE));
        res = benchmark.run(lplayers, description);
        results.put(description, res);

        description = "ArrayList<Integer> with 1000 elements";
        ArrayList<Integer> Aintegers = new ArrayList<>();
        r = new Random();
        for (int i = 0; i < 1000; i++) Aintegers.add(r.nextInt(Integer.MAX_VALUE));
        res = benchmark.run(Aintegers, description);
        results.put(description, res);

        description = "ArrayList<Player> with 1000 elements";
        ArrayList<Player> Aplayers = new ArrayList<>();
        r = new Random();
        for (int i = 0; i < 1000; i++) Aplayers.add(new Player(r.nextInt(Integer.MAX_VALUE)));
        res = benchmark.run(Aplayers, description);
        results.put(description, res);

        description = "ArrayList<Integer> with 100000 elements";
        ArrayList<Integer> ALintegers = new ArrayList<>();
        r = new Random();
        for (int i = 0; i < 100000; i++) ALintegers.add(r.nextInt(Integer.MAX_VALUE));
        res = benchmark.run(ALintegers, description);
        results.put(description, res);

        description = "ArrayList<Player> with 100000 elements";
        ArrayList<Player> ALplayers = new ArrayList<>();
        r = new Random();
        for (int i = 0; i < 100000; i++) ALplayers.add(new Player(r.nextInt(Integer.MAX_VALUE)));
        res = benchmark.run(ALplayers, description);
        results.put(description, res);

        description = "HashMap<Player, Game> with 1000 mappings";
        HashMap<Player, Game> pgMap = new HashMap<>();
        r = new Random();
        for (int i = 0; i < 1000; i++) pgMap.put(new Player(r.nextInt(Integer.MAX_VALUE)), new Game(1280, 1080, r.nextInt(100)));
        res = benchmark.run(pgMap, description);
        results.put(description, res);

        description = "HashMap<Player, Game> with 10000 mappings";
        HashMap<Player, Game> PGmap = new HashMap<>();
        r = new Random();
        for (int i = 0; i < 10000; i++) PGmap.put(new Player(r.nextInt(Integer.MAX_VALUE)), new Game(1280, 1080, r.nextInt(100)));
        res = benchmark.run(PGmap, description);
        results.put(description, res);

        description = "HashMap<Integer, Player> with 100000 mappings";
        HashMap<Integer, Player> IPmap = new HashMap<>();
        r = new Random();
        for (int i = 0; i < 100000; i++) IPmap.put(r.nextInt(Integer.MAX_VALUE), new Player(r.nextInt(Integer.MAX_VALUE)));
        res = benchmark.run(IPmap, description);
        results.put(description, res);

        System.out.println(ANSI_CYAN + "final results:" + ANSI_RESET);

        for (HashMap.Entry<String, Double> e : results.entrySet()) {
            if(BENCHMARK_OPTION <= 1) System.out.println(ANSI_YELLOW + e.getKey() + ANSI_RESET + " : " + ANSI_BLUE + e.getValue() + "%" + ANSI_RESET);
            overallAvg = (overallAvg * resCounter + e.getValue()) / (resCounter + 1);
            resCounter++;
        }

        if(BENCHMARK_OPTION <= 2) System.out.println(ANSI_RED + "overall average improvement - " + overallAvg + "%\n" + ANSI_RESET);
    }

    private static void benchmarkReadArray(String benchTitle, readArrayBenchmark benchmark) {
        Random r;
        LinkedHashMap<String, Double> results = new LinkedHashMap<>();

        double res = 0;
        String description = "";
        double overallAvg = 0;
        int resCounter = 0;

        System.out.println(ANSI_CYAN + "running " + benchTitle + " benchmark..." + ANSI_RESET);

        description = "int[] with 1000000 elements";
        int[] ints = new int[1000000];
        int[] rInts = new int[1000000];
        r = new Random();
        for (int i = 0; i < ints.length; i++) ints[i] = r.nextInt(Integer.MAX_VALUE);
        res = benchmark.run(ints, description, rInts);
        results.put(description, res);

        description = "double[] with 1000000 elements";
        double[] doubles = new double[1000000];
        double[] rDoubles = new double[1000000];
        r = new Random();
        for (int i = 0; i < doubles.length; i++) doubles[i] = r.nextDouble();
        res = benchmark.run(doubles, description, rDoubles);
        results.put(description, res);

        description = "int[] with 10000000 elements";
        int[] lInts = new int[10000000];
        int[] rlInts = new int[10000000];
        r = new Random();
        for (int i = 0; i < lInts.length; i++) lInts[i] = r.nextInt(Integer.MAX_VALUE);
        res = benchmark.run(lInts, description, rlInts);
        results.put(description, res);

        description = "double[] with 10000000 elements";
        double[] ldoubles = new double[10000000];
        double[] rldoubles = new double[10000000];
        r = new Random();
        for (int i = 0; i < ldoubles.length; i++) ldoubles[i] = r.nextDouble();
        res = benchmark.run(ldoubles, description, rldoubles);
        results.put(description, res);

        description = "Player[] with 1000 elements";
        Player[] players = new Player[1000];
        Player[] rPlayers = new Player[1000];
        r = new Random();
        for (int i = 0; i < players.length; i++) players[i] = new Player(r.nextInt(Integer.MAX_VALUE));
        res = benchmark.run(players, description, rPlayers);
        results.put(description, res);

        description = "Player[] with 100000 elements";
        Player[] lplayers = new Player[100000];
        Player[] rlPlayers = new Player[100000];
        r = new Random();
        for (int i = 0; i < lplayers.length; i++) lplayers[i] = new Player(r.nextInt(Integer.MAX_VALUE));
        res = benchmark.run(lplayers, description, rlPlayers);
        results.put(description, res);

        System.out.println(ANSI_CYAN + "final results:" + ANSI_RESET);

        for (HashMap.Entry<String, Double> e : results.entrySet()) {
            if(BENCHMARK_OPTION <= 1) System.out.println(ANSI_YELLOW + e.getKey() + ANSI_RESET + " : " + ANSI_BLUE + e.getValue() + "%" + ANSI_RESET);
            overallAvg = (overallAvg * resCounter + e.getValue()) / (resCounter + 1);
            resCounter++;
        }

        if(BENCHMARK_OPTION <= 2) System.out.println(ANSI_RED + "overall average improvement - " + overallAvg + "%\n" + ANSI_RESET);
    }

    private static void benchmarkReadCollection(String benchTitle, readCollectionBenchmark benchmark) {
        Random r;
        LinkedHashMap<String, Double> results = new LinkedHashMap<>();

        double res = 0;
        String description = "";
        double overallAvg = 0;
        int resCounter = 0;

        System.out.println(ANSI_CYAN + "running " + benchTitle + " benchmark..." + ANSI_RESET);

        description = "ArrayList<Integer> with 1000 elements";
        ArrayList<Integer> Aintegers = new ArrayList<>();
        ArrayList<Integer> rAintegers = new ArrayList<>();
        r = new Random();
        for (int i = 0; i < 1000; i++) Aintegers.add(r.nextInt(Integer.MAX_VALUE));
        res = benchmark.run(Aintegers, description, rAintegers, Integer.class);
        results.put(description, res);

        description = "ArrayList<Player> with 1000 elements";
        ArrayList<Player> Aplayers = new ArrayList<>();
        ArrayList<Player> rAplayers = new ArrayList<>();
        r = new Random();
        for (int i = 0; i < 1000; i++) Aplayers.add(new Player(r.nextInt(Integer.MAX_VALUE)));
        res = benchmark.run(Aplayers, description, rAplayers, Player.class);
        results.put(description, res);

        description = "ArrayList<Integer> with 100000 elements";
        ArrayList<Integer> ALintegers = new ArrayList<>();
        ArrayList<Integer> rALintegers = new ArrayList<>();
        r = new Random();
        for (int i = 0; i < 100000; i++) ALintegers.add(r.nextInt(Integer.MAX_VALUE));
        res = benchmark.run(ALintegers, description, rALintegers, Integer.class);
        results.put(description, res);

        description = "ArrayList<Player> with 100000 elements";
        ArrayList<Player> ALplayers = new ArrayList<>();
        ArrayList<Player> rALplayers = new ArrayList<>();
        r = new Random();
        for (int i = 0; i < 100000; i++) ALplayers.add(new Player(r.nextInt(Integer.MAX_VALUE)));
        res = benchmark.run(ALplayers, description, rALplayers, Player.class);
        results.put(description, res);

        System.out.println(ANSI_CYAN + "final results:" + ANSI_RESET);

        for (HashMap.Entry<String, Double> e : results.entrySet()) {
            if(BENCHMARK_OPTION <= 1) System.out.println(ANSI_YELLOW + e.getKey() + ANSI_RESET + " : " + ANSI_BLUE + e.getValue() + "%" + ANSI_RESET);
            overallAvg = (overallAvg * resCounter + e.getValue()) / (resCounter + 1);
            resCounter++;
        }

        if(BENCHMARK_OPTION <= 2) System.out.println(ANSI_RED + "overall average improvement - " + overallAvg + "%\n" + ANSI_RESET);
    }

    private static void benchmarkReadMap(String benchTitle, readMapBenchmark benchmark) {
        Random r;
        LinkedHashMap<String, Double> results = new LinkedHashMap<>();

        double res = 0;
        String description = "";
        double overallAvg = 0;
        int resCounter = 0;

        System.out.println(ANSI_CYAN + "running " + benchTitle + " benchmark..." + ANSI_RESET);

        description = "HashMap<Player, Game> with 1000 mappings";
        HashMap<Player, Game> pgMap = new HashMap<>();
        HashMap<Player, Game> rpgMap = new HashMap<>();
        r = new Random();
        for (int i = 0; i < 1000; i++) pgMap.put(new Player(r.nextInt(Integer.MAX_VALUE)), new Game(1280, 1080, r.nextInt(100)));
        res = benchmark.run(pgMap, description, rpgMap, Player.class, Game.class);
        results.put(description, res);

        description = "HashMap<Player, Game> with 10000 mappings";
        HashMap<Player, Game> PGmap = new HashMap<>();
        HashMap<Player, Game> rPGmap = new HashMap<>();
        r = new Random();
        for (int i = 0; i < 10000; i++) PGmap.put(new Player(r.nextInt(Integer.MAX_VALUE)), new Game(1280, 1080, r.nextInt(100)));
        res = benchmark.run(PGmap, description, rpgMap, Player.class, Game.class);
        results.put(description, res);

        description = "HashMap<Integer, Player> with 100000 mappings";
        HashMap<Integer, Player> IPmap = new HashMap<>();
        HashMap<Integer, Player> rIPmap = new HashMap<>();
        r = new Random();
        for (int i = 0; i < 100000; i++) IPmap.put(r.nextInt(Integer.MAX_VALUE), new Player(r.nextInt(Integer.MAX_VALUE)));
        res = benchmark.run(IPmap, description, rIPmap, Integer.class, Player.class);
        results.put(description, res);

        System.out.println(ANSI_CYAN + "final results:" + ANSI_RESET);

        for (HashMap.Entry<String, Double> e : results.entrySet()) {
            if(BENCHMARK_OPTION <= 1) System.out.println(ANSI_YELLOW + e.getKey() + ANSI_RESET + " : " + ANSI_BLUE + e.getValue() + "%" + ANSI_RESET);
            overallAvg = (overallAvg * resCounter + e.getValue()) / (resCounter + 1);
            resCounter++;
        }

        if(BENCHMARK_OPTION <= 2) System.out.println(ANSI_RED + "overall average improvement - " + overallAvg + "%\n" + ANSI_RESET);

    }


    private static <T> double benchmarkWriteOnDisk(T data, String description) {
        int iterations = 100;
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
                bufFOS.write(buf.cloneSerialized());
                bufFOS.flush();
                bufFOS.close();
                res = timer.stop();

                //buffer statistics
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

                //oos statistics
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

        if (BENCHMARK_OPTION <= 0)
            System.out.println("Buffer runtime - min =" + minTimeBUF + ", max = " + maxTimeBUF + ", average = " + avgTimeBUF + "\n" +
                    "ObjectOutputStream runtime - min =" + minTimeOOS + ", max = " + maxTimeOOS + ", average = " + avgTimeOOS + "\n" +
                    ANSI_BLUE + "average runtime improvement - " + runTimeImprove + "%" + "\n" + ANSI_RESET +
                    "Buffer file size = " + avgSizeBUF + "B, " + (avgSizeBUF / 1024) + "Kb, " + (avgSizeBUF / (1024 * 1024)) + "Mb" + "\n" +
                    "ObjectOutputStream file size = " + avgSizeOOS + "B, " + (avgSizeOOS / 1024) + "Kb, " + (avgSizeOOS / (1024 * 1024)) + "Mb" + "\n" +
                    ANSI_BLUE + "average file size improvement - " + sizeImprove + "%\n" + ANSI_RESET +
                    ANSI_BLUE + "average time saved - " + (avgTimeOOS - avgTimeBUF) + ANSI_RESET);

        overallTimeSaved += avgTimeOOS - avgTimeBUF;
        benchmarkCounter++;

        return 0.8 * runTimeImprove + 0.2 * sizeImprove;
    }

    private static <T> double benchmarkWriteInMemory(T data, String description) {
        int iterations = 100;
        System.out.println("running Write benchmark on " + ANSI_YELLOW +  description + ANSI_RESET + " (" + iterations + " iterations)...");
        double minTimeOOS = Double.MAX_VALUE, maxTimeOOS = 0, avgTimeOOS = 0;
        double minTimeBUF = Double.MAX_VALUE, maxTimeBUF = 0, avgTimeBUF = 0;
        byte[] bufBytes, oosBytes;

        long avgSizeBUF = 0, avgSizeOOS = 0;

        for (int i = 0; i < iterations; i++) {
            try {
                Timer timer = new Timer();
                double res;

                Buffer buf = new Buffer(Buffer.TYPE.DYNAMIC);
                ByteArrayOutputStream bytesStream = new ByteArrayOutputStream();
                ObjectOutputStream oos = new ObjectOutputStream(bytesStream);

                long bufSize = 0, oosSize = 0;

                //write buffer
                timer.start();
                buf.write(data);
                bufBytes = buf.cloneSerialized();
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
                ANSI_BLUE + "average file size improvement - " + sizeImprove + "%\n" + ANSI_RESET +
                ANSI_BLUE + "average time saved - " + (avgTimeOOS - avgTimeBUF) + ANSI_RESET);

        overallTimeSaved += avgTimeOOS - avgTimeBUF;
        benchmarkCounter++;

        return 0.5 * runTimeImprove + 0.5 * sizeImprove;
    }

    private static <T> double benchmarkReadInMemoryArray(Object data, String description, Object read) {

        int iterations = 1000;
        System.out.println("running Read benchmark on " + ANSI_YELLOW +  description + ANSI_RESET + " (" + iterations + " iterations)...");
        double minTimeOIS = Double.MAX_VALUE, maxTimeOIS = 0, avgTimeOIS = 0;
        double minTimeBUF = Double.MAX_VALUE, maxTimeBUF = 0, avgTimeBUF = 0;

        Buffer buf = new Buffer(Buffer.TYPE.DYNAMIC);
        buf.write(data);
        buf.setRead();
        ByteArrayOutputStream byteOStream = new ByteArrayOutputStream();
        ObjectOutputStream oos = null;
        try {
            oos = new ObjectOutputStream(byteOStream);
            oos.writeObject(data);
        } catch (IOException e) { e.printStackTrace(); }

        for (int i = 0; i < iterations; i++) {
            try {
                Timer timer = new Timer();
                double res;
                buf.rewind();

                ByteArrayInputStream byteIStream = new ByteArrayInputStream(byteOStream.toByteArray());
                ObjectInputStream ois = new ObjectInputStream(byteIStream);

                //read buffer
                timer.start();
                buf.read(read);
                res = timer.stop();

                //buffer statistics
                minTimeBUF = Math.min(res, minTimeBUF);
                maxTimeBUF = Math.max(res, maxTimeBUF);
                avgTimeBUF = (avgTimeBUF * i + res) / (i + 1);

                // read object stream
                timer.start();
                read = ois.readObject();
                ois.close();
                res = timer.stop();

                //ois statistics
                minTimeOIS = Math.min(res, minTimeOIS);
                maxTimeOIS = Math.max(res, maxTimeOIS);
                avgTimeOIS = (avgTimeOIS * i + res) / (i + 1);

            }
            catch (Exception e) { e.printStackTrace(); }
        }

        double runTimeImprove = ((avgTimeOIS / avgTimeBUF) * 100) - 100;

        if (BENCHMARK_OPTION <= 0) System.out.println("Buffer runtime - min =" + minTimeBUF + ", max = " + maxTimeBUF + ", average = " + avgTimeBUF + "\n" +
                "ObjectOutputStream runtime - min =" + minTimeOIS + ", max = " + maxTimeOIS + ", average = " + avgTimeOIS + "\n" +
                ANSI_BLUE + "average runtime improvement - " + runTimeImprove + "%\n" + ANSI_RESET +
                ANSI_BLUE + "average time saved - " + (avgTimeOIS - avgTimeBUF) + ANSI_RESET);

        overallTimeSaved += avgTimeOIS - avgTimeBUF;
        benchmarkCounter++;

        return runTimeImprove;
    }

    @SuppressWarnings("unchecked")
    private static <T> double benchmarkReadInMemoryCollection(Collection<T> data, String description, Collection<T> read, Class<T> type) {

        int iterations = 1000;
        System.out.println("running Read benchmark on " + ANSI_YELLOW +  description + ANSI_RESET + " (" + iterations + " iterations)...");
        double minTimeOIS = Double.MAX_VALUE, maxTimeOIS = 0, avgTimeOIS = 0;
        double minTimeBUF = Double.MAX_VALUE, maxTimeBUF = 0, avgTimeBUF = 0;

        Buffer buf = new Buffer(Buffer.TYPE.DYNAMIC);
        buf.write(data);
        buf.setRead();
        ByteArrayOutputStream byteOStream = new ByteArrayOutputStream();
        ObjectOutputStream oos = null;
        try {
            oos = new ObjectOutputStream(byteOStream);
            oos.writeObject(data);
        } catch (IOException e) { e.printStackTrace(); }

        for (int i = 0; i < iterations; i++) {
            try {
                Timer timer = new Timer();
                double res;
                buf.rewind();

                ByteArrayInputStream byteIStream = new ByteArrayInputStream(byteOStream.toByteArray());
                ObjectInputStream ois = new ObjectInputStream(byteIStream);

                //read buffer
                timer.start();
                buf.read(read, type, data.size());
                res = timer.stop();

                //buffer statistics
                minTimeBUF = Math.min(res, minTimeBUF);
                maxTimeBUF = Math.max(res, maxTimeBUF);
                avgTimeBUF = (avgTimeBUF * i + res) / (i + 1);

                // read object stream
                timer.start();

                read = (Collection<T>) ois.readObject();
                ois.close();
                res = timer.stop();

                //ois statistics
                minTimeOIS = Math.min(res, minTimeOIS);
                maxTimeOIS = Math.max(res, maxTimeOIS);
                avgTimeOIS = (avgTimeOIS * i + res) / (i + 1);

            }
            catch (Exception e) { e.printStackTrace(); }
        }

        double runTimeImprove = ((avgTimeOIS / avgTimeBUF) * 100) - 100;

        if (BENCHMARK_OPTION <= 0) System.out.println("Buffer runtime - min =" + minTimeBUF + ", max = " + maxTimeBUF + ", average = " + avgTimeBUF + "\n" +
                "ObjectOutputStream runtime - min =" + minTimeOIS + ", max = " + maxTimeOIS + ", average = " + avgTimeOIS + "\n" +
                ANSI_BLUE + "average runtime improvement - " + runTimeImprove + "%\n" + ANSI_RESET +
                ANSI_BLUE + "average time saved - " + (avgTimeOIS - avgTimeBUF) + ANSI_RESET);

        overallTimeSaved += avgTimeOIS - avgTimeBUF;
        benchmarkCounter++;

        return runTimeImprove;
    }

    @SuppressWarnings("unchecked")
    private static <K, V> double benchmarkReadInMemoryMap(Map<K, V> data, String description, Map<K, V> read, Class<K> kType, Class<V> vType) {

        int iterations = 1000;
        System.out.println("running Read benchmark on " + ANSI_YELLOW +  description + ANSI_RESET + " (" + iterations + " iterations)...");
        double minTimeOIS = Double.MAX_VALUE, maxTimeOIS = 0, avgTimeOIS = 0;
        double minTimeBUF = Double.MAX_VALUE, maxTimeBUF = 0, avgTimeBUF = 0;

        Buffer buf = new Buffer(Buffer.TYPE.DYNAMIC);
        buf.write(data);
        buf.setRead();
        ByteArrayOutputStream byteOStream = new ByteArrayOutputStream();
        ObjectOutputStream oos = null;
        try {
            oos = new ObjectOutputStream(byteOStream);
            oos.writeObject(data);
        } catch (IOException e) { e.printStackTrace(); }

        for (int i = 0; i < iterations; i++) {
            try {
                Timer timer = new Timer();
                double res;
                buf.rewind();

                ByteArrayInputStream byteIStream = new ByteArrayInputStream(byteOStream.toByteArray());
                ObjectInputStream ois = new ObjectInputStream(byteIStream);

                //read buffer
                timer.start();
                buf.read(read, kType, vType, data.size());
                res = timer.stop();

                //buffer statistics
                minTimeBUF = Math.min(res, minTimeBUF);
                maxTimeBUF = Math.max(res, maxTimeBUF);
                avgTimeBUF = (avgTimeBUF * i + res) / (i + 1);



                // read object stream
                timer.start();
                read = (Map<K, V>) ois.readObject();
                ois.close();
                res = timer.stop();

                //ois statistics
                minTimeOIS = Math.min(res, minTimeOIS);
                maxTimeOIS = Math.max(res, maxTimeOIS);
                avgTimeOIS = (avgTimeOIS * i + res) / (i + 1);

            }
            catch (Exception e) { e.printStackTrace(); }
        }

        double runTimeImprove = ((avgTimeOIS / avgTimeBUF) * 100) - 100;

        if (BENCHMARK_OPTION <= 0) System.out.println("Buffer runtime - min =" + minTimeBUF + ", max = " + maxTimeBUF + ", average = " + avgTimeBUF + "\n" +
                "ObjectOutputStream runtime - min =" + minTimeOIS + ", max = " + maxTimeOIS + ", average = " + avgTimeOIS + "\n" +
                ANSI_BLUE + "average runtime improvement - " + runTimeImprove + "%\n" + ANSI_RESET +
                ANSI_BLUE + "average time saved - " + (avgTimeOIS - avgTimeBUF) + ANSI_RESET);

        overallTimeSaved += avgTimeOIS - avgTimeBUF;
        benchmarkCounter++;

        return runTimeImprove;
    }

    private static <T> double benchmarkReadOnDiskArray(Object data, String description, Object read) {

        int iterations = 1000;
        System.out.println("running Read benchmark on " + ANSI_YELLOW +  description + ANSI_RESET + " (" + iterations + " iterations)...");
        double minTimeOIS = Double.MAX_VALUE, maxTimeOIS = 0, avgTimeOIS = 0;
        double minTimeBUF = Double.MAX_VALUE, maxTimeBUF = 0, avgTimeBUF = 0;

        Buffer buf = new Buffer(Buffer.TYPE.DYNAMIC);
        File bufFile = null, oosFile = null;

        try {
            bufFile = new File("buf.test");
            if (!bufFile.createNewFile()) throw new Exception("cannot create new File");
            oosFile = new File("oos.test");
            if (!oosFile.createNewFile()) throw new Exception("cannot create new File");

            FileOutputStream bufFOS = new FileOutputStream(bufFile);
            FileOutputStream oosFOS = new FileOutputStream(oosFile);

            buf = new Buffer(Buffer.TYPE.DYNAMIC);
            buf.write(data);
            bufFOS.write(buf.cloneSerialized());
            bufFOS.flush();
            bufFOS.close();
            buf.setRead();

            ObjectOutputStream oos = new ObjectOutputStream(oosFOS);
            oos.writeObject(data);
            oos.flush();
            oos.close();

            bufFOS.close();
            oosFOS.close();

        } catch (Exception e) { e.printStackTrace(); }


        for (int i = 0; i < iterations; i++) {
            try {
                Timer timer = new Timer();
                double res;

                FileInputStream bufFIS = new FileInputStream(bufFile);
                FileInputStream oisFIS = new FileInputStream(oosFile);

                //read buffer
                timer.start();
                buf = new Buffer(bufFIS);
                buf.read(read);
                res = timer.stop();

                //buffer statistics
                minTimeBUF = Math.min(res, minTimeBUF);
                maxTimeBUF = Math.max(res, maxTimeBUF);
                avgTimeBUF = (avgTimeBUF * i + res) / (i + 1);


                // read object stream
                timer.start();
                byte[] fileBytes = new byte[oisFIS.available()];
                oisFIS.read(fileBytes);
                ByteArrayInputStream byteStream = new ByteArrayInputStream(fileBytes);
                ObjectInputStream ois = new ObjectInputStream(byteStream);
                read = ois.readObject();
                ois.close();
                res = timer.stop();

                //ois statistics
                minTimeOIS = Math.min(res, minTimeOIS);
                maxTimeOIS = Math.max(res, maxTimeOIS);
                avgTimeOIS = (avgTimeOIS * i + res) / (i + 1);

                bufFIS.close();
                oisFIS.close();
            }
            catch (Exception e) { e.printStackTrace(); }
        }

        if (!bufFile.delete()) System.out.println("error deleting the files.");
        if (!oosFile.delete()) System.out.println("error deleting the files.");

        double runTimeImprove = ((avgTimeOIS / avgTimeBUF) * 100) - 100;

        if (BENCHMARK_OPTION <= 0) System.out.println("Buffer runtime - min =" + minTimeBUF + ", max = " + maxTimeBUF + ", average = " + avgTimeBUF + "\n" +
                "ObjectOutputStream runtime - min =" + minTimeOIS + ", max = " + maxTimeOIS + ", average = " + avgTimeOIS + "\n" +
                ANSI_BLUE + "average runtime improvement - " + runTimeImprove + "%\n" + ANSI_RESET +
                ANSI_BLUE + "average time saved - " + (avgTimeOIS - avgTimeBUF) + ANSI_RESET);

        overallTimeSaved += avgTimeOIS - avgTimeBUF;
        benchmarkCounter++;

        return runTimeImprove;
    }

    @SuppressWarnings("unchecked")
    private static <T> double benchmarkReadOnDiskCollection(Collection<T> data, String description, Collection<T> read, Class<T> type) {

        int iterations = 1000;
        System.out.println("running Read benchmark on " + ANSI_YELLOW +  description + ANSI_RESET + " (" + iterations + " iterations)...");
        double minTimeOIS = Double.MAX_VALUE, maxTimeOIS = 0, avgTimeOIS = 0;
        double minTimeBUF = Double.MAX_VALUE, maxTimeBUF = 0, avgTimeBUF = 0;

        Buffer buf;
        File bufFile = null, oosFile = null;

        try {
            bufFile = new File("buf.test");
            if (!bufFile.createNewFile()) throw new Exception("cannot create new File");
            oosFile = new File("oos.test");
            if (!oosFile.createNewFile()) throw new Exception("cannot create new File");

            FileOutputStream bufFOS = new FileOutputStream(bufFile);
            FileOutputStream oosFOS = new FileOutputStream(oosFile);

            buf = new Buffer(Buffer.TYPE.DYNAMIC);
            buf.write(data);
            bufFOS.write(buf.cloneSerialized());
            bufFOS.flush();
            bufFOS.close();
            buf.setRead();

            ObjectOutputStream oos = new ObjectOutputStream(oosFOS);
            oos.writeObject(data);
            oos.flush();
            oos.close();

            bufFOS.close();
            oosFOS.close();

        } catch (Exception e) { e.printStackTrace(); }

        for (int i = 0; i < iterations; i++) {
            try {
                Timer timer = new Timer();
                double res;

                FileInputStream bufFIS = new FileInputStream(bufFile);
                FileInputStream oisFIS = new FileInputStream(oosFile);

                //read buffer
                timer.start();
                buf = new Buffer(bufFIS);
                buf.read(read, type, data.size());
                res = timer.stop();

                //buffer statistics
                minTimeBUF = Math.min(res, minTimeBUF);
                maxTimeBUF = Math.max(res, maxTimeBUF);
                avgTimeBUF = (avgTimeBUF * i + res) / (i + 1);

                // read object stream
                timer.start();
                byte[] fileBytes = new byte[oisFIS.available()];
                oisFIS.read(fileBytes);
                ByteArrayInputStream byteStream = new ByteArrayInputStream(fileBytes);
                ObjectInputStream ois = new ObjectInputStream(byteStream);
                read = (Collection<T>) ois.readObject();
                ois.close();
                res = timer.stop();

                //ois statistics
                minTimeOIS = Math.min(res, minTimeOIS);
                maxTimeOIS = Math.max(res, maxTimeOIS);
                avgTimeOIS = (avgTimeOIS * i + res) / (i + 1);

                bufFIS.close();
                oisFIS.close();
            }
            catch (Exception e) { e.printStackTrace(); }
        }

        if (!bufFile.delete()) System.out.println("error deleting the files.");
        if (!oosFile.delete()) System.out.println("error deleting the files.");

        double runTimeImprove = ((avgTimeOIS / avgTimeBUF) * 100) - 100;

        if (BENCHMARK_OPTION <= 0) System.out.println("Buffer runtime - min =" + minTimeBUF + ", max = " + maxTimeBUF + ", average = " + avgTimeBUF + "\n" +
                "ObjectOutputStream runtime - min =" + minTimeOIS + ", max = " + maxTimeOIS + ", average = " + avgTimeOIS + "\n" +
                ANSI_BLUE + "average runtime improvement - " + runTimeImprove + "%\n" + ANSI_RESET +
                ANSI_BLUE + "average time saved - " + (avgTimeOIS - avgTimeBUF) + ANSI_RESET);

        overallTimeSaved += avgTimeOIS - avgTimeBUF;
        benchmarkCounter++;

        return runTimeImprove;
    }

    @SuppressWarnings("unchecked")
    private static <K, V> double benchmarkReadOnDiskMap(Map<K, V> data, String description, Map<K, V> read, Class<K> kType, Class<V> vType) {
        int iterations = 1000;
        System.out.println("running Read benchmark on " + ANSI_YELLOW +  description + ANSI_RESET + " (" + iterations + " iterations)...");
        double minTimeOIS = Double.MAX_VALUE, maxTimeOIS = 0, avgTimeOIS = 0;
        double minTimeBUF = Double.MAX_VALUE, maxTimeBUF = 0, avgTimeBUF = 0;

        Buffer buf;
        File bufFile = null, oosFile = null;

        try {
            bufFile = new File("buf.test");
            if (!bufFile.createNewFile()) throw new Exception("cannot create new File");
            oosFile = new File("oos.test");
            if (!oosFile.createNewFile()) throw new Exception("cannot create new File");

            FileOutputStream bufFOS = new FileOutputStream(bufFile);
            FileOutputStream oosFOS = new FileOutputStream(oosFile);

            buf = new Buffer(Buffer.TYPE.DYNAMIC);
            buf.write(data);
            bufFOS.write(buf.cloneSerialized());
            bufFOS.flush();
            bufFOS.close();
            buf.setRead();

            ObjectOutputStream oos = new ObjectOutputStream(oosFOS);
            oos.writeObject(data);
            oos.flush();
            oos.close();

            bufFOS.close();
            oosFOS.close();

        } catch (Exception e) { e.printStackTrace(); }


        for (int i = 0; i < iterations; i++) {
            try {
                Timer timer = new Timer();
                double res;

                FileInputStream bufFIS = new FileInputStream(bufFile);
                FileInputStream oisFIS = new FileInputStream(oosFile);

                //read buffer
                timer.start();
                buf = new Buffer(bufFIS);
                buf.read(read, kType, vType, data.size());
                res = timer.stop();

                //buffer statistics
                minTimeBUF = Math.min(res, minTimeBUF);
                maxTimeBUF = Math.max(res, maxTimeBUF);
                avgTimeBUF = (avgTimeBUF * i + res) / (i + 1);



                // read object stream
                timer.start();
                byte[] fileBytes = new byte[oisFIS.available()];
                oisFIS.read(fileBytes);
                ByteArrayInputStream byteStream = new ByteArrayInputStream(fileBytes);
                ObjectInputStream ois = new ObjectInputStream(byteStream);
                read = (Map<K, V>) ois.readObject();
                ois.close();
                res = timer.stop();

                //ois statistics
                minTimeOIS = Math.min(res, minTimeOIS);
                maxTimeOIS = Math.max(res, maxTimeOIS);
                avgTimeOIS = (avgTimeOIS * i + res) / (i + 1);

                bufFIS.close();
                oisFIS.close();
            }
            catch (Exception e) { e.printStackTrace(); }
        }

        if (!bufFile.delete()) System.out.println("error deleting the files.");
        if (!oosFile.delete()) System.out.println("error deleting the files.");

        double runTimeImprove = ((avgTimeOIS / avgTimeBUF) * 100) - 100;

        if (BENCHMARK_OPTION <= 0) System.out.println("Buffer runtime - min =" + minTimeBUF + ", max = " + maxTimeBUF + ", average = " + avgTimeBUF + "\n" +
                "ObjectOutputStream runtime - min =" + minTimeOIS + ", max = " + maxTimeOIS + ", average = " + avgTimeOIS + "\n" +
                ANSI_BLUE + "average runtime improvement - " + runTimeImprove + "%\n" + ANSI_RESET +
                ANSI_BLUE + "average time saved - " + (avgTimeOIS - avgTimeBUF) + ANSI_RESET);

        overallTimeSaved += avgTimeOIS - avgTimeBUF;

        return runTimeImprove;
    }

    private static class Timer {

        private long startTime = System.nanoTime();

        public void start() { startTime = System.nanoTime(); }

        public double stop() { return (double)(System.nanoTime() - startTime) / 1000000; }

    }

}