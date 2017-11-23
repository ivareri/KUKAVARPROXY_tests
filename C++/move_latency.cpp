#include <string>
//#include <boost/asio/serial_port.hpp> 
//#include <boost/asio.hpp> 
#include <boost/timer/timer.hpp>
#include <chrono>
#include <thread>

#include "posix_serial.c"
//#include "blocking_reader.h"
#include "BoostClientCross.h"

int robotMoving(BoostClientCross *robot) {
  std::string var("$ROB_STOPPED");
  std::vector<unsigned char> read_from(var.begin(), var.end());

  std::vector<unsigned char> formated_read = robot->formatReadMsg(read_from, 1);
  std::vector<unsigned char> reply = robot->sendMsg(formated_read);

  std::vector<unsigned char> test {'T','R','U'};

  if (reply == test) 
	  return false;
  
  return true;

}

void printPosition(BoostClientCross *robot) {
  std::string var("$AXIS_ACT");
  std::vector<unsigned char> read_from(var.begin(), var.end());

  std::vector<unsigned char> formated_read = robot->formatReadMsg(read_from, 1);
  std::vector<unsigned char> reply = robot->sendMsg(formated_read);
  std::cerr << "Position: " << reply.data() << std::endl;
}
 
int main(int argc, char **argv) {

	// Setup
	char usb[] = "/dev/ttyACM0";
	int baud = 115200;
	int timeout = 1000; // ms
	std::string kuka_ip = "10.0.0.1";
	std::string  kuka_port = "7000";
	int num_tests = 300;
	

	// Declare variables
	boost::asio::io_service io;
	boost::asio::serial_port port(io);
	BoostClientCross robot;
  unsigned char c[1];
	
	// Connect to serial port	
	//port.open(usb);
	//port.set_option(boost::asio::serial_port_base::baud_rate(baud));
  //	blocking_reader reader(port, timeout);
  int fd;

  const char *port_name = "/dev/ttyACM0";
  fd = open_serial(port_name);	

	// connect to robot
  robot.connectSocket(kuka_ip, kuka_port);
	
	// Set up robot movement
	std::string var("AXIS_SET");
	std::vector<unsigned char> write_to(var.begin(), var.end());

	std::string out;
  out = "{E6AXIS: A1 -31.95492, A2 -98.78010, A3 101.0818, A4 0, A5 0, A6 0, E1 1153.259, E2 2800.001, E3 -749.9890, E4 0.0, E5 0.0, E6 0.0}";
  std::vector<unsigned char> out_vector1(out.begin(), out.end());
  std::vector<unsigned char> position1 = robot.formatWriteMsg(write_to, out_vector1, 1);

  out = "{E6AXIS: A1 -31.95387, A2 -98.78010, A3 101.0818, A4 28.0, A5 32.0, A6 35.0, E1 1153.259, E2 2800.001, E3 -749.9890, E4 0.0, E5 0.0, E6 0.0}";
	std::vector<unsigned char> out_vector2(out.begin(), out.end());
	std::vector<unsigned char> position2 = robot.formatWriteMsg(write_to, out_vector2, 2);


  // Move robot to inital position
  std::cerr << "Moving to start point" << std::endl;
  robot.sendMsg(position1);

	std::cout << "Test,Time" << std::endl;
  
	for (int i=0; i < num_tests; i++) {
		// check robot not moving
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		while ( robotMoving(&robot) ) {}

		// clear serial buffer 
	//	while ( reader.read_char(c) ) {}
   while (read(fd,c,1) > 0) {}

		// Timer output. 
		std::string format = boost::lexical_cast<std::string>(i) + ",%w\n";

    unsigned char d[1] = {'\0'};
		std::cerr << "Moving robot" << std::endl;
    std::cerr.flush();
    std::cout.flush();

		// start timer
		boost::timer::auto_cpu_timer t(9, format);

		// move robot pos[test_num%2]
 		if ( i % 2 ) {
			robot.sendMsg(position1);
		} else {
			robot.sendMsg(position2);
		}

		// read imu
  //	reader.read_char(d);
  //    boost::asio::read(port, boost::asio::buffer(&d, 1));
    
    while (read(fd,d,1) == -1) {}
		// stop timer
		t.stop();

		// Check if read timed out
		if (d[0] != 'A') {
			// Serial read must have timed out.
			std::cout << "Read timed out!" << d[0] << std::endl;
			// wait for keypress
		} else {
			// save result
			t.report();
      std::cerr << "Got byte :" << d[0] << std::endl;
      std::cerr.flush();
			std::cout.flush();
		}

	} // end for loop

	// close serialport
  port.close();
 	// close robot
	robot.disconnectSocket();
	// happy days
}
