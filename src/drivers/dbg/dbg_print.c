#include "cmsis_os2.h"
#include "dbg_print.h"

static osMutexId_t logMutex; 

void dbg_init(void)
{
#if (_DEBUG)   
    logMutex = osMutexNew(NULL);
    if (logMutex == NULL) {
        printf("%s(),%d,osMutexNew(0) failed\n", __func__, __LINE__);
        return;
    }
#endif    
}

void dbg_lock(void)
{    
    osStatus_t err;
    
    err = osMutexAcquire(logMutex, osWaitForever);
    if (err) {
        printf("%s(),%d,osMutexAcquire(%p) failed,err=%d\n", __func__, __LINE__, logMutex, err);
        return;
    } 
}

void dbg_unlock(void)
{
    osStatus_t err;

    err = osMutexRelease(logMutex); 
    if (err) {
        printf("%s(),%d,osMutexRelease(%p) failed,err=%d\n", __func__, __LINE__,logMutex, err);
        return;
    } 
}
