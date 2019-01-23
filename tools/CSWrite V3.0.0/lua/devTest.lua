require("uart")

THRESHOLD_UNDETECTED_MA     = 0
THRESHOLD_SHORT_MA          = 400

ERR_TEST_OK                 =   0
ERR_TEST_ENTER              =  -1
ERR_TEST_VER                =  -2
ERR_TEST_QMAC               =  -3
ERR_TEST_PIN_LOW            =  -4
ERR_TEST_PIN_HIGH           =  -5
ERR_TEST_CSE7761            =  -6
ERR_TEST_RFTEST             =  -7
ERR_TEST_SETPIN             =  -8
ERR_TEST_SETCODE            =  -9

CSE7761_U_RATED     = 200000
CSE7761_U_OFFSET    = 0.25
CSE7761_Ai_RATED    = 5000
CSE7761_Ai_OFFSET   = 0.25
CSE7761_Ap_RATED    = 1000000
CSE7761_Ap_OFFSET   = 0.30
CSE7761_Bi_RATED    = 5000
CSE7761_Bi_OFFSET   = 0.20
CSE7761_Bp_RATED    = 1000000
CSE7761_Bp_OFFSET   = 0.30

RFTEST_RSSI_RATED   = 78

ERR_MSG_FAILED      = "Failed"
ERR_MSG_PASSED      = "Passed"
ERR_MSG_NONE_TESTED = "None Test"
ERR_MSG_FRAGMENT    = "Fragment"

function sendCmd(handle, dev, cmd)
    local ack
    local ret
    local strCmd
    local setCmd

    assert(handle ~= nil)
    assert(dev ~= nil)
    assert(cmd ~= nil)

    ack    = uartClear(handle, dev, 1)

    setCmd = "testCmd:" .. cmd;
    trace("[Test] wr_set(handle, "..dev..", " .. setCmd..")")
    setCmd = setCmd.."\r\n"
    ret = wr_set(handle, dev, setCmd);
    if (ret == nil) then
        trace("wr_set(handle, "..dev..", " .. setCmd..") failed")
        return nil
    end

    return ack
end

function doCmd(handle, dev, cmd, pattern)
    local ack
    local ret
    local times = 0
    local again = 1

    assert(handle ~= nil)
    assert(dev ~= nil)

    while (again == 1 and times < TEST_AGAIN_TIMES) do
        ack = sendCmd(handle,  dev, cmd)
        if (ack == nil) then
            trace("[Test] sendCmd(handle, "..dev..", \""..cmd.."\") failed")
        else
            ret = uartFindOK(ack)
            if (ret ~= 0) then
                trace("[Test] Can not find +OK")
            else
                again = 0
                if (pattern ~= nil) then
                    again = 1
                    ret = uartFindValue(ack, pattern, 4)
                    if (ret ~= nil) then
                        again = 0
                    else
                        ret = ERR_MSG_FAILED
                    end
                end
            end
        end
        times = times + 1
    end

    return again, ret
end

