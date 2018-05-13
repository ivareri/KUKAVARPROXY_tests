#include <vector>
#include <string>
#include <iostream>
#include <boost/tokenizer.hpp>
#include <boost/timer/timer.hpp>
#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>
#include "BoostClientCross.h"
#include <boost/algorithm/string.hpp>

#define NUM_THREADS 1
#define READ_NUM 100
#define WRITE_NUM 100
#define SYNC_NUM 100
#define E6AXIS 1

boost::mutex mutex;

void printvar(std::vector<unsigned char> var) {
	for(auto it = var.begin(); it != var.end(); it++) {
		std::cout << "'" << *it << "'";
	}
	std::cout << std::endl;
}
bool timed_read(BoostClientCross *clientcross, int round, int id = 0,
                bool e6axis = false) {

  std::vector<unsigned char> read_from;
  if (e6axis)
    read_from = { '$', 'A', 'X', 'I', 'S', '_', 'A', 'C', 'T' };

  if (!e6axis)
    read_from = { 'T', 'E', 'S', 'T', '1' };

  std::string format = boost::lexical_cast<std::string>(id) + ", " +
                       boost::lexical_cast<std::string>(NUM_THREADS) +
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
                 bool e6axis = false) {

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
                       boost::lexical_cast<std::string>(NUM_THREADS) +
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
                     bool e6axis = false) {

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
                       boost::lexical_cast<std::string>(NUM_THREADS) +
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

void *thread_worker(boost::barrier &cur_barier, int t_id) {

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
  if ((NUM_THREADS == 1) || (t_id != 0)) {
    for (int i = 0; i < READ_NUM; i++) {
      timed_read(&clientcross, i, t_id, E6AXIS);
    }
  }

  // write test
  if ((NUM_THREADS == 1) || (t_id != 0)) {
    for (int i = 0; i < WRITE_NUM; i++) {
      timed_write(&clientcross, i, t_id, E6AXIS);
    }
  }

  // sync_write_read
  if (t_id == 0) {
    for (int i = 0; i < SYNC_NUM; i++) {
      sync_write_read(&clientcross, i, t_id, E6AXIS);
    }
  }

  // Disconnect
  clientcross.disconnectSocket();
}

int main() {

  std::cout << "starting workers" << std::endl;

  boost::thread threads[NUM_THREADS];
  boost::barrier bar(NUM_THREADS + 1);

  for (int i = 0; i < NUM_THREADS; i++) {
    threads[i] = boost::thread(boost::bind(thread_worker, boost::ref(bar), i));
  }

  mutex.lock();
  std::cout << "starting test" << std::endl;
  std::cout << "Thread, # threads, Test type, e6axis, Test #, Time (s)"
            << std::endl;
  mutex.unlock();

  bar.wait();

  for (int i = 0; i < NUM_THREADS; i++) {
    threads[i].join();
  }

  return 0;
}
