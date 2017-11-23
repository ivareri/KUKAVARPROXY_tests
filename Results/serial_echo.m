
clear all;

%% Java

filename = 'Raw data/Java serial echo';
java = move_importfile(strcat(filename, '.csv'));

% plot
figure(1);
clf;
histogram((java(:,'Time').Time-0.500)*1000);
axis tight;

%statistics
Max(1,1)  = max((java(:,'Time').Time-0.500)*1000);
Min(1,1)   = min((java(:,'Time').Time-0.500)*1000);
Median(1,1) = median((java(:,'Time').Time-0.500)*1000);
Mean(1,1) = mean((java(:,'Time').Time-0.500)*1000);
Var(1,1) = var((java(:,'Time').Time-0.500)*1000);
StdDev(1,1) = std((java(:,'Time').Time-0.500)*1000);



%% C++

filename = 'Raw data/c++_serial_echo';
cpp = move_importfile(strcat(filename, '.csv'));

% plot
figure(2);
clf;
histogram((cpp(:,'Time').Time-0.500)*1000);
axis tight;

% statistics
Max(2,1)  = max((cpp(:,'Time').Time-0.500)*1000);
Min(2,1)   = min((cpp(:,'Time').Time-0.500)*1000);
Median(2,1) = median((cpp(:,'Time').Time-0.500)*1000);
Mean(2,1) = mean((cpp(:,'Time').Time-0.500)*1000);
Var(2,1) = var((cpp(:,'Time').Time-0.500)*1000);
StdDev(2,1) = std((cpp(:,'Time').Time-0.500)*1000);


%% Output
Language = {'Java';'C++'};
result = table(Language,Max,Min,Median,Mean,StdDev)
writetable(result, 'Matlab output/serial_echo.csv');