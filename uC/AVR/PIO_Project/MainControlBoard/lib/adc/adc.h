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

// #define ADC_8_BIT_RESOLUTION        // uncomment this line to get 8 bit resolution

#define TEMPSENS_L_MTRX 0x23
#define TEMPSENS_R_MTRX 0x24

#define TEMP_SENSE_R1 10000
#define V_ADC_REF   5.0

#define THERMISTOR_B 3965.0f    //https://www.ametherm.com/thermistor/precision-interchangeable-thermistor
#define THERMISTOR_NOMINAL 10e3
#define TEMPERATURE_NOMINAL 25.0f

#define CURRENTSENSE_1_MTRX 0x25
#define CURRENTSENSE_2_MTRX 0x26
#define CURRENTSENSE_3_MTRX 0x27


#define CURRENTSENSE_AMP_FACTOR 11



#define ADC_SCAN_RATE 100           // adc scan rate 


/**
 *  @brief initializes ADC but does not yet start it. 
 * */
void adc_init();


/**
 * @brief start adc conversions in single conversion mode, interrupt driven
 * */
void adc_start();




#endif