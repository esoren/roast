

%% User Parameters
duration            = 600;
start_val           = 100;
end_val             = 234;
bend                = -40;
bend_peak_time      = 100;

%% Intermediate Calculations

bend_peak_pre_dur = bend_peak_time;
bend_peak_post_dur = duration-bend_peak_time;

bend_pre_increment = bend/bend_peak_pre_dur;
bend_post_increment = -1*bend/bend_peak_post_dur;

span = end_val-start_val; 
slope = span/duration;

bend_addval = 0;
bend_state = 0; %pre
for inc = 1:duration
    
   
    if(bend_state == 0) %pre
   
        bend_addval = bend_addval + bend_pre_increment;
        if(bend > 0) 
            if(bend_addval >= bend)
                bend_state = 1;
            end
        else
            if(bend_addval <= bend)
                bend_state = 1;
            end
        end
        
    else
        bend_addval = bend_addval + bend_post_increment;       
    end
    
    curve(inc) = start_val + slope*inc + bend_addval;
    
end

%%

figure(1); clf; 
plot(1:duration, curve);
grid on;