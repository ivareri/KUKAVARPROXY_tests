package move_test;

import no.hials.crosscom.CrossComClient;
import no.hials.crosscom.KRL.structs.KRLE6Axis;
import no.hials.crosscom.KRL.KRLBool;


import java.io.IOException;
import java.io.InputStream;
import java.util.concurrent.TimeUnit;

import gnu.io.CommPortIdentifier;
import gnu.io.NoSuchPortException;
import gnu.io.PortInUseException;
import gnu.io.SerialPort;
import gnu.io.UnsupportedCommOperationException;

public class Main {
	 
	/**
	 * @param args
	 */
	public static void main(String[] args) {

	   int num_tests = 1000;
	   int timeout = 1000; //ms
	   String kuka_ip = "10.0.0.1";
	   KRLE6Axis pos1 = new KRLE6Axis("AXIS_SET");
	   KRLE6Axis pos2 = new KRLE6Axis("AXIS_SET");
	   KRLBool rob_stopped = new KRLBool("$ROB_STOPPED");
	   
	   pos1.setA1ToA6(-31.95492, -98.78010, 101.0818, 0, 0, 0);
	   pos1.setE1ToE6(1153.259, 2800.001, -749.9890, 0.0, 0.0, 0.0);
	   
	   pos2.setA1ToA6(-31.95387, -98.78010, 101.0818, 28.0, 32.0, 35.0);
	   pos2.setE1ToE6(1153.259, 2800.001, -749.9890, 0.0, 0.0, 0.0);
	   
	   // Set up serial port
	   InputStream inputStream = null;
	try {
		CommPortIdentifier portIdentifier = CommPortIdentifier.getPortIdentifier("/dev/ttyACM0" +
				"");
		SerialPort serialPort = (SerialPort) portIdentifier.open("Port", timeout);
		serialPort.setSerialPortParams(115200, SerialPort.DATABITS_8, SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);
		serialPort.enableReceiveTimeout(timeout);
		inputStream = serialPort.getInputStream();
		System.out.println("Timeout :" + serialPort.getReceiveTimeout());
	
	} catch (NoSuchPortException e2) {
		// TODO Auto-generated catch block
		e2.printStackTrace();
		System.exit(1);

	} catch (PortInUseException e1) {
		
		// TODO Auto-generated catch block
		e1.printStackTrace();
		System.exit(1);
	} catch (UnsupportedCommOperationException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		System.exit(1);	
	} catch (IOException e1) {
		// TODO Auto-generated catch block
		e1.printStackTrace();
		System.exit(1);
	}
	   
	
	// Set up KVP
	CrossComClient robot = null;
	
	try {
		robot = new CrossComClient(kuka_ip, 7000);
	} catch ( IOException e1) {
		// TODO Auto-generated catch block
		e1.printStackTrace();
	}
	
	
	
	// Run tests
		System.out.println("Test,Time");
	   for (int i=0; i < num_tests; ++i) {
	        try {
	        	
	        	
	        	// Wait for move to complete
	        	TimeUnit.SECONDS.sleep(1);
	        	do {
	        		robot.readVariable(rob_stopped);
	        	}	while (!rob_stopped.getValue());
	        	
	        	// flush input
	        	while(inputStream.available()!=0){
	                inputStream.read(); // probably smarter way to do this
	            }
	        	
	        	// Get start time
	        	long t0 = System.nanoTime();
	        	
	        	// send movement
	        	if (i % 2 == 0 ) {
	        		robot.writeVariable(pos1);
	        	} else {
	        		robot.writeVariable(pos2);
	        	}
	        	
	            // Blocking read
	            byte input = (byte) inputStream.read();	            
	            
	            // Get stop time
	            double elapsed_time = (System.nanoTime() - t0) / 1000000000d;
	            
	            // Print result
	            if (elapsed_time >= timeout / 1000) {
	            	System.out.println(i + ",Timed out");
	            } else {
	            	System.out.println(i + "," + elapsed_time);
	            }
	            
	        } catch (IOException | InterruptedException e) {
	        	e.printStackTrace();
	        	try {
					robot.close();
				} catch (IOException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
	        	System.exit(1);
	        } // try catch
	} // for
	try {
		robot.close();
	} catch (IOException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
	}
	} // Main
}