function cse7761Compare(cse7761Value)
    local i = 1
    local cse7761_U
    local cse7761_Ai
    local cse7761_Ap
    local cse7761_Bi
    local cse7761_Bp
    local cse7761    =  {U = nil, Ai = nil, Ap = nil, Bi = nil, Bp = nil}
    local value = {}

    cse7761_U  = {lower = CSE7761_U_RATED  * (1 - CSE7761_U_OFFSET ), upper = CSE7761_U_RATED  * (1 + CSE7761_U_OFFSET )}
    cse7761_Ai = {lower = CSE7761_Ai_RATED * (1 - CSE7761_Ai_OFFSET), upper = CSE7761_Ai_RATED * (1 + CSE7761_Ai_OFFSET)}
    cse7761_Ap = {lower = CSE7761_Ap_RATED * (1 - CSE7761_Ap_OFFSET), upper = CSE7761_Ap_RATED * (1 + CSE7761_Ap_OFFSET)}
    cse7761_Bi = {lower = CSE7761_Bi_RATED * (1 - CSE7761_Bi_OFFSET), upper = CSE7761_Bi_RATED * (1 + CSE7761_Bi_OFFSET)}
    cse7761_Bp = {lower = CSE7761_Bp_RATED * (1 - CSE7761_Bp_OFFSET), upper = CSE7761_Bp_RATED * (1 + CSE7761_Bp_OFFSET)}

    for word in string.gmatch(cse7761Value, "%d+") do
        if (word ~= nil) then
            value[i] = word
            i = i + 1
        end
    end

    if (i < 5) then
        return ERR_MSG_FRAGMENT
    end

    if (value[1] ~= nil) then
        cse7761.U  = tonumber(value[1])
        if (cse7761.U < cse7761_U.lower or cse7761.U > cse7761_U.upper) then
            return ERR_MSG_FAILED
        end
    else
        return ERR_MSG_FAILED
    end

    if (value[2] ~= nil) then
        cse7761.Ai = tonumber(value[2])
        if (cse7761.Ai < cse7761_Ai.lower or cse7761.Ai > cse7761_Ai.upper) then
            return ERR_MSG_FAILED
        end
    else
        return ERR_MSG_FAILED
    end

    if (value[3] ~= nil) then
        cse7761.Ap = tonumber(value[3])
        if (cse7761.Ap < cse7761_Ap.lower or cse7761.Ap > cse7761_Ap.upper) then
            return ERR_MSG_FAILED
        end
    else
        return ERR_MSG_FAILED
    end

    if (value[4] ~= nil) then
        cse7761.Bi = tonumber(value[4])
        if (cse7761.Bi < cse7761_Bi.lower or cse7761.Bi > cse7761_Bi.upper) then
            return ERR_MSG_FAILED
        end
    else
        return ERR_MSG_FAILED
    end

    if (value[5] ~= nil) then
        cse7761.Bp = tonumber(value[5])
        if (cse7761.Bp < cse7761_Bp.lower or cse7761.Bp > cse7761_Bp.upper) then
            return ERR_MSG_FAILED
        end
    else
        return ERR_MSG_FAILED
    end

    return ERR_MSG_PASSED
end

function rfTestCompare(rfTestValue)
    local i = 1
    local channel
    local lost
    local rssi
    local value = {}

    for word in string.gmatch(rfTestValue, "%d+") do
        if (word ~= nil) then
            value[i] = word
            i = i + 1
        end
    end

    if (i < 3) then
        return ERR_MSG_FRAGMENT
    end

    if (value[1] ~= nil) then
        channel = tonumber(value[1])

    end

    if (value[2] ~= nil) then
        lost = tonumber(value[2])
    else
        return ERR_MSG_FAILED
    end

    if (value[3] ~= nil) then
        rssi = tonumber(value[3])
        if (rssi > RFTEST_RSSI_RATED) then
            return ERR_MSG_FAILED
        end
    else
        return ERR_MSG_FAILED
    end

    return ERR_MSG_PASSED
end

function pinCompare(handle, dev, pattern)
    --pin postion please read the document "pinMap.xlsx"
    --Writer connect is: V2P,V2N,WIFI_LED(GPIO8),KEY(GPIO2),RELAY3(GPIO6),ACN,ACNOUT,ACLOUT,RELAY2(GPIO27),RELAY1(GPIO19),RELAY_LED(GPIO20)
    pinState   = wr_get(handle, dev, "pin")
    if (pinState == nil) then
        trace("[Test] wr_get(handle, "..dev..", \"pin\") failed")
        return ERR_MSG_FAILED
    else
        trace("[Test] Get Pin: ".. pinState)
        ret = string.match(pinState, pattern)
        if (ret == nil) then
            trace("[Test] Pin: ".. pinState.." not match Pattern: "..pattern)
            return ERR_MSG_FAILED
        else
            return ERR_MSG_PASSED
        end
    end
end

function gpio2CfgOutputHigh(handle, dev)
    local cfg = {dev = dev, pin = 3, mode = GPIO_OUTPUT_PUSH_PULL, pull= GPIO_PULL_UP, value = GPIO_HIGH}
    devGpioCfg(handle, cfg)
end

function gpio19CfgOutputLow(handle, dev)
    local cfg = {dev = dev, pin = 9, mode = GPIO_OUTPUT_PUSH_PULL, pull= GPIO_PULL_UP, value = GPIO_LOW}
    devGpioCfg(handle, cfg)
