package jopenshowvar_latency;

import java.io.IOException;

import no.hials.crosscom.CrossComClient;
import no.hials.crosscom.KRL.*;

public class Main {
	CrossComClient client = null;
	
	public void main(String[] args) {
		// TODO Auto-generated method stub
		connect("10.0.0.1", 7000);
		System.out.println("");
		
		
		try {
			client.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}
	
	private double timedRead(String var) {
		KRLInt read = new KRLInt(var);
		
		try {
			client.readVariable(read);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		return read.getReadTimeSec();
	}
	
	private double timedWrite(String var, int round) {
	  KRLInt write = new KRLInt(var);
	  write.setValue(round);
	  try {
		  client.writeVariable(write);
	  } catch(IOException e) {
		  e.printStackTrace();
	  }
	  return write.getReadTimeSec();
	}
	
	private double syncWriteRead(String var1, String var2, int round) {
		KRLInt write = new KRLInt(var1);
		KRLInt read = new KRLInt(var2);
		double time;
		
		try {
			  client.writeVariable(write);
		  } catch(IOException e) {
			  e.printStackTrace();
		  }
		
		time = write.getReadTimeSec();
		
		while (read.getValue() != write.getValue()) {
			try {
				client.readVariable(read);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}	
			time += read.getReadTimeSec();
		}
		return time;
	}
	
	private void connect(String ip, int port) {
		try {
		client = new CrossComClient(ip, port);
		} catch (IOException ex) {
            System.out.println(ex);
            System.out.println("Terminating..");
            System.exit(1);
        }
	}

}
