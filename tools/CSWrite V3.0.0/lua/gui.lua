TotalNum = 0
ProgramOKNum = 0
ProgramFailedNum = 0
TestOKNUm = 0
TestFailedNum = 0


function guiInit()
    --device status: NoDetected(Gray), Idle(Purple), Programming/Testing(Yellow), Program/Test Successed(Green), Program/Test Failed(Red)
    --设备状态：未检测到（灰白），空闲（紫色），编程/测试中（黄色），编程/测试成功（绿色）,编程/测试失败（红色）
    guiID_noDetected          = "O"
    guiID_idle                = "P"
    guiID_programming         = "Y"
    guiID_testing             = "Y"
    guiID_programSuccessed    = "G"
    guiID_testSuccessed       = "G"
    guiID_programFailed       = "R"
    guiID_testFailed          = "R"

    guiID_dev1                = "D1"
    guiID_dev2                = "D2"
    guiID_dev3                = "D3"
    guiID_dev4                = "D4"

    guiDevIDTable             = {guiID_dev1, guiID_dev2, guiID_dev3, guiID_dev4}

    guiMsg_noDetected         = { "Channel %d: No Detected"       , "通道 %d: 未检测到模块"  }
    guiMsg_idle               = { "Channel %d: Idle"              , "通道 %d: 空闲"          }
    guiMsg_programming        = { "Channel %d: programming"       , "通道 %d: 编程中"        }
    guiMsg_testing            = { "Channel %d: Testing"           , "通道 %d: 测试中"        }
    guiMsg_programSuccessed   = { "Channel %d: Program Successed" , "通道 %d: 编程成功"      }
    guiMsg_testSuccessed      = { "Channel %d: Test Successed"    , "通道 %d: 测试成功"      }
    guiMsg_programFailed      = { "Channel %d: Program Failed"    , "通道 %d: 编程失败"      }
    guiMsg_testFailed         = { "Channel %d: Test Failed"       , "通道 %d: 测试失败"      }

    guiMsgTable               = { guiMsg_noDetected,        guiMsg_idle,          guiMsg_programming,   guiMsg_testing,
                                  guiMsg_programSuccessed,  guiMsg_testSuccessed, guiMsg_programFailed, guiMsg_testFailed }

    guiMsgID_noDetected       =  1
    guiMsgID_idle             =  1 + guiMsgID_noDetected
    guiMsgID_programming      =  1 + guiMsgID_idle
    guiMsgID_testing          =  1 + guiMsgID_programming
    guiMsgID_programSuccessed =  1 + guiMsgID_testing
    guiMsgID_testSuccessed    =  1 + guiMsgID_programSuccessed
    guiMsgID_programFailed    =  1 + guiMsgID_testSuccessed
    guiMsgID_testFailed       =  1 + guiMsgID_programFailed

    guiPrompt_press           = {"Please setup moudle and press handle"                                         , "请安装模块，并按下手柄"                }
    guiPrompt_release         = {"Please release handle and Take out module)"                                   , "请释放手柄，并取出模块"                }
    guiPrompt_download        = {"Please wait，CSWriter is updating firmware and configuration"                 , "请等待，烧录器正更新模块固件和配置"    }
    guiPrompt_downloadErr     = {"Can't connect server, please contact with FAE"                                , "无法连接服务器，请联系FAE"             }
    guiPrompt_working         = {"Please waiti, CSWriter is programming/testing module"                         , "请等待，烧录器正在编程/测试模块"       }
    guiPrompt_complete        = {"Program/Test completed, the maximum number of licenses has been reached"      , "烧录/测试已完成，已经达到最大许可数量" }

    guiStatus_msg             = {"\r\n\r\nTotal: %6d   Program OK: %6d   Program Failed: %6d   Test OK: %6d   Test Failed: %6d",
                                 "\r\n\r\n总数：%6d   烧录成功：%6d   烧录失败：%6d   测试成功：%6d   测试失败：%6d "}

    guiPromptTable            = {guiPrompt_press, guiPrompt_release, guiPrompt_download, guiPrompt_downloadErr, guiPrompt_working}
    guiPrompID_press          = 1
    guiPrompID_release        = 2
    guiPrompt_download        = 3
    guiPromptID_downloadErr   = 4
    guiPromptID_working       = 5

    --English
    guiID_eng                 = 1
    --Chinese
    guiID_chn                 = 2
    guiLanguageID             = guiID_chn

    local de1DefaultMsg = string.format(guiMsgTable[guiMsgID_idle][guiLanguageID], 1)
    local de2DefaultMsg = string.format(guiMsgTable[guiMsgID_idle][guiLanguageID], 2)
    local de3DefaultMsg = string.format(guiMsgTable[guiMsgID_idle][guiLanguageID], 3)
    local de4DefaultMsg = string.format(guiMsgTable[guiMsgID_idle][guiLanguageID], 4)

    gui_devMsgTable    = {de1DefaultMsg, de2DefaultMsg, de3DefaultMsg, de4DefaultMsg}
    gui_devStatusTable = {guiID_dev1..guiID_idle, guiID_dev2..guiID_idle, guiID_dev3..guiID_idle, guiID_dev4..guiID_idle}

    gui_promptMst      = guiPromptTable[guiPrompt_download][guiLanguageID]
    gui_status         = string.format(guiStatus_msg[guiLanguageID], TotalNum, ProgramOKNum, ProgramFailedNum, TestOKNUm, TestFailedNum)

    guiRender()

end

function guiShowPrompt(promptID)
    gui_promptMst = guiPromptTable[promptID][guiLanguageID]
    guiRender()
end

function guiShowDeviceMsg(id, statusID, msgID, errMsg)
    local devStatus

    if (errMsg == nil) then
        errMsg = ""
    end

    devStatus = guiDevIDTable[id]..statusID

    gui_devStatusTable[id] = devStatus
    gui_devMsgTable[id]    = string.format(guiMsgTable[msgID][guiLanguageID], id)..errMsg

    guiRender()

end

function guiRender()

    sdlg_clear()

    for i = 1, 4 do
        sdlg_info(gui_devStatusTable[i], gui_devMsgTable[i])
    end
    --gui_status = string.format(guiStatus_msg[guiLanguageID], TotalNum, ProgramOKNum, ProgramFailedNum, TestOKNUm, TestFailedNum)
    --sdlg_info("P", gui_promptMst..gui_status)
	sdlg_info("P", gui_promptMst)

    sdlg_show()
end

