
import java.rmi.Naming;

public class CalculatorServer {

   public CalculatorServer() {
     Calculator c = null;

     try {
       c  = new CalculatorImpl();
     } catch (Exception e) {
       System.out.println("Can't create Calculator: " + e);
     }

     while(true) {
       try {
         Naming.rebind("rmi://localhost:1099/CalculatorService", c);
       } catch (Exception e) {
         System.out.println("Can't rebind: " + e);
       }

       try {
         Thread.sleep(60000); //10 min.
       } catch (InterruptedException e) {}
     }
   }

   public static void main(String args[]) {
     new CalculatorServer();
   }
}