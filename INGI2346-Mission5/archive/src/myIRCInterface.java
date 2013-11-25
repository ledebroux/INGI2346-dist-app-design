import java.util.ArrayList;

public interface myIRCInterface 
extends java.rmi.Remote {

	public String connect(String name, myIRCCallbackInterface callbackObj)
			throws java.rmi.RemoteException;

	public void disconnect(String name)
			throws java.rmi.RemoteException;
	
	public ArrayList<String> who()
			throws java.rmi.RemoteException;

	public boolean sendMsg(String dest, String sender, String msg)
			throws java.rmi.RemoteException;
}
