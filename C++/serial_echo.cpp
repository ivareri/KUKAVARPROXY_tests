#include <string>
#include <boost/asio/serial_port.hpp> 
#include <boost/asio.hpp> 
#include <boost/timer/timer.hpp>
#include "boost/lexical_cast.hpp"

#include "blocking_reader.h"


int main(int argc, char **argv) {

	// Setup
	char usb[] = "/dev/ttyACM0";
	int baud = 115200;
	int timeout = 1000; // ms
	int num_tests = 200;
	

	// Declare variables
	boost::asio::io_service io;
	boost::asio::serial_port port(io);
  char c = 'A';
	
	// Connect to serial port	
	port.open(usb);
	port.set_option(boost::asio::serial_port_base::baud_rate(baud));
	blocking_reader reader(port, timeout);
	

	std::cout << "Test,Time" << std::endl;

	for (int i=0; i < num_tests; i++) {
		// clear serial buffer 
		while ( reader.read_char(c) ) {}

		// Timer output. 
		std::string format = boost::lexical_cast<std::string>(i) + ",%w\n";

		// start timer
    char d;
		std::cerr << "Writing byte" << std::endl;
		boost::timer::auto_cpu_timer t(9, format);
    // Write byte
    port.write_some(boost::asio::buffer(&c, 1));

		// read back
		reader.read_char(d);

		// stop timer
		t.stop();

		// Check if read timed out
		if (d != c) {
			// Serial read must have timed out.
			std::cout << "Read timed out!" << std::endl;
		} else {
			// save result
			t.report();
			std::cout.flush();
		}

	} // end for loop

}
