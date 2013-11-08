

import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;

public class myIRC {
	public static void main(String[] args) {
		
		String name = args[1];
		
		try {
			myIRCInterface c = 
					(myIRCInterface)Naming.lookup("rmi://localhost:1099/myIRC");
			myIRCCallbackImpl callbackObj = 
					new myIRCCallbackImpl();
			c.connect(name, callbackObj);
			System.out.println(c.who().toString());
			c.sendMsg(name, name, "Hello me!");
			//c.who();
			System.exit(0);
		}
		catch (MalformedURLException murle) {
			System.out.println();
			System.out.println(
					"MalformedURLException");
			System.out.println(murle);
		}
		catch (RemoteException re) {
			System.out.println();
			System.out.println(
					"RemoteException");
			System.out.println(re);
		}
		catch (NotBoundException nbe) {
			System.out.println();
			System.out.println(
					"NotBoundException");
			System.out.println(nbe);
		}
		catch (
				java.lang.ArithmeticException ae) {
			System.out.println();
			System.out.println(
					"java.lang.ArithmeticException");
			System.out.println(ae);
		}
	}
}
