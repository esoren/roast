

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


%% solution that can be run on an individual point




curveStartTemp  = 100;
curveEndTemp    = 234;
curveEndTime    = 600;
curveBend       = 20;
curveRampTime   = 100;

for currentTime = 1:curveEndTime
    % first calculate without the bend 

    slope = (curveEndTemp-curveStartTemp)/curveEndTime; %C/S
    currentTemp = curveStartTemp + slope*currentTime;

    % calculate the bend value
    if(currentTime == curveRampTime)
        bend = curveBend;
    elseif(currentTime < curveRampTime)
        bendSlope = curveBend/curveRampTime;
        bend = bendSlope*currentTime;
    else
        bendSlope = curveBend/(curveEndTime-curveRampTime);
        bend = bendSlope*(curveEndTime-currentTime);
    end
    
    % combine
    nb(currentTime) = currentTemp;
    currentTemp = currentTemp + bend;
    
    ct(currentTime) = currentTemp;
end

%
figure(2); clf; 

plot(1,ct(1), '*', 'color', '#f60c7b', 'Marker', '.', 'MarkerSize', 30); %start temp
hold all;
plot(curveEndTime,ct(curveEndTime), '*', 'color', '#f6de18', 'Marker', '.', 'MarkerSize', 30); %end temp
plot(nb, 'LineStyle', '--', 'color', 'black');
plot(curveRampTime*ones(100,1), linspace(nb(curveRampTime), ct(curveRampTime), 100), 'color', '#b400f6', 'LineWidth', 1); %bend
plot(linspace(1,curveRampTime, 100), curveStartTemp*ones(100,1), 'color', '#18f618', 'LineWidth', 1 ); %ramp time 
plot(linspace(1,curveEndTime, 100), curveStartTemp*ones(100,1)-0.5, 'color', '#390cf6', 'LineWidth', 1 ); %end time 
plot(1:curveEndTime, ct, 'color', '#e49e18', 'LineWidth', 2); %output

%don't include these dashed lines on legend
if(curveBend > 0) 
    plot(curveRampTime*ones(100,1), linspace(curveStartTemp, nb(curveRampTime), 100), '--', 'color','#18f618') ;
end
plot(curveEndTime*ones(100,1), linspace(curveStartTemp, ct(curveEndTime), 100), '--', 'color','#390cf6') ;
legend(             ['StartTemp=' num2str(curveStartTemp) 'C'],...
                    ['EndTemp=' num2str(curveEndTemp) 'C'],...
                    'Linear',...
                    ['Bend=' num2str(curveBend) 'C'],...
                    ['RampTime=' num2str(curveRampTime) 's'],...
                    ['EndTime=' num2str(curveEndTime) 's'],...
                    'Output=Linear+Bend',...
                        'location', 'NorthWest');
ylabel('Temp (C)');
xlabel('Time (s)');
xlim([-10 curveEndTime+10]);
ylim([min(ct)-5 max(ct)+5]);
grid on;

title('Curve Calculation Example');