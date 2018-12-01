import java.net.URL;
import java.sql.*;
import java.util.*;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

import java.math.BigDecimal;
import java.util.Date;
import java.text.SimpleDateFormat;

public class TestConn {
	public static void main(String[] args) {
		/* load mysql jdbc driver */
		try {
			Class.forName("com.mysql.Driver");
		} catch(Exception e) {
			System.out.println(e.getMessage());
			e.printStackTrace ();
		}	

		try {
			int i;
			Connection conn = DriverManager.getConnection("jdbc:mysql://localhost:55555/TEST", "mysql", "mysql");
			if(conn != null)
				System.out.println("connection sucessful!");
			else
				System.out.println("connection fail!");
			conn.setAutoCommit(false);
			System.out.println("begin");

			String create_table = "CREATE TABLE TEST(ID CHARACTER(50) PRIMARY KEY, 最新状态 CHARACTER(50), 最新遥测 float)";
			Statement statement = conn.createStatement();
			statement.executeUpdate("drop table test");
			conn.commit();
			statement.executeUpdate(create_table);
			conn.commit();

			String sql_insert = "insert into test values(?, 'xxx', NULL)";
			PreparedStatement pstmt_ins = conn.prepareStatement(sql_insert);
			for (i = 0; i < 20000; i++)
			{
				pstmt_ins.setString(1, Integer.toString(i));
				pstmt_ins.addBatch();
			}
			pstmt_ins.executeBatch();
			conn.commit();
			System.out.println("finish insert");

			String sql_update = "update test set 最新状态=?, 最新遥测=? where ID=?;";
			PreparedStatement pstmt_upd = conn.prepareStatement(sql_update);
			for (i = 0; i < 20000; i++)
			{
				Date day = new Date();
				BigDecimal bd = new BigDecimal("0.00"); 

				pstmt_upd.setString(1, "1");
				pstmt_upd.setBigDecimal(2, bd);
				pstmt_upd.setString(3, Integer.toString(i));
				pstmt_upd.addBatch();
			}
			pstmt_upd.executeBatch();
			conn.commit();
			System.out.println("finish update");

			if(conn != null)
				conn.close();
		} catch(Exception ex) {
			System.out.println(ex.getMessage());
			ex.printStackTrace ();
		}
	}
}
