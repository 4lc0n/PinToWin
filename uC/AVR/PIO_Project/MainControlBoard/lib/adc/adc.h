/**
 *  Header file for ADC unit on ATMeag2560
 * 
 *  will start up the adc in single conversion mode, on interrupt a new conversion 
 *  will be started to assure input channel change is updated
 * 
 *  on conversion finished: Interrupt will store data into a buffer in ping-pong mode
 *  and start new conversion
 * 
 * 
 * 
 * 
 * */



#ifndef ADC_H
#define ADC_H



#define TEMPSENS_L_MTRX 0x13
#define TEMPSENS_R_MTRX 0x14



/**
 *  @brief initializes ADC but does not yet start it. 
 * */
void adc_init();


/**
 * @brief start adc conversions in single conversion mode, interrupt driven
 * */
void adc_start();




#endif