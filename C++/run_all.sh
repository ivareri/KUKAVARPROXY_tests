#!/bin/bash
# Run all crosscom_latency tests
echo "Crosscom_latency"
if [ -f "./crosscom_latency" ]
then
    echo "crosscom_latency already compiled"
else
    echo "Compiling crosscom_latency"
    g++ crosscom_latency.cpp -o crosscom_latency -std=c++11 -lboost_thread -lboost_system -lboost_timer -lboost_program_options
fi

echo "Running int test, 1 thread, 50 000 tests"
./crosscom_latency -t 1 -r 50000 -e 0 > ../Results/raw_data/crosscom_latency_int_1_thread_50ktests.csv
echo "Running int test, 5 threads, 50 000 tests"
./crosscom_latency -t 5 -r 50000 -e 0 > ../Results/raw_data/crosscom_latency_int_5_thread_50ktests.csv
echo "Running e6axis test, 1 thread, 50 000 tests"
./crosscom_latency -t 1 -r 50000 -e 1 > ../Results/raw_data/crosscom_latency_e6axis_1_thread_50ktests.csv
echo "Running e6axis test, 5 threads, 50 000 tests"
./crosscom_latency -t 5 -r 50000 -e 1 > ../Results/raw_data/crosscom_latency_e6axis_5_thread_50ktests.csv

# Run all move_latency tests
echo "Move_latency"
echo "WARNING! You acknowledge that you have one hand on the E-STOP!"
echo "And that you are vigilant of any trouble!"
read answer
if [ -f "./move_latency" ]
then
    echo "move_latency already compiled"
else
    echo "Compiling move-latency"
    g++ move_latency.cpp -o move_latency -std=c++11 -lboost_system -lboost_thread -lboost_timer -lboost_program_options
fi

echo "Running 5000 move latency tests"
./move_latency --num_tests 5000
