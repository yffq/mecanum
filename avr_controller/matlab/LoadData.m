function LoadData()

dirname = 'R:\mecanum\avr_controller\';
backup = 'C:\Users\Garrett\Dropbox\Documents\Projects\Robot\Encoders\Data\';
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
if ~exist([backup filename], 'file')
    copyfile([dirname filename], [backup filename]);
end

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
start = x(1);
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

    %offset1 = ceil((flip + 1) / 2);
    %offset2 = floor(period - (flip - 1) / 2);
    offset1 = (flip + 1) / 2;
    offset2 = period - (flip - 1) / 2;
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

% Got our period and duty cycle
period * dt
duty = flip / period

end