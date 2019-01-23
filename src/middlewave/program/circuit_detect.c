#include <stm32f4xx_hal.h>
#include "circuit_detect.h"
#include "power.h"
#include "error.h"
#include "delay.h"

#define     GPIO_PIN_OUTPUT                1
#define     GPIO_PIN_ANALOG                0

#define     V_REF                          (2500)
#define     ADC_PROGRAM_GET_VALUE(v)       (V_REF * (v) / 4096 )

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;

struct program_pin {
    GPIO_TypeDef*   port;
    uint32_t        pin;
};

struct program_info {   
    ADC_HandleTypeDef* hadcx;
    struct program_pin port_value;
    ADC_ChannelConfTypeDef cfg;
};

struct program_info program_infos[7] ={
    {
        .hadcx = &hadc1,
        .port_value = {
            ADC_PIN1_GPIO_Port,
            ADC_PIN1_Pin
        },
        .cfg = {
            .Channel = ADC_CHANNEL_0                   /*PA0-WKUP ------> ADC1_IN0  1*/
        }
    },
    {
        .hadcx = &hadc2,     
        .port_value = {
            ADC_PIN2_GPIO_Port,
            ADC_PIN2_Pin
        },
        .cfg = {
            .Channel = ADC_CHANNEL_1                   /* PA1     ------> ADC2_IN1  2*/
        }
    },
    {
        .hadcx = &hadc3,     
        .port_value = {
            ADC_PIN3_GPIO_Port,
            ADC_PIN3_Pin
        },
        .cfg = {
            .Channel = ADC_CHANNEL_2                   /* PA2     ------> ADC3_IN2 3*/
        }
    },
    {
        .hadcx = &hadc3,     
        .port_value = {
            ADC_PIN4_GPIO_Port,
            ADC_PIN4_Pin
        },
        .cfg = {
            .Channel = ADC_CHANNEL_3                   /* PA3     ------> ADC3_IN3 4*/
        }
    },
    {
        .hadcx = &hadc1,     
        .port_value = {
            ADC_PIN5_GPIO_Port,
            ADC_PIN5_Pin
        },
        .cfg = {
            .Channel = ADC_CHANNEL_6                   /* PA6     ------> ADC1_IN6 5*/
        }
    },
    {
        .hadcx = &hadc1,     
        .port_value = {
            ADC_PIN6_GPIO_Port,
            ADC_PIN6_Pin
        },
        .cfg = {
            .Channel = ADC_CHANNEL_7                   /* PA7     ------> ADC1_IN7 6*/
        }
    },
    {
        .hadcx = &hadc1,     
        .port_value = {
            ADC_PIN7_GPIO_Port,
            ADC_PIN7_Pin
        },
        .cfg = {
            .Channel = ADC_CHANNEL_14                  /* PC4    ------> ADC1_IN14 7*/
        }
    }
}; 

static void txb0108pwr_enable_switch(uint32_t flag)
{
    HAL_GPIO_WritePin(OE_GPIO_Port, OE_Pin, (GPIO_PinState)flag);
}

static void gnd_voltage_switch(uint32_t flag)
{
    HAL_GPIO_WritePin(ADC_PIN8_GPIO_Port, ADC_PIN8_Pin, (GPIO_PinState)flag);
}

static void circuit_detect_peripheral_ports_set(void)
{
    txb0108pwr_enable_switch(POWER_FLAG_OFF);
    power_voltage_switch(POWER_TYPE_VDD,POWER_FLAG_OFF);
    power_voltage_switch(POWER_TYPE_VPP,POWER_FLAG_OFF);
    HAL_GPIO_WritePin(VS_ON_GPIO_Port, VS_ON_Pin, POWER_FLAG_OFF);
    HAL_GPIO_WritePin(GND_CTRL_GPIO_Port, GND_CTRL_Pin, POWER_FLAG_OFF);
}

static void adc_pin8_status_set(uint32_t status)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    HAL_GPIO_DeInit(GPIOC,ADC_PIN8_Pin);
    GPIO_InitStructure.Pin = ADC_PIN8_Pin;
    if(!status){
        GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    }
    else{
        GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    }
    
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
}

static void all_adc_pin_status_set(uint32_t status)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    HAL_GPIO_DeInit(GPIOA,ADC_PIN1_Pin|ADC_PIN2_Pin|ADC_PIN3_Pin|
                          ADC_PIN4_Pin|ADC_PIN5_Pin|ADC_PIN6_Pin);
    GPIO_InitStructure.Pin = ADC_PIN1_Pin|ADC_PIN2_Pin|ADC_PIN3_Pin|
                             ADC_PIN4_Pin|ADC_PIN5_Pin|ADC_PIN6_Pin;
    if(!status){
        GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    }
    else{
        GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    }
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);

    HAL_GPIO_DeInit(GPIOC,ADC_PIN7_Pin|ADC_PIN8_Pin);
    GPIO_InitStructure.Pin = ADC_PIN7_Pin|ADC_PIN8_Pin;
    if(!status){
        GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    }
    else{
        GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    }
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
}

static void circuit_detect_init(void)
{
    circuit_detect_peripheral_ports_set();
    all_adc_pin_status_set(GPIO_PIN_ANALOG);   
    adc_pin8_status_set(GPIO_PIN_OUTPUT);
}

static void ad_pin_output_set_switch(uint32_t shielding_channel)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    if(shielding_channel<7){
        HAL_GPIO_DeInit(program_infos[shielding_channel].port_value.port,program_infos[shielding_channel].port_value.pin);
        GPIO_InitStructure.Pin = program_infos[shielding_channel].port_value.pin;
        GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStructure.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
    }
    HAL_GPIO_WritePin(program_infos[shielding_channel].port_value.port, program_infos[shielding_channel].port_value.pin, POWER_FLAG_OFF);   
}

