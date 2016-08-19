import java.util.Vector;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.ReentrantLock;

public class ProducerConsumerTest {
   public static void main(String[] args) {
      SharedArray a = new SharedArray();
      Producer p1 = new Producer(a, 1);
      Consumer c1 = new Consumer(a, 1);
      Consumer c2 = new Consumer(a, 2);
      Producer p2 = new Producer(a, 2);
      Consumer c3 = new Consumer(a, 3);
      Producer p3 = new Producer(a, 3);
      Producer p4 = new Producer(a, 4);
      Consumer c4 = new Consumer(a, 4);
      Consumer c5 = new Consumer(a, 5);
      Producer p5 = new Producer(a, 5);
      Consumer c6 = new Consumer(a, 6);
      Producer p6 = new Producer(a, 6);
      p1.start(); 
      p2.start(); 
      c1.start();      
      c3.start();
      p3.start(); 
      c2.start();
      p4.start(); 
      p5.start();
      p6.start();  
      c4.start(); 
      c5.start();
      c6.start();  
   }
}

class SharedArray {
   private int[] contents = new int[5];
   private int size = 0;
   final Lock lock = new ReentrantLock();
   final Condition notFull = lock.newCondition();
   final Condition notEmpty = lock.newCondition();

   public int get() {
      lock.lock();
      System.out.println("getENTER():::POINT");
      System.out.println("size");
      System.out.println(size);
      System.out.println("1\n");

      try {
         while (size == 0) {
          notEmpty.await();
         }
         int x = contents[size - 1];
         size--;
         notFull.signal();
         return x;
      } catch (InterruptedException e){
         return -1;
      } finally {
         System.out.println("getEXIT():::POINT");
         System.out.println("size");
         System.out.println(size);
         System.out.println("1\n");
         lock.unlock();
      }
   }
    
   public void put(int value) {
      lock.lock();
         System.out.println("putENTER():::POINT");
         System.out.println("size");
         System.out.println(size);
         System.out.println("1\n");
      try {
         while (size >= contents.length) {
          notFull.await();
         }
         contents[size] = value;
         size++;
         notEmpty.signal();
      } catch (InterruptedException e){
      } finally {
         System.out.println("putEXIT():::POINT");
         System.out.println("size");
         System.out.println(size);
         System.out.println("1\n");
         lock.unlock();
      }
   }
}

class Consumer extends Thread {
   private SharedArray array;
   private int number;
   public Consumer(SharedArray a, int number) {
      array = a;
      this.number = number;
   }
   public void run() {
      int value = 0;
         for (int i = 0; i < 10; i++) {
            value = array.get();
      	 // try {
       	//	  sleep((int)(Math.random() * 100));
       	// } catch (InterruptedException e) { }
      
      }
    }   
   }

class Producer extends Thread {
   private SharedArray array;
   private int number;

   public Producer(SharedArray a, int number) {
      array = a;
      this.number = number;
   }

   public void run() {
      for (int i = 0; i < 10; i++) {
        array.put(i);
        try {
         sleep((int)(Math.random() * 100));
        } catch (InterruptedException e) { }
      }
   }
}
