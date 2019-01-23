#include <stdint.h>
#include <stdlib.h>

#include "cmsis_os2.h"
#include "files.h"
#include "cswrite_cfg.h"
#include "error.h"
#include "key.h"

static struct tlv_file ic;

static osSemaphoreId_t sem_id;

int32_t file_ic_init(void)
{
    sem_id = osSemaphoreNew(1, 0, NULL);
    if (sem_id == NULL) {
        return ERR_SYS_OS;
    }

    tlv_file_init(&ic);

    ;
    
    return ERR_OK;
}

void* file_ic_open(const char* mode)
{
    struct key_event key;
    key.code = KEY_CODE_READ_IC;
    key.state = KEY_STATE_PRESS;
    key_set(&key);
    return fopen(FILE_IC, mode);
}


int32_t file_ic_write(const uint8_t* data, size_t size, void* fp)
{
    return fwrite(data, 1, size, fp);
}

int32_t file_ic_read(uint8_t* buf, size_t size, void* fp)
{

    return fread(buf, 1, size, fp);

}

int32_t file_ic_close(void* fp)
{

    return fclose(fp);

}

uint32_t file_ic_get_size(void)
{
    return cswrite_cfg_get_chip_size();
}