end

function gpio2CfgInput(handle, dev)
    local cfg = {dev = dev, pin = 3, mode = GPIO_INPUT, pull= GPIO_NO_PULL}
    devGpioCfg(handle, cfg)
end

function gpio19CfgInput(handle, dev)
    local cfg = {dev = dev, pin = 9, mode = GPIO_INPUT, pull= GPIO_NO_PULL}
    devGpioCfg(handle, cfg)
end

function gpioRFTestEnable(handle, dev)
    local cfg = {dev = dev, pin = 0, mode = GPIO_OUTPUT_PUSH_PULL, pull= GPIO_PULL_UP, value = GPIO_HIGH}
    devGpioCfg(handle, cfg)
end

function test_AT_TESTST(handle, dev)
    local err
    local cmd

    cmd =  "AT+TESTST"
    err = doCmd(handle, dev, cmd)
    if (err ~= 0) then
        return ERR_TEST_ENTER
    end

    return ERR_TEST_OK
end

function test_AT_VER(handle, dev)
    local cmd
    local version

    cmd =  "AT+VER"
    ret, version = doCmd(handle, dev, cmd, "VER:")
    if (ret ~= 0) then
        return ERR_TEST_VER
    end

    return ERR_TEST_OK, version
end

function test_AT_QMAC(handle, dev)
    local cmd
    local MAC

    cmd =  "AT+QMAC"
    ret, MAC = doCmd(handle, dev, cmd, "MAC:")
    if (ret ~= 0) then
        return ERR_TEST_QMAC
    end

    return ERR_TEST_OK, MAC
end

function test_AT_SetCode(handle, dev)
    local err
    local cmd

    cmd = "AT+SETCODE=6869f137f19648e98a85089de96ea1ba"
    err = doCmd(handle, dev, cmd)
    if (err ~= 0) then
        return ERR_TEST_SETCODE
    end

    return ERR_TEST_OK
end


function test_AT_TEST7761(handle, dev)
    local cmd
    local cse7761

    cmd =  "AT+TEST7761"

    while true do
        ret, cse7761 = doCmd(handle, dev, cmd, "CSE7761:")
        if (ret ~= 0) then
            return ERR_TEST_CSE7761
        else
            cse7761 = cse7761Compare(cse7761)
            if (cse7761 == ERR_MSG_PASSED) then
                return ERR_TEST_OK, cse7761
            elseif (ERR_MSG_FAILED == cse7761)then
                return ERR_TEST_CSE7761, cse7761
            end
        end
    end
end

function test_AT_SETPIN(handle, dev, pattern)
    local cmd

    --set GPIO2,6,8,19,20,27 to low level
    cmd = "AT+SETPIN="..pattern
    ret = doCmd(handle, dev, cmd)
    if (ret ~= 0) then
        return ERR_TEST_SETPIN
    end
    pinLow = pinCompare(handle, dev, "%d%d000%d%d%d00%d")
    if (pinLow == ERR_MSG_FAILED) then
        return ERR_TEST_SETPIN
    end

    return ERR_TEST_OK
end

function test_AT_SETPIN_Low(handle, dev)
    local err
    local pattern = "XX0XXX0X0XXXXXXXXXX00XXXXXX0"

    err = test_AT_SETPIN(handle, dev, pattern)
    if (err ~= ERR_TEST_OK) then
        return ERR_TEST_PIN_LOW
    end

    return ERR_TEST_OK, ERR_MSG_PASSED
end

function test_AT_SETPIN_High(handle, dev)
    local err
    local pattern = "XX1XXX1X1XXXXXXXXXX11XXXXXX1"

    err = test_AT_SETPIN(handle, dev, pattern)
    if (err ~= ERR_TEST_OK) then
        return ERR_TEST_PIN_HIGH
    end

    return ERR_TEST_OK, ERR_MSG_PASSED
end

