#include "misc.h"

#include <avr/io.h>


/**
 *  @brief toggle LED on PB7
 * 
 * */
void toggle_led()
{
    if(!(DDRB & (1 << PB7))){
        DDRB |= (1 << PB7);
    }

    PORTB ^= (1 << PB7);
}