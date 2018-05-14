#include <string>
#include <boost/asio/serial_port.hpp> 
#include <boost/asio.hpp> 
#include <boost/timer/timer.hpp>
#include "boost/lexical_cast.hpp"

#include "blocking_reader.h"
#include <boost/program_options.hpp>
namespace po = boost::program_options;


int main(int argc, char **argv) {
  /// Parse options
  try{
    po::options_description desc("Allowed options");
    desc.add_options()
      ("help", "produce help message")
      ("timeout", po::value<int>(), "set timeout length (ms), default=1000")
      ("num_tests", po::value<int>(), "set number of tests, default=100")
      ("baudrate", po::value<int>(), "set baudrate, default=115200");
     po::variables_map vm;
     po::store(po::parse_command_line(argc, argv, desc), vm);
     po::notify(vm);
     if (vm.count("help")) {
       std::cout << desc << std::endl;
       return 0;
     }
     
     if (vm.count("timeout")) {
       int timeout = vm["timeout"].as<int>();
     } else {
       int timeout = 1000;
     }
     if (vm.count("num_tests")) {
       int num_tests = vm["num_tests"].as<int>();
     } else {
       int num_tests = 100;
     }
     if (vm.count("baudrate")) {
       int baud = vm["baudrate"].as<int>();
     } else {
       int baud = 115200;
     }
  }
  catch(exception& e){
    std::cerr <<  "error:" << e.what() << "\n";
    return 1;
  }
  catch(...){
    std::cerr << "Exception of unknown type!";
    return 1;
  }

  // Setup
  char usb[] = "/dev/ttyACM0";
  
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
