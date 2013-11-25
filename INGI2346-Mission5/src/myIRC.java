import java.net.MalformedURLException;
import java.rmi.Naming;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Scanner;

public class myIRC {
	
	static String name = null;
	static String addr;
	static myIRCInterface c;
	static myIRCCallbackImpl callbackObj;
	
	static String cmd; 
	static Scanner scanIn;
	static String to;
	static String msg;
	
	public static void main(String[] args) {
		
		if(args.length == 0) {
			System.out.println("Should be used as: myIRC server_addr username");
		}
		
		addr = args[0];
		if(args.length == 2){
			name = args[1];
		}
		
		scanIn = new Scanner(System.in);
		
		try {
			c = (myIRCInterface)Naming.lookup("rmi://"+addr+":1099/myIRC");
			connect();
			who();
			
			/*
			 * loop and read the commands that the user enters 
			 */
			while(true){
				System.out.print("#");
				cmd = scanIn.nextLine();
				if(cmd.equals("who")){
					who();
				} 
				else if(cmd.equals("quit")) {
					disconnect();
				}
				else if(cmd.equals("msg")) {
					sendMsg();
				}
				else {
					System.out.println("Unknown command");
				}
			}
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
	
	/**
	 * connect
	 * creates an object for the client and sends it to the server
	 * reads the value returned by the server and acts accordingly
	 */
	public static void connect(){
		try {
			callbackObj = new myIRCCallbackImpl();
			String connect = c.connect(name, callbackObj);
			if(connect.equals("failed")){
				System.out.println("<" + name + "> is already in use.");
				System.exit(0);
			} else if (connect.equals("success")){
				System.out.println("Connected as " + name);
			} else {
				name = connect;
				System.out.println("Connected as " + name);
			}
		} catch (RemoteException re) {
			System.out.println();
			System.out.println(
					"RemoteException");
			System.out.println(re);
		}
	}
	
	/**
	 * disconnect
	 * informs the server that the client wants to disconnect
	 */
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
	
	/**
	 * sendMsg
	 * upon a msg command, the user is asked for a recipient and a message to send
	 * the server method is then called
	 */
	public static void sendMsg(){
		try {
			System.out.print("To: ");
			to = scanIn.nextLine();
			System.out.print("Message: ");
			msg = scanIn.nextLine();
			if(to.equals(name)){
				System.out.println("From me: " + msg);
			}
			else {
				boolean result = c.sendMsg(to, name, msg);
				if(!result){
					System.out.println("Message to " + to + " could not be delivered.");
				}
			}
		} catch (RemoteException re) {
			System.out.println();
			System.out.println(
					"RemoteException");
			System.out.println(re);
		}
	}
	
	/**
	 * who
	 * asks the server for the list of connected users
	 */
	public static void who(){
		try {
			System.out.println("Connected users:");
			String[] users = c.who().toArray(new String[0]);
			Arrays.sort(users);
			for(String u: users){
				System.out.println(u);
			}
		} catch (RemoteException re) {
			System.out.println();
			System.out.println(
					"RemoteException");
			System.out.println(re);
		}
	}
}
