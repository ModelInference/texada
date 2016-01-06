public class C {
   int x;
   
   public void setToTwo(){
      x = 2;
   }

   public void setToOne(){
      x = 1;
   }

   public static void main(String[] args){
       C c1 = new C();
       C c2 = new C();
       c1.setToOne();
       c2.setToTwo();
       c1.setToOne();
       c2.setToTwo();
       c1.setToOne();
       c2.setToTwo();
       c1.setToOne();
       c2.setToTwo();     
   }


}
