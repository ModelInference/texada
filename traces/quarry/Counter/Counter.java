public class Counter {
   private int size;
   
   public Counter(){
       size = 0;
   }

   public void add(){
      size++;
   }

   public void remove(){
      size--;
   }

   public static void main(String[] args) {
       // Test 1;
       Counter c1 = new Counter();
       c1.add();
       c1.remove();
       // Test 2;
       Counter c2 = new Counter();
       c2.add();
       c2.add();
       c2.remove();
       c2.add();
       c2.remove();
       c2.remove();
       // Test 3;
       Counter c3 = new Counter();
       for (int i = 0; i < 27; i++)
           c3.add();
       for (int i = 0; i < 27; i++)
           c3.remove();
       // Test 4;
       Counter c4 = new Counter();
       for (int i = 0; i < 148; i++)
           c4.add();
       for (int i = 0; i < 148; i++)
           c4.remove();
   }

}