static int32_t single_channel_read(struct program_info *program_info_point,uint32_t *adc_value,uint32_t *voltage_value)
{   
    uint32_t err;
    uint32_t i,value = 0;   
    program_info_point->cfg.Rank = 1;
    program_info_point->cfg.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    err = HAL_ADC_ConfigChannel(program_info_point->hadcx, &(program_info_point->cfg));
    if (err){
        return err;
    }  
    for(i=0;i<16;i++){ 
        err = HAL_ADC_Start(program_info_point->hadcx);
        if (err) {
            return err;
        }
        err = HAL_ADC_PollForConversion(program_info_point->hadcx, 10);
        if (err) {
            return err;
        }
        if ((HAL_ADC_GetState(program_info_point->hadcx) & HAL_ADC_STATE_EOC_REG) != HAL_ADC_STATE_EOC_REG) {
            return ERR_PROGRAM_DISTURB;
        }   
        value += HAL_ADC_GetValue(program_info_point->hadcx);
    }
    value = value>>4;
    *adc_value = value;
    *voltage_value = ADC_PROGRAM_GET_VALUE(value);  
    return ERR_OK;     
}

int32_t circuit_detect_polling_all_adc_read(uint32_t shielding_channel,uint32_t *adc_value)
{
    uint8_t i;
    int32_t err;
    uint32_t adc_result,voltage_result;
    circuit_detect_init();
    ad_pin_output_set_switch(shielding_channel);
    gnd_voltage_switch(POWER_FLAG_ON);
    delay_us(600000);                               /*  Here must be delayed more than 20 milliseconds */
    for(i = 0;i<7;i++){
        if(shielding_channel != i){       
            err = single_channel_read(&program_infos[i],&adc_result,&voltage_result);
            if(err){
                return err;                
            }
            adc_value[i] = adc_result;
        }
    }
    gnd_voltage_switch(POWER_FLAG_OFF);
    all_adc_pin_status_set(GPIO_PIN_ANALOG);   
    return ERR_OK;
}

int32_t circuit_detect_pin_status_is_open(uint32_t pin_no, uint32_t *status)
{
    int32_t err;
    uint32_t i,j,adc_result,voltage_result;
    *status = 0;
    circuit_detect_init();
    gnd_voltage_switch(POWER_FLAG_ON);
    delay_us(600000);                               /*  Here must be delayed more than 20 milliseconds */
    for(i=0;i<7;i++){
        j = 1<<i;
        if((pin_no&j) == j)
        {
            err = single_channel_read(&program_infos[i],&adc_result,&voltage_result);   
            if(err)
            {
                return err;
            } 
            if(adc_result<PROGRAM_OPEN_THRESHOLD_VALUE)
            {
                *status |= j;
            }
        }
    }
    gnd_voltage_switch(POWER_FLAG_OFF);
    return ERR_OK;
}

int32_t circuit_detect_pin_status_is_short(uint32_t pin_no, uint32_t *status)
{
    int32_t err;
    uint32_t i,j,adc_result,voltage_result;
    *status = 0;
    circuit_detect_init();
    gnd_voltage_switch(POWER_FLAG_ON);
    delay_us(600000);                               /*  Here must be delayed more than 20 milliseconds */
    for(i=0;i<7;i++){
        j = 1<<i;
        if((pin_no&j) == j)
        {
            err = single_channel_read(&program_infos[i],&adc_result,&voltage_result);   
            if(err)
            {
                return err;
            } 
            if(adc_result>PROGRAM_SHORT_THRESHOLD_VALUE)
            {
                *status |= j;
            }
        }
    }
    gnd_voltage_switch(POWER_FLAG_OFF);
    return ERR_OK;
}

int32_t circuit_detect_pin_get_adc_value(uint32_t pin_no,uint32_t *adc_value)
{
    int32_t err;
    uint32_t i,j,adc_result,voltage_result;
    *adc_value = 0;
    circuit_detect_init();
    gnd_voltage_switch(POWER_FLAG_ON);
    delay_us(600000);                            /*  Here must be delayed more than 20 milliseconds */
    for(i=0;i<7;i++){
        j = 1<<i;
        if((pin_no&j) == j)
        {
            err = single_channel_read(&program_infos[i],&adc_result,&voltage_result);   
            if(err)
            {
                return err;
            }           
            *adc_value = adc_result;          
        }
    }
    gnd_voltage_switch(POWER_FLAG_OFF);
    return ERR_OK;
}

int32_t circuit_detect_pin_get_voltage_value(uint32_t pin_no,uint32_t *voltage_value)
{
    int32_t err;
    uint32_t i,j,adc_result,voltage_result;
    *voltage_value = 0;
    circuit_detect_init();
    gnd_voltage_switch(POWER_FLAG_ON);
    delay_us(600000);                            /*  Here must be delayed more than 20 milliseconds */
    for(i=0;i<7;i++){
        j = 1<<i;
        if((pin_no&j) == j)
        {
            err = single_channel_read(&program_infos[i],&adc_result,&voltage_result);   
            if(err)
            {
                return err;
            }           
            *voltage_value = voltage_result;          
        }
    }
    gnd_voltage_switch(POWER_FLAG_OFF);
    return ERR_OK;
}      


int32_t circuit_detect_turn_off(void)
{
    HAL_GPIO_WritePin(OE_GPIO_Port, OE_Pin, 1);
}

int32_t circuit_detect_turn_on(void)
{
}


