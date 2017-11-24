%filename = '1_thread_500000_int';
filename = '1_thread_500000_e6axis';
%filename = '5_thread_100000_int';
%filename = 'C++ - 5 thread - INT';
%filename = 'C++ - Single thread - READ INT';

[test,maximum,minimum,med,expected,variance,deviation, data] = analyze(filename);