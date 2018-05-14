To compile:
`g++ -o crosscom_latency crosscom_latency.cpp -std=c++11 -lboost_timer -lboost_system -lboost_thread -lboost_program_options`

`g++ -o move_latency move_latency.cpp -std=c++11 -lboost_timer -lboost_system -lboost_thread -lboost_program_options`


Output:

Move latency test writes some debug output to stderr. Pipe only stdout to a textfile to get pure CSV output.

