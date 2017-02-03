package jopenshowvar_latency;

import java.io.IOException;

import no.hials.crosscom.CrossComClient;
import no.hials.crosscom.KRL.KRLInt;
import no.hials.crosscom.KRL.structs.KRLE6Axis;

public class RunTest {
	private CrossComClient client = null;
	
	public void testAll(int rounds, int t_id, int t_num, Boolean e6axis) {
		System.out.println("Starting tests");
		System.out.println("Thread, #Threads, Test type, E6AXIS, Test #, Time (s)");
		
		// Read test
		if ((t_id != 0) || (t_num == 1)) {
			for (int i = 0; i < rounds; i++) {
				System.out.println(t_id + ", " + t_num + ", Read, " + e6axis + ", " + 2500+i + ", " + timedRead(e6axis));
			}
		}

		// Write test
		if ((t_id != 0) || (t_num == 1)) {
			for (int i = 0; i < rounds; i++) {
				System.out.println(t_id + ", " + t_num + ", Write, " + e6axis + ", " + 2500+i + ", " + timedWrite(e6axis, i));
			}
		}
		
		// Synced write-read test
		if ((t_id == 0)) {
			Boolean hack = e6axis;
			
			if (t_num >= 2)
				hack = true;
			
			for (int i = 0; i < rounds; i++) {
				System.out.println(t_id + ", " + t_num + ", sync_write_read, " + e6axis + ", " + 2500+i + ", " + syncWriteRead(hack, i));
			}		
		}
	}
	public double timedRead(Boolean e6axis) {
		KRLInt read_int = new KRLInt("test1");
		KRLE6Axis read_e6axis = new KRLE6Axis("axis_set");
		
		
		long t0 = System.nanoTime();
		
		try {
			client.readVariable(e6axis ? read_e6axis : read_int);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return (double) (System.nanoTime() - t0) / 1000000000d;
	}
	
	public double timedWrite(Boolean e6axis, int round) {
	  KRLInt write_int = new KRLInt("test1");
	  KRLE6Axis write_e6axis = new KRLE6Axis("axis_tset");
		
	  write_int.setValue(round);
	  write_e6axis.setA1ToA6(0.0, (double) round, 0.0, 0.0, 0.0, 0.0);
	  write_e6axis.setE1ToE6(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
	  
	  long t0 = System.nanoTime();
	  
	  try {
		  client.writeVariable(e6axis ? write_e6axis : write_int);
	  } catch(IOException e) {
		  e.printStackTrace();
	  }
	  return (double) (System.nanoTime() - t0) / 1000000000d;
	}
	
	public double syncWriteRead(Boolean e6axis, int round) {
		KRLInt write_int = new KRLInt("test1");
		KRLInt read_int = new KRLInt("test2");
		KRLE6Axis write_e6axis = new KRLE6Axis("axis_set");
		KRLE6Axis read_e6axis = new KRLE6Axis("axis_test");
		
		write_int.setValue(round);
		write_e6axis.setA1ToA6(0.0, (double) round, 0.0, 0.0, 0.0, 0.0);
		write_e6axis.setE1ToE6(0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
		
		long t0 = System.nanoTime();
		
		try {
			  client.writeVariable(e6axis ? write_e6axis : write_int);
		  } catch(IOException e) {
			  e.printStackTrace();
		  }
		
		
		
	    try {
			client.readVariable(e6axis ? read_e6axis : read_int);
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
	    
		while (true) {
			try {
				client.readVariable(e6axis ? read_e6axis : read_int);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}	
			if (e6axis) {
				if (read_e6axis.getValue().equals(write_e6axis.getValue()))
					break;
				
			} else {
				if (read_int.getValue().equals(write_int.getValue()))
					break;
			}
		}
		return (double) (System.nanoTime() - t0) / 1000000000d;
		
	}
	
	public void connect(String ip, int port) {
		try {
		client = new CrossComClient(ip, port);
		} catch (IOException ex) {
            System.out.println(ex);
            System.out.println("Terminating..");
            System.exit(1);
        }
	}
	
	public void disconnect() {
		try {
			client.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
