import java.io.IOException;
import java.io.Serializable;
import java.net.Socket;
import java.rmi.server.RMIClientSocketFactory;

import javax.net.ssl.SSLSocket;
import javax.net.ssl.SSLSocketFactory;

public class RMISSLClientSocketFactory
	implements RMIClientSocketFactory, Serializable {

    public Socket createSocket(String host, int port)
	throws IOException
	{
	    SSLSocketFactory sf = null;
	    SSLSocket s = null;

	     sf = (SSLSocketFactory)SSLSocketFactory.getDefault();

	    s = (SSLSocket)sf.createSocket(host, port);
	    return s;
	}
}