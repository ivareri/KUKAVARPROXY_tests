
Max = max(pingresult.Time);
Min = min(pingresult.Time);
Median = median(pingresult.Time);
Mean = mean(pingresult.Time);
Deviation = std(pingresult.Time);

out = table(Max,Min,Median,Mean,Deviation);
writetable(out, 'Matlab output/ping_times.csv');
