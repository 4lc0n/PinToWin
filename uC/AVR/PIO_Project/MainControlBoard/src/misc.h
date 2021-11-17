#ifndef MISC_H
#define MISC_H

#include <avr/io.h>

#define DEBUG_UART 0

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

#endif