public class B {

   int x;

   public B() {
      x = 0;
   }

   public void print(){
      x = 4;
      System.out.println("x is " + x);
   }

   public static void main(String[] args){
       A a = new A();
       B b = new B();
       b.print();
       a.print();
       b.print();
       a.print();
       a.print();
       a.print();
       b.print();
       a.print();
       b.print();
       b.print();
   }
   
}
