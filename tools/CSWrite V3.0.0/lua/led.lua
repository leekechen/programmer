require("devCtrl")

LED_ON  = 1
LED_OFF = 0

ledGpioPin = 10

function ledSwitch(handle, dev, ledStatus)
    local cfg = {dev = dev, pin = ledGpioPin, mode = GPIO_OUTPUT_PUSH_PULL, pull= GPIO_PULL_UP, value = GPIO_LOW}

    if (ledStatus == LED_ON) then
        cfg.value = GPIO_HIGH
    else
        cfg.value = GPIO_LOW
    end

    if (dev == "dev3") then
        cfg.dev = "dev2"
        cfg.pin = 11
    end

    devGpioCfg(handle, cfg)

end


