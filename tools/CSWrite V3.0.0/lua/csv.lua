function csvInit()
    local file
    local csvName

    os.execute("mkdir log")
    os.execute("mkdir _obddata")

    csvName   = string.format("log\\log_%s_%s.csv", DATE, TIME)
    file      = io.open(csvName, "w+")
    file:write("flagh ID,MAC,Program Status,Version,CSE7761,pinLow,pinHigh,rfTest,Date,time\n")
    file:close()
end

function csvInsert(csvRow)
    local file
    local line
    local csvName
    local numMAC

    numMAC = tonumber(csvRow.MAC)

    csvName   = string.format("log\\log_%s_%s.csv", DATE, TIME)
    file = io.open(csvName, "a+")
    line = string.format("%s,%X,%s,%s,%s,%s,%s,%s,%s,%s\n", csvRow.flashID, numMAC, csvRow.programStatus, csvRow.version, csvRow.CSE7761,csvRow.pinLow, csvRow.pinHigh, csvRow.rfTest, csvRow.date, csvRow.time)
    file:write(line)
    file:close()
end

