public interface myIRCCallbackInterface 
extends java.rmi.Remote {

	public void receiveMsg(String msg, String sender)
			throws java.rmi.RemoteException;
	
	public void left(String user)
			throws java.rmi.RemoteException;
	
	public void arrived(String user)
			throws java.rmi.RemoteException;
}