import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

/**
 * A ``table'' at which the dining philosophers eat and think.
 * 
 * @author Stephen J. Hartley
 * @version 2005 July
 */
class DiningServerImpl extends DiningServer {
    /**
     * Mutual exclusion lock for read-update-write access to philosopher state
     * information to prevent race conditions.
     */
    private Lock mutex = null;
    /**
     * An array of condition variables, one for each philosopher, on which to
     * block if hungry and its forks are not both available.
     */
    private Condition[] self = null;

    /**
     * Constructor.
     * 
     * @param numPhils
     *            The number of dining philosophers.
     */
    private DiningServerImpl(int numPhils) {
        super(numPhils);
        mutex = new ReentrantLock();
        self = new Condition[numPhils];
        for (int i = 0; i < numPhils; i++)
            self[i] = mutex.newCondition();
    }

    /**
     * Factory method.
     * 
     * @param numPhils
     *            The number of dining philosophers.
     * @return A dining server table object.
     */
    public static DiningServer newInstance(int numPhils) {
        DiningServer instance = new DiningServerImpl(numPhils);
        return instance;
    }

    /**
     * Test the availability of the two forks for a hungry philosopher. If
     * available, set the state of the hungry philosopher to eating.
     * 
     * @param k
     *            The number of the hungry philosopher.
     */
    private void test(int k) {
        if (state[left(k)] != State.EATING && state[k] == State.HUNGRY
                && state[right(k)] != State.EATING) {
            state[k] = State.EATING;
        }
    }

    /**
     * A hungry philosopher attempts to pick up its two forks. If available, the
     * philosopher eats, else waits.
     * 
     * @param i
     *            The number of the hungry philosopher.
     * @throws InterruptedException
     */
    protected void takeForks(int i) throws InterruptedException {
        mutex.lock();
        try {
            state[i] = State.HUNGRY;
            printState("begin takeForks");
            test(i);
            printState("end   takeForks");
            while (state[i] != State.EATING)
                self[i].await();
        } finally {
            mutex.unlock();
        } // unlock whether or not exceptions thrown
    }

    /**
     * A philosopher has finished eating. Return its two forks to the table and
     * check for hungry neighbors. If a hungry neighbor's two forks are now
     * available, nudge the neighbor.
     * 
     * @param i
     *            The number of the philosopher finished eating.
     */
    protected void putForks(int i) {
        mutex.lock();
        try {
            if (state[i] != State.EATING)
                return;
            state[i] = State.THINKING;
            printState("begin  putForks");
            test(left(i));
            test(right(i));
            printState("end    putForks");
            if (state[left(i)] == State.EATING)
                self[left(i)].signal();
            if (state[right(i)] == State.EATING)
                self[right(i)].signal();
        } finally {
            mutex.unlock();
        } // unlock whether or not exceptions thrown
    }
}
