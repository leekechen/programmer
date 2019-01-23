#include <string.h>
#include <cmsis_os2.h>
#include <stm32f4xx_hal.h>
#include "RTX_Config.h"
#include "MX_Device.h"
#include "w25qxx.h"
#include "error.h"


#define MODULE_DEBUG            0
#define MODULE_ERROR            1

#include "dbg_print.h"

#ifndef MX_SPI1_NSS_Pin
#error "Please modify the MX_Device.h. Modify the the MX_SPI2_NSS_Pin  to MX_SPI1_NSS_Pin" 
#endif


/* SPI Data Flash definitions */
#define PAGE_SIZE_MASK          (PAGE_SIZE - 1)
#define FLASH_SECTOR_SIZE_MASK  (SECTOR_SIZE - 1)

/* SPI Data Flash Commands */
#define CMD_RPDDD               (0xABU) /* Release Power Down Device ID */
#define CMD_READ_DATA           (0x03U)
#define CMD_READ_ID             (0x90U)
#define CMD_READ_STATUS_1       (0x05U)
#define CMD_READ_STATUS_2       (0x35U)
#define CMD_READ_STATUS_3       (0x15U)
#define CMD_WRITE_ENABLE        (0x06U)
#define CMD_WRITE_DISABLE       (0x04U)
#define CMD_WRITE_SR1           (0x01U)
#define CMD_WRITE_SR2           (0x31U)
#define CMD_WRITE_SR3           (0x11U)
#define CMD_WRITE_VSR_ENABLE    (0x50U)
#define CMD_PAGE_PROGRAM        (0x02U)
#define CMD_SECTOR_ERASE        (0x20U)
#define CMD_CHIP_ERASE          (0x60U)
#define CMD_READ_UID            (0x4BU)

/* Flash Driver Flags */
#define FLASH_INIT              (0x01U)
#define FLASH_POWER             (0x02U)

#define MS_TICK_COUNT           (OS_TICK_FREQ/1000)

#define TIMEOUT_MAX             (500/MS_TICK_COUNT)

#if 0
#define SPI_BUS_FLAG            (ARM_SPI_MODE_MASTER | ARM_SPI_CPOL0_CPHA0  \
                                | ARM_SPI_DATA_BITS(8) | ARM_SPI_MSB_LSB    \
                                | ARM_SPI_SS_MASTER_UNUSED)

#else
#define SPI_BUS_FLAG            (ARM_SPI_MODE_MASTER | ARM_SPI_CPOL0_CPHA0  \
                                | ARM_SPI_DATA_BITS(8) | ARM_SPI_MSB_LSB    \
                                | ARM_SPI_SS_MASTER_SW)
#endif

                                
#if ((SPI_BUS_FLAG & ARM_SPI_SS_MASTER_SW) == ARM_SPI_SS_MASTER_SW) 
#define SPI_CS_CONTROL(s, f)    (s)->Control(ARM_SPI_CONTROL_SS, (f))
#else
#define SPI_CS_CONTROL(s, f)    HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, !(f))
#endif

struct flash_dev {
    const char*         name;    
    ARM_DRIVER_SPI*     spi;
    uint32_t            speed;
    uint32_t            flags;
    size_t              size;
    osMutexId_t         lock;
} ;

#if MX_SPI1
extern ARM_DRIVER_SPI Driver_SPI1; 

static struct flash_dev flashDev1 = {
    .name = DEV_SPI1,
    .spi = &Driver_SPI1,
    .lock = NULL,
} ;
#endif 



static struct flash_dev* flash_dev_list[] = { 
#if MX_SPI1 
    &flashDev1,
#endif      
};


#define FLASH_DEV_NUM       (sizeof(flash_dev_list)/sizeof(struct flash_dev*))

static int32_t wait_until_timeout(ARM_DRIVER_SPI* spi, uint32_t length, uint32_t timeout)
{
    uint32_t cur;
    uint32_t end;
    
    end = osKernelGetTickCount() + timeout;
   
    while (spi->GetDataCount() != length) {
        cur = osKernelGetTickCount();   
        if (cur > end) {
            err_print("spi->GetDataCount() timeout!\n");
            return ARM_DRIVER_ERROR_TIMEOUT;
        }
    }

    return ARM_DRIVER_OK;
}

