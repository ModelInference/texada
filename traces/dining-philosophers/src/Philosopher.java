import static javaconcprog.AgeRandom.age;
import static javaconcprog.AgeRandom.random;

/**
 * Philosopher for the dining philosophers simulation. Think, get hungry, eat,
 * ad nauseam. Boring....
 * 
 * @author Stephen J. Hartley
 * @version 2005 July
 */
class Philosopher implements Runnable {
    /**
     * Identifying name for a philosopher.
     */
    private String name = null;
    /**
     * Identifying number for a philosopher.
     */
    private int id = 0;
    /**
     * Amount of time in milliseconds to simulate thinking.
     */
    private int napThink = 0;
    /**
     * Amount of time in milliseconds to simulate eating.
     */
    private int napEat = 0;
    /**
     * The dining ``table'' or server.
     */
    private DiningServer ds = null;
    /**
     * Thread internal to a philosopher.
     */
    private Thread me = null;

    /**
     * Constructor.
     * 
     * @param id
     *            Identifying number for a philosopher.
     * @param napThink
     *            Amount of time in milliseconds to simulate thinking.
     * @param napEat
     *            Amount of time in milliseconds to simulate eating.
     * @param ds
     *            The dining ``table'' or server.
     */
    private Philosopher(int id, int napThink, int napEat, DiningServer ds) {
        this.name = "Philosopher " + id;
        this.id = id;
        this.napThink = napThink;
        this.napEat = napEat;
        this.ds = ds;
        /*
         * Do the following
         * 
         * (me = new Thread(this)).start();
         * 
         * in `newInstance' below instead of here so that we do not pass `this'
         * to a constructor of another class, which is a memory model no-no.
         */
    }

    /**
     * Factory method.
     * 
     * @param id
     *            Identifying number for a philosopher.
     * @param napThink
     *            Amount of time in milliseconds to simulate thinking.
     * @param napEat
     *            Amount of time in milliseconds to simulate eating.
     * @param ds
     *            The dining ``table'' or server.
     * @return A philosopher object.
     */
    public static Philosopher newInstance(int id, int napThink, int napEat,
            DiningServer ds) {
        Philosopher instance = new Philosopher(id, napThink, napEat, ds);
        (instance.me = new Thread(instance)).start();
        return instance;
    }

    /**
     * Tell the thread inside this object that it is time to terminate.
     */
    public void timeToQuit() {
        me.interrupt();
    }

    /**
     * Caller blocks until the thread inside this object terminates.
     * 
     * @throws InterruptedException
     */
    public void pauseTilDone() throws InterruptedException {
        me.join();
    }

    /**
     * A philosopher thinks productively after eating to a genteel sufficiency.
     * 
     * @throws InterruptedException
     */
    private void think() throws InterruptedException {
        int napping;
        napping = 1 + (int) random(napThink);
        System.out.println("age=" + age() + ", " + name + " is thinking for "
                + napping + " ms");
        Thread.sleep(napping);
    }

    /**
     * Code for thread inside this object to execute.
     */
    public void run() {
        if (Thread.currentThread() != me)
            return;
        while (true) {
            if (Thread.interrupted()) {
                System.out.println("age=" + age() + ", " + name
                        + " interrupted");
                return;
            }
            try {
                think();
            } catch (InterruptedException e) {
                System.out.println("age=" + age() + ", " + name
                        + " interrupted out of think");
                return;
            }
            System.out.println("age=" + age() + ", " + name + " wants to dine");
            try {
                ds.dine(name, id, napEat); // got hungry, try to eat
            } catch (InterruptedException e) {
                System.out.println("age=" + age() + ", " + name
                        + " interrupted out of dine");
                return;
            }
        }
    }
}
