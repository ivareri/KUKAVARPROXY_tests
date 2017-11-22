package serial_echo;

import gnu.io.CommPortIdentifier;
import gnu.io.NoSuchPortException;
import gnu.io.PortInUseException;
import gnu.io.SerialPort;
import gnu.io.UnsupportedCommOperationException;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

public class Main {

	/**
	 * @param args
	 */
	public static void main(String[] args) {

		int num_tests = 1000;
		int timeout = 1000; // ms

		// Set up serial port
		InputStream inputStream = null;
		OutputStream outputStream = null;

		try {
			CommPortIdentifier portIdentifier = CommPortIdentifier
					.getPortIdentifier("/dev/ttyACM0" + "");
			SerialPort serialPort = (SerialPort) portIdentifier.open("Port",
					timeout);
			serialPort.setSerialPortParams(115200, SerialPort.DATABITS_8,
					SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);
			serialPort.enableReceiveTimeout(timeout);
			inputStream = serialPort.getInputStream();
			outputStream = serialPort.getOutputStream();

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

		System.out.println("Test,Time");
		for (int i = 0; i < num_tests; ++i) {
			try {

				// flush input
				while (inputStream.available() != 0) {
					inputStream.read(); // probably smarter way to do this
				}

				// Get start time
				long t0 = System.nanoTime();

				outputStream.write('A');
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

			} catch (IOException e) {
				e.printStackTrace();
				System.exit(1);
			} // try catch
		} // for
	}

}