/* Send command with optional data and wait until busy */
static int32_t transfer(ARM_DRIVER_SPI* spi, const uint8_t* send, uint32_t sendSize, uint8_t* recv, uint32_t recvSize) 
{
    int32_t  err;   
    
    /* Select Slave */
    SPI_CS_CONTROL(spi, ARM_SPI_SS_ACTIVE);
    
    /* Send Data */
    if ((send != NULL) && (sendSize != 0)) {   
        err = spi->Send(send, sendSize);
        if (err != ARM_DRIVER_OK) {            
            err_print("spi->Send(%p,%d),ret=%d\n", send, sendSize, err);
            goto transfer_error;
        }
        err = wait_until_timeout(spi, sendSize, TIMEOUT_MAX);
        if (err) {
            err_print("spi->Send() timeout!\n");
            goto transfer_error;
        } 
    }

    /* Receive Data */
    if ((recv != NULL) && (recvSize != 0)) {
        err = spi->Receive(recv, recvSize);
        if (err != ARM_DRIVER_OK) {
            err_print("spi->Receive(%p,%d),ret=%d\n", recv, recvSize, err);
            goto transfer_error;
        }
        err = wait_until_timeout(spi, recvSize, TIMEOUT_MAX);
        if (err) {
            err_print("spi->Receive() timeout!\n");            
            goto transfer_error;
        }
    }
    
    /* Deselect Slave */
    SPI_CS_CONTROL(spi, ARM_SPI_SS_INACTIVE);    

    return err;

transfer_error:
    spi->Control(ARM_SPI_ABORT_TRANSFER, 0);
    SPI_CS_CONTROL(spi, ARM_SPI_SS_INACTIVE);   
    return err;
}

static int32_t wait_for_busy(ARM_DRIVER_SPI* spi)
{
    uint8_t cmd[4];
    uint8_t sr[2];  
    int32_t err; 
    
    /* Prepare Reaf Status Command */
    cmd[0] = CMD_READ_STATUS_1;

    /* Select Slave */
    SPI_CS_CONTROL(spi, ARM_SPI_SS_ACTIVE); 
    
    /* Send Command */    
    err = spi->Send(cmd, 1);
    if (err != ARM_DRIVER_OK) {
        err_print("spi->Send(cmd, 1), err=%d\n", err);
        goto transfer_error;
    }
    err = wait_until_timeout(spi, 1, TIMEOUT_MAX);
    if (err) {
        err_print("spi->Receive() timeout!\n");
        goto transfer_error;
    }

    /* Check Status Register */        
    do {
        err = spi->Receive(&sr[0], 1);
        if (err != ARM_DRIVER_OK) {
            err_print("spi->Receive(&sr[0], 1), ret=%d\n", err);
            goto transfer_error;
        }

        err = wait_until_timeout(spi, 1, TIMEOUT_MAX);
        if (err) {
            err_print("spi->Receive(), timeout, sr=0x%x\n", sr[0]);
            goto transfer_error;
        }
    } while (sr[0] & 0x01);

    /* Deselect Slave */
    SPI_CS_CONTROL(spi, ARM_SPI_SS_INACTIVE); 

    return err;


transfer_error:    
    spi->Control(ARM_SPI_ABORT_TRANSFER, 0);
    SPI_CS_CONTROL(spi, ARM_SPI_SS_INACTIVE); 
    return err;
}


/* Send command with optional data and wait until busy */
static bool read_id (ARM_DRIVER_SPI* spi, uint8_t* deviceID, uint32_t len) 
{
    uint8_t  cmd[4];

    /* Prepare Command with address */
    cmd[0] = CMD_READ_ID;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = 0;

    return transfer(spi, cmd, 4, deviceID, len);    
}


static int32_t read_sr(ARM_DRIVER_SPI* spi, uint8_t* sr, int32_t len)
{
    int32_t  err;
    uint8_t  cmd[4];

    /* Prepare Command with address */
    cmd[0] = CMD_READ_STATUS_1;

    err = transfer(spi, cmd, 1, &sr[0], 1);
    if (err) {
        return err;
    }

    if (len > 1) {
        cmd[0] = CMD_READ_STATUS_2;

        err = transfer(spi, cmd, 1, &sr[1], 1);
        if (err) {
            return err;
        }
    }

    if (len > 2) {
        cmd[0] = CMD_READ_STATUS_3;

        err = transfer(spi, cmd, 1, &sr[2], 1);
        if (err) {
            return err;
        }
    }
    
    return err;
}

