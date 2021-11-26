#include "misc.h"

#include <avr/io.h>

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <Arduino_FreeRTOS.h>


#include "uart.h"
#include "main.h"



extern uint16_t PWM_RANGE;


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
 *  @brief set LED on PB7
 * 
 * */
void set_led()
{
    if(!(DDRB & (1 << PB7))){
        DDRB |= (1 << PB7);
    }

    PORTB |= (1 << PB7);
}
/**
 *  @brief clear LED on PB7
 * 
 * */
void clear_led()
{
    if(!(DDRB & (1 << PB7))){
        DDRB |= (1 << PB7);
    }

    PORTB &= ~(1 << PB7);
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


// welcome screen:
// ========================================
// ||            Main Control            ||
// ||               Board                ||
// ||------------------------------------||
// ||        Firmware: 2021-11-07        ||
// ========================================
// UART0: initialized
// UART1: initialized ...
void draw_welcome(void){
  


  print_debug("\n\n========================================\n"); 
  print_debug("||            Main Control            ||\n");
  print_debug("||               Board                ||\n");
  print_debug("||------------------------------------||\n");
  print_debug("||        Firmware: 2021-11-26        ||\n");
  print_debug("========================================\n");
 
  print_debug("UART0: initialized\n");

  print_debug("UART1: initialized\n");
}

/** 
 *  @brief: function to initialize inputs for button with ISR pin change * 
 * */
void setup_button_inputs(void){

  // set to input
  BUTTONL_DDR &= ~(1 << BUTTONL_P);
  BUTTONR_DDR &= ~(1 << BUTTONR_P);

  // enable internal pullup (around 35k)
  BUTTONL_PORT |= (1 << BUTTONL_P);
  BUTTONR_PORT |= (1 << BUTTONR_P);


  // PCINT9 and PCINT10 on 
  // enable corresponding PCINT 
  // ! this has to be changed manually on pin change
  PCICR |= (1 << PCIE1);

  // enable interrupt for PCIE1 pins
  PCMSK1 |= (1 << BUTTONL_INT) | (1 << BUTTONR_INT);

  // activate GND pin for the button input
  BUTTON_GND_DDR |= (1 << BUTTON_GND_P);
  BUTTON_GND_PORT &= ~(1 << BUTTON_GND_P);
}



/**
 *  @brief setup function to setup timers for pwm operation on solenoids
 *  MAX value for PWM in OCR4x is set in PWM_RANGE
 *  TIMER 4, pins PH3, PH4, PH5 
 * 
 * */
void setup_pwm_outputs(void)
{

  // using timer 4 for pins
  // up counting, with max accordingly set to match pwm frequency in ICRn
  // fast pwm  mode with set on compare, so a value of 0 means always on, as input of 
  // solenoid drivers is inverted

  
  ICR4 = (F_CPU / (1 * PWM_FREQ)) - 1;                                              // calculate max. value for set PWM frequency, MUST NOT EXCEED 16 bit!

  // no interrupt needed, so none is ste

  OCR4A = 0;
  OCR4B = 0;
  OCR4C = 0;

  TCCR4A = (0x3 << COM4A0) | (0x3 << COM4B0) | (0x3 << COM4C0) | (0x2 << WGM40);    // set to set on compare, fast pwm

  TCCR4B = (0x1 << CS40) | (0x3 << WGM42);                                          // clock prescaler = 1, fast pwms


  // set PWM_RANGE to ICR4 value, to have a reference frame for pwm signal
  PWM_RANGE = ICR4;

}