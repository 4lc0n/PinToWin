#include "adc.h"

#include <avr/io.h>
#include <avr/interrupt.h>



// structure for sampling structure
int8_t apin[] = {};     // list for analog pins to sample 
int8_t dpin[] = {};     // list of corresponding digial pins (used for LDR matrix)




// const of nuber of conversions for 1 scan
const uint8_t n_steps = sizeof(apin);


// ping and pong buffer for conversion result (8 bit adjusted)
volatile uint8_t ping_buffer[sizeof(apin)], pong_buffer[sizeof(apin)];      

volatile uint8_t pp_select = 0;         // selector for ping or pong buffer: 0 = ping, 1 = pong


/**
 *  @brief initializes ADC but does not yet start it. Mode will be free-running mode
 * */
void adc_init()
{

    // select reference as AVCC, no left-adjust
    ADMUX = (0 << REFS0) | (1 << REFS1);

    //  Enable ADC, auto trigger disabled (for single mode), interrupt enable, prescaler 64 (250 kHz)
    ADCSRA = (1 << ADEN) | (0 << ADATE) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0);

    

    // TODO: disable digial input for used pins


}


/**
 * @brief start adc conversions in free-running mode, interrupt driven
 * */
void adc_start()
{

    // TODO: set first pin to sample

    // start first conversion, free running is enabled
    ADCSRA |= (1 << ADSC);


}



/**
 *  @brief function to switch between all inputs by containing all needed information here
 * 
 *  @param step: integer with the current step count to set correct pins
 * 
 * */
void input_change(uint8_t step)
{

    // TODO: this
    
}

ISR(ADC_vect)
{
    // TODO: this
    ;
}