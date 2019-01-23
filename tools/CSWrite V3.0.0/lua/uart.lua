function uartFindOK(ack, timeOutValue)
    local startTime
    local timeout

    if (ack == nil) then
        trace("[Test] Error Argument! uartFindOK() ack == nil")
        return -1
    end

    if (timeOutValue == nil) then
        timeOutValue = 2
    end

    startTime = os.time()
    timeout   = os.time() + timeOutValue

    while (1) do
        for line in io.lines(ack) do
            if (line ~= nil) then
                local ret = string.find(line, "+OK");
                if (ret ~= nil) then
                    trace("[Test] found response \"+OK\"")
                    return 0
                end

                if (os.time() >= timeout) then
                    trace("[Test] uartFindOK(ack, "..timeOutValue..") failed!")
                    trace("timeout: start: "..startTime..", end: "..timeout)
                    return -1
                end
            end
        end
    end
end

function uartFindValue(ack, pattern, timeOutValue)
    local startTime
    local timeout
    local value
    local index

    if (timeOutValue == nil) then
        timeOutValue = 2
    end

    startTime = os.time()
    timeout   = os.time() + timeOutValue

    while (1) do
        for line in io.lines(ack) do
            if (line ~= nil) then
                index = string.find(line, pattern)
                if (index ~= nil) then
                    index = index + string.len(pattern)
                    value = string.sub(line, index)
                    trace("[Test] found response: \""..value.."\"")
                    return value
                end

                if (os.time() >= timeout) then
                    trace("[Test] uartFindValue(ack, "..pattern..", "..timeOutValue..") failed!")
                    trace("[Test] timeout: start: "..startTime..", end: "..timeout)
                    return nil
                end
            end
        end
    end
end

function uartClear(handle, dev, traceFlag)
    local uartName
    local ret

    assert(handle ~= nil, "Error Argument! cleaFile() handle == nil")

    uartName = wr_lockodb(handle);
    if (uartName == nil) then
        trace("[Test] wr_lockodb(handle) failed")
        return nil
    end

    --recording all of the response of the device
    if (traceFlag == 1) then
        for line in io.lines(uartName) do
            if (line ~= nil) then
                trace(line, dev)
            end
        end
    end

    --clear the last response of the device
    file = io.open(uartName, "w+")
    if (file == nil) then
        trace("[Test] io.open("..uartName..") failed!")
        return nil
    end
    file:close()

    wr_unlockodb(handle);

    return uartName
end

