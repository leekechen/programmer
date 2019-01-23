KEY_PRESS         = 0
KEY_RELEASE       = 1


function waitingForKey(handle, keyState)
    local strKeyState
    local curKeyState

    if (keyState == 0) then
        trace("[Key] Waiting for key press down ...")
        --��ѯͨ��4�Ƿ���ģ�飬��о��ֱ����£�KEY_PRESSΪ0����֮KEY_PRESSΪ1
        guiShowPrompt(guiPrompID_press)
    else
        trace("[Key] Waiting for key release up ...")
        --�ȴ���ģ��ȡ�£����о��ֱ��򿪣�ģ��ȡ���ٽ���һ�´���¼��������Ϊ����˲�
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