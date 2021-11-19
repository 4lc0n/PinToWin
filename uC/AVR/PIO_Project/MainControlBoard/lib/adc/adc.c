#include "adc.h"

#include <avr/io.h>
#include <avr/interrupt.h>


#include "main.h"

// structure for sampling structure
int16_t amux[] = {TEMPSENS_L_MTRX, TEMPSENS_R_MTRX};     // list for analog pins to sample 
int16_t dpin[] = {-1, -1, -1, -1, -1};     // list of corresponding digial pins (used for LDR matrix)




// const of nuber of conversions for 1 scan
const uint8_t n_steps = sizeof(amux);

#ifdef ADC_8_BIT_RESOLUTION
// ping and pong buffer for conversion result (8 bit adjusted)
volatile uint8_t ping_buffer[sizeof(amux)], pong_buffer[sizeof(amux)];      
#else
// ping and pong buffer for conversion result (8 bit adjusted)
volatile uint16_t ping_buffer[sizeof(amux)], pong_buffer[sizeof(amux)];      
#endif

volatile uint8_t pp_select = 0;         // selector for ping or pong buffer: 0 = ping, 1 = pong
volatile uint8_t current_p = 0;

/**
 *  @brief initializes ADC but does not yet start it. Mode will be free-running mode
 * */
void adc_init()
{

    // activate ADC
    ADCSRA |= (1 << ADEN);

    // select reference as AVCC
    ADMUX = (0 << REFS0) | (1 << REFS1);

#ifdef ADC_8_BIT_RESOLUTION
    ADCMUX |= (1 << ADLAR);     // left adjust result, read only high bit
#else
    ADMUX &= ~(1 << ADLAR);     // right adjust, read full 16 bit reg
#endif


    //  Enable ADC, auto trigger disabled (for single mode), interrupt enable, prescaler 64 (250 kHz)
    ADCSRA |= (0 << ADATE) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0);

    


    // TODO: disable digial input for used pins
    DIDR2 |= (1 << ADC12D) | (1 << ADC11D);     // pins for temperature sensor
    DIDR2 |= (1 << ADC15D) | (1 << ADC14D) | (1 << ADC13D);     // pins for current sensing

    input_change(current_p);

}


/**
 * @brief start adc conversions in free-running mode, interrupt driven
 * */
void adc_start()
{

    // TODO: set first pin to sample

    // start first conversion, free running is handled by software
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



    // disable last dpin
    if(step - 1 < 0){
        switch(dpin[n_steps - step - 1])
        {
            case 0:
                MATRIX_ROW0_PORT &= ~(1 << MATRIX_ROW0_P);
                break;
            case 1:
                MATRIX_ROW1_PORT &= ~(1 << MATRIX_ROW1_P);
                break;
            case 2:
                MATRIX_ROW2_PORT &= ~(1 << MATRIX_ROW2_P);
                break;
            default:
                break;
        }
    }
    else{
        switch(dpin[step - 1])
        {
            case 0:
                MATRIX_ROW0_PORT &= ~(1 << MATRIX_ROW0_P);
                break;
            case 1:
                MATRIX_ROW1_PORT &= ~(1 << MATRIX_ROW1_P);
                break;
            case 2:
                MATRIX_ROW2_PORT &= ~(1 << MATRIX_ROW2_P);
                break;
            default:
                break;
        }
    }

    // activate next pin
    if(dpin[step] != -1)
    {
        switch(dpin[step])
        {
            case 0:
                MATRIX_ROW0_PORT |= (1 << MATRIX_ROW0_P);
                break;
            case 1:
                MATRIX_ROW1_PORT |= (1 << MATRIX_ROW1_P);
                break;
            case 2:
                MATRIX_ROW2_PORT |= (1 << MATRIX_ROW2_P);
                break;
            default:
                break;
        }
    }

    // set next adc mux 
    ADMUX &= ~(0x1F);
    ADMUX |= (amux[step] & 0x1F);

    ADCSRB &= ~(1 << MUX5);
    ADCSRB |= ( ((amux[step] & 0x20) >> 5) << MUX5 );

    
}

ISR(ADC_vect)
{

#ifdef ADC_8_BIT_RESOLUTION
    uint8_t data = ADCH;
#else
    uint16_t data = ADC;
#endif

    // read adc data
    if(pp_select)
    {
        ping_buffer[current_p] = data;
    }
    else{
        pong_buffer[current_p] = data;
    }

    // increase step
    current_p++;

    // if done with this scan: change buffer,
    if(current_p == n_steps){
        pp_select = !pp_select;
    }
    // give semaphore
    // TODO: this


    // set new pins
    input_change(current_p);

    // start new conversion
    adc_start();

}