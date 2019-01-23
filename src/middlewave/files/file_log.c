#include "files.h"
#include "error.h"
#include "tlv.h"
#include "fram_map.h"
#include "cswrite_cfg.h"
#include "crc32_soft.h"

#define MODULE_DEBUG       1
#define MODULE_ERROR       1

#include "dbg_print.h"




int32_t file_log_upgrade(void)
{
    struct tlv_file log;

    cswrite_cfg_get_id(&log.list_id);
    cswrite_cfg_get_version(&log.list_version);
//    cswrite_get_desc_time(&log.list_time);

//    cswrite_log_get_desc_log(&log.list_log);

    return tlv_file_pack(&log);
}

void* file_log_open(const char* mode)
{
    return fopen(FILE_LOG, mode);
}

int32_t file_log_read(uint8_t* buf, size_t size, void* fp)
{    
    return fread(buf, 1, size, fp);
}

int32_t file_log_write(const uint8_t* data, size_t size, void* fp)
{
    return fwrite(data, 1, size, fp);
}

int32_t file_log_close(void* fp)
{
    return fclose(fp);
}




