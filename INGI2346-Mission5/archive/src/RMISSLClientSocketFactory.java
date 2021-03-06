import java.io.FileInputStream;
import java.io.IOException;
import java.io.Serializable;
import java.net.Socket;
import java.rmi.server.RMIClientSocketFactory;
import java.security.KeyStore;

import javax.net.ssl.KeyManagerFactory;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSocket;
import javax.net.ssl.SSLSocketFactory;
import javax.security.cert.X509Certificate;

public class RMISSLClientSocketFactory
implements RMIClientSocketFactory, Serializable {

	public Socket createSocket(String host, int port)
			throws IOException
			{
		
		System.setProperty("javax.net.ssl.trustStore", "servertesttrust");
		SSLSocketFactory sf = null;
		SSLSocket s = null;

		try {
			SSLContext ctx;
			KeyManagerFactory kmf;
			KeyStore ks;
			char[] passphrase = "clientpassphrase".toCharArray();

			ctx = SSLContext.getInstance("TLS");
			kmf = KeyManagerFactory.getInstance("SunX509");
			ks = KeyStore.getInstance("JKS");

			ks.load(new FileInputStream("clienttestkeys"), passphrase);
			kmf.init(ks, passphrase);
			ctx.init(kmf.getKeyManagers(), null, null);

			sf = ctx.getSocketFactory();
		} catch (Exception e) {
			e.printStackTrace();
		}

		s = (SSLSocket)sf.createSocket(host, port);
		return s;
			}
}