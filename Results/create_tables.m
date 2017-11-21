clear all;

filename = ["C++ - Single thread - E6AXIS","C++ - Single thread - INT","Java - single thread - E6AXIS","Java - single thread - INT","C++ - 5 thread - E6AXIS","C++ - 5 thread - INT","Java - 5 thread - E6AXIS","Java - 5 thread - INT"];
lang = {'C++';'C++';'Java';'Java';'C++';'C++';'Java';'Java'};
type = {'E6AXIS';'INT';'E6AXIS';'INT';'E6AXIS';'INT';'E6AXIS';'INT'};
threads = [1,1,1,1,5,5,5,5];

%% Analyze results
for x = 1:numel(filename)
   
   [Test,Maximum,Minimum,Median,Mean,Variance,Deviation] = analyze(char(filename(x)));
   
   Language = [lang(x);lang(x);lang(x)];
   Type = [type(x);type(x);type(x)];
   Threads = [threads(x);threads(x);threads(x)];
   
   if exist("bigtable", "var") == 0
        bigtable = table(Language,Test,Type,Threads,Maximum,Minimum,Median,Mean,Variance,Deviation);
   else
       bigtable = [bigtable;table(Language,Test,Type,Threads,Maximum,Minimum,Median,Mean,Variance,Deviation)];
   end
end

writetable(bigtable, 'Matlab output/Combined table.csv');

vars = {'Language','Type','Threads','Maximum','Minimum','Median','Mean','Deviation'};
type = ["Read","Write","Sync"];

for x = 1:3
    rows = bigtable.Test==type(x);
    t = sortrows(bigtable(rows,vars), [3 2 1],{'ascend' 'descend' 'descend'});
    writetable(t, strcat('Matlab output/', type(x), ' - Table.csv'));
end