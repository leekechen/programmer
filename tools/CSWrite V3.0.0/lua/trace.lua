TRACE_VERBOSE   = 0
TRACE_NOTIFY    = 1
TRACE_ERROR     = 2

TRACE_LEVEL     = TRACE_VERBOSE


function traceInit()
    local file
    local traceName

    traceName = string.format("log\\trace_%s_%s.log", DATE, TIME)
    file      = io.open(traceName, "w+")
    file:close()
	
	return traceName
end

function trace(msg, dev)
    local file
    local line
    local time
    local traceName

    if (dev == nil) then
        dev = "Writer"
        print(msg)
    end

    traceName = string.format("log\\trace_%s_%s.log", DATE, TIME)

    file = io.open(traceName, "a+")
    time = os.date("%H:%M:%S")
    line = string.format("[%s] %s# %s \r\n", time, dev, msg)
    file:write(line)
    file:close()
    log(line)
end
