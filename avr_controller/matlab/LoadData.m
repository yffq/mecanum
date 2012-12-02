function LoadData()

%dirname = 'R:\mecanum\avr_controller\';
%backup = 'C:\Users\Garrett\Dropbox\Documents\Projects\Robot\Encoders\Data\';
dirname = 'C:\Users\Garrett\Dropbox\Documents\Projects\Robot\Encoders\Data\';
dirlist = dir([dirname '*.txt']);

% Loop in reverse to find the last qualifying file name
for i = length(dirlist):-1:1
    filename = dirlist(i).name;
    if '0' <= filename(1) && filename(1) <= '9' && ...
            length(filename) == length('YYYYmmdd1234.txt')
        break
    end
end

% Backup the data first
%{
if ~exist([backup filename], 'file')
    copyfile([dirname filename], [backup filename]);
end
%}

dt = .0005; % s
x = load([dirname filename]);
t = 0 : dt : (length(x) - 1) * dt;

% Fast-forward to first switch
start = x(1);
for i = 1 : length(x)
    if start ~= x(i)
        break;
    end
end
t = t(i:end);
x = x(i:end);

% Strategy: every time a bit is flipped and back again, calculate the
% period and duty cycle. Check the rest of the signal midpoints against the
% expected values.
start = x(1); % Re-evaluate at new (flipped) value
minFlip = 1; % Skip this many samples before finding the first flip

while 2 + minFlip < length(x)
    
    flip = length(x);
    % We already know the first bit == start, don't test it
    for i = 1 + minFlip : length(x)
        if x(i) ~= start
            % flip is the number of bits that == start (including start)
            % Remember, we skipped minFlip samples
            flip = i - 1;
            break;
        end
    end

    % flop is the number of bits that ~= start
    flop = length(x) - flip;
    % Start after the first bit that ~= start
    for i = flip + 2 : length(x)
        if x(i) == start
            flop = i - 1 - flip;
            break;
        end
    end

    period = flip + flop;
    disp(['Trying period ' num2str(period * dt)]);
    
    %{
    if start == 1
        duty = flip / period;
    else
        duty = flop / period;
    end
    %}

    % Round to the right
    offset1 = ceil((flip + 1) / 2);
    offset2 = floor(period - (flip - 1) / 2);
    err = 0;
    max_err = 1;
    while offset2 <= length(x) && err < max_err
        % If start == 1, x(offset1) should be 1 and x(offset2) should be 0
        if start == 1
            err = err + (1 - x(offset1)) + x(offset2);
        else
            err = err + x(offset1) + (1 - x(offset2));
        end
        offset1 = offset1 + period;
        offset2 = offset2 + period;
    end

    % Found our period
    if err < max_err
        break;
    end
    
    % Make sure we fast-forward to the next bit == start
    for i = flip + 2 : length(x)
        if x(i) == start
            break;
        end
    end
    minFlip = i;
end

% If the while loop was invalidated we ran out of periods to try
if 2 + minFlip >= length(x)
    period = length(x);
    flip = length(x);
    for i = 2 : length(x)
        if x(i) ~= start
            flip = i - 1;
            break;
        end
    end
    %flop = period - flip;
end

% Recalculate offsets
offset1 = ceil((flip + 1) / 2);
offset2 = floor(period - (flip - 1) / 2);

% Got our period and duty cycle, look for edges
% Start left of period, sweep right until we hit a bit == start
leadingRisingEdges = ones(1, floor(length(x) / period));
for i = 2 : floor(length(x) / period) % First edge is obviously at offset 0
    for j = period * (i-2) + offset2 : period * (i-1) + offset1
        if x(j) == start
            leadingRisingEdges(i) = j;
            break;
        end
    end
end

% Assuming a duty cycle with a leading 1:

% Default rise time of 0, can't have a rise time of 1
riseTime = zeros(1, length(leadingRisingEdges));
for i = 1 : length(leadingRisingEdges)
    % Sweep in reverse direction looking for maximum rise time
    for j = offset1 : -1 : 1
        if x(min(leadingRisingEdges(i) + j, length(x))) ~= start
            riseTime(i) = j + 1;
            break;
        end
    end
end

leadingFallingEdges = zeros(1, floor(length(x) / period));
for i = 1 : length(leadingFallingEdges)
    for j = period * (i-1) + offset1 : period * (i-1) + offset2
        if x(j) ~= start
            leadingFallingEdges(i) = j;
            break;
        end
    end
end

fallTime = zeros(1, length(leadingFallingEdges));
for i = 1 : length(fallTime)
    % Sweep in reverse direction looking for maximum fall time
    for j = period - offset2 : -1 : 1
        if x(min(leadingFallingEdges(i) + j, length(x))) == start
            fallTime(i) = j + 1;
            break;
        end
    end
end


period
duty = flip / period

end