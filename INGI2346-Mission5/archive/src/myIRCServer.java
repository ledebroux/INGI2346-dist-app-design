

import java.rmi.Naming;

public class myIRCServer {

	public myIRCServer() {
		myIRCObject c = null;

		try {
			c  = new myIRCObject();
		} catch (Exception e) {
			System.out.println("Can't create myIRCObject: " + e);
		}

		while(true) {
			try {
				Naming.rebind("rmi://localhost:1099/myIRC", c);
			} catch (Exception e) {
				System.out.println("Can't rebind: " + e);
			}
		
			try {
				Thread.sleep(60000); //10 min.
			} catch (InterruptedException e) {}
		}
	}
	
	public static void main(String args[]) {
		new myIRCServer();
	}

}
