#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <stdint.h>
#include <stdlib.h>

/* Hardware version defination */
#define FIRMWARE_VERSION_MAJOR                      3
#define FIRMWARE_VERSION_MINI                       0
#define FIRMWARE_VERSION_REVERSION                  0

#define BOOTLOADER_VERSION_MAJOR                    3
#define BOOTLOADER_VERSION_MIN                      0
#define BOOTLOADER_VERSION_REVERSION                0

#define HW_VERSION_MAJOR                            3
#define HW_VERSION_MINI                             0
#define HW_VERSION_REVERSION                        0

/* uncomment the MACROs will cause to firmware to support programming the chip */
#ifndef BOOTLOADER

#define CSU8RT30213
#define CSU8RP3228

#endif

/* Don't change these MACROs at below, except you add a new chip */
#ifdef CSU8RT30213
    #define STR_CSU8RP3212          "CSU8RP3212",
    #define STR_CSU8RP3215          "CSU8RP3215",
    #define STR_CSU8RP3216          "CSU8RP3216",
    #define STR_CSU8RP3217          "CSU8RP3217",
    #define STR_CSU8RP3218          "CSU8RP3218",
    #define STR_CSU8RP3219          "CSU8RP3219",
    #define STR_CSU8RP2215          "CSU8RP2215",
    #define STR_CSU8RP2218          "CSU8RP2218",
#else
    #define STR_CSU8RP3212          
    #define STR_CSU8RP3215          
    #define STR_CSU8RP3216          
    #define STR_CSU8RP3217          
    #define STR_CSU8RP3218          
    #define STR_CSU8RP3219          
    #define STR_CSU8RP2215          
    #define STR_CSU8RP2218              
#endif


#ifdef CSU8RP3228
    #define STR_CSU8RP3424          "CSU8RP3424",
    #define STR_CSU8RP3425          "CSU8RP3425",
    #define STR_CSU8RP3426          "CSU8RP3426",
    #define STR_CSU8RP3427          "CSU8RP3427",
    #define STR_CSU8RP3429          "CSU8RP3429",
    #define STR_CSU8RP3418          "CSU8RP3418",
    #define STR_CSU8RP3234          "CSU8RP3234",
    #define STR_CSU8RP3235          "CSU8RP3235",
    #define STR_CSU8RP3236          "CSU8RP3236",
    #define STR_CSU8RP3225          "CSU8RP3225",
    #define STR_CSU8RP3226          "CSU8RP3226", 
#else
    #define STR_CSU8RP3424          
    #define STR_CSU8RP3425          
    #define STR_CSU8RP3426          
    #define STR_CSU8RP3427          
    #define STR_CSU8RP3429          
    #define STR_CSU8RP3418          
    #define STR_CSU8RP3234          
    #define STR_CSU8RP3235          
    #define STR_CSU8RP3236          
    #define STR_CSU8RP3225          
    #define STR_CSU8RP3226 
#endif


#endif /* __CONFIG_H__ */
