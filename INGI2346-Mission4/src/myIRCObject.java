

public class myIRCObject
	extends java.rmi.server.UnicastRemoteObject 
	implements myIRCInterface {
	
	public myIRCObject()
        throws java.rmi.RemoteException {
        super();
    }
	
	public int print(int a)
		throws java.rmi.RemoteException {
		System.out.println("Kikoo " + a);
		return 1;
	}
}
