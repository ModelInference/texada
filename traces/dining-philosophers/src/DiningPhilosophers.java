import static javaconcprog.AgeRandom.age;

/**
 * Simulation of the dining philosophers.
 * 
 * @author Stephen J. Hartley
 * @version 2005 July
 */
class DiningPhilosophers {

    /**
     * Driver.
     * 
     * @param args
     *            Command line arguments.
     */
    public static void main(String[] args) {
        int numPhilosophers = 5;
        int runTime = 60; // seconds
        int napThink = 8, napEat = 2;
        try {
            numPhilosophers = Integer.parseInt(args[0]);
            runTime = Integer.parseInt(args[1]);
            napThink = Integer.parseInt(args[2]);
            napEat = Integer.parseInt(args[3]);
        } catch (Exception e) { /* use defaults */
        }
        System.out.println("DiningPhilosophers: numPhilosophers="
                + numPhilosophers + ", runTime=" + runTime + ", napThink="
                + napThink + ", napEat=" + napEat);

        // create the DiningServer object
        DiningServer ds = DiningServerImpl.newInstance(numPhilosophers);

        // create the Philosophers
        // (they have self-starting threads)
        Philosopher[] p = new Philosopher[numPhilosophers];
        for (int i = 0; i < numPhilosophers; i++)
            p[i] = Philosopher.newInstance(i, napThink * 1000, napEat * 1000,
                    ds);
        System.out.println("All Philosopher threads started");

        // let the Philosophers run for a while
        try {
            Thread.sleep(runTime * 1000);
            System.out.println("age=" + age()
                    + ", time to terminate the Philosophers and exit");
            for (int i = 0; i < numPhilosophers; i++) {
                p[i].timeToQuit();
                System.out.println("age=" + age() + ", philosopher" + i
                        + " told");
            }
            for (int i = 0; i < numPhilosophers; i++) {
                p[i].pauseTilDone();
                System.out.println("age=" + age() + ", philosopher" + i
                        + " done");
            }
        } catch (InterruptedException e) { /* ignored */
        }
        System.out.println("age=" + age() + ", all Philosophers are done");
    }
}
