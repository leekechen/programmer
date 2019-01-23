DEVICE_POWER_ON_AND_RUN     = 1
DEVICE_POWER_OFF_AND_STOP   = 0
PROGRAM_AGAIN_TIMES         = 2
TEST_AGAIN_TIMES            = 2

GPIO_INPUT                  = 0
GPIO_OUTPUT_PUSH_PULL       = 1
GPIO_OUTPUT_OPEN_DRAIN      = 2

GPIO_PULL_DOWN              = 0
GPIO_PULL_UP                = 1
GPIO_NO_PULL                = 3

GPIO_HIGH                   = 1
GPIO_LOW                    = 0


function devPowerCtrl(handle, devList, ctrl)
    local err
    local setCmd
    local ret = {}
	local line

    assert(handle ~= nil)
    assert(ctrl == DEVICE_POWER_ON_AND_RUN or ctrl == DEVICE_POWER_OFF_AND_STOP)

    if (ctrl == DEVICE_POWER_ON_AND_RUN) then
        trace("[DevCtrl] Power on and run!")
    elseif (ctrl == DEVICE_POWER_OFF_AND_STOP) then
        trace("[DevCtrl] Stop and power off!")
    end

    setCmd  = string.format("run:%d", ctrl)
    for i = 1, 4 do
        if (devList[i] ~= nil) then
            err = wr_set(handle, devList[i], setCmd);
            if (err == nil) then
                trace("[DevCtrl] wr_set(handle, "..devList[i] ..", "..setCmd..") failed")
                ret[i] = -1
            else
                ret[i] = 0
            end
        end
    end

    if (ctrl == DEVICE_POWER_ON_AND_RUN) then
        sleep(2000)
    end

    return ret
end


function devGpioCfg(handle, gpioCfg)
    local cfg
    local setCmd

    cfg = string.format("%d,", gpioCfg.pin)
    if (gpioCfg.mode == GPIO_INPUT) then
        cfg = cfg.."i,"
    elseif (gpioCfg.mode == GPIO_OUTPUT_PUSH_PULL) then
        cfg = cfg.."opp,"
    elseif (gpioCfg.mode == GPIO_OUTPUT_OPEN_DRAIN) then
        cfg = cfg.."ood,"
    end

    if (gpioCfg.pull == GPIO_PULL_DOWN) then
        cfg = cfg.."pd,"
    elseif (gpioCfg.pull ==  GPIO_PULL_UP) then
        cfg = cfg.."pu,"
    elseif (gpioCfg.pull ==  GPIO_NO_PULL) then
        cfg = cfg.."np,"
    end

    if (gpioCfg.value ~= nil) then
        if (gpioCfg.value ~= GPIO_HIGH) then
            cfg = cfg.."0"
        else
            cfg = cfg.."1"
        end
    end

    setCmd = "gpioCfg:"..cfg
    trace("[DevCtrl] wr_set(handle, \""..gpioCfg.dev..", "..setCmd..")")
    wr_set(handle, gpioCfg.dev, setCmd)

end

function devGetFlashID(handle, devList)
    local i
    local flashID = {}

    assert(handle ~= nil)
    assert(devList ~= nil)

    for i = 1, 4 do
        if (devList[i] ~= nil) then
            flashID[i] = wr_get(handle, devList[i], "uniqueID")
            if (flashID[i] == nil) then
                trace("[DevCtrl] wr_get(handle, \""..devList[i]..",\"uniqueID\") failed")
            else
                trace("[DevCtrl] wr_get(handle, \""..devList[i]..",\"uniqueID\") = "..flashID[i])
            end

            if (flashID[i] == "00000000000000000000000000000000" or flashID[i] == "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF") then
                flashID[i] = nil
            end
        end
    end

    return flashID
end

function buzzerBeepOK(handle)
    wr_set(handle, "writer", "buzzerBeep:long")
end

function buzzerBeepFailed()
    wr_set(handle, "writer", "buzzerBeep:short3Times")
end

function devGetCurrent(handle)
    local i
    local ret     = {}
    local current = {}

    assert(handle ~= nil)

    devPowerCtrl(handle, DEVICE_TABLE, DEVICE_POWER_ON_AND_RUN)
    for i = 1, 4 do
        ret[i] = wr_get(handle, DEVICE_TABLE[i], "iSense")
        if (ret[i] == nil) then
            trace("[DevCtrl] wr_get(handle, \""..DEVICE_TABLE[i].."\", \"iSense\") failed!")
            devPowerCtrl(handle, DEVICE_POWER_OFF_AND_STOP)
            return current
        end
        current[i] = tonumber(ret[i])
    end
    trace("[DevCtrl] dev1 = "..current[1].."mA, dev2 = "..current[2].."mA, dev3 = "..current[3].."mA, dev4 = "..current[4].."mA")

    devPowerCtrl(handle, DEVICE_TABLE, DEVICE_POWER_OFF_AND_STOP)

    return current
end

function devCtrlUartTurnOn(handle, dev)
    local ret
    local setCmd

    assert(handle ~= nil)
    assert(dev ~= nil)

    uartClear(handle, dev, 0)

    setCmd  = "uart:0";
    ret     = wr_set(handle, dev, setCmd);
    if (ret == nil) then
        trace("[DevCtrl] wr_set(handle, "..dev..", "..setCmd..") failed")
        return -1
    end

    setCmd  = "uart:1";
    ret     = wr_set(handle, dev, setCmd);
    if (ret == nil) then
        trace("[DevCtrl] wr_set(handle, \""..dev.."\", \""..setCmd.."\") failed")
        return -1
    end

     trace("[DevCtrl] Uart turn on OK")

    return 0
end
