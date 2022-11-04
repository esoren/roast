%% variable declaration
clc
for inc = 0:228
    fprintf('int curveDisp%i=0\n',inc);
end


%% display update 
clc
dispId = 1;
ch = 0;

for inc = 0:19
    fprintf('add %i,%i,curveDisp%i\n',dispId,ch,inc);
end    


%% arduino function
clc
for inc = 0:19
    fprintf('\tmyNex.writeNum("curveDisp%i", displayWaveform[%i]);\n', inc, inc);
end    


%% arduino function
clc
for inc = 0:19
    fprintf('\twriteAndConfirmNumber("curveDisp%i", displayWaveform[%i]);\n', inc, inc);
end    