static int32_t write_enable(ARM_DRIVER_SPI* spi)
{
    uint8_t  cmd[4];

    cmd[0] = CMD_WRITE_ENABLE;
    return transfer(spi, cmd, 1, NULL, 0);
}

//static int32_t write_disable(ARM_DRIVER_SPI* spi)
//{
//    uint8_t  cmd[4];

//    cmd[0] = CMD_WRITE_DISABLE;
//    return  transfer(spi, cmd, 1, NULL, 0);
//}

static int32_t write_vsr_enable(ARM_DRIVER_SPI* spi)
{
    uint8_t cmd[4];  
    
    cmd[0] = CMD_WRITE_VSR_ENABLE;
    return transfer(spi, cmd, 1, NULL, 0);
}


static int32_t write_sr(ARM_DRIVER_SPI* spi, uint8_t* sr, int32_t flagVSR)
{
    uint8_t cmd[4];

    if (flagVSR) {
        write_vsr_enable(spi);
    } else {
        write_enable(spi);
    }
    
    cmd[0] = CMD_WRITE_SR1;
    cmd[1] = sr[0];
    cmd[2] = sr[1];
    return transfer(spi, cmd, 3, NULL, 0);
}

//static int32_t write_sr2(ARM_DRIVER_SPI* spi, uint8_t sr, int32_t flagVSR)
//{
//    uint8_t cmd[4];
//    
//    if (flagVSR) {
//        write_vsr_enable(spi);
//    } else {
//        write_enable(spi);
//    }
//    
//    cmd[0] = CMD_WRITE_SR2;
//    cmd[1] = sr;
//    return transfer(spi, cmd, 2, NULL, 0);
//}


//static int32_t write_sr3(ARM_DRIVER_SPI* spi, uint8_t sr, int32_t flagVSR)
//{
//    uint8_t cmd[4];
//    
//    if (flagVSR) {
//        write_vsr_enable(spi);
//    } else {
//        write_enable(spi);
//    }    
//    
//    cmd[0] = CMD_WRITE_SR3;
//    cmd[1] = sr;
//    return transfer(spi, cmd, 2, NULL, 0);
//}

//static int32_t reset(ARM_DRIVER_SPI* spi)
//{ 
//    int32_t err;    
//    uint8_t cmd[4]; 
//    
//    write_enable(spi);
//    
//    cmd[0] = 0x66;
//    err = transfer(spi, cmd, 1, NULL, 0);
//    if (err != ARM_DRIVER_OK) {
//        err_print("transfer(cmd, 1, NULL, 0) failed, ret=%d\n", err);
//        return err;
//    }

//    cmd[0] = 0x99;
//    err = transfer(spi, cmd, 1, NULL, 0);
//    if (err != ARM_DRIVER_OK) {
//        err_print("transfer(cmd, 1, NULL, 0) failed, ret=%d\n", err);
//        return err;
//    }
//    
//    return 0;
//}

static int32_t page_program(struct flash_dev* dev, uint32_t addr, const uint8_t *data, uint32_t size) 
{
    int32_t  err;
    uint8_t  cmd[4];

    osMutexAcquire(dev->lock, osWaitForever);
    err = write_enable(dev->spi);
    if (err) {
        osMutexRelease(dev->lock);
        err_print("write_enable() failed,ret=%d\n", err);
        return ARM_DRIVER_ERROR;
    }

    /* Prepare Command with address */
    cmd[0] = CMD_PAGE_PROGRAM;
    cmd[1] = (uint8_t)(addr >> 16);
    cmd[2] = (uint8_t)(addr >>  8);
    cmd[3] = (uint8_t)(addr >>  0);

    /* Select Slave */
    SPI_CS_CONTROL(dev->spi, ARM_SPI_SS_ACTIVE); 
    
    /* Send Command with address */
    err = dev->spi->Send(cmd, sizeof(cmd));
    if (err != ARM_DRIVER_OK) {
        err_print("spi->Send(cmd, sizeof(cmd)), ret=%d\n", err);
        goto transfer_error;
    }

    err = wait_until_timeout(dev->spi, 4, TIMEOUT_MAX);
    if (err) {
        err_print("spi->Send() timeout!\n");
        goto transfer_error;
    }
    
    /* Send Data */
    if ((data != NULL) && (size != 0)) {
        err = dev->spi->Send(data, size);
        if (err != ARM_DRIVER_OK) {
            err_print("spi->Send(data, size), ret=%d\n", err);
            goto transfer_error;
        }
        err = wait_until_timeout(dev->spi, size, TIMEOUT_MAX);
        if (err) {
            err_print("spi->Send() timeout!\n");
            goto transfer_error;
        }
    }

    /* Deselect Slave */
    SPI_CS_CONTROL(dev->spi, ARM_SPI_SS_INACTIVE);

    err = wait_for_busy(dev->spi);
    if (err != ARM_DRIVER_OK) { 
        osMutexRelease(dev->lock);
        err_print("wait_for_busy(),ret=%d\n", err);
        return err;
    }  
    osMutexRelease(dev->lock);
    
    return err;

transfer_error:    
    dev->spi->Control(ARM_SPI_ABORT_TRANSFER, 0);
    SPI_CS_CONTROL(dev->spi, ARM_SPI_SS_INACTIVE);
    osMutexRelease(dev->lock);
    return err;    
}


