
%s = serial('/dev/cu.RNBT-60BF-RNI-SPP','BaudRate',115200,'DataBits',8,'StopBits',1,'Parity','None',...
    'FlowControl','None','ByteOrder','bigEndian','DataTerminalReady','off')
%s.terminator = 0;
%fopen(s)

%%
close all;

fwrite(s, 'g');

N = 4000;

%plot([0:(N/1000):N], [0:0.2:200], 'w'); hold on;

values = zeros([1, N]);

for i = 1:N
    distanceRaw = fscanf(s);
    if (distanceRaw == 0)
        continue
    end
    distanceDouble = str2double(distanceRaw);
    %scaled = min(distanceDouble); % assume this is less than 2000
    values(i) = distanceDouble;
    %plot(i, scaled, 'k*'), drawnow
end

values = values(values > 0);
n = numel(values);

figure(1)
title('ADC Readings over Time');
xlabel('time');
ylabel('distance sensor measurement');
plot(values, 'k*');


% smoothen out with a median
medvalues = zeros([1, n]);
smooth = 100;
for i = 1:n
    newfrom = max(1, i - smooth);
    newto = min(n, i + smooth);
    medvalues(i) = median(values(newfrom:newto));
end

figure(2)
plot(medvalues, 'b*');
title('Smoothened ADC Readings over Time');
set(gca, 'ylim', [0 1200]);
xlabel('time');
ylabel('distance sensor measurement');

fwrite(s, 's')



% next steps
% make it record for five seconds
% plot full graph of recording
% make into a heatmap at 200-data point intervals or something