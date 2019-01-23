require("database")

function dowload(handle, hex)
    local crc32
    local cmdDownload
    local cmdCRC32
    local err = nil
    local MAC = {mini = nil, max = nil, cur = nil}
	local line

    assert(handle ~= NULL, "[Download] Error Argument, when called dowloadICFirmware() handle == nil")

    if(hex.name == nil) then
        -- use the default local firmware file
        hex.name = DEFAULT_LOCAL_FIRMWRAE
    end
    if (hex.times == nil) then
        hex.times = 100
    end
    if (hex.crc32 == nil) then
        hex.crc32 = DEFAULT_LOCAL_FIRMWARE_CRC32
    end

    guiShowPrompt(guiPrompt_download)

    trace("[Download] Starting")
    trace("[Download] hex.name="..hex.name)
    trace("[Download] hex.crc32="..hex.crc32)
    trace("[Download] hex.times="..hex.times)
    trace("[Download] wr_get(handle, \"writer\", \"icCRC32\")")
	
    crc32 = wr_get(handle, "writer", "icCRC32")
    if (crc32 == nil) then
        trace("[Download] wr_get(handle, \"writer\", \"icCRC32\") failed")
    else
        --Got the CSWriter buffer firmware CRC32
        local crc32_hex   = tonumber(hex.crc32)
        local crc32_local = tonumber(crc32)
        if (crc32_hex == crc32_local) then
            trace("[Download] find the firmware in CSWriter's buffer")
            trace("[Download] restore the MAC list")
            MAC = dbTableSummaryGet(crc32)
            if (MAC.mini ~= nil) then
				line = string.format("[Download] MAC info: mini=%s, max=%s, cur=%s", MAC.mini, MAC.max, MAC.cur)
                trace(line)
                return 0, MAC, hex.crc32
            else
                trace("[Download] restore failed, download again")
            end
        else
			line = string.format("[Download] find a old version firmware CRC32=0x%x, new CRC32=0x%x", crc32_local, crc32_hex)
            trace(line)
        end
    end

    cmdDownload = hex.name .. ";" .. hex.times
    cmdCRC32    = string.format("icCRC32:0x%8x", hex.crc32)

    --Set the CRC32 of firmware at first
    trace("[Download] wr_set(handle, \"writer\", "..cmdCRC32..")")
    err = wr_set(handle, "writer", cmdCRC32)
    if (err == nil) then
        trace("[Download] wr_set(handle, \"writer\", cmdCRC32) failed!")
        return err, MAC, hex.crc32
    end

    --Start download the firmware to writer
    trace("[Download] wr_download(handle, "..cmdDownload..")")
    err, MAC.mini, MAC.max = wr_download(handle, cmdDownload)
    if( err == nil ) then
        trace("[Download] wr_download(handle, "..cmdDownload..") failed!"..error_msg())
    else
        if (MAC.mini == nil or MAC.max == nil) then
            trace("[Download] wr_download(handle, "..cmdDownload..") failed!")
            trace("[Download] Got miniMAC= "..MAC.mini.." maxMAC="..MAC.max)
        else
            trace("[Download] wr_download(handle, "..cmdDownload..") successed!")
			--the database MAC.cur must be updated in porgram.lua
            MAC.cur = MAC.mini
            trace("[Download] Update Database MAC info: mini="..MAC.mini..", max="..MAC.max..", cur="..MAC.cur)
            dbTableSummaryInsert(MAC, crc32, DATE, TIME)
        end
    end
	
    return err, MAC, crc32
end
