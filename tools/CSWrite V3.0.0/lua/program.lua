ERR_PROGRAM_OK                  = 0
ERR_PROGRAM_VERIFY_FAIILED      = -10
ERR_PROGRAM_MAC                 = -40

function getStrDevList(devList)
    local line

    if (devList[1] ~= nil) then
        line = devList[1]
        guiShowDeviceMsg(1, guiID_programming, guiMsgID_programming)
    end
    if (devList[2] ~= nil) then
        guiShowDeviceMsg(2, guiID_programming, guiMsgID_programming)
        if (line == nil) then
            line = devList[2]
        else
            line = line..","..devList[2]
        end
    end
    if (devList[3] ~= nil) then
        guiShowDeviceMsg(3, guiID_programming, guiMsgID_programming)
        if (line == nil) then
            line = devList[3]
        else
            line = line..","..devList[3]
        end
    end
    if (devList[4] ~= nil) then
        guiShowDeviceMsg(4, guiID_programming, guiMsgID_programming)
        if (line == nil) then
            line = devList[4]
        else
            line = line..","..devList[4]
        end
    end

    return line
end

function programFirmware(handle, devList)
    local i
    local line
    local index = 1
    local wrRet = {}
    local errCode = {}

    assert(handle ~= nil)

    line = getStrDevList(devList)
    if (line == nil) then
        return errCode
    end

    guiShowPrompt(guiPromptID_working)
    trace("[Program] programming firmware")
    trace("[Program] wr_write(handle, \""..line.."\")")

    wrRet[1], wrRet[2], wrRet[3], wrRet[4] = wr_write(handle, line)
    for i = 1, 4 do
        if (devList[i] == nil) then
            errCode[i] = -1
        else
            errCode[i] = wrRet[index]
            index = index + 1
        end
    end

    line = string.format("[Program] program completed! dev1=%d, dev2=%d, dev3=%d, dev4=%d", errCode[1], errCode[2], errCode[3], errCode[4])
    trace(line)

    return errCode
end

function programMAC(handle, id, dev, mac)
    local err
    local setCmd

    setCmd = string.format("%s:%s","mac", mac)
    trace("[Program] wr_set(handle, "..dev..", "..setCmd..")")

    err = wr_set(handle, dev, setCmd)
    if( err == nil ) then
        trace("[Program] program MAC failed!")
        guiShowDeviceMsg(id, guiID_programming, guiMsgID_programming)
        return -1
    else
        trace("[Program] program MAC successed!")
        return 0
    end
end

function program(handle, flashID, devList, MAC)
    local i
    local line
    local programflag = 1
    local ret = {}
    local again = 0
    local curDev
    local errDev
    local row        = {}
    local curDevList = {}
    local errDevList = {}
    local okDevList  = {}
    local reportList = {0, 0, 0, 0}
    local errCode    = {}
    local MACList    = {0, 0, 0, 0}
    local errNum     = 0
    local OkNum      = 0

    curDevList = devList
    while (again < PROGRAM_AGAIN_TIMES and programflag == 1) do
        errDevList  = {}
        programflag = 0
        ret  = programFirmware(handle, curDevList)
        for i = 1, 4 do
            if (ret[i] ~= ERR_PROGRAM_OK and devList[i] ~= nil) then
                errDevList[i] = devList[i]
                errCode[i]    = ret[i]
                programflag   = 1
            else
                okDevList[i]  = devList[i]
                errCode[i]    = ERR_PROGRAM_OK
            end
        end
        curDevList = errDevList
        again      = again + 1
    end

    --insert program firmware failed device to database
    for i = 1, 4 do
        if (errDevList[i] ~= nil) then
            guiShowDeviceMsg(i, guiID_programFailed, guiMsgID_programFailed)
            row = {flashID = flashID[i], MAC = nil, errCode = errCode[i], date = os.date("%y/%m/%d"), time = os.date("%H:%M:%S")}
            dbTableProgramInsert(row)
            errNum = errNum + 1
        end
    end

    curDevList = okDevList
    for i = 1, 4 do
        curDev = curDevList[i]
        if (curDev ~= nil) then
            again       = 0
            programflag = 1
            while (again < PROGRAM_AGAIN_TIMES and (MAC.cur <= MAC.max and programflag == 1)) do
                programflag = 0
                errDevList  = {}
                ret [1] = programMAC(handle, i, curDevList[i], MAC.cur)
                if (ret[1] == ERR_PROGRAM_OK) then
                    okDevList[i]  = curDevList[i]
                    MACList[i]    = MAC.cur
                    MAC.cur       = MAC.cur + 1
                    errCode[i]    = ERR_PROGRAM_OK
                else
                    ledSwitch(handle, devList[i], LED_ON)
                    errDevList[i] = curDev
                    errCode[i]    = ERR_PROGRAM_MAC
                    programflag   = 1
                end
                again  = again + 1
            end
        end
    end

    for i = 1, 4 do
        if (errDevList[i] ~= nil) then
            guiShowDeviceMsg(i, guiID_programFailed, guiMsgID_programFailed)
            row = {flashID = flashID[i], MAC = nil, errCode = errCode[i], date = os.date("%y/%m/%d"), time = os.date("%H:%M:%S")}
            row.MAC = MACList[i]
            dbTableProgramInsert(row)
            errNum = errNum + 1
            ledSwitch(handle, devList[i], LED_ON)
        end
        if (okDevList[i] ~= nil) then
            row = {flashID = flashID[i], MAC = MACList[i], errCode = errCode[i], date = os.date("%y/%m/%d"), time = os.date("%H:%M:%S")}
            row.MAC = MACList[i]
            dbTableProgramInsert(row)
            OkNum = OkNum + 1
            reportList[i] = 0
            guiShowDeviceMsg(i, guiID_programSuccessed, guiMsgID_programSuccessed)
        end
    end

    wr_reportstate(handle, WRITER_ID, reportList[1], reportList[2], reportList[3], reportList[4], MACList[1], MACList[2], MACList[3], MACList[4]);

    return okDevList, MAC
end