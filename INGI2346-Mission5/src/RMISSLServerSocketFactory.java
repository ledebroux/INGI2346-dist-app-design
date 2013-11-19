import java.io.*;
import java.net.*;
import java.rmi.server.*;
import java.security.KeyStore;
import javax.net.*;
import javax.net.ssl.*;
import javax.security.cert.X509Certificate;

public class RMISSLServerSocketFactory
	implements RMIServerSocketFactory, Serializable {

   public ServerSocket createServerSocket(int port)
	  throws IOException
  	{ 
     SSLServerSocket ss = null;
	    SSLServerSocketFactory ssf = null;
	    try {
	       	// set up key manager to do server authentication
       		SSLContext ctx;
       		KeyManagerFactory kmf;
	       	KeyStore ks;
	       	char[] filepassphrase = "serverfilepass".toCharArray();
	       	char[] keypassphrase = "serverkeypass".toCharArray();

	       	ctx = SSLContext.getInstance("TLS");
	       	kmf = KeyManagerFactory.getInstance("SunX509");
	       	ks = KeyStore.getInstance("JKS");

	       	ks.load(new FileInputStream("servertestkeys"), filepassphrase);
	       	kmf.init(ks, keypassphrase);
       		ctx.init(kmf.getKeyManagers(), null, null);

      		ssf = ctx.getServerSocketFactory();
	    } catch (Exception e) {
       		e.printStackTrace();
	    }
     ss = (SSLServerSocket) ssf.createServerSocket(port);
	    return ss;
 	}
}
