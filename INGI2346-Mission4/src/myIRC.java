

import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.util.Scanner;

public class myIRC {
	
	static String name;
	static myIRCInterface c;
	static myIRCCallbackImpl callbackObj;
	
	static String cmd; 
	static Scanner scanIn;
	
	public static void main(String[] args) {
		
		name = args[1];
		scanIn = new Scanner(System.in);
		
		try {
			c = (myIRCInterface)Naming.lookup("rmi://localhost:1099/myIRC");
			connect();
			while(true){
				System.out.println("Enter a command");
				cmd = scanIn.nextLine();
				//if()
				System.out.println(cmd);
			}
//			who();
//			sendMsg(name, "Hello me!");
//			disconnect();
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
	
	public static void connect(){
		try {
			callbackObj = new myIRCCallbackImpl();
			boolean connect = c.connect(name, callbackObj);
			if(connect){
				System.out.println("Connected as " + name);
			} else {
				System.out.println("<" + name + "> is already in use.");
				System.exit(0);
			}
		} catch (RemoteException re) {
			System.out.println();
			System.out.println(
					"RemoteException");
			System.out.println(re);
		}
	}
	
	public static void disconnect(){
		try {
			c.disconnect(name);
			scanIn.close();
			System.exit(0);
		} catch (RemoteException re) {
			System.out.println();
			System.out.println(
					"RemoteException");
			System.out.println(re);
		}
	}
	
	public static void sendMsg(String dest, String msg){
		try {
			boolean result = c.sendMsg(dest, name, "Hello me!");
			if(!result){
				System.out.println("Message to " + dest + " could not be delivered.");
			}
		} catch (RemoteException re) {
			System.out.println();
			System.out.println(
					"RemoteException");
			System.out.println(re);
		}
	}
	
	public static void who(){
		try {
			System.out.println(c.who().toString());
		} catch (RemoteException re) {
			System.out.println();
			System.out.println(
					"RemoteException");
			System.out.println(re);
		}
	}
}
