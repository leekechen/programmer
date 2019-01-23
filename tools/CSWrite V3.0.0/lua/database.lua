sqlite3 = assert(require("lsqlite3"))

DB_TABLE_PROGRAM               = "program"
DB_TABLE_TEST                  = "test"
DB_TABLE_SUMMARY               = "summary"
DB_FILE_NAME                   = "log\\test.db"

ERR_MSG_FAILED      = "Failed"
ERR_MSG_PASSED      = "Passed"
ERR_MSG_NONE_TESTED = "None Test"

tableProgramRow = {flashID = nil, MAC = nil, status = nil, date = nil, time = nil}

function dbInit()
    local ret
    local sql
    local db

    db, errCode, errMsg = sqlite3.open(DB_FILE_NAME)
    assert(db ~= nil, errMsg)

    sql = [=[
            CREATE TABLE IF NOT EXISTS program(
                flashID     TEXT    PRIMARY KEY,
                MAC         TEXT               ,
                errCode     INTEGER            ,
                date        TEXT               ,
                time        TEXT);
        ]=]

    ret = db:exec(sql)
    assert(ret == sqlite3.OK, db:errmsg())

    sql = [=[
            CREATE TABLE IF NOT EXISTS  test(
                flashID     TEXT    PRIMARY KEY,
                MAC         TEXT               ,
                version     TEXT               ,
                CSE7761     TEXT               ,
                pinLow      TEXT               ,
                pinHigh     TEXT               ,
                rfTest      TEXT               ,
                errCode     TEXT               ,
                date        TEXT               ,
                time        TEXT                );
        ]=]

    ret = db:exec(sql)
    assert(ret == sqlite3.OK, db:errmsg())

    sql = [=[
            CREATE TABLE IF NOT EXISTS  summary(
                miniMAC             TEXT    NOT NULL   ,
                maxMAC              TEXT    NOT NULL   ,
                curMAC              TEXT    NOT NULL   ,
                firmwareCRC32       TEXT    NOT NULL   ,
                date                TEXT               ,
                time                TEXT               ,
                programOkNum        TEXT               ,
                programFailedNum    TEXT               ,
                testOkNum           TEXT               ,
                testFailedNum       TEXT               ,
                shortCircuitNum     TEXT               ,
                openCircuitNUm      TEXT               ,
                PRIMARY KEY(miniMAC, maxMAC, firmwareCRC32));
        ]=]

    ret = db:exec(sql)
    assert(ret == sqlite3.OK, db:errmsg())

    db:close()
end

function dbTableSummaryInsert(MAC, firmwareCRC32, data, time)
    local db
    local sql = string.format("INSERT INTO %s (miniMAC, maxMAC, curMAC, firmwareCRC32, date, time) VALUES (\"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\");",
                            DB_TABLE_SUMMARY, MAC.mini, MAC.max, MAC.cur, firmwareCRC32, data, time)

    db, errCode, errMsg = sqlite3.open(DB_FILE_NAME)
    assert(db ~= nil, errMsg)

    ret = db:exec(sql)
    assert(ret == sqlite3.OK, db:errmsg())

--[[    sql = string.format("SELECT * FROM %s", DB_TABLE_SUMMARY)
    for row in db:nrows(sql) do
        print("dbTableSummaryInsert() miniMAC",         row.miniMAC)
        print("dbTableSummaryInsert() maxMAC",          row.maxMAC)
        print("dbTableSummaryInsert() curMAC",          row.curMAC)
        print("dbTableSummaryInsert() firmwareCRC32",   row.firmwareCRC32)
    end--]]

    db:close()
end

function dbTableSummaryGet(crc32)
    local db
    local ret
    local stmt
    local sql
    local MAC = {mini = nil, max = nil, cur = nil}

    assert(crc32 ~= nil)

    db, errCode, errMsg = sqlite3.open(DB_FILE_NAME)
    assert(db ~= nil, errMsg)

    sql = string.format("SELECT miniMAC, maxMAC, curMAC FROM %s WHERE firmwareCRC32 = ? AND curMAC <= maxMAC;", DB_TABLE_SUMMARY)
    stmt = db:prepare(sql)
    assert(stmt ~= nil, db:errmsg())

    ret = stmt:bind_values(crc32)
    assert(ret == sqlite3.OK, db:errmsg())

    ret = stmt:step()
    if (ret == sqlite3.ROW) then
        MAC.mini, MAC.max, MAC.cur = stmt:get_uvalues()

        print("dbTableSummaryGet() MAC.mini", MAC.mini)
        print("dbTableSummaryGet() MAC.max", MAC.max)
        print("dbTableSummaryGet() MAC.cur", MAC.cur)
    end

    stmt:finalize()

    sql = string.format("SELECT * FROM %s", DB_TABLE_SUMMARY)
    for row in db:nrows(sql) do
        print("dbTableSummaryGet() miniMAC", row.miniMAC)
        print("dbTableSummaryGet() maxMAC", row.maxMAC)
        print("dbTableSummaryGet() curMAC", row.curMAC)
        print("dbTableSummaryGet() firmwareCRC32", row.firmwareCRC32)
    end
    
    db:close()

    return MAC
