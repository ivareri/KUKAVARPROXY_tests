#include <vector>
#include <string>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <boost/timer/timer.hpp>
#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>
#include "BoostClientCross.h"
#include <boost/algorithm/string.hpp>

#include <boost/program_options.hpp>
namespace po = boost::program_options;


boost::mutex mutex;

void printvar(std::vector<unsigned char> var) {
	for(auto it = var.begin(); it != var.end(); it++) {
		std::cout << "'" << *it << "'";
	}
	std::cout << std::endl;
}
bool timed_read(BoostClientCross *clientcross, int round, int id = 0,
                bool e6axis = false, int num_threads = 1) {

  std::vector<unsigned char> read_from;
  if (e6axis)
    read_from = { '$', 'A', 'X', 'I', 'S', '_', 'A', 'C', 'T' };

  if (!e6axis)
    read_from = { 'T', 'E', 'S', 'T', '1' };

  std::string format = boost::lexical_cast<std::string>(id) + ", " +
                       boost::lexical_cast<std::string>(num_threads) +
                       ", Read, " + boost::lexical_cast<std::string>(e6axis) +
                       ", " + boost::lexical_cast<std::string>(round) +
                       ", %w\n";
  // Start timer
  boost::timer::auto_cpu_timer t(9, format);
  std::vector<unsigned char> formated_read =
      clientcross->formatReadMsg(read_from, id);
  std::vector<unsigned char> reply = clientcross->sendMsg(formated_read);
  t.stop();
  // Stop timer

  mutex.lock();
  t.report();
  std::cout.flush();
  mutex.unlock();
  return true;
}

bool timed_write(BoostClientCross *clientcross, int round, int id = 0,
                 bool e6axis = false, int num_threads = 1) {

  std::vector<unsigned char> write_to;
  std::string out;

  if (e6axis) {
    write_to = { 'A', 'X', 'I', 'S', '_', 'T', 'S', 'E', 'T' };
    out = "{E6AXIS: A1 0.0, A2 0.0, A3 0.0, A4 " +
          boost::lexical_cast<std::string>(round%200) +
          ".0, A5 0.0, A6 0.0, E1 0.0, E2 0.0, E3 0.0, E4 0.0, E5 "
          "0.0, E6 0.0}";
  } else {
    write_to = { 'T', 'E', 'S', 'T', '1' };
    out = boost::lexical_cast<std::string>(round);
  }

  std::vector<unsigned char> out_vector(out.begin(), out.end());
  std::string format = boost::lexical_cast<std::string>(id) + ", " +
                       boost::lexical_cast<std::string>(num_threads) +
                       ", Write, " + boost::lexical_cast<std::string>(e6axis) +
                       ", " + boost::lexical_cast<std::string>(round) +
                       ", %w\n";

  // Start timer
  boost::timer::auto_cpu_timer t(9, format);
  std::vector<unsigned char> formated_out =
      clientcross->formatWriteMsg(write_to, out_vector, id);
  std::vector<unsigned char> reply = clientcross->sendMsg(formated_out);
  t.stop();
  // Stop timer

  mutex.lock();
  t.report();
  std::cout.flush();
  mutex.unlock();
  return true;
}