int32_t w25qxx_init(void)
{
    size_t i;
    int32_t err;

    if (flash_dev_list[0]->lock != NULL) {
        return ARM_DRIVER_OK;
    }

#if MX_SPI1
    NVIC_ClearPendingIRQ(SPI1_IRQn);
    NVIC_EnableIRQ(SPI1_IRQn);
#endif
    
    for (i = 0; i < FLASH_DEV_NUM; i++) {
        err = flash_dev_list[i]->spi->Initialize(NULL);
        if (err != ARM_DRIVER_OK) { 
            err_print("flash %s initialize failed!\n", flash_dev_list[i]->name);
            continue;
        };
        flash_dev_list[i]->flags = FLASH_INIT;
        flash_dev_list[i]->lock = osMutexNew(NULL);
        if (flash_dev_list[i]->lock == NULL) {
            err_print("osMutexNew(0) failed\n");
        }
    }
    
    return ARM_DRIVER_OK;
}

int32_t w25qxx_enable_qe(int32_t handle, uint32_t flag)
{
    int32_t err;
    struct flash_dev* dev;
    uint8_t sr[2];
    dev = (struct flash_dev*)handle;
    
    osMutexAcquire(dev->lock, osWaitForever);

    err = read_sr(dev->spi, sr, 2);
    if (err) {
        osMutexRelease(dev->lock);
        return err;
    }

    if (flag == FLASH_QUAD_MODE_ENABLE) {
        sr[1] |= 0x2;
    } else {
        sr[1] &= ~0x2;
    }

    err = write_sr(dev->spi, sr, 0);
    if (err) {
        osMutexRelease(dev->lock);
        return err;
    }

//    err = read_sr(dev->spi, sr, 2);
//    if (err) {
//        osMutexRelease(dev->lock);
//        return err;
//    }
        
    err = wait_for_busy(dev->spi);
    if (err != ARM_DRIVER_OK) { 
        osMutexRelease(dev->lock);
        err_print("wait_for_busy(),ret=%d\n", err);
        return err;
    }  

//    err = read_sr(dev->spi, sr, 2);
//    if (err) {
//        osMutexRelease(dev->lock);
//        return err;
//    }

    osMutexRelease(dev->lock);
    return err;
}

int32_t w25qxx_read_sr(int32_t handle)
{
    int32_t err;
    struct flash_dev* dev;
    uint8_t sr[2];
    dev = (struct flash_dev*)handle;
    
    osMutexAcquire(dev->lock, osWaitForever);

    err = read_sr(dev->spi, sr, 2);
    if (err) {
        osMutexRelease(dev->lock);
        return err;
    }
    
    osMutexRelease(dev->lock);
    return err;
}


int32_t w25qxx_open(const char* dev, size_t size)
{
    size_t  i;
    int32_t err;

    for (i = 0; i < FLASH_DEV_NUM; i++) {
        flash_dev_list[i]->size = size;
        err = strncmp(dev, flash_dev_list[i]->name, strlen(flash_dev_list[i]->name));
        if (err == 0) {
            return (int32_t)flash_dev_list[i];
        }
    }
    err_print("%s invalid!\n", dev);
    
    return ARM_DRIVER_ERROR_PARAMETER;
}

