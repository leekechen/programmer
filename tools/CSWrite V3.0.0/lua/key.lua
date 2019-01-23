KEY_PRESS         = 0
KEY_RELEASE       = 1


function waitingForKey(handle, keyState)
    local strKeyState
    local curKeyState

    if (keyState == 0) then
        trace("[Key] Waiting for key press down ...")
        --查询通道4是否有模块，如夹具手柄按下，KEY_PRESS为0，反之KEY_PRESS为1
        guiShowPrompt(guiPrompID_press)
    else
        trace("[Key] Waiting for key release up ...")
        --等待将模块取下，即夹具手柄打开，模块取出再进行一下次烧录，读两次为软件滤波
        guiShowPrompt(guiPrompID_release)
    end

    while (1) do
        sleep(100);
        strKeyState = wr_get(handle,  "writer", "keyPress")
        if (strKeyState ~= nil) then
            curKeyState = tonumber(strKeyState)
            if (curKeyState == keyState) then
                sleep(5);
                strKeyState = wr_get(handle,  "writer", "keyPress")
                if (strKeyState ~= nil) then
                    curKeyState = tonumber(strKeyState)
                    if (curKeyState == keyState) then
                        return
                    end
                end
            end
        end
    end
end