bool sync_write_read(BoostClientCross *clientcross, int round, int id = 0,
                     bool e6axis = false, int num_threads = 1) {

  std::vector<unsigned char> write_to;
  std::vector<unsigned char> read_from;
  std::string out;

  if (e6axis) {
    write_to = { 'A', 'X', 'I', 'S', '_', 'S', 'E', 'T','x' };
    read_from = { 'A', 'X', 'I', 'S', '_', 'T', 'E', 'S', 'T','x' };
    out = "{E6AXIS: A1 " + boost::lexical_cast<std::string>(int(round%10)) +
	  ".0, A2 0.0, A3 0.0, A4 "
          "0.0, A5 0.0, A6 0.0, E1 0.0, E2 0.0, E3 0.0, E4 0.0, E5 "
          "0.0, E6 0.0}";

  } else {
    write_to = { 'T', 'E', 'S', 'T', '1','x' };
    read_from = { 'T', 'E', 'S', 'T', '2','x' };
    out = boost::lexical_cast<std::string>(round);
  }

  std::vector<unsigned char> out_vector(out.begin(), out.end());

  std::string format = boost::lexical_cast<std::string>(id) + ", " +
                       boost::lexical_cast<std::string>(num_threads) +
                       ", sync_write_read, " +
                       boost::lexical_cast<std::string>(e6axis) + ", " +
                       boost::lexical_cast<std::string>(round) + ", %w\n";

  // Start timer
  boost::timer::auto_cpu_timer t(9, format);

  // Write value
  std::vector<unsigned char> formated_out =
      clientcross->formatWriteMsg(write_to, out_vector, id);
  std::vector<unsigned char> reply_write = clientcross->sendMsg(formated_out);

  // read back value and compare
  std::vector<unsigned char> formated_read =
      clientcross->formatReadMsg(read_from, id);
  std::vector<unsigned char> reply_read = clientcross->sendMsg(formated_read);
 
  if (e6axis) {
	  while (reply_read[12] != boost::lexical_cast<unsigned char>(round%10)) {
	    reply_read = clientcross->sendMsg(formated_read);
	  }
  } else {
	while(boost::lexical_cast<int>(reply_read.data()) != round) {
 	  reply_read = clientcross->sendMsg(formated_read);
	}
  }

  t.stop();
  // Stop timer

  mutex.lock();
  t.report();
  std::cout.flush();
  mutex.unlock();
  return true;
}

void *thread_worker(boost::barrier &cur_barier, int t_id, int num_threads, int num_tests, bool e6axis) {

  // Connect to KRC
  std::string ip = "10.0.0.101";
  std::string port = "7000";
  BoostClientCross clientcross;
  clientcross.connectSocket(ip, port);

  mutex.lock();
//  std::cout << "Worker " << t_id << " started" << std::endl;
  mutex.unlock();
  // synchronize with other threads
  cur_barier.wait();

  // Read test
  if ((num_threads == 1) || (t_id != 0)) {
    for (int i = 0; i < num_tests; i++) {
      timed_read(&clientcross, i, t_id, e6axis, num_threads);
    }
  }

  // write test
  if ((num_threads == 1) || (t_id != 0)) {
    for (int i = 0; i < num_tests; i++) {
      timed_write(&clientcross, i, t_id, e6axis, num_threads);
    }
  }

  // sync_write_read
  if (t_id == 0) {
    for (int i = 0; i < num_tests; i++) {
      sync_write_read(&clientcross, i, t_id, e6axis, num_threads);
    }
  }

  // Disconnect
  clientcross.disconnectSocket();
}

int main(int argc, char **argv) {
  // Set defaults
  int num_threads = 1;
  int num_tests = 100;
  bool e6axis = false;
  // Parse options
  try {
    po::options_description desc("Allowed options");
    desc.add_options()
      ("help,h", "produce help message")
      ("num_threads,t", po::value<int>(), "set number of threads, default=1")
      ("num_tests,r", po::value<int>(), "set number of tests, default=100")
      ("e6axis,e", po::value<bool>(), "true is e6axis, false is int, default=false");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    if (vm.count("help")) {
      std::cout << desc << std::endl;
      return 0;
    }
    if (vm.count("num_threads")) {
      num_threads = vm["num_threads"].as<int>();
    }
    if (vm.count("num_tests")) {
      num_tests = vm["num_tests"].as<int>();
    } 
    if (vm.count("e6axis")) {
      e6axis = vm["e6axis"].as<bool>();
    }
  }
  catch(std::exception& e) {
    std::cerr << "error:" << e.what() << "\n";
    return 1;
  }
  std::cout << "starting workers" << std::endl;

  boost::thread threads[num_threads];
  boost::barrier bar(num_threads + 1);

  for (int i = 0; i < num_threads; i++) {
    threads[i] = boost::thread(boost::bind(thread_worker, boost::ref(bar), i, num_threads, num_tests, e6axis));
  }

  mutex.lock();
  std::cout << "starting test" << std::endl;
  std::cout << "Thread, # threads, Test type, e6axis, Test #, Time (s)"
            << std::endl;
  mutex.unlock();

  bar.wait();

  for (int i = 0; i < num_threads; i++) {
    threads[i].join();
  }

  return 0;
}
