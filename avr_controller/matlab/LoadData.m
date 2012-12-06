function LoadData()

dirname = 'R:\mecanum\avr_controller\';
%dirname = 'C:\Users\Garrett\Dropbox\Documents\Projects\Robot\Encoders\Data\';
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

% We advance x until we find at least two periods, but we don't want to
% ignore too much initial data. Ignore max 1 period worth of data (1/3 of
% the data, assuming, worst case, that 1 period is noise and 2 periods are
% signal).
minLength = ceil(length(x) * 2 / 3);

while length(x) > minLength

    % Fast-forward to first switch
    start = x(1);
    for i = 1 : length(x)
        if start ~= x(i)
            break;
        end
    end
    disp(['Ingoring ' num2str(i - 1) ' samples']);
    t = t(i:end);
    x = x(i:end);

    % Strategy: every time a bit is flipped and back again, calculate the
    % period and duty cycle. Check the rest of the signal midpoints against the
    % expected values.
    start = x(1); % Re-evaluate at new (flipped) value
    minFlip = 1; % Skip this many samples before finding the first flip

    % Use length(x) / 2 because we want to find 2 periods
    while 2 + minFlip < length(x) / 2
        %disp(['Using minFlip of ' num2str(minFlip)]);
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
        disp(['Trying period ' num2str(period) ' (of ' ...
            num2str(length(x)) ' samples)']);

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
            disp(['Found a period (' num2str(period) '), breaking']);
            break;
        end

        % Make sure we fast-forward to the next bit that equals start
        for i = flip + 2 : length(x)
            if x(i) == start
                break;
            end
        end
        minFlip = i;
    end

    % If the while loop ran its course, we found a period
    if 2 + minFlip < length(x) / 2
        disp('Found a period, breaking');
        break;
    end
end

% If the while loop was invalidated, we failed to find a period
if length(x) <= minLength
    disp('Failed to find a period');
    return;
end

% Recalculate offsets
offset1 = ceil((flip + 1) / 2);
offset2 = floor(period - (flip - 1) / 2);

% Got our period and duty cycle, look for edges. Start left of period,
% sweep right until we hit a bit that equals start. Remember minFlip? The
% period calculation starts from there, so calculate the number of periods
% using this offset.
leadingRisingEdges = ones(1, floor((length(x) + minFlip) / period));
for i = 2 : length(leadingRisingEdges) % First edge is obviously at offset 1
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
% First rise time is 0
for i = 2 : length(leadingRisingEdges)
    % Sweep in reverse direction looking for maximum rise time
    for j = offset1 : -1 : 1
        % Don't overshoot the length of the array
        if x(min(leadingRisingEdges(i) + j, length(x))) ~= start
            riseTime(i) = j + 1;
            break;
        end
    end
end

% Using full periods, so length(leadingRisingEdges) ==
% length(leadingFallingEdges)
leadingFallingEdges = zeros(1, length(leadingRisingEdges));
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

plot(x)

end