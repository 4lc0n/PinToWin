#ifndef MISC_H
#define MISC_H

#include <avr/io.h>



/**
 *  @brief toggle LED on PB7
 * 
 * */
void toggle_led();

/**
 *  @brief transmits debug messages (const char*) via UART1
 * 
 *  @param c: char pointer with debug message
 * 
 * */
void print_debug(const char *c);

#endif