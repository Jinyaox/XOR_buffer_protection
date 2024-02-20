#include <stdint.h>
#include <stdbool.h>
#include "driverlib/adc.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "ustdlib.h"

#define ADC_SINGLE_SAMPLE (3)
#define ADC_EIGHT_SAMPLE (0)
#define ADC_FOUR_SAMPLE (2)
#define ADC_HIGHEST_PRIORITY (0)

void entrophy_init(void){
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);

    ADCSequenceConfigure(ADC0_BASE, ADC_SINGLE_SAMPLE, ADC_TRIGGER_PROCESSOR, ADC_HIGHEST_PRIORITY);
    ADCSequenceConfigure(ADC1_BASE, ADC_SINGLE_SAMPLE, ADC_TRIGGER_PROCESSOR, ADC_HIGHEST_PRIORITY);

    ADCSequenceStepConfigure(ADC0_BASE, ADC_SINGLE_SAMPLE, ADC_TRIGGER_PROCESSOR, ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END);
    ADCSequenceStepConfigure(ADC1_BASE, ADC_SINGLE_SAMPLE, ADC_TRIGGER_PROCESSOR, ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END);
    ADCClockConfigSet(ADC0_BASE, ADC_CLOCK_SRC_PIOSC | ADC_CLOCK_RATE_FULL, 1);

    ADCSequenceEnable(ADC0_BASE, ADC_SINGLE_SAMPLE);
    ADCIntClear(ADC0_BASE, ADC_SINGLE_SAMPLE);
    ADCSequenceEnable(ADC1_BASE, ADC_SINGLE_SAMPLE);
    ADCIntClear(ADC1_BASE, ADC_SINGLE_SAMPLE);
}

void get_temp_entrophy(volatile uint32_t *result, short select){
    if(select != 0 && select != 1){
        return;
    }

    uint32_t adc_base=ADC0_BASE;

    if(select > 128){
      adc_base = ADC1_BASE;
    }

    //trigger sample
    ADCProcessorTrigger(adc_base, ADC_SINGLE_SAMPLE);


    while(!ADCIntStatus(adc_base, ADC_SINGLE_SAMPLE, false))
    {}
    //Clear int
    ADCIntClear(adc_base, ADC_SINGLE_SAMPLE);

    ADCSequenceDataGet(adc_base, ADC_SINGLE_SAMPLE, (uint32_t *)(result));
}


uint32_t rand_init(){
    volatile uint32_t Sram_rand[64];
    volatile uint32_t thermoentrophy;


    //do the thermo entrophy extraction, the selector is the flash value's
    entrophy_init();
    get_temp_entrophy(&thermoentrophy, thermoentrophy & 0x00000001); //make the select from the flash


    int rand_select=thermoentrophy%64;
    int entrophy2=Sram_rand[rand_select];

    //XOR thermoentrophy and flash
    return entrophy2 ^ thermoentrophy;
}

uint32_t gen_rand(){
    //generate initial vector
    uint32_t IV=rand_init();

    //return that random number
    usrand(IV);

    return urand();
}
