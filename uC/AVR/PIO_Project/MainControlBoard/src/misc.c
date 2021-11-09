#include "misc.h"

#include <avr/io.h>

#include <stdio.h>
#include <string.h>

#include <Arduino_FreeRTOS.h>


#include "uart.h"


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

/**
 *  @brief transmits debug messages (const char*) via UART1
 * 
 *  @param c: char pointer with debug message
 * 
 * */
void print_debug(const char *c)
{
    char tbuff[64];                                // allocate buffer
    sprintf(tbuff, c);                                      // print to buffer
    while(uart_tx_buffer_state(DEBUG_UART) < strlen(tbuff)) vTaskDelay(15 / portTICK_PERIOD_MS);         // wait until enough space is free in tx buffer
    uart_puts(DEBUG_UART, tbuff);                                    // simply call function
}