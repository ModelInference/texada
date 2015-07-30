package javaconcprog;

import java.util.Random;

/**
  * Utility methods age and random for multithreaded simulations.
  *
  * @author Stephen J. Hartley
  * @version 2005 July
  */
public abstract class AgeRandom {
   /**
     * Starting time of the simulation in milliseconds since 1 Jan 1970.
     */
   private static final long startTime = System.currentTimeMillis();
   /**
     * Object used for generating pseudo random numbers.
     */
   private static final Random rnd = new Random();

   /**
     * Computes the number of milliseconds elapsed since program start.
     * @return The number of milliseconds elapsed since program start.
     */
   public static final long age() {
      return System.currentTimeMillis() - startTime;
   }

   /**
     * Generates a pseudo random number in the range [0, 1).
     * @return The number generated.
     */
   public static final double random() {
      return rnd.nextDouble();
   }

   /**
     * Generates a pseudo random number in the range [0, ub).
     * @param ub The upper bound (exclusive) of the range.
     * @return The number generated.
     */
   public static final double random(int ub) {
      return rnd.nextDouble()*ub;
   }

   /**
     * Generates a pseudo random number in the range [lb, ub).
     * @param lb The lower bound (inclusive) of the range.
     * @param ub The upper bound (exclusive) of the range.
     * @return The number generated.
     */
   public static final double random(int lb, int ub) {
      return lb + rnd.nextDouble()*(ub - lb);
   }
}