int32_t w25qxx_read(int32_t handle, uint32_t addr, uint8_t* buf, size_t size)
{
    uint8_t cmd[4];
    int32_t err;
    struct flash_dev* dev;

    dbg_print("w25qxx_read() handle=0x%x,addr=0x%x,buf=%p,size=%d\n",
            handle, addr, buf, size);

    dev = (struct flash_dev*)handle;
    handle_check(dev, flash_dev_list, struct flash_dev*, ARM_DRIVER_ERROR_PARAMETER);

    if (buf == NULL) {
        return ARM_DRIVER_ERROR_PARAMETER;
    }     

    if (size == 0) {
        return ARM_DRIVER_OK;
    }

    /* Prepare Command with address */
    cmd[0] = CMD_READ_DATA;
    cmd[1] = (uint8_t)(addr >> 16);
    cmd[2] = (uint8_t)(addr >>  8);
    cmd[3] = (uint8_t)(addr >>  0);
    
    osMutexAcquire(dev->lock, osWaitForever);

    err = transfer(dev->spi, cmd, 4, buf, size);    
    if (err != ARM_DRIVER_OK) {
        err_print("%s,transfer() failed,ret=%d, buf=%p,size=%d\n", dev->name, err, buf, size);
    } else {    
        osMutexRelease(dev->lock);
        return (int32_t)dev->spi->GetDataCount();
    }
    osMutexRelease(dev->lock);
    
    return err;
}

int32_t w25qxx_program(int32_t handle, uint32_t addr, const uint8_t* data, size_t size)
{
    int32_t err;
    const uint8_t* buf;
    uint32_t len;    
    struct flash_dev* dev;    

    dbg_print("w25qxx_program() handle=0x%x,addr=0x%x,data=%p,size=%d\n",
            handle, addr, data, size);

    dev = (struct flash_dev*)handle;  

    handle_check(dev, flash_dev_list, struct flash_dev*, ARM_DRIVER_ERROR_PARAMETER);

    for (buf = data; size > 0; size -= len, buf += len, addr += len) {
        len = PAGE_SIZE - (addr % PAGE_SIZE);
        len = len > size ? size : len;
        err = page_program(dev, addr, buf, len);
        if (err) {
            err_print("page_program() failed!\n");
            return err;
        }
    }
    return (int32_t)(buf - data);
}


int32_t w25qxx_erase_sector(int32_t handle, uint32_t addr)
{
    uint8_t cmd[4];
    int32_t err;
    struct flash_dev* dev;

    dev = (struct flash_dev*)handle;
    handle_check(dev, flash_dev_list, struct flash_dev*, ARM_DRIVER_ERROR_PARAMETER);

    dbg_print("%s,EraseSector(0x%x)\n", dev->name, addr);    

    cmd[0] = CMD_SECTOR_ERASE;
    cmd[1] = (uint8_t)(addr >> 16);
    cmd[2] = (uint8_t)(addr >>  8);
    cmd[3] = (uint8_t)(addr >>  0);

    osMutexAcquire(dev->lock, osWaitForever);
    err = write_enable(dev->spi);
    if (err) {
        osMutexRelease(dev->lock);
        err_print("%s,write_enable() failed\n", dev->name);
        return ARM_DRIVER_ERROR;
    }

    err = transfer(dev->spi, cmd, 4, NULL, 0);
    if (err) {   
        osMutexRelease(dev->lock);
        err_print("%s,transfer(%p,4,0,0),ret=%d\n", dev->name, cmd, err);
        return err;
    }

    err = wait_for_busy(dev->spi);
    if (err) {
        osMutexRelease(dev->lock); 
        err_print("%s,wait_for_busy(),ret=%d\n", dev->name, err);
        return err;
    }
    
    osMutexRelease(dev->lock);
    
    return ARM_DRIVER_OK;
}

int32_t w25qxx_chip_erase(int32_t handle)
{
    uint8_t cmd[4];
    int32_t err;
    struct flash_dev* dev;

    dev = (struct flash_dev*)handle;
    handle_check(dev, flash_dev_list, struct flash_dev*, ARM_DRIVER_ERROR_PARAMETER);

    dbg_print("%s,ChipErase()\n", dev->name);    

    cmd[0] = CMD_CHIP_ERASE;

    osMutexAcquire(dev->lock, osWaitForever);
    err = write_enable(dev->spi);
    if (err) {
        osMutexRelease(dev->lock);
        err_print("%s,write_enable() failed\n", dev->name);
        return ARM_DRIVER_ERROR;
    }

    err = transfer(dev->spi, cmd, 1, NULL, 0);
    if (err) {   
        osMutexRelease(dev->lock);
        err_print("%s,transfer(%p,4,0,0),ret=%d\n", dev->name, cmd, err);
        return err;
    }

    err = wait_for_busy(dev->spi);
    if (err) {
        osMutexRelease(dev->lock); 
        err_print("%s,wait_for_busy(),ret=%d\n", dev->name, err);
        return err;
    }
    
    osMutexRelease(dev->lock);
    
    return ARM_DRIVER_OK;
}

