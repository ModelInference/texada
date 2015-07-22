import static javaconcprog.AgeRandom.age;
import static javaconcprog.AgeRandom.random;

/**
 * A ``table'' at which the dining philosophers eat and think. Concrete
 * subclasses implement the takeForks and putForks methods.
 * 
 * @author Stephen J. Hartley
 * @version 2005 July
 */
abstract class DiningServer {
    /**
     * Constants for thinking, hungry, and eating states.
     */
    protected enum State {
        THINKING, HUNGRY, EATING
    }

    /**
     * Number of chairs at the table (number of philosophers).
     */
    protected int numPhils = 0;
    /**
     * Array of state information, one entry per philosopher (thinking, hungry,
     * eating).
     */
    protected State[] state = null;

    /**
     * Constructor.
     * 
     * @param numPhils
     *            The number of dining philosophers.
     */
    protected DiningServer(int numPhils) {
        this.numPhils = numPhils;
        state = new State[numPhils];
        for (int i = 0; i < numPhils; i++)
            state[i] = State.THINKING;
    }

    /**
     * A dining philosopher wants to pick up its forks if available and eat.
     * 
     * @param name
     *            The name of the philosopher.
     * @param id
     *            The number of the philosopher.
     * @param napEat
     *            The time the philosopher wants to eat in milliseconds.
     * @throws InterruptedException
     */
    public void dine(String name, int id, int napEat)
            throws InterruptedException {
        try {
            takeForks(id);
            eat(name, napEat);
        } finally { // Make sure we return the
            putForks(id); // forks if interrupted
        }
    }

    /**
     * Compute the identifying number of left neighbor at table.
     * 
     * @param i
     *            Philosopher number.
     * @return Right neighbor number.
     */
    protected final int left(int i) {
        return (numPhils + i - 1) % numPhils;
    }

    /**
     * Compute the identifying number of right neighbor at table.
     * 
     * @param i
     *            Philosopher number.
     * @return Right neighbor number.
     */
    protected final int right(int i) {
        return (i + 1) % numPhils;
    }

    /**
     * A philosopher eats to a genteel sufficiency after thinking productively,
     * becoming hungry, and taking its forks if available.
     * 
     * @param name
     *            The name of the philosopher.
     * @param napEat
     *            The time the philosopher wants to eat in milliseconds.
     * @throws InterruptedException
     */
    private void eat(String name, int napEat) throws InterruptedException {
        int napping;
        napping = 1 + (int) random(napEat);
        //System.out.println("age=" + age() + ", " + name + " is eating for "
        //+ napping + " ms");
        Thread.sleep(napping);
    }

    /**
     * A hungry philosopher attempts to pick up its two forks. If available, the
     * philosopher eats, else waits.
     * 
     * @param i
     *            The number of the hungry philosopher.
     * @throws InterruptedException
     */
    protected abstract void takeForks(int i) throws InterruptedException;

    /**
     * A philosopher has finished eating. Return its two forks to the table and
     * check for hungry neighbors. If a hungry neighbor's two forks are now
     * available, nudge the neighbor.
     * 
     * @param i
     *            The number of the philosopher finished eating.
     */
    protected abstract void putForks(int i) throws InterruptedException;

    /**
     * Print the state of the philosophers.
     * 
     * @param caller
     *            The name of the calling method.
     */
    protected void printState(String caller) {
        // Build a line and use one println() instead of several print()'s
        // so that multiple threads' output does not become interleaved.
        //StringBuffer line = new StringBuffer();
        //line.append(caller);
        System.out.println("==");
        for (int i = 0; i < numPhils; i++) {
            //line.append(", " + i + " is " + state[i]);
            System.out.println(i + " is " + state[i] + "\n..");
        }
        //System.out.println(line);
    }
}
