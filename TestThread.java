import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.net.URL;
import java.sql.*;
import java.util.*;
import java.io.IOException;

public class TestThread {
	public static void main(String args[]) {
        	/* load mysql jdbc driver */
        	try {
        	    Class.forName("com.mysql.Driver");
        	} catch(Exception e) {
        	    System.out.println(e.getMessage());
        	    e.printStackTrace ();
        	}

		List<ConnectionServer> list = new ArrayList<ConnectionServer>();
		for (int i = 0; i < 1000; i++) {
			list.add(new ConnectionServer("connection" + " " + i));
		}

		ExecutorService pool = Executors.newFixedThreadPool(1000);
		int size = list.size();
		for (int i = 0; i < size; i++) {
			final ConnectionServer conServer = list.get(i);
			pool.submit(new Callable<Object>() {
				@Override
				public Object call() throws Exception {
					conServer.testConnection();
					return null;
				}
			});
		}
		pool.shutdown();
		boolean flag = true;
		while (flag) {
			if (pool.isTerminated()) {
				flag = false;
			}
		}
	}
}

class ConnectionServer {
	private String conName;

	public ConnectionServer(String conName) {
		this.conName = conName;
	}

	public void testConnection() {
                try {
                        Connection conn = DriverManager.getConnection("jdbc:mysql://192.168.56.100:55555/TEST", "mysql", "mysql");
                        if (conn != null)
                                System.out.println("connection sucessful!");
                        else
                                System.out.println("connection fail!");

                        Statement stmt = conn.createStatement();
                        ResultSet rs=stmt.executeQuery("select count(*) from dual");
                        stmt.close();

                        System.out.println("finish: " + this.conName);
                        Thread.sleep(600000);
                        conn.close();
                } catch (Exception ex) {
                        System.out.println(ex.getMessage());
                        ex.printStackTrace();
                }

        }
}