end

function dbTableSummaryUpdateCurMAC(MAC, crc32)
    local db
    local stmt
    local sql
    local err

    assert(MAC ~= nil)
    assert(MAC.mini ~= nil)
    assert(MAC.max ~= nil)
    assert(MAC.cur ~= nil)
    assert(crc32 ~= nil)

    db, err, errMsg = sqlite3.open(DB_FILE_NAME)
    assert(db ~= nil, errMsg)

    sql = string.format("UPDATE summary SET curMAC = ? WHERE firmwareCRC32 = \"%s\" AND (miniMAC = \"%s\" AND maxMAC = \"%s\");",
                        crc32, MAC.mini, MAC.max)
    stmt = db:prepare(sql)
    assert(stmt ~= nil, db:errmsg())

    err = stmt:bind_values(MAC.cur)
    assert(err == sqlite3.OK, db:errmsg())

    err = stmt:step()

    stmt:finalize()
    db:close()

    return strMiniMAC, strMaxMAC, strCurMAC
end

function dbTableGetErrCode(db, dbTable, flashID)
    local err
    local stmt
    local sql

    assert(db ~= nil)
    assert(dbTable ~= nil)
    assert(flashID ~= nil)

    sql = string.format("SELECT errCode FROM %s WHERE flashID = ?;", dbTable)
    stmt = db:prepare(sql)
    assert(stmt ~= nil, db:errmsg())

    err = stmt:bind_values(flashID)
    assert(err == sqlite3.OK, db:errmsg())

    err = stmt:step()
    if (err == sqlite3.ROW) then
        err = stmt:get_uvalues()
    else
        err = nil
    end

    stmt:finalize()

    return err
end

function dbDevIsProgramed(flashID)
    local db
    local i
    local errCode
    local num     = 0
    local devList = {}
	local line
	
    assert(flashID ~= nil)

    db, err, errMsg = sqlite3.open(DB_FILE_NAME)
    assert(db ~= nil, errMsg)

--[[    sql = string.format("SELECT * FROM %s", DB_TABLE_PROGRAM)
    for row in db:nrows(sql) do
        print("dbDevIsTested() flashID", row.flashID)
        print("dbDevIsTested() MAC", row.MAC)
        print("dbDevIsTested() errCode", row.errCode)
        print("dbDevIsTested() date", row.date)
        print("dbDevIsTested() time", row.time)
    end --]]

    for i = 1, 4 do
        if (flashID[i] ~= nil) then
            errCode = dbTableGetErrCode(db, DB_TABLE_PROGRAM, flashID[i])
            if (errCode == ERR_PROGRAM_OK) then
				line = string.format("[DB] device had been programmed ok! flashID==%s", flashID[i])                
            else
                num = num + 1
                devList[i] = DEVICE_TABLE[i]
                if (ret == nil) then
					line = string.format("[DB] device had not been found! flashID=%s", flashID[i])
                else
					line = string.format("[DB] device had been programmed failed, last errCode=%s,flashID=%s", errCode, flashID[i])
                end
            end
			trace(line)
        end
    end

    db:close()

    return devList, num
end

function dbDevIsTested(devList, flashID)
    local i
    local errCode
    local num           = 0
    local failedDevList = {}
	local line

    assert(flashID ~= nil)
    assert(devList ~= nil)

    db, err, errMsg = sqlite3.open(DB_FILE_NAME)
    assert(db ~= nil, errMsg)

