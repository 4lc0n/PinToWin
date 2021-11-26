#include "adc.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "../../src/main.h"

#include <Arduino_FreeRTOS.h>
#include <semphr.h>



// structure for sampling structure
//                  temp sensor     temp sensor         current sensors                                                     // starter      // wheels                                       // slingshot    target center   slingshoht      // buttons      
int16_t amux[] = {TEMPSENS_L_MTRX, TEMPSENS_R_MTRX, CURRENTSENSE_1_MTRX,    CURRENTSENSE_2_MTRX,    CURRENTSENSE_3_MTRX,    LDR_ROW0_MTRX,  LDR_ROW0_MTRX,  LDR_ROW1_MTRX,  LDR_ROW2_MTRX,  LDR_ROW0_MTRX,  LDR_ROW1_MTRX,  LDR_ROW2_MTRX,  LDR_ROW0_MTRX,  LDR_ROW1_MTRX,  LDR_ROW2_MTRX};     // list for analog pins to sample 
int16_t dpin[] = {-1,               -1,             -1,                     -1,                     -1,                     0,              1,              1,              1,              2,              2,              2,              3,              3,              4};                         // list of corresponding digial pins (used for LDR matrix)
extern SemaphoreHandle_t xSemaphore_adc_complete;


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
 * @brief start adc conversions in free-running mode, interrupt driven
 * */
void adc_start()
{



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
                MATRIX_COL0_PORT &= ~(1 << MATRIX_COL0_P);
                break;
            case 1:
                MATRIX_COL1_PORT &= ~(1 << MATRIX_COL1_P);
                break;
            case 2:
                MATRIX_COL2_PORT &= ~(1 << MATRIX_COL2_P);
                break;
            case 3:
                MATRIX_COL3_PORT &= ~(1 << MATRIX_COL3_P);
                break;
            default:
                break;
        }
    }
    else{
        switch(dpin[step - 1])
        {
            case 0:
                MATRIX_COL0_PORT &= ~(1 << MATRIX_COL0_P);
                break;
            case 1:
                MATRIX_COL1_PORT &= ~(1 << MATRIX_COL1_P);
                break;
            case 2:
                MATRIX_COL2_PORT &= ~(1 << MATRIX_COL2_P);
                break;
            case 3:
                MATRIX_COL3_PORT &= ~(1 << MATRIX_COL3_P);
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
                MATRIX_COL0_DDR |= (1 << MATRIX_COL0_P);
                break;
            case 1:
                MATRIX_COL1_DDR |= (1 << MATRIX_COL1_P);
                break;
            case 2:
                MATRIX_COL2_DDR |= (1 << MATRIX_COL2_P);
                break;
            case 3:
                MATRIX_COL3_DDR |= (1 << MATRIX_COL3_P);
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


/**
 *  @brief initializes ADC but does not yet start it. Mode will be free-running mode
 * */
void adc_init()
{

    // activate ADC
    ADCSRA |= (1 << ADEN);

    // select reference as AVC
    ADMUX = (0 << REFS1) | (1 << REFS0);

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
        current_p = 0;
    }
    // give semaphore
    xSemaphoreGiveFromISR(xSemaphore_adc_complete, NULL);


    // set new pins
    input_change(current_p);

    // start new conversion only if not finished with this scan
    if(current_p != 0)
        adc_start();

}