int32_t w25qxx_power_control (int32_t handle, ARM_POWER_STATE state, uint32_t speed) 
{
    int32_t err;
    struct flash_dev* dev;

    dev = (struct flash_dev*)handle;
    handle_check(dev, flash_dev_list, struct flash_dev*, ARM_DRIVER_ERROR_PARAMETER);

    switch (state) {
    case ARM_POWER_OFF:
        osMutexAcquire(dev->lock, osWaitForever);
        err = dev->spi->PowerControl(ARM_POWER_OFF);
        osMutexRelease(dev->lock);
        break;
    case ARM_POWER_FULL:
        osMutexAcquire(dev->lock, osWaitForever);
        if ((dev->flags & FLASH_INIT) == 0U) {
            osMutexRelease(dev->lock);
            return ARM_DRIVER_ERROR;
        }

        if ((dev->flags & FLASH_POWER) == 0U) {            
            err = dev->spi->PowerControl(ARM_POWER_FULL);
            if (err != ARM_DRIVER_OK) {
                osMutexRelease(dev->lock);
                return ARM_DRIVER_ERROR;
            }
            err = dev->spi->Control(SPI_BUS_FLAG, speed);
            if (err != ARM_DRIVER_OK) {
                osMutexRelease(dev->lock);
                return ARM_DRIVER_ERROR;
            }
           
            dev->flags |= FLASH_POWER;
        }
        osMutexRelease(dev->lock);
        
        return ARM_DRIVER_OK;

    case ARM_POWER_LOW:
        return ARM_DRIVER_ERROR_UNSUPPORTED;

    default:
        return ARM_DRIVER_ERROR;
    }
    return ARM_DRIVER_ERROR;
}

int32_t w25qxx_erase(int32_t handle, uint32_t addr, size_t size)
{
    int32_t  err;
    uint32_t alignAddr;
    uint32_t endAddr;
    struct flash_dev* dev;    

    dbg_print("w25qxx_erase() handle=0x%x,addr=0x%x,size=%d\n",
            handle, addr, size);

    dev = (struct flash_dev*)handle;
    handle_check(dev, flash_dev_list, struct flash_dev*, ARM_DRIVER_ERROR_PARAMETER);
    
    if (addr == 0 && size == dev->size) {
        return w25qxx_chip_erase(handle);
    }
    alignAddr = addr & (~FLASH_SECTOR_SIZE_MASK);
    endAddr   = addr + size;

    for (err = -1; alignAddr < endAddr; alignAddr += SECTOR_SIZE) {
        err = w25qxx_erase_sector(handle, alignAddr);
        if (err) {            
            err_print("%s,EraseSector(0x%x),result=%d\n", dev->name, alignAddr, err);
            return err;
        }
    }
    
    return err;
}

int32_t w25qxx_read_id(int32_t handle, uint8_t* buf, size_t size)
{
    int32_t err;
    struct flash_dev* dev;

    dev = (struct flash_dev*)handle;
    handle_check(dev, flash_dev_list, struct flash_dev*, ARM_DRIVER_ERROR_PARAMETER);

    osMutexAcquire(dev->lock, osWaitForever);
    err = read_id(dev->spi, buf, size);
    osMutexRelease(dev->lock);
    
    return err;
}


int32_t w25qxx_read_uid(int32_t handle, uniqueID_t id)
{
    int32_t err;
    struct flash_dev* dev;
    uint8_t cmd[5];

    dev = (struct flash_dev*)handle;
    handle_check(dev, flash_dev_list, struct flash_dev*, ARM_DRIVER_ERROR_PARAMETER);

    cmd[0] = CMD_READ_UID;    

    osMutexAcquire(dev->lock, osWaitForever);
    err = transfer(dev->spi, cmd, 5, id, sizeof(uniqueID_t));
    osMutexRelease(dev->lock);
    
    return err;
}


int32_t w25qxx_close(int32_t handle)
{   
    (void)handle;
    return 0;
}



