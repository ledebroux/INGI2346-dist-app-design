import java.rmi.RemoteException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class myIRCObject
extends java.rmi.server.UnicastRemoteObject 
implements myIRCInterface {

	private static final long serialVersionUID = -1246918689085174832L;

	Map<String, myIRCCallbackInterface> clients = new HashMap<String, myIRCCallbackInterface>();
	ArrayList<String> clientList = new ArrayList<String>();

	public myIRCObject()
			throws java.rmi.RemoteException {
		super();
	}

	public void connect(String name, myIRCCallbackInterface callbackObj)
			throws java.rmi.RemoteException {
		//TODO : make sure that the client does't already exist
		// return a value to inform of the status
		clients.put(name, callbackObj);
		clientList.add(name);
		System.out.println("New client: " + name);
	}

	public void disconnect(String name)
			throws java.rmi.RemoteException {
		clients.remove(name);
		clientList.remove(name);
		System.out.println("Client left: " + name);
	}
	
	public ArrayList<String> who()
			throws java.rmi.RemoteException {
		return clientList;
	}

	public void sendMsg(String dest, String sender, String msg) 
			throws RemoteException {
		if(clients.containsKey(dest)){
			clients.get(dest).receiveMsg(msg, sender);
		}
	}
}
