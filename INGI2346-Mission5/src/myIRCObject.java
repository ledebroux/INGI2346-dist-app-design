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
		super();
	}

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
	
	public ArrayList<String> who()
			throws java.rmi.RemoteException {
		return clientList;
	}

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
	
	private String randomName(){
		String res;
		do {
			int n = rand.nextInt(99999);
			res = "guest" + n;
		} while (clientList.contains(res));
		return res;
	}
}
