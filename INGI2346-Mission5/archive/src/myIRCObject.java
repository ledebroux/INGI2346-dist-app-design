import java.rmi.RemoteException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;
import java.util.Random;

public class myIRCObject
extends java.rmi.server.UnicastRemoteObject 
implements myIRCInterface {

	private static final long serialVersionUID = -1246918689085174832L;

	Map<String, myIRCCallbackInterface> clients = new HashMap<String, myIRCCallbackInterface>();
	ArrayList<String> clientList = new ArrayList<String>();
	Random rand = new Random();
	
	public myIRCObject()
			throws java.rmi.RemoteException {
		super(0, new RMISSLClientSocketFactory(), new RMISSLServerSocketFactory());
	}
	
	/**
	 * connect
	 * adds the client and its object to the Map and ArrayList containing the clients
	 * if the client did not provide a name, one is given for it
	 * every other client is noticed: if a client in the list can't be reached, 
	 * it is considered disconnected and is removed from the data structures
	 */
	public String connect(String name, myIRCCallbackInterface callbackObj)
			throws java.rmi.RemoteException {
		String res;
		if(clients.containsKey(name)){
			return "failed";
		}
		if(name == null){
			name = randomName();
			res = name;
		} else {
			res = "success";
		}
		for(String c: clients.keySet().toArray(new String[0])) {
			try {
				clients.get(c).arrived(name);
			} catch (RemoteException re) {
				System.out.println();
				System.out.println(
						"RemoteException");
				System.out.println(re);
				disconnect(c);
			}
		}
		clients.put(name, callbackObj);
		clientList.add(name);
		System.out.println("New client: " + name);
		return res;
	}

	/**
	 * disconnect
	 * removes the client from the data structures
	 * notices all the other clients: if a client in the list can't be reached, 
	 * it is considered disconnected and is removed from the data structures
	 */
	public void disconnect(String name)
			throws java.rmi.RemoteException {
		clients.remove(name);
		clientList.remove(name);
		System.out.println("Client left: " + name);
		for(String c: clients.keySet().toArray(new String[0])) {
			try {
				clients.get(c).left(name);
			} catch (RemoteException re) {
				System.out.println();
				System.out.println(
						"RemoteException");
				System.out.println(re);
				disconnect(c);
			}
		}
	}
	
	/**
	 * who
	 * returns the list of all connected clients
	 */
	public ArrayList<String> who()
			throws java.rmi.RemoteException {
		return clientList;
	}

	/**
	 * sendMsg
	 * calls the method on the destination client object to forward 
	 * 		the message from the sender
	 */
	public boolean sendMsg(String dest, String sender, String msg) 
			throws RemoteException {
		if(clients.containsKey(dest)){
			try {
				clients.get(dest).receiveMsg(msg, sender);
				return true;
			} catch (RemoteException re) {
				System.out.println();
				System.out.println(
						"RemoteException");
				System.out.println(re);
				disconnect(dest);
				return false;
			}
		}
		return false;
	}
	
	/**
	 * randomName
	 * choose a random name that is not already taken
	 * 		in the case of the client not giving a name
	 */
	private String randomName(){
		String res;
		do {
			int n = rand.nextInt(99999);
			res = "guest" + n;
		} while (clientList.contains(res));
		return res;
	}
}
