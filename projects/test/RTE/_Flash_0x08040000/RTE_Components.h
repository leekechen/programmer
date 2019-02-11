
/*
 * Auto generated Run-Time-Environment Component Configuration File
 *      *** Do not modify ! ***
 *
 * Project: 'test' 
 * Target:  'Flash_0x08040000' 
 */

#ifndef RTE_COMPONENTS_H
#define RTE_COMPONENTS_H


/*
 * Define the Device Header File: 
 */
#define CMSIS_device_header "stm32f4xx.h"

#define RTE_CMSIS_RTOS2                 /* CMSIS-RTOS2 */
        #define RTE_CMSIS_RTOS2_RTX5            /* CMSIS-RTOS2 Keil RTX5 */
#define RTE_Compiler_EventRecorder
          #define RTE_Compiler_EventRecorder_DAP
#define RTE_Compiler_IO_File            /* Compiler I/O: File */
          #define RTE_Compiler_IO_File_FS         /* Compiler I/O: File (File System) */
#define RTE_Compiler_IO_STDERR          /* Compiler I/O: STDERR */
          #define RTE_Compiler_IO_STDERR_User     /* Compiler I/O: STDERR User */
#define RTE_Compiler_IO_STDIN           /* Compiler I/O: STDIN */
          #define RTE_Compiler_IO_STDIN_User      /* Compiler I/O: STDIN User */
#define RTE_Compiler_IO_STDOUT          /* Compiler I/O: STDOUT */
          #define RTE_Compiler_IO_STDOUT_User     /* Compiler I/O: STDOUT User */
#define RTE_Compiler_IO_TTY             /* Compiler I/O: TTY */
          #define RTE_Compiler_IO_TTY_User        /* Compiler I/O: TTY User */
#define RTE_DEVICE_FRAMEWORK_CUBE_MX
#define RTE_DEVICE_HAL_ADC
#define RTE_DEVICE_HAL_COMMON
#define RTE_DEVICE_HAL_CORTEX
#define RTE_DEVICE_HAL_CRC
#define RTE_DEVICE_HAL_DAC
#define RTE_DEVICE_HAL_DMA
#define RTE_DEVICE_HAL_FLASH
#define RTE_DEVICE_HAL_GPIO
#define RTE_DEVICE_HAL_I2C
#define RTE_DEVICE_HAL_PCD
#define RTE_DEVICE_HAL_PWR
#define RTE_DEVICE_HAL_RCC
#define RTE_DEVICE_HAL_SPI
#define RTE_DEVICE_HAL_TIM
#define RTE_DEVICE_HAL_UART
#define RTE_DEVICE_STARTUP_STM32F4XX    /* Device Startup for STM32F4 */
#define RTE_Drivers_SPI1                /* Driver SPI1 */
        #define RTE_Drivers_SPI2                /* Driver SPI2 */
        #define RTE_Drivers_SPI3                /* Driver SPI3 */
        #define RTE_Drivers_SPI4                /* Driver SPI4 */
        #define RTE_Drivers_SPI5                /* Driver SPI5 */
        #define RTE_Drivers_SPI6                /* Driver SPI6 */
#define RTE_Drivers_USBD0               /* Driver USBD0 */
#define RTE_FileSystem_Core             /* File System Core */
          #define RTE_FileSystem_LFN              /* File System with Long Filename support */
          #define RTE_FileSystem_Debug            /* File System Debug Version */
#define RTE_FileSystem_Drive_NOR_0      /* File System NOR Flash Drive 0 */
#define RTE_USB_Core                    /* USB Core */
          #define RTE_USB_Core_Release            /* USB Core Release Version */
#define RTE_USB_Device_0                /* USB Device 0 */
#define RTE_USB_Device_HID_0            /* USB Device HID instance 0 */

#endif /* RTE_COMPONENTS_H */
