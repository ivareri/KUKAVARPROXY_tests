clear all;

%filename = 'C++ - single thread - E6AXIS';
%filename = 'C++ - single thread - INT'; %% fail
%filename = 'Java - single thread - E6AXIS';
%filename = 'Java - single thread - INT'; %% fail
%filename = 'C++ - 5 thread - E6AXIS';
%filename = 'C++ - 5 thread - INT'; %% fail
%filename = 'Java - 5 thread - E6AXIS';
%filename = 'Java - 5 thread - INT';

lim_read = 100;
lim_write = 100;
lim_sync = 100;

%% Code
data = importfile(strcat(filename, '.csv'));
clf;
figure(1);
clf;
hold all;


%% Read
rows = strcmp(data.Testtype, 'Read') & (data.Times*1000 > lim_read);
read_outliers = (data(rows,'Times').Times*1000)';

rows = strcmp(data.Testtype, 'Read') & (data.Times*1000 < lim_read);
histogram(data(rows,'Times').Times*1000);
maximum(1,1)  = max(data(rows,'Times').Times*1000);
minimum(1,1)   = min(data(rows,'Times').Times*1000);
med(1,1) = median(data(rows,'Times').Times*1000);
expected(1,1) = mean(data(rows,'Times').Times*1000);
variance(1,1) = var(data(rows,'Times').Times*1000);
axis tight;

print('-deps', strcat(filename, ' - plot read.eps'));

%% Write
figure(2);
clf;
rows = strcmp(data.Testtype, 'Write') & (data.Times*1000 > lim_write);
write_outliers = (data(rows,'Times').Times*1000)';

rows = strcmp(data.Testtype, 'Write') & (data.Times*1000 < lim_write);
histogram(data(rows,'Times').Times*1000);
maximum(2,1)  = max(data(rows,'Times').Times*1000);
minimum(2,1)   = min(data(rows,'Times').Times*1000);
expected(2,1) = mean(data(rows,'Times').Times*1000);
med(2,1) = median(data(rows,'Times').Times*1000);
variance(2,1) = var(data(rows,'Times').Times*1000);
axis tight;
print('-deps', strcat(filename, ' - plot write.eps'));


%% sync_write read
figure(3);
clf;
rows = strcmp(data.Testtype, 'sync_write_read') & (data.Times*1000 > lim_sync);
sync_outliers = (data(rows,'Times').Times*1000)';

rows = strcmp(data.Testtype, 'sync_write_read') & (data.Times*1000 < lim_sync);
histogram(data(rows,'Times').Times*1000);
maximum(3,1)  = max(data(rows,'Times').Times*1000);
minimum(3,1)   = min(data(rows,'Times').Times*1000);
expected(3,1) = mean(data(rows,'Times').Times*1000);
med(3,1) = median(data(rows,'Times').Times*1000);
variance(3,1) = var(data(rows,'Times').Times*1000);
axis tight;
print('-deps', strcat(filename, ' - plot sync.eps'));

%% Output
type = {'Read';'Write';'Sync'};
result = table(type,maximum,minimum,med,expected,variance);
writetable(result, strcat(filename, ' - Table.csv'));

outliers = sprintf('Read outliers [%d]: %s \nWrite outliers [%d]: %s \nSync outliers [%d]: %s \n', ...
                lim_read, mat2str(read_outliers), ...
                lim_write, mat2str(write_outliers), ...
                lim_sync, mat2str(sync_outliers));
            
fid = fopen(strcat(filename, ' - Outliers.txt'),'w');
fprintf(fid, outliers);
fclose(fid);

%% Print stuff to terminal
result
outliers