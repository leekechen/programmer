#include <cmsis_os2.h>
#include <stm32f4xx_hal.h>
#include <stdint.h>
#include "error.h"
#include "power.h"


#define VDD_OPAMP_MAGNIFICATION         4
#define VPP_OPAMP_MAGNIFICATION         4

#define V_REF                           2500

#define DAC_VDD_GET_VALUE(v)            ((v) * 4096 / (VDD_OPAMP_MAGNIFICATION  * V_REF))
#define DAC_VPP_GET_VALUE(v)            ((v) * 4096 / (VPP_OPAMP_MAGNIFICATION  * V_REF))

#define ADC_VDD_GET_VALUE(v)            (V_REF * (v) * VDD_OPAMP_MAGNIFICATION /4096 )
#define ADC_VPP_GET_VALUE(v)            (V_REF * (v) * VPP_OPAMP_MAGNIFICATION /4096 )

extern DAC_HandleTypeDef hdac;
extern ADC_HandleTypeDef hadc1;

struct power_info {
    uint32_t                voltage;
    uint32_t                state;
    ADC_ChannelConfTypeDef  adc_cfg;
} ;

static struct power_info vdd_info = {
    .voltage = 0,
    .state = POWER_FLAG_OFF,
    .adc_cfg = {
        .Channel = ADC_CHANNEL_11,
        .Rank    = 1,
        .SamplingTime = ADC_SAMPLETIME_3CYCLES,
    },
} ;

static struct power_info vpp_info = {
    .voltage = 0,
    .state = POWER_FLAG_OFF,
    .adc_cfg = {
        .Channel = ADC_CHANNEL_10,
        .Rank    = 1,
        .SamplingTime = ADC_SAMPLETIME_3CYCLES,
    },
} ;

//static void (*power_callback)(uint32_t event);

static int32_t power_ctrl_vdd_setup(uint32_t voltage)
{
    int32_t  err;
    uint16_t dor;

    vdd_info.voltage = voltage;
    dor = DAC_VDD_GET_VALUE(voltage);
    
    err = HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, dor);
    if (err) {
        return err;
    }

    err = HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
    
    
    return err;
}

static int32_t power_ctrl_vdd_readback(uint32_t* voltage)
{
    int32_t err;
    uint32_t value;

    *voltage = 0;

    err = HAL_ADC_ConfigChannel(&hadc1, &vdd_info.adc_cfg);
    if (err) {
        return err;
    }
    
    err = HAL_ADC_Start(&hadc1);
    if (err) {
        return err;
    }

    err = HAL_ADC_PollForConversion(&hadc1, 10);
    if (err) {
        return err;
    }

    if ((HAL_ADC_GetState(&hadc1) & HAL_ADC_STATE_EOC_REG) != HAL_ADC_STATE_EOC_REG) {
        return ERR_VDD;
    }

    
    value = HAL_ADC_GetValue(&hadc1);
    *voltage = ADC_VDD_GET_VALUE(value);

    return ERR_OK;
}

static int32_t power_ctrl_vpp_setup(uint32_t voltage)
{
    int32_t err;
    uint16_t dor;
    
    vpp_info.voltage = voltage;
    dor = DAC_VPP_GET_VALUE(voltage);

    err = HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, dor);
    if (err) {
        return err;
    }

    err = HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
    
    return err;
}

static int32_t power_ctrl_vpp_readback(uint32_t* voltage)
{
    int32_t err;
    uint32_t value;

    *voltage = 0;

    err = HAL_ADC_ConfigChannel(&hadc1, &vpp_info.adc_cfg);
    if (err) {
        return err;
    }
    
    err = HAL_ADC_Start(&hadc1);
    if (err) {
        return err;
    }

    err = HAL_ADC_PollForConversion(&hadc1, 10);
    if (err) {
        return err;
    }

    if ((HAL_ADC_GetState(&hadc1) & HAL_ADC_STATE_EOC_REG) != HAL_ADC_STATE_EOC_REG) {
        return ERR_VDD;
    }

    
    value = HAL_ADC_GetValue(&hadc1);
    *voltage = ADC_VDD_GET_VALUE(value);

    return ERR_OK;
}



int32_t power_voltage_set(struct tlv_voltage* voltage)
{
    int32_t  err;
    uint32_t voltage_readback;
    
    switch (voltage->type) {
    case TLV_DESC_VOLTAGE_TYPE_VDD:
        err = power_ctrl_vdd_setup(voltage->value);
        if (err) {
            return err;
        }
        power_ctrl_vdd_readback(&voltage_readback);
        break;
    case TLV_DESC_VOLTAGE_TYPE_VPP:
        err = power_ctrl_vpp_setup(voltage->value);
        if (err) {
            return err;
        }
        power_ctrl_vpp_readback(&voltage_readback);
        break;
    default:
        return ERR_SYS_ARGUMENT;
    }
    
    return err;
}

int32_t power_voltage_get(int32_t type, uint32_t* mv)
{
//    int32_t  err;

    if (mv == NULL) {
        return ERR_SYS_ARGUMENT;
    }

    *mv = 0;
    switch (type) {
    case TLV_DESC_VOLTAGE_TYPE_VDD:
        power_ctrl_vdd_readback(mv);
        break;
    case TLV_DESC_VOLTAGE_TYPE_VPP:
        power_ctrl_vpp_readback(mv);
        break;
    default:
        return ERR_SYS_ARGUMENT;
    }
    
    return ERR_OK;
}

int32_t power_voltage_switch(int32_t type, uint32_t flag)
{
    switch (type) {
    case TLV_DESC_VOLTAGE_TYPE_VDD:
        HAL_GPIO_WritePin(VDD_ON_GPIO_Port, VDD_ON_Pin, (GPIO_PinState)flag);
        break;
    case TLV_DESC_VOLTAGE_TYPE_VPP:
        HAL_GPIO_WritePin(VPP_ON_GPIO_Port, VPP_ON_Pin, (GPIO_PinState)flag);
        break;
    }
    return ERR_OK;
}


int32_t power_init(void (*power_event_callback)())
{
//    power_callback = power_event_callback;

    return ERR_OK;
}
