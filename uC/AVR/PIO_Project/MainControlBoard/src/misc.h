#ifndef MISC_H
#define MISC_H

#include <avr/io.h>

#define DEBUG_UART 1

/**
 *  @brief toggle LED on PB7
 * 
 * */
void toggle_led();
/**
 *  @brief clears LED on PB7
 * 
 * */
void clear_led();
/**
 *  @brief sets LED on PB7
 * 
 * */
void set_led();

/**
 *  @brief transmits debug messages (const char*) via UART1
 * 
 *  @param c: char pointer with debug message
 * 
 * */
void print_debug(const char *c);


void draw_welcome(void);

/** 
 *  @brief: function to initialize inputs for button with ISR pin change * 
 * */
void setup_button_inputs(void);

/**
 *  @brief setup function to setup timers for pwm operation on solenoids
 *  MAX value for PWM in OCR4x is set in PWM_RANGE
 *  TIMER 4, pins PH3, PH4, PH5 
 * 
 * */
void setup_pwm_outputs(void);


/**
 * @brief Set the up watchdog timer in reset mode, 0.5sec timeout
 * 
 */
void setup_watchdog(void);

#endif