function test_AT_RFTEST(handle, dev, id)
    local cmd
    local rfTest
    --gpioRFTestEnable(handle, dev)

    cmd = string.format("AT+RFTEST=%d", id)

    while true do
        ret, rfTest = doCmd(handle, dev, cmd, "RFTEST:")
        if (ret ~= 0) then
            return ERR_TEST_RFTEST, ERR_MSG_FAILED
        else
            rfTest = rfTestCompare(rfTest)
            if (rfTest == ERR_MSG_PASSED) then
                return ERR_TEST_OK, rfTest
            elseif (ERR_MSG_FAILED == rfTest)then
                return ERR_TEST_RFTEST, rfTest
            end
        end
    end
end

function testDev(handle, dev, flashID, id)
    local err = -1
    local row = {
                    flashID = flashID,
                    MAC     = ERR_MSG_NONE_TESTED,
                    version = ERR_MSG_NONE_TESTED,
                    CSE7761 = ERR_MSG_NONE_TESTED,
                    pinLow  = ERR_MSG_NONE_TESTED,
                    pinHigh = ERR_MSG_NONE_TESTED,
                    rfTest  = ERR_MSG_NONE_TESTED,
                    errCode = ERR_MSG_NONE_TESTED,
                    date    = ERR_MSG_NONE_TESTED,
                    time    = ERR_MSG_NONE_TESTED
                }

    assert(handle ~= nil)
    assert(dev ~= nil)

    gpio2CfgOutputHigh(handle, dev)
    gpio19CfgOutputLow(handle, dev)

    trace("[Test] Turn on "..dev.." Uart")
    ret = devCtrlUartTurnOn(handle, dev)
    if (ret ~= 0) then
        trace("[Test] Devcie initialize failed!")
        return row
    end

    row.errCode = test_AT_TESTST(handle, dev)

    gpio2CfgInput(handle, dev)
    gpio19CfgInput(handle, dev)

    row.errCode, row.version = test_AT_VER(handle, dev)
    if (row.errCode ~= ERR_TEST_OK) then
        return row, err
    end

    row.errCode, row.MAC = test_AT_QMAC(handle, dev)
    if (row.errCode ~= ERR_TEST_OK) then
        return row, err
    end

    row.errCode = test_AT_SetCode(handle, dev)

    row.errCode, row.pinHigh = test_AT_SETPIN_High(handle, dev)
    if (row.errCode ~= ERR_TEST_OK) then
        return row, err
    end

    row.errCode, row.pinLow = test_AT_SETPIN_Low(handle, dev)
    if (row.errCode ~= ERR_TEST_OK) then
        return row, err
    end

    row.errCode, row.rfTest = test_AT_RFTEST(handle, dev, id)
    if (row.errCode ~= ERR_TEST_OK) then
        return row, err
    end

    row.errCode, row.CSE7761 = test_AT_TEST7761(handle, dev)
    if (row.errCode ~= ERR_TEST_OK) then
        return row, err
    end

    err = 0

    return row, err
end

function test(handle, devList, flashID)
    local i
    local err
    local row = {}

    guiShowPrompt(guiPromptID_working)

    devPowerCtrl(handle, devList, DEVICE_POWER_ON_AND_RUN)
    for i = 1, 4 do
        if (devList[i] ~= nil and flashID[i] ~= nil) then
            guiShowDeviceMsg(i, guiID_testing, guiMsgID_testing)

            row, err = testDev(handle, devList[i], flashID[i], i)
            if (err ~= 0) then
                ledSwitch(handle, devList[i], LED_ON)
                guiShowDeviceMsg(i, guiID_testFailed, guiMsgID_testFailed)
            else
                guiShowDeviceMsg(i, guiID_testSuccessed, guiMsgID_testSuccessed)
            end
            row.date = os.date("%y/%m/%d")
            row.time = os.date("%H:%M:%S")
            dbTableTestInsert(row)
        end
    end
    devPowerCtrl(handle, devList, DEVICE_POWER_OFF_AND_STOP)
end

function testDevCurrentIsValid(current)
    local i
    local devList = {}

    for i = 1, 4 do
        if (current[i] == THRESHOLD_UNDETECTED_MA) then
            devList[i] = nil
            guiShowDeviceMsg(i, guiID_noDetected, guiMsgID_noDetected)
        elseif (current[i] >= THRESHOLD_SHORT_MA) then
            devList[i] = nil
        else
            devList[i] = DEVICE_TABLE[i]
        end
    end

    return devList
end
