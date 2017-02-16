#include <string>
#include <boost/asio/serial_port.hpp> 
#include <boost/asio.hpp> 
#include <boost/timer/timer.hpp>

#include "blocking_reader.h"
#include "BoostClientCross.h"

int robotMoving(BoostClientCross *robot) {
  std::string var("$PRO_ACT");
  std::vector<unsigned char> read_from(var.begin(), var.end());

  std::vector<unsigned char> formated_read = robot->formatReadMsg(read_from, 1);
  std::vector<unsigned char> reply = robot->sendMsg(formated_read);

  if (reply.data() == (unsigned char*) "true") 
	return true;
  
  return false;

}

void printPosition(BoostClientCross *robot) {
  std::string var("$AXIS_ACT");
  std::vector<unsigned char> read_from(var.begin(), var.end());

  std::vector<unsigned char> formated_read = robot->formatReadMsg(read_from, 1);
  std::vector<unsigned char> reply = robot->sendMsg(formated_read);
  std::cout << "Position: " << reply.data() << std::endl;
}
 
int main(int argc, char **argv) {

	// Setup
	char usb[] = "/dev/ttyACM0";
	int baud = 38400;
	int timeout = 100; // ms
	std::string kuka_ip = "10.0.0.1";
	std::string  kuka_port = "7000";
	int num_tests = 100;
	

	// Declare variables
	boost::asio::io_service io;
	boost::asio::serial_port port(io);
	BoostClientCross robot;
  	char c;
	
	// Connect to serial port	
	port.open(usb);
	port.set_option(boost::asio::serial_port_base::baud_rate(baud));
	blocking_reader reader(port, timeout);
	
	// connect to robot
  	robot.connectSocket(kuka_ip, kuka_port);
	
	// Set up robot movement
	std::string var("AXIS_SET");
	std::vector<unsigned char> write_to(var.begin(), var.end());

	std::string out;
	out = "{E6AXIS: A1 -31.95492, A2 -103.7478, A3 104.9128, A4 -3.821331E-04, A5 -3.800799E-04, A6 5.252814E-03, E1 1153.259, E2 2800.001, E3 -749.9890, E4 0.0, E5 0.0, E6 0.0}";
	std::vector<unsigned char> out_vector1(out.begin(), out.end());
	std::vector<unsigned char> position1 = robot.formatWriteMsg(write_to, out_vector1, 1);

	
	out = "{E6AXIS: A1 -31.95387, A2 -98.78010, A3 101.0818, A4 3.209918E-02, A5 -1.140894, A6 -2.575621E-02, E1 1153.259, E2 2800.001, E3 -749.9890, E4 0.0, E5 0.0, E6 0.0}";
	std::vector<unsigned char> out_vector2(out.begin(), out.end());
	std::vector<unsigned char> position2 = robot.formatWriteMsg(write_to, out_vector2, 2);

	std::cout << "Test,Time" << std::endl;

	for (int i=0; i < num_tests; i++) {
		// check robot not moving
		while ( robotMoving(&robot) ) {}

		// clear serial buffer 
		while ( reader.read_char(c) ) {}

		// Timer output. 
		std::string format = boost::lexical_cast<std::string>(i) + ",%w\n";

		// start timer
		boost::timer::auto_cpu_timer t(9, format);
		std::cerr << "Moving robot" << std::endl;
		// move robot pos[test_num%2]
  		if ( i % 2 ) {
			robot.sendMsg(position1);
		} else {

			 robot.sendMsg(position2);
		}

		// read imu
		reader.read_char(c);

		// stop timer
		t.stop();

		// Check if read timed out
		if (c != '\n') {
			// Serial read must have timed out.
			std::cout << "Read timed out!" << std::endl;
			// wait for keypress
			std::cin.ignore();
		} else {
			// save result
			t.report();
			std::cout.flush();
		}

	} // end for loop

	// close serialport
 	// close robot
	robot.disconnectSocket();
	// happy days
}
