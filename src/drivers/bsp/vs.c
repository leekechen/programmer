#include <stm32f4xx_hal.h>
#include "error.h"
#include "vs.h"

#define V_REF                           (2500)

#define VS_OPAMP_MAGNIFICATION          (2)
#define ADC_VS_GET_VALUE(v)             (V_REF * (v) * VS_OPAMP_MAGNIFICATION / 4096 )

#define VS_B_OPAMP_MAGNIFICATION        (2)
#define ADC_VS_B_GET_VALUE(v)           (V_REF * (v) * VS_B_OPAMP_MAGNIFICATION / 4096 )

#define VS_OUTPUT_PRECISE               (2500)
#define VS_OUTPUT_PERMISSIBLE_ERROR     ()
#define VS_OUTPUT_FLAG_OFF              (0)
#define VS_OUTPUT_FLGA_ON               (1)
#define VS_OUTPUT_IS_VALID              ()

#define VS_INPUT_FLAG_OFF               (0)
#define VS_INPUT_FLAG_IN                (1)

extern ADC_HandleTypeDef hadc1;

static int32_t vs_cali_output_read(uint32_t* voltage)
{
    int32_t err;
    uint32_t value;    
    
    ADC_ChannelConfTypeDef  cfg;
    cfg.Channel      = ADC_CHANNEL_12;
    cfg.Rank         = 1;
    cfg.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    
    *voltage = 0;

    err = HAL_ADC_ConfigChannel(&hadc1, &cfg);
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
    *voltage = ADC_VS_GET_VALUE(value);

    return ERR_OK;
}

int32_t vs_cali_config(uint32_t mode)
{
    int32_t     err;
    uint32_t    voltage;
    
    switch (mode) {
    case VS_CALI_MODE_INPUT:
        /* Turn off 2.5V output at first */
        HAL_GPIO_WritePin(VS_ON_GPIO_Port, VS_ON_Pin, VS_OUTPUT_FLAG_OFF);
        HAL_GPIO_WritePin(VS_ON_B_GPIO_Port, VS_ON_B_Pin, VS_INPUT_FLAG_IN);
        break;
    case VS_CALI_MODE_OUTPUT_2500_MV:
        HAL_GPIO_WritePin(VS_ON_B_GPIO_Port, VS_ON_B_Pin, VS_INPUT_FLAG_OFF);
        HAL_GPIO_WritePin(VS_ON_GPIO_Port, VS_ON_Pin, VS_OUTPUT_FLGA_ON);
        err = vs_cali_output_read(&voltage);
        if (err) {
            return err;
        }        
        
        break;
    default:
        return ERR_SYS_ARGUMENT;
    }
    return ERR_OK;
}

int32_t vs_cali_input_read(uint32_t* voltage)
{
    int32_t err;
    uint32_t value;    
    
    ADC_ChannelConfTypeDef  cfg;
    cfg.Channel = ADC_CHANNEL_13;
    cfg.Rank = 1;
    cfg.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    
    *voltage = 0;

    err = HAL_ADC_ConfigChannel(&hadc1, &cfg);
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
    *voltage = ADC_VS_B_GET_VALUE(value);

    return ERR_OK;
}

int32_t vs_cali_read(uint32_t mode, uint32_t* voltage)
{
    int32_t     err;
    
    switch (mode) {
    case VS_CALI_MODE_INPUT:
        err = vs_cali_input_read(voltage);
        break;
    case VS_CALI_MODE_OUTPUT_2500_MV:
        err = vs_cali_output_read(voltage);
        break;
    default:
        return ERR_SYS_ARGUMENT;
    }
    return ERR_OK;
}
