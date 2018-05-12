function [test,maximum,minimum,med,expected,variance,deviation, result] = analyze(filename)

%filename = 'C++ - single thread - E6AXIS';
%filename = 'C++ - single thread - INT';
%filename = 'Java - single thread - E6AXIS';
%filename = 'Java - single thread - INT';
%filename = 'C++ - 5 thread - E6AXIS';
%filename = 'C++ - 5 thread - INT';
%filename = 'Java - 5 thread - E6AXIS';
%filename = 'Java - 5 thread - INT';

% Outlier limit
lim_read = 30;
lim_write = 30;
lim_sync = 30;

%% Code

data = importfile(strcat(filename, '.csv'));
%clf;
hold all;


%% Read
figure(1);
clf;
rows = strcmp(data.Testtype, 'Read') & (data.Times*1000 > lim_read);
read_rows = sum(strcmp(data.Testtype, 'Read'));

read_outliers = (data(rows,'Times').Times*1000)';
read_outliers_precent = sum(rows) / read_rows * 100;


rows = strcmp(data.Testtype, 'Read') & (data.Times*1000 < lim_read);
histogram(data(rows,'Times').Times*1000);
maximum(1,1)  = max(data(rows,'Times').Times*1000);
minimum(1,1)   = min(data(rows,'Times').Times*1000);
med(1,1) = median(data(rows,'Times').Times*1000);
expected(1,1) = mean(data(rows,'Times').Times*1000);
variance(1,1) = var(data(rows,'Times').Times*1000);
deviation(1,1) = std(data(rows,'Times').Times*1000);
axis tight;

%print(f, '-deps', strcat(filename, ' - plot read.eps'));

%% Write
%figure(2);
%clf;
rows = strcmp(data.Testtype, 'Write') & (data.Times*1000 > lim_write);
write_rows = sum(strcmp(data.Testtype, 'Write'));
if any(rows)
    write_outliers = (data(rows,'Times').Times*1000)';
    write_outliers_precent = sum(rows) / write_rows * 100;
else
    write_outliers = 0;
    write_outliers_precent = 0;
end

rows = strcmp(data.Testtype, 'Write') & (data.Times*1000 < lim_write);
if any(rows)
histogram(data(rows,'Times').Times*1000);
maximum(2,1)  = max(data(rows,'Times').Times*1000);
minimum(2,1)   = min(data(rows,'Times').Times*1000);
expected(2,1) = mean(data(rows,'Times').Times*1000);
med(2,1) = median(data(rows,'Times').Times*1000);
variance(2,1) = var(data(rows,'Times').Times*1000);
deviation(2,1) = std(data(rows,'Times').Times*1000);
axis tight;
print('-deps', strcat(filename, ' - plot write.eps'));
else
    maximum(3,1) = 0;
    minimum(3,1) = 0;
    expected(3,1) = 0;
    med(3,1) = 0;
    variance(3,1) = 0;
    deviation(3,1) = 0;
end

%% sync_write read
%figure(3);
%clf;
rows = strcmp(data.Testtype, 'sync_write_read') & (data.Times*1000 > lim_sync);
sync_rows = sum(strcmp(data.Testtype, 'sync_write_read'));

if any(rows)
    sync_outliers = (data(rows,'Times').Times*1000)';
    sync_outliers_precent = sum(rows) / sync_rows * 100;
else
    sync_outliers = 0;
    sync_outliers_precent = 0;
end

rows = strcmp(data.Testtype, 'sync_write_read') & (data.Times*1000 < lim_sync);
if any(rows)
    %histogram(data(rows,'Times').Times*1000);
    maximum(3,1)  = max(data(rows,'Times').Times*1000);
    minimum(3,1)   = min(data(rows,'Times').Times*1000);
    expected(3,1) = mean(data(rows,'Times').Times*1000);
    med(3,1) = median(data(rows,'Times').Times*1000);
    variance(3,1) = var(data(rows,'Times').Times*1000);
    deviation(3,1) = std(data(rows,'Times').Times*1000);
    %axis tight;
    %print('-deps', strcat(filename, ' - plot sync.eps'));
else
    maximum(3,1) = 0;
    minimum(3,1) = 0;
    expected(3,1) = 0;
    med(3,1) = 0;
    variance(3,1) = 0;
    deviation(3,1) = 0;
end

%% Output
test = {'Read';'Write';'Sync'};
result = table(test,maximum,minimum,med,expected,variance,deviation);
writetable(result, strcat('Matlab output/',filename, ' - Table.csv'));

outliers = sprintf('%i of %i (%i %%) Read outliers [%dms]: %s \n%i of %i (%i %%) Write outliers [%dms]: %s \n%i of %i(%i %%) Sync outliers [%dms]: %s \n', ...
                numel(read_outliers), ...
                read_rows, ...
                read_outliers_precent, ...
                lim_read, mat2str(read_outliers), ...
                numel(write_outliers), ...
                write_rows, ...
                write_outliers_precent, ...
                lim_write, mat2str(write_outliers), ...
                numel(sync_outliers), ...
                sync_rows, ...
                sync_outliers_precent, ...
                lim_sync, mat2str(sync_outliers));
            
fid = fopen(strcat('Matlab output/',filename, ' - Outliers.txt'),'w');
fprintf(fid, outliers);
fclose(fid);


%% Print stuff to terminal
result
outliers

