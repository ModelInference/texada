import java.util.concurrent.*;
import javaconcprog.AgeRandom;
import java.util.HashSet;

// inspired by http://www.cs.helsinki.fi/u/kerola/rio/ohjeet/Java/semaphores/s06e-huhtamaki-merikanto-nevalainen/SleepingBarber.java

public class SleepingBarber extends Thread {
  
   // Constants to adjust if desired
   public static final int CHAIRS = 3;
   public static final int CUSTOMERS = 7;
   public static final int NUM_HAIRCUTS = 1; 

   // Semaphores
   // signalled when a customer enters and gets a chair to sit in
   public static Semaphore custWaiting = new Semaphore(0);
   // signalled when barber is free to cut hair
   public static Semaphore barberReady = new Semaphore(0);
   // wait to finish haircut
   public static Semaphore haircutDone = new Semaphore(0);
   // guards the acces to waiting room seats
   public static Semaphore waitingRoom = new Semaphore(1);
   // number of seats to sit in available
   public int freeSeats = CHAIRS;
   public static Semaphore printState = new Semaphore(1);

   // Customer State
   public enum customerState { Entering, Exiting, Waiting, GettingHairCut, Chilling }
   // Barber State
   public enum barberState {Sleeping, CuttingHair, Checking}
 
   public Barber barber;
   public HashSet<Customer> customers = new HashSet<Customer>();
   
   // Prints the state of all customers and barber 
   void printState() {
     try {
       printState.acquire(); 
     } catch (InterruptedException ex) {}       
     System.out.println("Barber is " + barber.state);
     for (Customer c: customers) 
       System.out.println("Customer" + c.iD + " is " + c.state);
     System.out.println("..");
     printState.release();
   }

   // The customer
   class Customer extends Thread {
      
      // customer has an id, wants some number of haircuts, and starts off chilling
      int iD;
      int haircutsRemaining = NUM_HAIRCUTS;
      customerState state = customerState.Chilling;
      
      public Customer(int i) {
        iD = i; 
      }
      
      public void run() {
        while (haircutsRemaining > 0) {
          try {
            // chill a bit before trying to get your haircut
            sleep((long) javaconcprog.AgeRandom.random(300,6000));
            // try to get WR access
            waitingRoom.acquire();
            state = customerState.Entering;
            printState();
            // if there are any free seats
            if (freeSeats > 0) {
              freeSeats--; 
              state = customerState.Waiting;
              // tell the barber we're waiting
              custWaiting.release();
              waitingRoom.release();  
              try {
              barberReady.acquire(); 
              } catch (InterruptedException ex) {}
              // get haircut
              state = customerState.GettingHairCut;
              try {
              haircutDone.acquire(); 
              } catch (InterruptedException ex) {}
              // finish haircut
              state = customerState.Exiting;
              printState();
              state = customerState.Chilling;
              haircutsRemaining--;
                          
            } 
            // if no free seats
            else {
              // simulate exiting to print out, then go to chilling before
              // next customer acts
              state = customerState.Exiting;
              printState();
              state = customerState.Chilling;
              waitingRoom.release();
            }
          } catch (InterruptedException ex) {} // if could not aquire WR lock
         
        }
        printState();
      }
      

   }
   
   class Barber extends Thread {
   
     public barberState state = barberState.Sleeping;
     
     public Barber() {}

     public void run() {
     while(true) {
       try{
         // keep sleeping until notified by customer, then go check to get that
         // customer
         custWaiting.acquire();
         state = barberState.Checking;
         printState();
         // take customer out of waiting room, wake them up
         waitingRoom.acquire();
         freeSeats++;
         barberReady.release();
         waitingRoom.release();
         cutHair();
         state = barberState.Sleeping;
       } catch (InterruptedException ex) {}
     }
     }
     
     private void cutHair(){
       state = barberState.CuttingHair;
       try{
         sleep( (long) javaconcprog.AgeRandom.random(4000,6000));
       } catch (InterruptedException ex) {}
       haircutDone.release();
     }
 
   }
  

   public static void main(String args[]) {
      SleepingBarber barberShop = new SleepingBarber();
      barberShop.start();
   }

   public void run() {
     this.barber = new Barber();
     this.barber.start();
     for (int i = 0; i < CUSTOMERS; i++) { 
       this.customers.add(new Customer(i));
      }
     for (Customer c : customers) {
       c.start();
     }
   }
}