--[[    sql = string.format("SELECT * FROM %s", DB_TABLE_TEST)
    for row in db:nrows(sql) do
        print("dbDevIsTested() flashID", row.flashID)
        print("dbDevIsTested() MAC", row.MAC)
        print("dbDevIsTested() version", row.version)
        print("dbDevIsTested() CSE7761", row.CSE7761)
        print("dbDevIsTested() pinLow", row.pinLow)
        print("dbDevIsTested() pinHigh", row.pinHigh)
        print("dbDevIsTested() rfTest", row.rfTest)
        print("dbDevIsTested() errCode", row.errCode)
        print("dbDevIsTested() date", row.date)
        print("dbDevIsTested() time", row.time)
    end --]]

    for i = 1, 4 do
        if (devList[i] ~= nil and flashID[i] ~= nil) then
            errCode = dbTableGetErrCode(db, DB_TABLE_TEST, flashID[i])
            if (errCode == ERR_TEST_OK) then
				line = string.format("[DB] device had been tested ok! flashID=%s", flashID[i])                
            else
                num = num + 1
                failedDevList[i] = devList[i]
                if (errCode == nil) then
					line = string.format("[DB] device had not been found! flashID=%s", flashID[i])  
                else
					line = string.format("[DB] device had been tested failed, last errCode=%s, flashID=%s", errCode, flashID[i]) 
                end
            end
			trace(line)
        end
    end

    db:close()

    return failedDevList, num
end

--Database Table "program"
function dbTableProgramAdd(db, row)
    local err

    assert(db ~= nil)
    assert(row ~= nil)

    sql = string.format("INSERT INTO %s VALUES(\"%s\", \"%s\", \"%s\", \"%s\", \"%s\" )",
                        DB_TABLE_PROGRAM, row.flashID, row.MAC, row.errCode, row.date, row.time)

    err = db:exec(sql)
    assert(err == sqlite3.OK, db:errmsg())

    return err
end

function dbTableProgramUpdate(db, row)
    local err

    assert(db ~= nil)
    assert(row ~= nil)

--[[    sql = string.format("SELECT * FROM %s", DB_TABLE_PROGRAM)
    for row in db:nrows(sql) do
        print("1 dbTableProgramUpdate() flashID", row.flashID)
        print("1 dbTableProgramUpdate() MAC", row.MAC)
        print("1 dbTableProgramUpdate() errCode", row.errCode)
        print("1 dbTableProgramUpdate() date", row.date)
        print("1 dbTableProgramUpdate() time", row.time)
    end--]]

    sql = string.format("UPDATE %s SET MAC = \"%s\", errCode = \"%s\", date = \"%s\", time = \"%s\" WHERE flashID = \"%s\"",
            DB_TABLE_PROGRAM,row.MAC, row.errCode, row.date, row.time, row.flashID)
    err = db:exec(sql)
    assert(err == sqlite3.OK, db:errmsg())

--[[    sql = string.format("SELECT * FROM %s", DB_TABLE_PROGRAM)
    for row in db:nrows(sql) do
        print("2 dbTableProgramUpdate() flashID", row.flashID)
        print("2 dbTableProgramUpdate() MAC", row.MAC)
        print("2 dbTableProgramUpdate() errCode", row.errCode)
        print("2 dbTableProgramUpdate() date", row.date)
        print("2 dbTableProgramUpdate() time", row.time)
    end --]]

    return err
end

function dbTableProgramGetErrCode(db, flashID)
    return dbTableGetErrCode(db, DB_TABLE_PROGRAM, flashID)
end

function dbTableProgramInsert(row)
    local db
    local err

    db, err, errMsg = sqlite3.open(DB_FILE_NAME)
    assert(db ~= nil, errMsg)

    err = dbTableProgramGetErrCode(db, row.flashID)
    if (err ~= nil) then
        err = dbTableProgramUpdate(db, row)
    else
        err = dbTableProgramAdd(db, row)
    end

    db:close()

    return err
end

function dbTableTestAdd(db, row)
    local sql
    local err

    assert(db ~= nil)
    assert(row ~= nil)


--[[    print("dbTableTestAdd")--]]

    sql = string.format("INSERT INTO %s VALUES(\"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\", \"%s\" )",
                         DB_TABLE_TEST, row.flashID, row.MAC, row.version, row.CSE7761, row.pinLow, row.pinHigh, row.rfTest, row.errCode, row.date, row.time)
    err = db:exec(sql)
    assert(err == sqlite3.OK, db:errmsg())

    return err
end

function dbTableTestUpdate(db, row)
    local sql
    local err

    assert(db ~= nil)
    assert(row ~= nil)

--[[    print("dbTableTestUpdate")--]]

    sql = string.format("UPDATE %s SET MAC = \"%s\", version = \"%s\", CSE7761 = \"%s\", pinLow = \"%s\", pinHigh = \"%s\", rfTest = \"%s\", errCode = \"%s\", date = \"%s\", time = \"%s\" WHERE flashID = \"%s\"",
                         DB_TABLE_TEST, row.MAC, row.version, row.CSE7761, row.pinLow, row.pinHigh, row.rfTest, row.errCode, row.date, row.time, row.flashID)
    err = db:exec(sql)
    assert(err == sqlite3.OK, db:errmsg())

    return err
