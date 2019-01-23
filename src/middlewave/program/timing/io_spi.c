#include <stdlib.h>
#include <stm32f4xx_hal.h>
#include "Driver_SPI.h"
#include "error.h"

//#define SPI_BUS_FLAG            (ARM_SPI_MODE_MASTER | ARM_SPI_CPOL0_CPHA0  \
//                                | ARM_SPI_DATA_BITS(8) | ARM_SPI_MSB_LSB    \
//                                | ARM_SPI_SS_MASTER_UNUSED)


//extern ARM_DRIVER_SPI Driver_SPI3; 

//static ARM_DRIVER_SPI* drv = &Driver_SPI3;

//extern SPI_HandleTypeDef hspi3;

//int32_t io_spi_init(void)
//{
//    int32_t err;

//    NVIC_ClearPendingIRQ(SPI3_IRQn);
//    NVIC_EnableIRQ(SPI3_IRQn);

//    hspi3.Instance = SPI3;
//    hspi3.Init.Mode = SPI_MODE_MASTER;
//    hspi3.Init.Direction = SPI_DIRECTION_2LINES;
//    hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
//    hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
//    hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
//    hspi3.Init.NSS = SPI_NSS_SOFT;
//    hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
//    hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
//    hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
//    hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
//    hspi3.Init.CRCPolynomial = 10;
//    if (HAL_SPI_Init(&hspi3) != HAL_OK) {
//        _Error_Handler(__FILE__, __LINE__);
//    }    
//    
//    err = drv->Initialize(NULL);
//    if (err != ARM_DRIVER_OK) {
//        return err;
//    }
//    
//    err = drv->PowerControl(ARM_POWER_FULL);
//    if (err != ARM_DRIVER_OK) {
//        return err;
//    }
//    
//    err = drv->Control(SPI_BUS_FLAG, 2000000);
//    if (err != ARM_DRIVER_OK) {
//        return err;
//    }
//    
//    return err;
//}

//int32_t io_spi_send_byte(uint8_t data)
//{
//    int32_t err;
//    
//    err = drv->Send(&data, 1);
//    while (drv->GetDataCount() != 1) {
//        ;
//    }

//    return err;
//}

//int32_t io_spi_send_word(uint16_t word)
//{
//    int32_t err;
//    
//    err = drv->Send(&word, 2);
//    while (drv->GetDataCount() != 2) {
//        ;
//    }
//    
//    return err;
//}

//int32_t io_spi_read_byte(uint8_t* data)
//{
//    int32_t err;

//    err = drv->Receive(data, 1);
//    while (drv->GetDataCount() != 1) {
//        ;
//    }
//    
//    return err;
//}

//int32_t io_spi_read_word(uint16_t* word)
//{
//    int32_t err;

//    err = drv->Receive(word, 2);
//    while (drv->GetDataCount() != 2) {
//        ;
//    }
//    
//    return err;  
//}

//int32_t io_spi_send_dummy(uint8_t num)
//{
//    int32_t err;

//    uint8_t* data;

//    data = malloc(num);
//    if (data == NULL) {
//        return ERR_SYS_NOT_ENOUGH_MEMORY;
//    }
//    
//    err = drv->Send(&data, num);
//    while (drv->GetDataCount() != num) {
//        ;
//    }
//    
//    return err;
//}
