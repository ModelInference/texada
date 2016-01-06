public class Incrementer{

   public int increment(int x){
      return x + 3;
   }

   public static void main(String[] args){
       Incrementer inc = new Incrementer();
       inc.increment(0);
       inc.increment(1);
       inc.increment(2);
      // inc.increment(-1);
     //  inc.increment(-2);
       
   }
}