end

function dbTableTestGetErrCode(db, flashID)
    return dbTableGetErrCode(db, DB_TABLE_TEST, flashID)
end

function dbTableTestInsert(row)
    local db
    local sql
    local err

    db, err, errMsg = sqlite3.open(DB_FILE_NAME)
    assert(db ~= nil, errMsg)

--[[    sql = string.format("SELECT * FROM %s", DB_TABLE_TEST)
    for row in db:nrows(sql) do
        print("1 dbTableTestInsert() flashID", row.flashID)
        print("1 dbTableTestInsert() MAC", row.MAC)
        print("1 dbTableTestInsert() version", row.version)
        print("1 dbTableTestInsert() CSE7761", row.CSE7761)
        print("1 dbTableTestInsert() pinLow", row.pinLow)
        print("1 dbTableTestInsert() pinHigh", row.pinHigh)
        print("1 dbTableTestInsert() rfTest", row.rfTest)
        print("1 dbTableTestInsert() errCode", row.errCode)
        print("1 dbTableTestInsert() date", row.date)
        print("1 dbTableTestInsert() time", row.time)
    end --]]

    err = dbTableTestGetErrCode(db, row.flashID)
    if (err == nil) then
        err = dbTableTestAdd(db, row)
    else
        err = dbTableTestUpdate(db, row)
    end

--[[    sql = string.format("SELECT * FROM %s", DB_TABLE_TEST)
    for row in db:nrows(sql) do
        print("2 dbTableTestInsert() flashID", row.flashID)
        print("2 dbTableTestInsert() MAC", row.MAC)
        print("2 dbTableTestInsert() version", row.version)
        print("2 dbTableTestInsert() CSE7761", row.CSE7761)
        print("2 dbTableTestInsert() pinLow", row.pinLow)
        print("2 dbTableTestInsert() pinHigh", row.pinHigh)
        print("2 dbTableTestInsert() rfTest", row.rfTest)
        print("2 dbTableTestInsert() errCode", row.errCode)
        print("2 dbTableTestInsert() date", row.date)
        print("2 dbTableTestInsert() time", row.time)
    end --]]

    db:close()

    return err
end

function dbTabeGetRow(db, dbTable, flashID)
    local sql

    assert(db ~= nil)
    assert(dbTable ~= nil)
    assert(flashID ~= nil)

    sql = string.format("SELECT * FROM %s WHERE flashID=\"%s\"", dbTable, flashID )
    for row in db:nrows(sql) do
        return row
    end

    return row
end

function dbCsvRowCreate(flashIDList)
    local db
    local sql
    local err
    local programTableRow = {flashID = nil, MAC = nil, errCode = nil, date = nil, time =nil}
    local csvRow  = {flashID = nil, MAC = nil, programStatus = nil, CSE7761 = nil, pinLow = nil, pinHigh = nil, rfTest = nil, date = nil, time = nil}
    local testTableRow = {flashID = nil, MAC = nil, version = nil, CSE7761 = nil, pinLow = nil, pinHigh = nil, rfTest = nil, errCode = nil, date = nil, time = nil}

    db, err, errMsg = sqlite3.open(DB_FILE_NAME)
    assert(db ~= nil, errMsg)

    for i = 1, 4 do
        if (flashIDList[i] ~= nil) then
            csvRow.flashID = flashIDList[i]

            programTableRow = dbTabeGetRow(db, DB_TABLE_PROGRAM, csvRow.flashID)
            if (programTableRow ~= nil) then
                csvRow.MAC = programTableRow.MAC
                if (programTableRow.errCode ~= ERR_PROGRAM_OK) then
                    csvRow.programStatus = ERR_MSG_FAILED
                else
                    csvRow.programStatus = ERR_MSG_PASSED
                end
                csvRow.date = programTableRow.date
                csvRow.time = programTableRow.time
            end

            testTableRow = dbTabeGetRow(db, DB_TABLE_TEST, csvRow.flashID)
            if (testTableRow ~= nil) then
                csvRow.version  = testTableRow.version
                csvRow.CSE7761  = testTableRow.CSE7761
                csvRow.pinLow   = testTableRow.pinLow
                csvRow.pinHigh  = testTableRow.pinHigh
                csvRow.rfTest   = testTableRow.rfTest
                csvRow.date     = testTableRow.date
                csvRow.time     = testTableRow.time
            end
            csvInsert(csvRow)
        end
    end

    db:close()
end

